{

#define chowder
//#define gumbo
//#define stew
//#define topDiLepton2Electron
#define topDiLeptonMuonX
//#define topSemiLepElectron
//#define topSemiLepMuon
//#define topFullyHadronic

  const Int_t nrFiles = 18;
  TString fileNameBase = "CSA07EffAnalyser_";

#ifdef topDiLepton2Electron  // triggers for skims: topDiLepton2Electron
  const Int_t nrTrigs = 2;
  Int_t trigs[nrTrigs] = { 33, 35 };
#endif
#ifdef topDiLeptonMuonX      // triggers for skims: topDiLeptonMuonX
  const Int_t nrTrigs = 3;
  Int_t trigs[nrTrigs] = { 47, 49, 83 };
#endif
#ifdef topSemiLepElectron    // triggers for skims: topSemiLepElectron
  const Int_t nrTrigs = 2;
  Int_t trigs[nrTrigs] = { 32, 33 };
#endif
#ifdef topSemiLepMuon        // triggers for skims: topSemiLepMuon
  const Int_t nrTrigs = 1;
  Int_t trigs[nrTrigs] = { 47 };
#endif
#ifdef topFullyHadronic      // triggers for skims: topFullyHadronic
  const Int_t nrTrigs = 4;
  Int_t trigs[nrTrigs] = { 60, 61, 62, 63 };
#endif

#ifdef chowder
  const Int_t nrProcs = 27;
  const string procName[nrProcs] = {
    "W0j            :", "W1j 0<pTW<100  :", "W1j 100<pTW<300:", "W2j 0<pTW<100  :", "W2j 100<pTW<300:", "W3j 0<pTW<100  :", "W3j 100<pTW<300:", "W4j 0<pTW<100  :", "W4j 100<pTW<300:", "W5j 0<pTW<100  :", "W5j 100<pTW<300:",
    "Z0j            :", "Z1j 0<pTZ<100  :", "Z1j 100<pTZ<300:", "Z2j 0<pTZ<100  :", "Z2j 100<pTZ<300:", "Z+3j 0<pTZ<100 :", "Z3j 100<pTZ<300:", "Z4j 0<pTZ<100  :", "Z4j 100<pTZ<300:", "Z5j 0<pTZ<100  :", "Z5j 100<pTZ<300:",
    "tt+0j          :", "tt+1j          :", "tt+2j          :", "tt+3j          :", "tt+4j          :"
  };
  const Int_t nrEv[nrProcs] = {
    8796412, 9088026, 247013, 2380315, 287472, 352855, 117608, 125849, 39719, 62238, 43865,
    3251851, 944726,  36135,  289278,  35285,  73182,  24316,  33083,  6616,  12136, 5966,
    1456646, 361835, 81215, 14036, 5352
  };
#endif
#ifdef gumbo
  const Int_t nrProcs = 33;
  const string procName[nrProcs] = {
    "Minimum Bias   :",
    "QCD 15-20      :", "QCD 20-30      :", "QCD 30-50      :", "QCD 50-80      :", "QCD 80-120     :", "QCD 120-170    :", "QCD 170-230    :", "QCD 230-300    :", "QCD 300-380    :", "QCD 380-470    :", "QCD 470-600    :", "QCD 600-800    :", "QCD 800-1000   :", "QCD 1000-1400  :", "QCD 1400-1800  :", "QCD 1800-2200  :", "QCD 2200-2600  :", "QCD 2600-3000  :", "QCD 3000-3500  :", "QCD 3500-inf   :",
    "PhJets 0-15    :", "PhJets 15-20   :", "PhJets 20-30   :", "PhJets 30-50   :", "PhJets 50-80   :", "PhJets 80-120  :", "PhJets 120-170 :", "PhJets 170-300 :", "PhJets 300-500 :", "PhJets 500-7000:",
    "Higgs signal   :", "Zprime signal  :"
  };
  const Int_t nrEv[nrProcs] = {
    23683812,
    1300976, 2511934, 2441441, 2456689, 1178357, 1263201, 1160565, 1202951, 1183344, 1189350, 1229378, 727670, 762858+351585, 763147, 723170, 772233, 753720, 751840, 705554, 749522,
    308246, 520375, 604780, 510094, 517483, 532482, 555727, 535983, 501978, 483084,
    45200, 12300
  };
#endif
#ifdef stew // not done yet
  const Int_t nrProcs = 11;
  const string procName[nrProcs] = {
    "B(bar)toJpsi   :", "QCD 0-15       :",
    "B'onium 0-20   :", "B'onium 20-inf :", "C'onium 0-20   :", "C'onium 20-inf :",
    "bbe 5-50       :", "bbe 50-170     :", "bbe 170-inf    :",
    "ppEleX         :", "ppMuX          :"
  };
  const Int_t nrEv[nrProcs] = {
    509527+556324, 733104,
    972597, 1107068, 1021134, 1012650,
    2907738, 2910966, 2599997,
    8682737, 20697806
  };
#endif

  struct CSA07Info {
    Int_t procId;
    Float_t ptHat;
    Float_t filterEff;
    Float_t weight;
    Int_t trigBits[90];
  } csa07Info;

  TFile * outfile = new TFile("TriggerPlots.root", "RECREATE");
  TH1I * trigProcs[nrTrigs+1];
  for (Int_t i = 0; i < nrTrigs+1; ++i) {
    TString tmp = "trigProc"; tmp += i;
    trigProcs[i] = new TH1I(tmp, "Trigger process", nrProcs, 0, nrProcs);
  }

  Int_t nrEvents = 0;
  Int_t nrFilesRead = 0;

  for (Int_t nr = 1; nr <= nrFiles; nr++) {

    TString fileName = fileNameBase; fileName += nr; fileName += ".root";
    TFile * file = new TFile(fileName);
    if (!file || file->IsZombie()) continue;
    TTree * csa07T = (TTree *) file->FindObjectAny("CSA07T");
    TBranch * csa07B = csa07T->GetBranch("CSA07B");
    csa07B->SetAddress(&csa07Info);

    cout << "Reading in " << csa07B->GetEntries() << " events" << endl;
    nrFilesRead++;
    nrEvents += csa07B->GetEntries();
    for (Int_t i = 0; i < csa07B->GetEntries(); ++i) {
      csa07B->GetEntry(i);
      Int_t j;
#ifdef chowder
      if (csa07Info.procId == 1000)
        j = 0;   // W+0jet             (weight ~ 5.15)
      else if (csa07Info.procId == 1001 && csa07Info.weight < 1.03)
        j = 1;   // W+1jet 0<pTW<100   (weight ~ 1.02)
      else if (csa07Info.procId == 1001 && csa07Info.weight > 1.03)
        j = 2;   // W+1jet 100<pTW<300 (weight ~ 1.04)
      else if (csa07Info.procId == 1002 && csa07Info.weight > 1.)
        j = 3;   // W+2jet 0<pTW<100   (weight ~ 1.07)
      else if (csa07Info.procId == 1002 && csa07Info.weight < 1.)
        j = 4;   // W+2jet 100<pTW<300 (weight ~ 0.78)
      else if (csa07Info.procId == 1003 && csa07Info.weight > 1.)
        j = 5;   // W+3jet 0<pTW<100   (weight ~ 1.67)
      else if (csa07Info.procId == 1003 && csa07Info.weight < 1.)
        j = 6;   // W+3jet 100<pTW<300 (weight ~ 0.91)
      else if (csa07Info.procId == 1004 && csa07Info.weight > 0.96)
        j = 7;   // W+4jet 0<pTW<100   (weight ~ 0.98)
      else if (csa07Info.procId == 1004 && csa07Info.weight < 0.96)
        j = 8;   // W+4jet 100<pTW<300 (weight ~ 0.95)
      else if (csa07Info.procId == 1005 && csa07Info.weight > 1.)
        j = 9;   // W+5jet 0<pTW<100   (weight ~ 1.35)
      else if (csa07Info.procId == 1005 && csa07Info.weight < 1.)
        j = 10;  // W+5jet 100<pTW<300 (weight ~ 0.90)
      else if (csa07Info.procId == 2000)
        j = 11;   // Z+0jet             (weight ~ 1.38)
      else if (csa07Info.procId == 2001 && csa07Info.weight > 0.9)
        j = 12;   // Z+1jet 0<pTZ<100   (weight ~ 0.98)
      else if (csa07Info.procId == 2001 && csa07Info.weight < 0.9)
        j = 13;   // Z+1jet 100<pTZ<300 (weight ~ 0.83)
      else if (csa07Info.procId == 2002 && csa07Info.weight > 0.9)
        j = 14;   // Z+2jet 0<pTZ<100   (weight ~ 0.93)
      else if (csa07Info.procId == 2002 && csa07Info.weight < 0.9)
        j = 15;   // Z+2jet 100<pTZ<300 (weight ~ 0.80)
      else if (csa07Info.procId == 2003 && csa07Info.weight > 0.9)
        j = 16;   // Z+3jet 0<pTZ<100   (weight ~ 0.94)
      else if (csa07Info.procId == 2003 && csa07Info.weight < 0.9)
        j = 17;   // Z+3jet 100<pTZ<300 (weight ~ 0.53)
      else if (csa07Info.procId == 2004 && csa07Info.weight < 0.5)
        j = 18;   // Z+4jet 0<pTZ<100   (weight ~ 0.42)
      else if (csa07Info.procId == 2004 && csa07Info.weight > 0.5)
        j = 19;   // Z+4jet 100<pTZ<300 (weight ~ 0.63)
      else if (csa07Info.procId == 2005 && csa07Info.weight < 0.8)
        j = 20;   // Z+5jet 0<pTZ<100   (weight ~ 0.72)
      else if (csa07Info.procId == 2005 && csa07Info.weight > 0.8)
        j = 21;  // Z+5jet 100<pTZ<300 (weight ~ 0.85)
      else if (csa07Info.procId == 3000)
        j = 22;  // tt+0jet
      else if (csa07Info.procId == 3001)
        j = 23;  // tt+1jet
      else if (csa07Info.procId == 3002)
        j = 24;  // tt+2jet
      else if (csa07Info.procId == 3003)
        j = 25;  // tt+3jet
      else if (csa07Info.procId == 3004)
        j = 26;  // tt+4jet
      else cout << "OUCH! Unknown process with ID" << csa07Info.procId << ". Sure this is a chowder sample?" << endl;
#endif
#ifdef gumbo
      if (csa07Info.procId == 92 || csa07Info.procId == 93 || csa07Info.procId == 94 || csa07Info.procId == 95)
        j = 0;	  // min bias
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (15 < csa07Info.ptHat && csa07Info.ptHat < 20))
        j = 1;   // QCD_Pt_15_20
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (20 < csa07Info.ptHat && csa07Info.ptHat < 30))
        j = 2;   // QCD_Pt_20_30
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (30 < csa07Info.ptHat && csa07Info.ptHat < 50))
        j = 3;   // QCD_Pt_30_50
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (50 < csa07Info.ptHat && csa07Info.ptHat < 80))
        j = 4;   // QCD_Pt_50_80
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (80 < csa07Info.ptHat && csa07Info.ptHat < 120))
        j = 5;   // QCD_Pt_80_120
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (120 < csa07Info.ptHat && csa07Info.ptHat < 170))
        j = 6;   // QCD_Pt_120_170
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (170 < csa07Info.ptHat && csa07Info.ptHat < 230))
        j = 7;   // QCD_Pt_170_230
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (230 < csa07Info.ptHat && csa07Info.ptHat < 300))
        j = 8;   // QCD_Pt_230_300
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (300 < csa07Info.ptHat && csa07Info.ptHat < 380))
        j = 9;   // QCD_Pt_300_380
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (380 < csa07Info.ptHat && csa07Info.ptHat < 470))
        j = 10;   // QCD_Pt_380_470
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (470 < csa07Info.ptHat && csa07Info.ptHat < 600))
        j = 11;   // QCD_Pt_470_600
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (600 < csa07Info.ptHat && csa07Info.ptHat < 800))
        j = 12;   // QCD_Pt_600_800
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (800 < csa07Info.ptHat && csa07Info.ptHat < 1000))
        j = 13;   // QCD_Pt_800_1000
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (1000 < csa07Info.ptHat && csa07Info.ptHat < 1400))
        j = 14;   // QCD_Pt_1000_1400
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (1400 < csa07Info.ptHat && csa07Info.ptHat < 1800))
        j = 15;   // QCD_Pt_1400_1800
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (1800 < csa07Info.ptHat && csa07Info.ptHat < 2200))
        j = 16;   // QCD_Pt_1800_2200
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (2200 < csa07Info.ptHat && csa07Info.ptHat < 2600))
        j = 17;   // QCD_Pt_2200_2600
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (2600 < csa07Info.ptHat && csa07Info.ptHat < 3000))
        j = 18;   // QCD_Pt_2600_3000
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (3000 < csa07Info.ptHat && csa07Info.ptHat < 3500))
        j = 19;   // QCD_Pt_3000_3500
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (3500 < csa07Info.ptHat))
        j = 20;   // QCD_Pt_3500_inf
      else if ((csa07Info.procId == 14 || csa07Info.procId == 18 || csa07Info.procId == 29) &&
               (csa07Info.ptHat < 15))
        j = 21;   // QCD_Pt_0_15
      else if ((csa07Info.procId == 14 || csa07Info.procId == 18 || csa07Info.procId == 29) &&
               (15 < csa07Info.ptHat && csa07Info.ptHat < 20))
        j = 22;   // QCD_Pt_15_20
      else if ((csa07Info.procId == 14 || csa07Info.procId == 18 || csa07Info.procId == 29) &&
               (20 < csa07Info.ptHat && csa07Info.ptHat < 30))
        j = 23;   // QCD_Pt_20_30
      else if ((csa07Info.procId == 14 || csa07Info.procId == 18 || csa07Info.procId == 29) &&
               (30 < csa07Info.ptHat && csa07Info.ptHat < 50))
        j = 24;   // QCD_Pt_30_50
      else if ((csa07Info.procId == 14 || csa07Info.procId == 18 || csa07Info.procId == 29) &&
               (50 < csa07Info.ptHat && csa07Info.ptHat < 80))
        j = 25;   // QCD_Pt_50_80
      else if ((csa07Info.procId == 14 || csa07Info.procId == 18 || csa07Info.procId == 29) &&
               (80 < csa07Info.ptHat && csa07Info.ptHat < 120))
        j = 26;   // QCD_Pt_80_120
      else if ((csa07Info.procId == 14 || csa07Info.procId == 18 || csa07Info.procId == 29) &&
               (120 < csa07Info.ptHat && csa07Info.ptHat < 170))
        j = 27;   // QCD_Pt_120_170
      else if ((csa07Info.procId == 14 || csa07Info.procId == 18 || csa07Info.procId == 29) &&
               (170 < csa07Info.ptHat && csa07Info.ptHat < 300))
        j = 28;   // QCD_Pt_170_300
      else if ((csa07Info.procId == 14 || csa07Info.procId == 18 || csa07Info.procId == 29) &&
               (300 < csa07Info.ptHat && csa07Info.ptHat < 500))
        j = 29;   // QCD_Pt_300_500
      else if ((csa07Info.procId == 14 || csa07Info.procId == 18 || csa07Info.procId == 29) &&
               (500 < csa07Info.ptHat && csa07Info.ptHat < 7000))
        j = 30;   // QCD_Pt_500_7000
      else if (csa07Info.procId == 102 || csa07Info.procId == 123 || csa07Info.procId == 124)
        j = 31;   // higgs signal
      else if (csa07Info.procId == 141)
        j = 32;   // Zprime signal
      else cout << "OUCH! Unknown process with ID" << csa07Info.procId << ". Sure this is a gumbo sample?" << endl;
