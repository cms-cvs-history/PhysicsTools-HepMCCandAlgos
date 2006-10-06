// $Id: HepMCCandidateProducer.cc,v 1.6 2006/09/29 09:33:39 llista Exp $
#include "PhysicsTools/HepMCCandAlgos/src/HepMCCandidateProducer.h"
//#include "PhysicsTools/HepPDTProducer/interface/PDTRecord.h"
#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"
#include "DataFormats/HepMCCandidate/interface/HepMCCandidate.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "FWCore/Framework/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/EDMException.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <fstream>
using namespace edm;
using namespace reco;
using namespace std;

HepMCCandidateProducer::HepMCCandidateProducer( const ParameterSet & p ) :
  src_( p.getParameter<string>( "src" ) ),
  stableOnly_( p.getParameter<bool>( "stableOnly" ) ),
  excludeList_( p.getParameter<vstring>( "excludeList" ) ),
  verbose_( p.getUntrackedParameter<bool>( "verbose" ) ) {
  produces<CandidateCollection>();
}

HepMCCandidateProducer::~HepMCCandidateProducer() { 
}

void HepMCCandidateProducer::beginJob( const EventSetup & es ) {
  //  const PDTRecord & rec = es.get<PDTRecord>();
  ESHandle<DefaultConfig::ParticleDataTable> pdt;
  es.getData( pdt );
  
  if ( verbose_ && stableOnly_ )
    LogInfo ( "INFO" ) << "Excluding unstable particles";
  for( vstring::const_iterator e = excludeList_.begin(); 
       e != excludeList_.end(); ++ e ) {
    const DefaultConfig::ParticleData * p = pdt->particle( * e );
    if ( p == 0 ) 
      throw cms::Exception( "ConfigError", "can't find particle" )
	<< "can't find particle: " << * e;
    if ( verbose_ )
      LogInfo ( "INFO" ) << "Excluding particle " << *e << ", id: " << p->pid();
    excludedIds_.insert( abs( p->pid() ) );
  }
}

void HepMCCandidateProducer::produce( Event& evt, const EventSetup& ) {
  Handle<HepMCProduct> mcp;
  evt.getByLabel( src_, mcp );
  const HepMC::GenEvent * mc = mcp->GetEvent();
  if( mc == 0 ) 
    throw edm::Exception( edm::errors::InvalidReference ) 
      << "HepMC has null pointer to GenEvent" << endl;
  auto_ptr<CandidateCollection> cands( new CandidateCollection );
  cands->reserve( mc->particles_size() );
  for( HepMC::GenEvent::particle_const_iterator p = mc->particles_begin(); 
       p != mc->particles_end(); ++ p ) {
    if ( ! stableOnly_ || (*p)->status() == 1 ) {
      int id = abs( (*p)->pdg_id() );
      if ( excludedIds_.find( id ) == excludedIds_.end() ) {
	if ( verbose_ )
	  LogInfo( "INFO" ) << "Adding candidate for particle with id: " 
			    << (*p)->pdg_id() << ", status: " << (*p)->status();
	cands->push_back( new HepMCCandidate( * p ) );
      }
    }
  }

  evt.put( cands );
}

