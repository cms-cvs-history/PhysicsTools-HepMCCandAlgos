#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Event.h"
#include <TH1.h>
#include <TFile.h>
#include <TDCacheFile.h>
#include <TSystem.h>

#include "FWCore/FWLite/interface/AutoLibraryLoader.h"



#if !defined(__CINT__) && !defined(__MAKECINT__)
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/FlavorHistory.h"
#include "DataFormats/HepMCCandidate/interface/FlavorHistoryEvent.h"
#include "PhysicsTools/HepMCCandAlgos/interface/FlavorHistorySelectorUtil.h"
#endif

#include <iostream>
#include <map>
#include <string>

using namespace std;

int main(int argc, char ** argv)
{

  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

   
  vector<string> files;

  files.push_back("../test/testFlavorHistoryProducer.root");

  using namespace std;
  using namespace reco;

  vector<int> flavorVector;
  flavorVector.push_back( FlavorHistory::FLAVOR_EXC );
  flavorVector.push_back( FlavorHistory::FLAVOR_ME );

  FlavorHistorySelectorUtil * selector = 
    new FlavorHistorySelectorUtil( reco::FlavorHistory::cQuarkId,
				   1,
				   flavorVector,
				   0.0, 
				   0.0,
				   true 
				   );

  
  fwlite::ChainEvent ev(files);

  int npass = 0;
  int nfail = 0;

  for( ev.toBegin();
       ! ev.atEnd();
       ++ev) {

    fwlite::Handle<reco::FlavorHistoryEvent > h_c_history;
    h_c_history   .getByLabel(ev,"cFlavorHistoryProducer", "cPartonFlavorHistory");

    if (h_c_history.isValid() ) {

      // Get the number of matched b-jets in the event
      unsigned int nb = h_c_history->nb();
      // Get the number of matched c-jets in the event
      unsigned int nc = h_c_history->nc();
      // Get the highest flavor in the event
      unsigned int highestFlavor = h_c_history->highestFlavor();
      // Get the flavor source
      FlavorHistory::FLAVOR_T flavorSource = h_c_history->flavorSource();
      // Get the maximum delta R between two jets of the highest flavor
      // in the event
      double dr = h_c_history->deltaR();
      
      if ( selector->select( nb, nc, highestFlavor, flavorSource, dr ) ) {
	npass++;
      } else {
	nfail++;
      }
      
    }
    
  }

  cout << "npass = " << npass << endl;
  cout << "nfail = " << nfail << endl;

}