#endif
#ifdef stew
      if (csa07Info.filterEff == (Float_t) 0.00013)
        j = 0;    // B(bar)->JPsi
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68) &&
               (csa07Info.filterEff == (Float_t) 1. || csa07Info.filterEff == (Float_t) 0.964))
        j = 1;	  // QCD_Pt_0_15
      else if ((460 < csa07Info.procId && csa07Info.procId < 480) &&
               (0 < csa07Info.ptHat && csa07Info.ptHat < 20))
        j = 2;    // Bottomonium Pt_0_20
      else if ((460 < csa07Info.procId && csa07Info.procId < 480) &&
               (20 < csa07Info.ptHat))
        j = 3;    // Bottomonium Pt_20_inf
      else if ((420 < csa07Info.procId && csa07Info.procId < 440) &&
               (0 < csa07Info.ptHat && csa07Info.ptHat < 20))
        j = 4;    // Charmonium Pt_0_20
      else if ((420 < csa07Info.procId && csa07Info.procId < 440) &&
               (20 < csa07Info.ptHat))
        j = 5;    // Charmonium Pt_20_inf
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68 || csa07Info.procId == 95) &&
               (csa07Info.filterEff == (Float_t) 0.00019))
        j = 6;	  // bbe Pt_5_50
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68 || csa07Info.procId == 95) &&
               (csa07Info.filterEff == (Float_t) 0.0068))
        j = 7;	  // bbe Pt_50_170
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68 || csa07Info.procId == 95) &&
               (csa07Info.filterEff == (Float_t) 0.0195))
        j = 8;	  // bbe Pt_170_up
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68 || csa07Info.procId == 95) &&
               (csa07Info.filterEff == (Float_t) 0.0097))
        j = 9;	  // ppEleX
      else if ((csa07Info.procId == 11 || csa07Info.procId == 12 || csa07Info.procId == 13 || csa07Info.procId == 28 || csa07Info.procId == 53 || csa07Info.procId == 68 || csa07Info.procId == 95) &&
               (csa07Info.filterEff == (Float_t) 0.0008))
        j = 10;	  // ppMuX
      else cout << "OUCH! Unknown process with ID " << csa07Info.procId << ". Sure this is a stew sample?" << endl;
