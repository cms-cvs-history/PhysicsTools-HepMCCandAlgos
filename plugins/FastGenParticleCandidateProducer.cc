/** class 
 *
 * \author Luca Lista, INFN
 *
 * \version $Id: FastGenParticleCandidateProducer.cc,v 1.8 2007/03/27 08:43:51 llista Exp $
 *
 */
#include "FWCore/Framework/interface/EDProducer.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleCandidateFwd.h"
#include <vector>
#include <map>
#include <set>

namespace edm { class ParameterSet; }
namespace HepMC { class GenParticle; class GenEvent; }

class FastGenParticleCandidateProducer : public edm::EDProducer {
 public:
  /// constructor
  FastGenParticleCandidateProducer( const edm::ParameterSet & );
  /// destructor
  ~FastGenParticleCandidateProducer();

 private:
  /// module init at begin of job
  void beginJob( const edm::EventSetup & );
  /// process one event
  void produce( edm::Event& e, const edm::EventSetup& );
  /// source collection name  
  std::string src_;
  /// internal functional decomposition
  void fillIndices( const HepMC::GenEvent *, 
	     std::vector<const HepMC::GenParticle *> & ) const;
  /// internal functional decomposition
  void fillOutput( const std::vector<const HepMC::GenParticle *> &,
		   reco::CandidateCollection &, 
		   std::vector<reco::GenParticleCandidate *> & ) const;
  /// internal functional decomposition
  void fillRefs( const std::vector<const HepMC::GenParticle *> &,
		 const reco::CandidateRefProd,
		 const std::vector<reco::GenParticleCandidate *> & ) const;
  /// charge indices
  std::vector<int> chargeP_, chargeM_;
  std::map<int, int> chargeMap_;
  int chargeTimesThree( int ) const;
};

#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleCandidate.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/EDMException.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <fstream>
#include <algorithm>
#include <iostream>
using namespace edm;
using namespace reco;
using namespace std;
using namespace HepMC;

static const int protonId = 2212;
static const int gluonId = 21;
static const int uId = 1;
static const int tId = 6;
static const int stringId = 92;
static const int clusterId = 92;
static const int PDGCacheMax = 32768;
static const double mmToCm = 0.1;

FastGenParticleCandidateProducer::FastGenParticleCandidateProducer( const ParameterSet & p ) :
  src_( p.getParameter<string>( "src" ) ),
  chargeP_( PDGCacheMax, 0 ), chargeM_( PDGCacheMax, 0 ) {
  produces<CandidateCollection>();
}

FastGenParticleCandidateProducer::~FastGenParticleCandidateProducer() { 
}

int FastGenParticleCandidateProducer::chargeTimesThree( int id ) const {
  if( abs( id ) < PDGCacheMax ) 
    return id > 0 ? chargeP_[ id ] : chargeM_[ - id ];
  map<int, int>::const_iterator f = chargeMap_.find( id );
  if ( f == chargeMap_.end() )
    throw edm::Exception( edm::errors::InvalidReference ) 
      << "invalid PDG id: " << id << endl;
  return f->second;
}

void FastGenParticleCandidateProducer::beginJob( const EventSetup & es ) {
  ESHandle<HepPDT::ParticleDataTable> pdt;
  es.getData( pdt );
  for( HepPDT::ParticleDataTable::const_iterator p = pdt->begin(); p != pdt->end(); ++ p ) {
    const HepPDT::ParticleID & id = p->first;
    int pdgId = id.pid(), apdgId = abs( pdgId );
    int q3 = id.threeCharge();
    if ( apdgId < PDGCacheMax )
      if ( pdgId > 0 )
	chargeP_[ apdgId ] = q3;
      else
	chargeM_[ apdgId ] = q3;
    else
      chargeMap_[ pdgId ] = q3;
  }
}

void FastGenParticleCandidateProducer::produce( Event& evt, const EventSetup& es ) {
  Handle<HepMCProduct> mcp;
  evt.getByLabel( src_, mcp );
  const GenEvent * mc = mcp->GetEvent();
  if( mc == 0 ) 
    throw edm::Exception( edm::errors::InvalidReference ) 
      << "HepMC has null pointer to GenEvent" << endl;
  const size_t size = mc->particles_size();

  vector<const GenParticle *> particles( size );
  auto_ptr<CandidateCollection> cands( new CandidateCollection );
  const CandidateRefProd ref = evt.getRefBeforePut<CandidateCollection>();

  vector<GenParticleCandidate *> candVector( size );
  /// fill indices
  fillIndices( mc, particles );
  // fill output collection and save association
  fillOutput( particles, * cands, candVector );
  // fill references to daughters
  fillRefs( particles, ref, candVector );

  evt.put( cands );
}

void FastGenParticleCandidateProducer::fillIndices( const GenEvent * mc,
						    vector<const GenParticle *> & particles ) const {
  size_t idx = 0;
  for( GenEvent::particle_const_iterator p = mc->particles_begin(); 
       p != mc->particles_end(); ++ p ) {
    const GenParticle * particle = * p;
    size_t i = particle->barcode() - 1;
    if( i != idx ++ )
      throw cms::Exception( "WrongReference" )
	<< "barcodes is not properly ordered; got: " << i << " expected: " << idx ;
    particles[ i ] = particle;
  }
}

void FastGenParticleCandidateProducer::fillOutput( const std::vector<const GenParticle *> & particles,
						   CandidateCollection & cands, 
						   vector<GenParticleCandidate *> & candVector ) const {
  const size_t size = particles.size();
  cands.reserve( size );
  for( size_t i = 0; i < size; ++ i ) {
    const GenParticle * part = particles[ i ];
    Candidate::LorentzVector momentum( part->momentum() );
    Candidate::Point vertex( 0, 0, 0 );
    const GenVertex * v = part->production_vertex();
    if ( v != 0 ) {
      ThreeVector vtx = v->point3d();
      vertex.SetXYZ( vtx.x() * mmToCm, vtx.y() * mmToCm, vtx.z() * mmToCm );
    }
    int pdgId = part->pdg_id();
    GenParticleCandidate * c = 
      new GenParticleCandidate( chargeTimesThree( pdgId ), momentum, vertex, 
				pdgId, part->status(), false );
    auto_ptr<Candidate> ptr( c );
    candVector[ i ] = c;
    cands.push_back( ptr );
  }
}

void FastGenParticleCandidateProducer::fillRefs( const std::vector<const GenParticle *> & particles,
						 const CandidateRefProd ref,
						 const vector<GenParticleCandidate *> & candVector ) const {
  for( size_t d = 0; d < candVector.size(); ++ d ) {
    const GenParticle * part = particles[ d ];
    const GenVertex * productionVertex = part->production_vertex();
    if ( productionVertex == 0 ) 
      throw edm::Exception( edm::errors::InvalidReference ) 
	<< "particle has no production vertex. PDG id: " << part->pdg_id() << endl;
    size_t numberOfMothers = productionVertex->particles_in_size();
    if ( numberOfMothers > 0 ) {
      GenVertex::particles_in_const_iterator motherIt = productionVertex->particles_in_const_begin();
      const GenParticle * mother = * motherIt;
      size_t m = mother->barcode() - 1;
      candVector[ m ]->addDaughter( CandidateRef( ref, d ) );
      if ( numberOfMothers > 1 ) {
	++ motherIt;
	const GenParticle * mother2 = * motherIt;
	m = mother2->barcode() - 1;
	candVector[ m ]->addDaughter( CandidateRef( ref, d ) );
      }
    }
  }
}

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE( FastGenParticleCandidateProducer );
