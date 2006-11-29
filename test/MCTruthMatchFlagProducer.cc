#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/InputTag.h"
#include <vector>

class MCTruthMatchFlagProducer : public edm::EDProducer {
public:
  MCTruthMatchFlagProducer( const edm::ParameterSet & );
  ~MCTruthMatchFlagProducer();

private:
  void produce( edm::Event & , const edm::EventSetup & );
  edm::InputTag src_, matchMap_;
  std::vector<int> pdgId_;
};

#include "FWCore/Framework/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/ShallowCloneCandidate.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleCandidate.h"
#include "DataFormats/Candidate/interface/CandMatchMap.h"
#include <iostream>
#include <algorithm>
using namespace edm;
using namespace reco;
using namespace std;

MCTruthMatchFlagProducer::MCTruthMatchFlagProducer( const ParameterSet & cfg ) :
  src_( cfg.getParameter<InputTag>( "src" ) ),
  matchMap_( cfg.getParameter<InputTag>( "matchMap" ) ),
  pdgId_( cfg.getParameter<vector<int> >( "pdgId" ) ) {
  produces<vector<int> >();
  produces<CandidateCollection>();
  for( vector<int>::iterator i = pdgId_.begin(); i != pdgId_.end(); ++i ) {
    * i = abs( * i );
  }
}

MCTruthMatchFlagProducer::~MCTruthMatchFlagProducer() {
}

void MCTruthMatchFlagProducer::produce( Event & evt, const EventSetup & ) {
  Handle<CandidateCollection> cands;
  evt.getByLabel( src_, cands );
  Handle<CandMatchMap> matchMap;
  evt.getByLabel( matchMap_, matchMap );
  
  auto_ptr<vector<int> > flags( new vector<int>( cands->size(), 0 ) );
  auto_ptr<CandidateCollection> selected( new CandidateCollection );

  
  for( size_t c = 0; c != cands->size(); ++ c ) {
    CandidateRef cref( cands, c );
    CandMatchMap::const_iterator f = matchMap->find( cref );
    if( f != matchMap->end() ) {
      int id = abs( pdgId( * ( f->val ) ) );
      cout << c << ") found match with id: " << id
	   << "; pt, eta, phi = " << cref->pt() << ", " << cref->eta() << ", " << cref->phi() 
	   << " mass = " << cref->mass();
      if ( find( pdgId_.begin(), pdgId_.end(), id ) != pdgId_.end() ) {
	(*flags)[ c ] = 1;
	selected->push_back( new ShallowCloneCandidate( CandidateBaseRef( cref ) ) );
	cout << " [*]";
      }
      cout << endl;
    }
  }

  evt.put( flags );
  evt.put( selected );
}

#include "PluginManager/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_SEAL_MODULE();
DEFINE_ANOTHER_FWK_MODULE( MCTruthMatchFlagProducer );