#endif

      Bool_t orTrig = false;
      for (Int_t k = 0; k < nrTrigs; ++k) {
	if (csa07Info.trigBits[trigs[k]]) {
	   trigProcs[k]->Fill(j);
	   orTrig = true;
        }
      }
      if (orTrig) trigProcs[nrTrigs]->Fill(j);
    }

    file->Close();
    delete file;

  }

  cout << "Nr of files    : " << nrFilesRead << "/" << nrFiles << endl;
  cout << "Nr of events   : " << nrEvents << endl;
  cout << "Trigger bit    :          ";
  for (Int_t j = 0; j < nrTrigs; ++j) {
    printf("       [ %2d ]  ", trigs[j]);
  }
  cout << "       [.OR.]" << endl;
  Int_t nrEvTot = 0;
  for (Int_t i = 0; i < nrProcs; ++i) {
    nrEvTot += nrEv[i];
    printf(procName[i].c_str());
    printf("  %8d", nrEv[i]);
    for (Int_t j = 0; j <= nrTrigs; ++j) {
      printf("  %6d", trigProcs[j]->GetBinContent(i+1));
      printf("  %1.3f", 1./nrEv[i]*trigProcs[j]->GetBinContent(i+1));
    }
    cout << endl;
  }
  cout << "Total          :";
  printf("  %8d", nrEvTot);
  for (Int_t j = 0; j <= nrTrigs; ++j) {
    printf("  %6d", trigProcs[j]->Integral());
    printf("  %1.3f", 1./nrEvTot*trigProcs[j]->Integral());
  }
  cout << endl;

  outfile->Write();
  outfile->Close();  

}


