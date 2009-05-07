// This file was removed but it should not have been.
// This comment is to restore it. 

#include "PhysicsTools/HepMCCandAlgos/interface/FlavorHistoryFilter.h"
#include "PhysicsTools/HepMCCandAlgos/interface/FlavorHistoryProducer.h"

#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Math/interface/deltaR.h"

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
  src_           ( iConfig.getParameter<edm::InputTag>("src" ) ),
  schemeName_    ( iConfig.getParameter<string> ("scheme") ),
  flavor_        ( iConfig.getParameter<int>    ("flavor") ),
  noutput_       ( iConfig.getParameter<int>    ("noutput") ),
  flavorSource_  ( iConfig.getParameter<std::vector<int> >    ("flavorSource") ),
  minPt_         ( iConfig.getParameter<double> ("minPt") ),
  minDR_         ( iConfig.getParameter<double> ("minDR") ),
  maxDR_         ( iConfig.getParameter<double> ("maxDR") ),
  verbose_       ( iConfig.getParameter<bool>   ("verbose") )
{
  if ( schemeName_ != "deltaR" ) {
    throw cms::Exception("FatalError") << "Incorrect scheme for flavor history filter\n"
				       << "Curent available options are: \n" 
				       << "        deltaR\n";
  }
  
  // Deal with the case if minDR == maxDR, just increment maxDR by epsilon
  if ( minDR_ == maxDR_ ) maxDR_ += 0.001;
}


FlavorHistoryFilter::~FlavorHistoryFilter()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//---------------------------------------------------------------------------
//   FlavorHistoryFilter
//   This will filter events as follows:
//   - Inputs vector<FlavorHistory> from FlavorHistoryProducer
//   - Inputs GenJetCollection
//   - If there are no FlavorHistory's that have flavorSource of "type",
//     then the event is rejected.
//   - If there is at least one FlavorHistory that has flavorSource of "type",
//     then we examine the kinematic criteria:
//        - For delta R method, if there is a sister of the parton
//          that is within "minDR" of "this" parton, the event is rejected,
//          otherwise it is passed.
//        - For the pt method, if the parton itself is less than a pt
//          threshold, it is rejected, and if it is above, it is passed
//---------------------------------------------------------------------------
bool
FlavorHistoryFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  if ( verbose_ ) cout << "---------- Hello from FlavorHistoryFilter! -----" << endl;

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


  // Now get types present
  FlavorHistoryEvent::const_iterator iBegin = pFlavorHistoryEvent->begin(),
    iEnd = pFlavorHistoryEvent->end(), i = iBegin;

  if ( verbose_ ) {
    cout << "Looking at flavor history event: " << endl;
    cout << "source   = " << flavorSource << endl;
    cout << "npartons = " << pFlavorHistoryEvent->size() << endl;
    cout << "nbjet    = " << nb << endl;
    cout << "ncjet    = " << nc << endl;
    cout << "flavor   = " << highestFlavor << endl;
    cout << "dr       = " << dr << endl;
  }

  // First check that the highest flavor in the event is what this
  // filter is checking. Otherwise we need to fail the event,
  // since it should be handled by another filter
  if ( highestFlavor > static_cast<unsigned int>(flavor_) ) {
    if ( verbose_ ) cout << "Rejecting event, highest flavor is " << highestFlavor << endl;
    return false;
  }

  // Next check that the flavor source is one of the desired ones
  vector<int>::const_iterator iflavorSource = find( flavorSource_.begin(), flavorSource_.end(), static_cast<int>(flavorSource) );
  if ( iflavorSource == flavorSource_.end() ) {
    if ( verbose_ ) cout << "Rejecting event, didn't find flavor source " << static_cast<int>(flavorSource) << endl;
    return false;
  }
  
  // If we are examining b quarks
  if ( flavor_ == reco::FlavorHistory::bQuarkId ) {
    // if we have no b quarks, return false
    if ( nb <= 0 ) {
      if ( verbose_ ) cout << "Rejecting event, nb = " << nb << endl;
      return false;
    }
    // here, nb > 0
    else {
      // if we want 1 b, require nb == 1
      if ( noutput_ == 1 && nb == 1 ) {
	if ( verbose_ ) cout << "Accepting event" << endl;
	return true;
      }
      // if we want 2 b, then look at delta R
      else if ( noutput_ > 1 && nb > 1 ) {
	// If dr is within the range we want, pass.
	// Otherwise, fail.
	if ( verbose_ ) cout << "Want multiples, dr = " << dr << endl;
	return ( dr >= minDR_ && dr < maxDR_ );
      }
      // otherwise return false
      else {
	if ( verbose_ ) cout << "Rejecting event, isn't output = 1 + nb = 1, or output > 0 and delta R in proper range" << endl;
	return false;
      }
    }// end if nb > 0
    
  } // end if flavor is b quark

  // If we are examining c quarks
  else if ( flavor_ == reco::FlavorHistory::cQuarkId ) {
    // make sure there are no b quarks in the event.
    // If there are, another filter should handle it.
    if ( nb > 0 ) return false;
    
    // if we have no c quarks, return false
    if ( nc <= 0 ) return false;
    // here, nc > 0
    else {
      // if we want 1 c, require nc == 1
      if ( noutput_ == 1 && nc == 1 ) {
	return true;
      }
      // if we want 2 c, then look at delta R
      else if ( noutput_ > 1 && nc > 1 ) {
	// If dr is within the range we want, pass.
	// Otherwise, fail.
	return ( dr >= minDR_ && dr < maxDR_ );
      }
      // otherwise return false
      else {
	return false;
      }
    }// end if nc > 0
    
  }
  // Otherwise return false
  else {
    if ( verbose_ ) cout << "Something is weird, flavor is " << flavor_ << endl;
    return false;
  }

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
