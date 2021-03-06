#include "../interface/CodexAnalyzer.h"
#include "../interface/WeightCalculator.h"
#include "../interface/Corrector.h"
#include "../interface/Functions.h"
#include "../interface/makeHisto.h"
#include <string>
#include <ostream>
#include <vector>


int main(int argc, char** argv) {
    using namespace std;
    
    std::string out = *(argv + 1);
    
    cout << "\n\n\n OUTPUT NAME IS:    " << out << endl;     //PRINTING THE OUTPUT name
    TFile *fout = TFile::Open(out.c_str(), "RECREATE");
    
    myMap1 = new std::map<std::string, TH1F*>();
    myMap2 = new map<string, TH2F*>();
    
    std::vector<string> input;
    for (int f = 2; f < argc; f++) {
        input.push_back(*(argv + f));
        cout << "\n INPUT NAME IS:   " << input[f - 2] << "\n";
    }
    
    
    //########################################
    // Pileup files
    //########################################
    TFile * PUData= TFile::Open("../interface/pileup-hists/dataMoriondPU.root");
    TH1F * HistoPUData= (TH1F *) PUData->Get("pileup");
    HistoPUData->Scale(1.0/HistoPUData->Integral());
    
    TFile * PUMC= TFile::Open("../interface/pileup-hists/mcMoriondPU.root");
    TH1F * HistoPUMC= (TH1F *) PUMC->Get("pileup");
    HistoPUMC->Scale(1.0/HistoPUMC->Integral());
    
    
    //########################################
    // Muon Id, Iso, Trigger and Tracker Eff files
    //########################################
    TFile * MuCorrId_BCDEF= TFile::Open(("../interface/pileup-hists/ID_EfficienciesAndSF_BCDEF.root"));
    TH2F * HistoMuId_BCDEF= (TH2F *) MuCorrId_BCDEF->Get("MC_NUM_TightID_DEN_genTracks_PAR_pt_eta/pt_abseta_ratio");
    
    TFile * MuCorrId_GH= TFile::Open(("../interface/pileup-hists/ID_EfficienciesAndSF_GH.root"));
    TH2F * HistoMuId_GH= (TH2F *) MuCorrId_GH->Get("MC_NUM_TightID_DEN_genTracks_PAR_pt_eta/pt_abseta_ratio");
    
    TFile * MuCorrIso_BCDEF= TFile::Open(("../interface/pileup-hists/Iso_EfficienciesAndSF_BCDEF.root"));
    TH2F * HistoMuIso_BCDEF= (TH2F *) MuCorrIso_BCDEF->Get("TightISO_TightID_pt_eta/pt_abseta_ratio");
    
    TFile * MuCorrIso_GH= TFile::Open(("../interface/pileup-hists/Iso_EfficienciesAndSF_GH.root"));
    TH2F * HistoMuIso_GH= (TH2F *) MuCorrIso_GH->Get("TightISO_TightID_pt_eta/pt_abseta_ratio");
    
    TFile * MuCorrTrg_BCDEF= TFile::Open(("../interface/pileup-hists/Trigger_EfficienciesAndSF_RunBtoF.root"));
    TH2F * HistoMuTrg_BCDEF= (TH2F *) MuCorrTrg_BCDEF->Get("Mu50_OR_TkMu50_PtEtaBins/pt_abseta_ratio");
    
    TFile * MuCorrTrg_GH= TFile::Open(("../interface/pileup-hists/Trigger_EfficienciesAndSF_Period4.root"));
    TH2F * HistoMuTrg_GH= (TH2F *) MuCorrTrg_GH->Get("Mu50_OR_TkMu50_PtEtaBins/pt_abseta_ratio");
    
    TFile * MuCorrTrack= TFile::Open(("../interface/pileup-hists/Tracking_EfficienciesAndSF_BCDEFGH.root"));
    TGraphAsymmErrors * HistoMuTrack= (TGraphAsymmErrors *) MuCorrTrack->Get("ratio_eff_eta3_dr030e030_corr");
    
    TH2F* HistoMuId[2]={HistoMuId_BCDEF, HistoMuId_GH};
    TH2F* HistoMuIso[2]={HistoMuIso_BCDEF,HistoMuIso_GH};
    TH2F* HistoMuTrg[2]={HistoMuTrg_BCDEF, HistoMuTrg_GH};
    
    
    //########################################
    // Electron MVA IdIso files
    //########################################
    TFile * EleCorrMVAIdIso90= TFile::Open(("../interface/pileup-hists/egammaEffi.txt_EGM2D.root"));
    TH2F * HistoEleMVAIdIso90= (TH2F *) EleCorrMVAIdIso90->Get("EGamma_SF2D");
    
    
    //########################################
    // W and DY K-factor files
    //########################################
    TFile * KFactor= TFile::Open("../interface/pileup-hists/kfactors.root");
    TH1F * WLO= (TH1F *) KFactor->Get("WJets_LO/inv_pt");
    TH1F * WNLO= (TH1F *) KFactor->Get("EWKcorr/W");
    TH1F * ZLO= (TH1F *) KFactor->Get("ZJets_LO/inv_pt");
    TH1F * ZNLO= (TH1F *) KFactor->Get("EWKcorr/Z");
    
    
    
    for (int k = 0; k < input.size(); k++) {
        
        //std::string input = *(argv + 2);
        TFile *f_Double = TFile::Open(input[k].c_str());
        cout << "\n  Now is running on ------->   " << std::string(f_Double->GetName()) << "\n";
        
        std::string InputROOT= std::string(f_Double->GetName());
        TFile * myFile = TFile::Open(f_Double->GetName());
        TH1F * HistoTot = (TH1F*) myFile->Get("hcount");
        
        //        TTree *Run_Tree = (TTree*) f_Double->Get("ggNtuplizer/EventTree");
        TTree *Run_Tree = (TTree*) f_Double->Get("EventTree");
        
        cout.setf(ios::fixed, ios::floatfield);
        cout.precision(6);
        
        
        //########################################
        // BTag efficiency  files  (one need first to run CodexAnalyzer_BtagEff.cc)
        //########################################
        TFile * TTEff= TFile::Open(("OutFiles_BTagSF/TTJets.root"));
        TH2F * TTSF0_btagged= (TH2F *) TTEff->Get("BSF_FLV0_Btagged");
        TH2F * TTSF0_total= (TH2F *) TTEff->Get("BSF_FLV0_Total");
        TH2F * TTSF5_btagged= (TH2F *) TTEff->Get("BSF_FLV5_Btagged");
        TH2F * TTSF5_total= (TH2F*) TTEff->Get("BSF_FLV5_Total");
        
        TH2F * Btagg_TT[4]={TTSF0_btagged,TTSF0_total,TTSF5_btagged,TTSF5_total};
        
        
        //        std::string ROOTLoc= "/Users/abdollah1/GIT_abdollah110/DM2016/ROOT80X/";
        //        vector<float> DY_Events = DY_HTBin(ROOTLoc);
        //        vector<float> W_Events = W_HTBin(ROOTLoc);
        //        vector<float> W_EventsNLO = W_PTBinNLO(ROOTLoc); //This is for the NLO samples (as the stat is too low we do not use them)
        //        vector<float> W_EventsNLO = W_HTBin(ROOTLoc);
        
        //########################################   General Info
        Run_Tree->SetBranchAddress("isData", &isData);
        Run_Tree->SetBranchAddress("run", &run);
        Run_Tree->SetBranchAddress("lumis", &lumis);
        Run_Tree->SetBranchAddress("event", &event);
        Run_Tree->SetBranchAddress("genWeight",&genWeight);
        Run_Tree->SetBranchAddress("HLTEleMuX", &HLTEleMuX);
        Run_Tree->SetBranchAddress("puTrue", &puTrue);
        Run_Tree->SetBranchAddress("nVtx",&nVtx);
        
        //########################################   MC Info
        Run_Tree->SetBranchAddress("nMC", &nMC);
        Run_Tree->SetBranchAddress("mcPID", &mcPID);
        Run_Tree->SetBranchAddress("mcStatus", &mcStatus);
        Run_Tree->SetBranchAddress("mcPt", &mcPt );
        Run_Tree->SetBranchAddress("mcEta", &mcEta );
        Run_Tree->SetBranchAddress("mcPhi", &mcPhi );
        Run_Tree->SetBranchAddress("mcE", &mcE );
        Run_Tree->SetBranchAddress("mcMass", &mcMass );
        Run_Tree->SetBranchAddress("mcMomPID", &mcMomPID );
        Run_Tree->SetBranchAddress("mcGMomPID", &mcGMomPID );
        
        
        //########################################   Tau Info
        Run_Tree->SetBranchAddress("nTau", &nTau);
        Run_Tree->SetBranchAddress("tauPt"  ,&tauPt);
        Run_Tree->SetBranchAddress("tauEta"  ,&tauEta);
        Run_Tree->SetBranchAddress("tauPhi"  ,&tauPhi);
        Run_Tree->SetBranchAddress("tauMass"  ,&tauMass);
        Run_Tree->SetBranchAddress("tauCharge"  ,&tauCharge);
        Run_Tree->SetBranchAddress("taupfTausDiscriminationByDecayModeFinding", &taupfTausDiscriminationByDecayModeFinding);
        Run_Tree->SetBranchAddress("tauByTightMuonRejection3", &tauByTightMuonRejection3);
        Run_Tree->SetBranchAddress("tauByLooseMuonRejection3", &tauByLooseMuonRejection3);
        Run_Tree->SetBranchAddress("tauByMVA6MediumElectronRejection"  ,&tauByMVA6MediumElectronRejection);
        Run_Tree->SetBranchAddress("tauByLooseCombinedIsolationDeltaBetaCorr3Hits",&tauByLooseCombinedIsolationDeltaBetaCorr3Hits);
        Run_Tree->SetBranchAddress("tauByMediumCombinedIsolationDeltaBetaCorr3Hits",&tauByMediumCombinedIsolationDeltaBetaCorr3Hits);
        Run_Tree->SetBranchAddress("tauByMVA6LooseElectronRejection", &tauByMVA6LooseElectronRejection);
        Run_Tree->SetBranchAddress("tauDxy",&tauDxy);
        Run_Tree->SetBranchAddress("tauDecayMode",&tauDecayMode);
        Run_Tree->SetBranchAddress("tauByLooseIsolationMVArun2v1DBoldDMwLT",&tauByLooseIsolationMVArun2v1DBoldDMwLT);
        Run_Tree->SetBranchAddress("tauByVLooseIsolationMVArun2v1DBoldDMwLT",&tauByVLooseIsolationMVArun2v1DBoldDMwLT);
        
        //########################################   Mu Info
        Run_Tree->SetBranchAddress("nMu", &nMu);
        Run_Tree->SetBranchAddress("muPt"  ,&muPt);
        Run_Tree->SetBranchAddress("muEta"  ,&muEta);
        Run_Tree->SetBranchAddress("muPhi"  ,&muPhi);
        Run_Tree->SetBranchAddress("muIsoTrk", &muIsoTrk);
        Run_Tree->SetBranchAddress("muCharge",&muCharge);
        Run_Tree->SetBranchAddress("muIDbit",&muIDbit);//NEW
        Run_Tree->SetBranchAddress("muPFChIso", &muPFChIso);
        Run_Tree->SetBranchAddress("muPFPhoIso", &muPFPhoIso);
        Run_Tree->SetBranchAddress("muPFNeuIso", &muPFNeuIso);
        Run_Tree->SetBranchAddress("muPFPUIso", &muPFPUIso);
        Run_Tree->SetBranchAddress("muD0",&muD0);
        Run_Tree->SetBranchAddress("muDz",&muDz);
        
        //########################################   Ele Info
        Run_Tree->SetBranchAddress("nEle", &nEle);
        Run_Tree->SetBranchAddress("elePt"  ,&elePt);
        Run_Tree->SetBranchAddress("eleEta"  ,&eleEta);
        Run_Tree->SetBranchAddress("elePhi"  ,&elePhi);
        Run_Tree->SetBranchAddress("elePFChIso", &elePFChIso);
        Run_Tree->SetBranchAddress("eleIDMVA", &eleIDMVA);//NEW
        Run_Tree->SetBranchAddress("eleCharge",&eleCharge);
        Run_Tree->SetBranchAddress("eleSCEta",&eleSCEta);
        Run_Tree->SetBranchAddress("elePFChIso", &elePFChIso);
        Run_Tree->SetBranchAddress("elePFPhoIso", &elePFPhoIso);
        Run_Tree->SetBranchAddress("elePFNeuIso", &elePFNeuIso);
        Run_Tree->SetBranchAddress("elePFPUIso", &elePFPUIso);
        Run_Tree->SetBranchAddress("eleD0",&eleD0);
        Run_Tree->SetBranchAddress("eleDz",&eleDz);
        Run_Tree->SetBranchAddress("eleMissHits", &eleMissHits);
        Run_Tree->SetBranchAddress("eleConvVeto", &eleConvVeto);
        Run_Tree->SetBranchAddress("eleSCEta", &eleSCEta );
        
        //########################################   Jet Info
        Run_Tree->SetBranchAddress("nJet",&nJet);
        Run_Tree->SetBranchAddress("jetPt",&jetPt);
        Run_Tree->SetBranchAddress("jetEta",&jetEta);
        Run_Tree->SetBranchAddress("jetPhi",&jetPhi);
        Run_Tree->SetBranchAddress("jetEn",&jetEn);
        Run_Tree->SetBranchAddress("jetCSV2BJetTags",&jetCSV2BJetTags);
        Run_Tree->SetBranchAddress("jetPFLooseId",&jetPFLooseId);
        Run_Tree->SetBranchAddress("jetPUID",&jetPUID);
        Run_Tree->SetBranchAddress("jetRawPt",&jetRawPt);
        Run_Tree->SetBranchAddress("jetJECUnc",&jetJECUnc);
        Run_Tree->SetBranchAddress("jetRawEn",&jetRawEn);
        Run_Tree->SetBranchAddress("jetHadFlvr",&jetHadFlvr);
        Run_Tree->SetBranchAddress("jetP4Smear",&jetP4Smear);
        Run_Tree->SetBranchAddress("jetP4SmearUp",&jetP4SmearUp);
        Run_Tree->SetBranchAddress("jetP4SmearDo",&jetP4SmearDo);
        
        
        //########################################   MET Info
        Run_Tree->SetBranchAddress("pfMET",&pfMET);
        Run_Tree->SetBranchAddress("pfMET_T1UESUp",&pfMET_T1UESUp);
        Run_Tree->SetBranchAddress("pfMET_T1UESDo",&pfMET_T1UESDo);
        Run_Tree->SetBranchAddress("pfMET_T1JESUp",&pfMET_T1JESUp);
        Run_Tree->SetBranchAddress("pfMET_T1JESDo",&pfMET_T1JESDo);
        
        Run_Tree->SetBranchAddress("pfMETPhi",&pfMETPhi);
        Run_Tree->SetBranchAddress("pfMETPhi_T1UESUp",&pfMETPhi_T1UESUp);
        Run_Tree->SetBranchAddress("pfMETPhi_T1UESDo",&pfMETPhi_T1UESDo);
        Run_Tree->SetBranchAddress("pfMETPhi_T1JESUp",&pfMETPhi_T1JESUp);
        Run_Tree->SetBranchAddress("pfMETPhi_T1JESDo",&pfMETPhi_T1JESDo);
        
        Run_Tree->SetBranchAddress("metFilters",&metFilters);
        Run_Tree->SetBranchAddress("genHT",&genHT);
        
        
        //###############################################################################################
        //  Weight Calculation
        //###############################################################################################
        Int_t nBin=500;
        Int_t binMin=0;
        Int_t binMax= 5000;
                
        float MuMass= 0.10565837;
        float eleMass= 0.000511;
        float LeptonPtCut_=60;
        float TauPtCut_=20;
        float JetPtCut=100;
        float BJetPtCut=30;
        float ElectronPtCut_=15;
        float CSVCut= 0.9535 ;
        float LeptonIsoCut=0.15;
        
        float jetES[3]={-1,0,1};
        std::string ResolJet_Cat[1] = {""};
        std::string ScaleJet_Cat[3] = {"JetESDown", "", "JetESUp"};
        std::string ScaleMETUE_Cat[5] = {"METUESDown", "", "METUESUp","METJESDown","METJESUp"};
        
        
        
        
        Int_t nentries_wtn = (Int_t) Run_Tree->GetEntries();
        cout<<"nentries_wtn===="<<nentries_wtn<<"\n";
        for (Int_t i = 0; i < nentries_wtn; i++) {
            Run_Tree->GetEntry(i);
            if (i % 10000 == 0) fprintf(stdout, "\r  Processed events: %8d of %8d ", i, nentries_wtn);
            fflush(stdout);
            
            if (isData && (metFilters!=1536)) continue;
            std::vector<string> HistNamesFilled;
            HistNamesFilled.clear();
            //###############################################################################################
            //  Weight Calculation
            //###############################################################################################
            //###############################################################################################
            //############ Top Reweighting & W boson SCale Factor
            
            float GenTopPt=0;
            float GenAntiTopPt=0;
            float TopPtReweighting = 1;
            float WBosonPt=0;
            float WBosonKFactor=1;
            float ZBosonPt=0;
            float ZBosonKFactor=1;
            
            TLorentzVector GenMu4Momentum,GenAntiMu4Momentum;
            
            for (int igen=0;igen < nMC; igen++){
                if (mcPID->at(igen) == 6 && mcStatus->at(igen) ==62) GenTopPt=mcPt->at(igen) ;
                if (mcPID->at(igen) == -6 && mcStatus->at(igen) ==62) GenAntiTopPt=mcPt->at(igen);
                if (fabs(mcPID->at(igen)) ==24   && mcStatus->at(igen) ==22)  WBosonPt= mcPt->at(igen); // In inclusive we have status 62||22||44 while in HTbins we have just 22
                if (fabs(mcPID->at(igen)) ==23)  ZBosonPt= mcPt->at(igen); //FIXME somethime we do not have Z in the DY events
                if ( mcPID->at(igen) ==13  )  GenMu4Momentum.SetPtEtaPhiE(mcPt->at(igen),mcEta->at(igen),mcPhi->at(igen),mcMass->at(igen));
                if ( mcPID->at(igen) ==-13  )  GenAntiMu4Momentum.SetPtEtaPhiE(mcPt->at(igen),mcEta->at(igen),mcPhi->at(igen),mcMass->at(igen));
                
            }
            if (ZBosonPt ==0)
            ZBosonPt=(GenMu4Momentum+GenAntiMu4Momentum).Pt();  //This is a temp solution to the above problem
            
            
            size_t isTTJets = InputROOT.find("TTJets");
            if (isTTJets!= string::npos) TopPtReweighting = compTopPtWeight(GenTopPt, GenAntiTopPt);
            size_t isWJets = InputROOT.find("WJets");
            if (isWJets!= string::npos) WBosonKFactor=Get_W_Z_BosonKFactor(WBosonPt,WLO,WNLO);  //Swtch ON only for LO Madgraph sample
            size_t isDYJets = InputROOT.find("DYJets");
            if (isDYJets!= string::npos) ZBosonKFactor=Get_W_Z_BosonKFactor(ZBosonPt,ZLO,ZNLO);  //Swtch ON only for LO Madgraph sample
            //###############################################################################################
            float LumiWeight = 1;
            float GetGenWeight=1;
            float PUWeight = 1;
            
            if (!isData){
                
                if (HistoTot) LumiWeight = weightCalc(HistoTot, InputROOT);
                GetGenWeight=genWeight;
                
                int puNUmmc=int(puTrue->at(0)*10);
                int puNUmdata=int(puTrue->at(0)*10);
                float PUMC_=HistoPUMC->GetBinContent(puNUmmc+1);
                float PUData_=HistoPUData->GetBinContent(puNUmdata+1);
                if (PUMC_ ==0)
                    cout<<"PUMC_ is zero!!! & num pileup= "<< puTrue->at(0)<<"\n";
                else
                    PUWeight= PUData_/PUMC_;
            }
            
            
            //###############################################################################################
            //  Trigger Cut
            //###############################################################################################
            bool PassTrigger = (HLTEleMuX >> 21 & 1) == 1; //   else if (name.find("HLT_Mu50_v") != string::npos) bitEleMuX = 21;
            if (! PassTrigger) continue;
            
            //###########       tau Veto   ###########################################################
            int numTau=0;
            for  (int itau=0 ; itau < nTau; itau++){
                
                if (tauPt->at(itau) < TauPtCut_  || fabs(tauEta->at(itau)) > 2.3 ) continue;
                
                bool TauIdIso =  taupfTausDiscriminationByDecayModeFinding->at(itau) > 0.5 && tauByLooseMuonRejection3->at(itau) > 0 && tauByMVA6LooseElectronRejection->at(itau) > 0 && tauByLooseIsolationMVArun2v1DBoldDMwLT->at(itau) > 0;
                
                if (!TauIdIso) continue;
                numTau++;
            }
            //###########       Ele Veto   ###########################################################
            int numElectron=0;
            for  (int jele=0 ; jele < nEle; jele++){
                
                if ( elePt->at(jele) < 15 || fabs(eleEta->at(jele)) > 2.5) continue;
                
                bool eleMVAIdExtra= false;
                if (fabs (eleSCEta->at(jele)) <= 0.8 && eleIDMVA->at(jele) >   0.837  ) eleMVAIdExtra= true;
                else if (fabs (eleSCEta->at(jele)) >  0.8 &&fabs (eleSCEta->at(jele)) <=  1.5 && eleIDMVA->at(jele) >   0.715  ) eleMVAIdExtra= true;
                else if ( fabs (eleSCEta->at(jele)) >=  1.5 && eleIDMVA->at(jele) >  0.357  ) eleMVAIdExtra= true;
                else eleMVAIdExtra= false;
                
                
                if (!(eleMVAIdExtra )) continue;
                numElectron++;
                
            }
            //###########       bJet Veto   ###########################################################
            int numBJet=0;
            int numlightJet=0;
            float EffJet =1;
            float SF=1;
            float P_Data_P_mc=1;
            float FinalBTagSF=1;
            
            
            for (int ijet= 0 ; ijet < nJet ; ijet++){
                
                float HadronFlavor= isData ? 1 : jetHadFlvr->at(ijet);
                
                if (jetPFLooseId->at(ijet) > 0.5 && jetPt->at(ijet) > BJetPtCut && fabs(jetEta->at(ijet)) < 2.4 ){
                    
                    
                    if ( jetCSV2BJetTags->at(ijet) >  CSVCut ){
                        numBJet++;
                        EffJet= getBtagEfficiency( isData, 1,  jetPt->at(ijet), fabs(jetEta->at(ijet)), Btagg_TT);
                        SF= GetBJetSF(isData, jetPt->at(ijet), jetPt->at(ijet), HadronFlavor);
                        P_Data_P_mc=SF*EffJet/EffJet;
                        
                        
                    }
                    else{
                        EffJet= getBtagEfficiency( isData, 0,  jetPt->at(ijet), fabs(jetEta->at(ijet)), Btagg_TT);
                        numlightJet++;
                        SF=GetBJetSF(isData,jetPt->at(ijet), jetPt->at(ijet), HadronFlavor);
                        P_Data_P_mc=(1-SF*EffJet)/(1-EffJet);
                        
                    }
                    
                }
                
                FinalBTagSF *=P_Data_P_mc;
            }
            if (isData) FinalBTagSF=1;
                        
            
            //###########       Z boson Veto   ###########################################################
            int numZboson=0;
            if (nMu > 1){
                TLorentzVector Mu4Momentum_0,Mu4Momentum_1,Z4Momentum;
                Mu4Momentum_0.SetPtEtaPhiM(muPt->at(0),muEta->at(0),muPhi->at(0),MuMass);
                Mu4Momentum_1.SetPtEtaPhiM(muPt->at(1),muEta->at(1),muPhi->at(1),MuMass);
                Z4Momentum=Mu4Momentum_1+Mu4Momentum_0;
                
                float IsoMu1=muPFChIso->at(0)/muPt->at(0);
                if ( (muPFNeuIso->at(0) + muPFPhoIso->at(0) - 0.5* muPFPUIso->at(0) )  > 0.0)
                    IsoMu1= ( muPFChIso->at(0)/muPt->at(0) + muPFNeuIso->at(0) + muPFPhoIso->at(0) - 0.5* muPFPUIso->at(0))/muPt->at(0);
                
                float IsoMu2=muPFChIso->at(1)/muPt->at(1);
                if ( (muPFNeuIso->at(1) + muPFPhoIso->at(1) - 0.5* muPFPUIso->at(1) )  > 0.0)
                    IsoMu2= ( muPFChIso->at(1)/muPt->at(1) + muPFNeuIso->at(1) + muPFPhoIso->at(1) - 0.5* muPFPUIso->at(1))/muPt->at(1);
                
                if (  muPt->at(0) > 60 && muPt->at(1) > 15 && IsoMu1 < 0.25  && IsoMu2 < 0.25 && Z4Momentum.M() > 80 && Z4Momentum.M()< 100 ) numZboson++;
            }
            
            
            
            
            //############################################################################################
            //   Final Total Weight
            //############################################################################################
            float TotalWeight_withTopPtRW = LumiWeight * GetGenWeight * PUWeight * TopPtReweighting * WBosonKFactor * ZBosonKFactor * FinalBTagSF;
            float TotalWeight_NoTopPtRW = LumiWeight * GetGenWeight * PUWeight * WBosonKFactor * ZBosonKFactor * FinalBTagSF;
            
            //###############################################################################################
            //  Some Histogram Filling
            //###############################################################################################
            plotFill("_WeightLumi",LumiWeight,1000,0,10);
            plotFill("_TopPtReweighting",TopPtReweighting,100,0,2);
            plotFill("_WeightPU",PUWeight,100,0,5);
            plotFill("_TotalWeight_withTopPtRW",TotalWeight_withTopPtRW,50,0,2);
            plotFill("_TotalWeight_NoTopPtRW",TotalWeight_NoTopPtRW,50,0,2);
            plotFill("_nVtx_NoPUCorr",nVtx,60,0,60);
            plotFill("_nVtx_PUCorr",nVtx,60,0,60,PUWeight);
            plotFill("_WBosonPt",WBosonPt,150,0,1500,PUWeight);
            plotFill("_FinalBTagSF", FinalBTagSF,200,0,2);
            
            for (int qq=0; qq < 60;qq++){
                if ((HLTEleMuX >> qq & 1) == 1)
                    plotFill("_HLT",qq,60,0,60);
            }
            
            
            
            //############################################################################################
            //###########       Loop over MuJet events   #################################################
            //############################################################################################
            
            for  (int imu=0 ; imu < nMu; imu++){
                
                float IsoMu=muPFChIso->at(imu)/muPt->at(imu);
                if ( (muPFNeuIso->at(imu) + muPFPhoIso->at(imu) - 0.5* muPFPUIso->at(imu) )  > 0.0)
                    IsoMu= ( muPFChIso->at(imu)/muPt->at(imu) + muPFNeuIso->at(imu) + muPFPhoIso->at(imu) - 0.5* muPFPUIso->at(imu))/muPt->at(imu);
                
                bool MuPtCut = muPt->at(imu) > LeptonPtCut_ && fabs(muEta->at(imu)) < 2.4 ;
                bool MuIdIso=( (muIDbit->at(imu) >> 2 & 1)  && fabs(muD0->at(imu)) < 0.045 && fabs(muDz->at(imu)) < 0.2); //Tight Muon Id
                
                if (! MuPtCut || !MuIdIso ) continue;
                
                
                float LepCor=getCorrFactorMuon80X(isData,  muPt->at(imu), muEta->at(imu) , HistoMuId,HistoMuIso,HistoMuTrg,HistoMuTrack);
                
                
                TLorentzVector Mu4Momentum,Jet4MomentumNonSmear, Jet4Momentum,KJet4Momentum,NewJet4Collection,LQ;
                Mu4Momentum.SetPtEtaPhiM(muPt->at(imu),muEta->at(imu),muPhi->at(imu),MuMass);
                
                
                
                
                //###########      Finding the close jet near mu   ###########################################################
                
                float CLoseJetMuPt=muPt->at(imu);
                float CLoseJetMuEta=muEta->at(imu);
                
                if (MuPtCut && MuIdIso ){
                    
                    double Refer_R_jetmu = 5;
                    
                    for (int kjet= 0 ; kjet < nJet ; kjet++){
                        KJet4Momentum.SetPtEtaPhiE(jetPt->at(kjet),jetEta->at(kjet),jetPhi->at(kjet),jetEn->at(kjet));
                        
                        
                        if (KJet4Momentum.DeltaR(Mu4Momentum) < Refer_R_jetmu) {
                            Refer_R_jetmu = KJet4Momentum.DeltaR(Mu4Momentum);
                            if (Refer_R_jetmu < 0.5 && jetPt->at(kjet)  >= muPt->at(imu)) {
                                CLoseJetMuPt = jetPt->at(kjet);
                                CLoseJetMuEta = jetEta->at(kjet);
                                
                            }
                        }
                    }
                }
                
                
                
                
                //###########    loop over  Jet    ###########################################################
                
                for (int ijet= 0 ; ijet < nJet ; ijet++){
                    
                    
//                    float JetSmearResolution[3]={1,1,1};
////                    if (!isData){
////                        JetSmearResolution[0]=jetP4SmearDo->at(ijet);
////                        JetSmearResolution[1]=jetP4Smear->at(ijet);
////                        JetSmearResolution[2]=jetP4SmearUp->at(ijet);
////                    }
                        float JetSmearResolution=1;
                    
                    float UESMET[5]={pfMET_T1UESDo,pfMET,pfMET_T1UESUp,pfMET_T1JESDo,pfMET_T1JESUp};
                    float UESMETPhi[5]={pfMETPhi_T1UESDo,pfMETPhi,pfMETPhi_T1UESUp,pfMETPhi_T1JESDo,pfMETPhi_T1JESUp};
                    
                    
//                    for (int jetRes=0;jetRes<3;jetRes++){  // No need for JER  it has very small effect
                    for (int jetRes=0;jetRes<1;jetRes++){
                        for (int metUE=0; metUE < 5; metUE++){
                            for (int jetScl=0;jetScl<3;jetScl++){
                    
                                
                                // This is to check that we only make the plots only either JES or MET is applied (not both of them simultaneously)!
                                if (jetRes!=0 && metUE != 1  ) continue;
                                if (jetRes!=0 && jetScl != 1 ) continue;
                                if (metUE != 1 && jetScl != 1 ) continue;
                                
                                
                                Jet4MomentumNonSmear.SetPtEtaPhiE(jetPt->at(ijet),jetEta->at(ijet),jetPhi->at(ijet),jetEn->at(ijet));
//                                cout << "Jet pt before Smear="<<Jet4MomentumNonSmear.Pt()<<"\t";
//                                Jet4Momentum=Jet4MomentumNonSmear*JetSmearResolution[jetRes];
                                Jet4Momentum=Jet4MomentumNonSmear*JetSmearResolution;
//                                cout << "Jet pt after Smear="<<Jet4Momentum.Pt()<<"\n";
                                
                                
                                NewJet4Collection.SetPtEtaPhiE(Jet4Momentum.Pt()*(1+ jetES[jetScl]*jetJECUnc->at(ijet)) ,jetEta->at(ijet),jetPhi->at(ijet),Jet4Momentum.E()*(1+ jetES[jetScl]*jetJECUnc->at(ijet)));
                                
                                
                                
                                bool goodJet = (jetPFLooseId->at(ijet) > 0.5 && NewJet4Collection.Pt() > JetPtCut && fabs(NewJet4Collection.Eta() ) < 2.4 && NewJet4Collection.DeltaR(Mu4Momentum) > 0.5);
                                if (! goodJet) continue;
                                
                                
                                
                                float jetMET=UESMET[metUE];
                                float jetMETPhi=UESMETPhi[metUE];
                                
                                float jetMET_x = UESMET[metUE] * TMath::Cos(UESMETPhi[metUE]) - (Jet4Momentum.Px()- NewJet4Collection.Px()) ;
                                float jetMET_y = UESMET[metUE] * TMath::Sin(UESMETPhi[metUE]) - (Jet4Momentum.Py()- NewJet4Collection.Py()) ;
                                jetMET = sqrt (pow(jetMET_x,2)+ pow(jetMET_y,2));
                                jetMETPhi = atan(jetMET_y / jetMET_x);
                                if (UESMETPhi[metUE] > (TMath::Pi() / 2)) jetMETPhi += TMath::Pi();
                                if (UESMETPhi[metUE] < (-TMath::Pi() / 2)) jetMETPhi -= TMath::Pi();
                                
                                
                                
                                LQ=NewJet4Collection + Mu4Momentum;
                                if ((numTau+numElectron +numZboson + numBJet) > 0) continue;
//                                if ((numTau+numElectron +numZboson) > 0 || fabs(LQ.Eta()) > 1.5) continue;// FIXME
                                bool HighDPhi = deltaPhi(NewJet4Collection.Phi(),jetMETPhi) > 0.5;
                                if (!HighDPhi) continue;
                                
                                //###############################################################################################
                                //  Isolation Categorization
                                //###############################################################################################
                                //###############################################################################################
                                
                                
                                bool LepPassIsolation= IsoMu < LeptonIsoCut;
                                
                                const int size_isoCat = 2;
                                bool Isolation = LepPassIsolation;
                                bool AntiIsolation =  !LepPassIsolation;
                                
                                
                                bool Iso_category[size_isoCat] = {Isolation, AntiIsolation};
                                std::string iso_Cat[size_isoCat] = {"_Iso", "_AntiIso"};
                                //###############################################################################################
                                //  MT Categorization
                                //###############################################################################################
                                float tmass_MuMet= TMass_F(muPt->at(imu), muPt->at(imu)*cos(muPhi->at(imu)),muPt->at(imu)*sin(muPhi->at(imu)) , jetMET, jetMETPhi);
                                const int size_mTCat = 6;
                                
                                bool MT100 = tmass_MuMet > 100;
                                bool MT200 = tmass_MuMet > 200;
                                bool MT300 = tmass_MuMet > 300;
                                bool MT400 = tmass_MuMet > 400;
                                bool MT500 = tmass_MuMet > 500;
                                bool MT600 = tmass_MuMet > 600;
                                
                                
//                                bool MT_category[size_mTCat] = {MT100,MT150,MT200,MT250,MT300,MT350,MT400,MT450,MT500};
//                                std::string MT_Cat[size_mTCat] = {"_MT100", "_MT150","_MT200", "_MT250","_MT300", "_MT350","_MT400", "_MT450","_MT500"};
                                bool MT_category[size_mTCat] = {MT100,MT200,MT300,MT400,MT500,MT600};
                                std::string MT_Cat[size_mTCat] = {"_MT100", "_MT200","_MT300","_MT400", "_MT500", "_MT600"};
//                                bool MT_category[size_mTCat] = {MT250};
//                                std::string MT_Cat[size_mTCat] = {"_MT250"};
                                
                                //                    float tmass_JetMet= TMass_F(jetPt->at(ijet), jetPt->at(ijet)*cos(jetPhi->at(ijet)),jetPt->at(ijet)*sin(jetPhi->at(ijet)) , pfMET, pfMETPhi);
                                //                    float tmass_LQMet= TMass_F(LQ.Pt(), LQ.Px(),LQ.Py(), pfMET, pfMETPhi);
                                
                                
                                //###############################################################################################
                                //  Jet Pt Categorization
                                //###############################################################################################
                                
                                const int size_METcut = 6;
                                
                                bool MET100 = jetMET > 100;
                                bool MET200 = jetMET > 200;
                                bool MET300 = jetMET > 300;
                                bool MET400 = jetMET > 400;
                                bool MET500 = jetMET > 500;
                                bool MET600 = jetMET > 600;
                                
                                
                                
//                                bool MetCut_category[size_METcut] = {MET100,MET150,MET200,MET250,MET300,MET350,MET400,MET450,MET500};
//                                std::string MetCut_Cat[size_METcut] = {"_MET100", "_MET150","_MET200", "_MET250","_MET300", "_MET350","_MET400", "_MET450","_MET500"};
                            bool MetCut_category[size_METcut] = {MET100,MET200,MET300,MET400,MET500,MET600};
                            std::string MetCut_Cat[size_METcut] = {"_MET100","_MET200", "_MET300","_MET400", "_MET500", "_MET600"};

//                                bool MetCut_category[size_METcut] = {MET250};
//                                std::string MetCut_Cat[size_METcut] = {"_MET250"};
                                
                                
                                //###############################################################################################
                                
                                
                                //###############################################################################################
                                //  Top Pt Reweighting Cat: The SF is meant to correct only the shape of the pt(top) distribution- not the amount of generated events ( you have to consider that the average weight is not 1 ! ) So we define two category for ttbar events
                                
                                //###############################################################################################
                                int size_topPtRW =2;
                                
                                float TotalWeight[2] = {TotalWeight_withTopPtRW,TotalWeight_NoTopPtRW};
                                std::string topPtRW[2] = {"", "_NoTopRW"};
                                
                                if (isTTJets == string::npos) size_topPtRW = 1; // If the sample in not ttbar, don't care about new category
                                
                                //###############################################################################################
                                
                                
                                
                                std::string CHL="MuJet";
                                
                                plotFill("Weight_Mu", LepCor,200,0,2);
                                plotFill("TotalWeight_Mu",TotalWeight_NoTopPtRW*LepCor,1000,0,10);
                                plotFill("TotalNonLumiWeight_Mu",TotalWeight_NoTopPtRW*LepCor/LumiWeight,1000,0,10);
                                
                                
                                for (int iso = 0; iso < size_isoCat; iso++) {
                                    if (Iso_category[iso]) {
                                        for (int imt = 0; imt < size_mTCat; imt++) {
                                            if (MT_category[imt]) {
                                                for (int jpt = 0; jpt < size_METcut; jpt++) {
                                                    if (MetCut_category[jpt]) {
                                                        for (int itopRW = 0; itopRW < size_topPtRW; itopRW++) {
                                                            
                                                            float FullWeight = TotalWeight[itopRW] * LepCor ;
                                                            std::string FullStringName = topPtRW[itopRW] + MT_Cat[imt] + MetCut_Cat[jpt]+  iso_Cat[iso]+ ScaleJet_Cat[jetScl]+ResolJet_Cat[jetRes]+ ScaleMETUE_Cat[metUE];
                                                            
                                                            //This check is used to make sure that each event is just filled once for any of the categories ==> No doube-counting of events  (this is specially important for ttbar events where we have many jets and leptons)
                                                            if (!( std::find(HistNamesFilled.begin(), HistNamesFilled.end(), FullStringName) != HistNamesFilled.end())){
                                                                HistNamesFilled.push_back(FullStringName);
                                                                
                                                                
                                                                plotFill(CHL+"_tmass_MuMet"+FullStringName,tmass_MuMet,200,0,2000,FullWeight);
                                                                plotFill(CHL+"_MET"+FullStringName,UESMET[metUE],200,0,2000,FullWeight);
                                                                
                                                                plotFill(CHL+"_CloseJetLepPt"+FullStringName,CLoseJetMuPt,1000,0,1000,FullWeight);
                                                                plotFill(CHL+"_LQMass"+FullStringName,LQ.M(),300,0,3000,FullWeight);
//                                                                plotFill(CHL+"_LQEta"+FullStringName,LQ.Eta(),100,-5,5,FullWeight);
                                                                
                                                                if (isTTJets!= string::npos) plotFill(CHL+"_LQMassTopPtRWUp"+FullStringName,LQ.M(),nBin,binMin,binMax,FullWeight * TopPtReweighting);
                                                                if (isTTJets!= string::npos) plotFill(CHL+"_LQMassTopPtRWDown"+FullStringName,LQ.M(),nBin,binMin,binMax,FullWeight / TopPtReweighting);
                                                                
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    //qcd control region   MET > 100 && MT > 150  && Jet < 100 && jet > 50;
                    //qcd
                    
                    //###############################################################################################
                    //  Doing EleTau Analysis
                    //###############################################################################################
                    
                    
                } //End of Tree
            }//End of file
            //##############  end of dielectron
            
        }
        
    }
    
    fout->cd();
    //    BG_Tree->Write();
    
    map<string, TH1F*>::const_iterator iMap1 = myMap1->begin();
    map<string, TH1F*>::const_iterator jMap1 = myMap1->end();
    
    for (; iMap1 != jMap1; ++iMap1)
        nplot1(iMap1->first)->Write();
    
    map<string, TH2F*>::const_iterator iMap2 = myMap2->begin();
    map<string, TH2F*>::const_iterator jMap2 = myMap2->end();
    
    for (; iMap2 != jMap2; ++iMap2)
        nplot2(iMap2->first)->Write();
    
    fout->Close();
    
    
}