/* Full list of triggers

Tr.Bit  Name

    0	HLT1jet
    1	HLT2jet
    2	HLT3jet
    3	HLT4jet
    4	HLT1MET
    5	HLT2jetAco
    6	HLT1jet1METAco
    7	HLT1jet1MET
    8	HLT2jet1MET
    9	HLT3jet1MET
   10	HLT4jet1MET
   11	HLT1MET1HT
   12	CandHLT1SumET
   13	HLT1jetPE1
   14	HLT1jetPE3
   15	HLT1jetPE5
   16	CandHLT1jetPE7
   17	CandHLT1METPre1
   18	CandHLT1METPre2
   19	CandHLT1METPre3
   20	CandHLT2jetAve30
   21	CandHLT2jetAve60
   22	CandHLT2jetAve110
   23	CandHLT2jetAve150
   24	CandHLT2jetAve200
   25	HLT2jetvbfMET
   26	HLTS2jet1METNV
   27	HLTS2jet1METAco
   28	CandHLTSjet1MET1Aco
   29	CandHLTSjet2MET1Aco
   30	CandHLTS2jetAco
   31	CandHLTJetMETRapidityGap
   32	HLT1Electron
   33	HLT1ElectronRelaxed
   34	HLT2Electron
   35	HLT2ElectronRelaxed
   36	HLT1Photon
   37	HLT1PhotonRelaxed
   38	HLT2Photon
   39	HLT2PhotonRelaxed
   40	HLT1EMHighEt
   41	HLT1EMVeryHighEt
   42	CandHLT2ElectronZCounter
   43	CandHLT2ElectronExclusive
   44	CandHLT2PhotonExclusive
   45	CandHLT1PhotonL1Isolated
   46	HLT1MuonIso
   47	HLT1MuonNonIso
   48	CandHLT2MuonIso
   49	HLT2MuonNonIso
   50	HLT2MuonJPsi
   51	HLT2MuonUpsilon
   52	HLT2MuonZ
   53	HLTNMuonNonIso
   54	HLT2MuonSameSign
   55	CandHLT1MuonPrescalePt3
   56	CandHLT1MuonPrescalePt5
   57	CandHLT1MuonPrescalePt7x7
   58	CandHLT1MuonPrescalePt7x10
   59	CandHLT1MuonLevel1
   60	HLTB1Jet
   61	HLTB2Jet
   62	HLTB3Jet
   63	HLTB4Jet
   64	HLTBHT
   65	HLTB1JetMu
   66	HLTB2JetMu
   67	HLTB3JetMu
   68	HLTB4JetMu
   69	HLTBHTMu
   70	HLTBJPsiMuMu
   71	HLT1Tau
   72	HLT1Tau1MET
   73	HLT2TauPixel
   74	HLTXElectronBJet
   75	HLTXMuonBJet
   76	HLTXMuonBJetSoftMuon
   77	HLTXElectron1Jet
   78	HLTXElectron2Jet
   79	HLTXElectron3Jet
   80	HLTXElectron4Jet
   81	HLTXMuonJets
   82	HLTXElectronMuon
   83	HLTXElectronMuonRelaxed
   84	HLTXElectronTau
   85	HLTXMuonTau
   86	CandHLTHcalIsolatedTrack
   87	HLTMinBiasPixel
   88	HLTMinBias
   89	HLTZeroBias

*/
