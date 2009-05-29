// This file was removed but it should not have been.
// This comment is to restore it. 

#include "PhysicsTools/HepMCCandAlgos/interface/FlavorHistoryFilter.h"
#include "PhysicsTools/HepMCCandAlgos/interface/FlavorHistoryProducer.h"

#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "PhysicsTools/Utilities/interface/deltaR.h"

#include "PhysicsTools/HepMCCandAlgos/interface/FlavorHistorySelectorUtil.h"

#include <vector>

using namespace edm;
using namespace reco;
using namespace std;


//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
FlavorHistoryFilter::FlavorHistoryFilter(const edm::ParameterSet& iConfig) :
  src_           ( iConfig.getParameter<edm::InputTag>("src" ) )

{

  std::string schemeName( iConfig.getParameter<string> ("scheme") );
  unsigned int flavor        ( iConfig.getParameter<unsigned int>    ("flavor") );
  unsigned int noutput       ( iConfig.getParameter<unsigned int>    ("noutput") );
  std::vector<int> flavorSource  ( iConfig.getParameter<std::vector<int> >    ("flavorSource") );
  double minDR         ( iConfig.getParameter<double> ("minDR") );
  double maxDR         ( iConfig.getParameter<double> ("maxDR") );
  bool verbose         ( iConfig.getParameter<bool>   ("verbose") );

  if ( schemeName != "deltaR" ) {
    throw cms::Exception("FatalError") << "Incorrect scheme for flavor history filter\n"
				       << "Curent available options are: \n" 
				       << "        deltaR\n";
  }

  flavorHistorySelectorUtil_ = new FlavorHistorySelectorUtil( flavor,
							      noutput,
							      flavorSource,
							      minDR,
							      maxDR,
							      verbose );
}


FlavorHistoryFilter::~FlavorHistoryFilter()
{
 
  if ( flavorHistorySelectorUtil_ ) delete flavorHistorySelectorUtil_; 
 
}



bool
FlavorHistoryFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  // Get the flavor history
  Handle<FlavorHistoryEvent > pFlavorHistoryEvent;
  iEvent.getByLabel(src_,pFlavorHistoryEvent);

  // Get the number of matched b-jets in the event
  unsigned int nb = pFlavorHistoryEvent->nb();
  // Get the number of matched c-jets in the event
  unsigned int nc = pFlavorHistoryEvent->nc();
  // Get the highest flavor in the event
  unsigned int highestFlavor = pFlavorHistoryEvent->highestFlavor();
  // Get the flavor source
  FlavorHistory::FLAVOR_T flavorSource = pFlavorHistoryEvent->flavorSource();
  // Get the maximum delta R between two jets of the highest flavor
  // in the event
  double dr = pFlavorHistoryEvent->deltaR();

  return flavorHistorySelectorUtil_->select( nb, nc, highestFlavor, flavorSource, dr );


}

// ------------ method called once each job just before starting event loop  ------------
void 
FlavorHistoryFilter::beginJob(const edm::EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
FlavorHistoryFilter::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(FlavorHistoryFilter);
