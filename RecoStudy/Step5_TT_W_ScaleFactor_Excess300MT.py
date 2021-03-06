#!/usr/bin/env python
import ROOT
import re
from array import array

def returnSF_TT(FileName):
    
    
    file=ROOT.TFile(FileName,"r")
    categoriy='MuJet'
    Data=file.Get(categoriy).Get("data_obs")
    Data.Rebin(200)
    
    W=file.Get(categoriy).Get("W")
    W.Rebin(200)
    TT=file.Get(categoriy).Get("TT")
    TT.Rebin(200)
    SingleT=file.Get(categoriy).Get("SingleTop")
    SingleT.Rebin(200)
    VV=file.Get(categoriy).Get("VV")
    VV.Rebin(200)
    DYS=file.Get(categoriy).Get("ZTT")
    DYS.Rebin(200)


    ################################################################################################################
    #   Here we compute the TT scale factor
    #   (Data - allBkg)/TT
    TTScale=(Data.Integral() - SingleT.Integral() - DYS.Integral() - VV.Integral()- W.Integral() ) / TT.Integral()
    print "TT SF = %.2f , TT purity %.2f, TT normalization %.2f "%(TTScale,TT.Integral()/Data.Integral(),TT.Integral())
    ################################################################################################################

    return TTScale


def returnSF_W(FileName,TTSF):
    
    
    file=ROOT.TFile(FileName,"r")
    categoriy='MuJet'
    Data=file.Get(categoriy).Get("data_obs")
    Data.Rebin(200)
    
    W=file.Get(categoriy).Get("W")
    W.Rebin(200)
    TT=file.Get(categoriy).Get("TT")
    TT.Rebin(200)
    TT.Scale(TTSF)
    SingleT=file.Get(categoriy).Get("SingleTop")
    SingleT.Rebin(200)
    VV=file.Get(categoriy).Get("VV")
    VV.Rebin(200)
    DYS=file.Get(categoriy).Get("ZTT")
    DYS.Rebin(200)
    
    ################################################################################################################
    #   Here we compute the W scale factor
    #   (Data - allBkg)/W
    WScale=(Data.Integral() - SingleT.Integral() - DYS.Integral() - VV.Integral()- TT.Integral() ) / W.Integral()
    print "W purity =%.2f (%.2f , %.2f)"%(W.Integral()/Data.Integral() ,  W.Integral(), Data.Integral())
    ErrData=Data.GetBinError(1)
    ErrW=W.GetBinError(1)
    TotErr=(Data.GetBinError(1)/Data.GetBinContent(1) + W.GetBinError(1)/W.GetBinContent(1))* WScale
    print "SF= %.2f +- %.2f"  %(WScale, TotErr)
    ################################################################################################################
    
    return WScale

MTRange=["_NoMT","_HighMT"]




def SF_TT(Name):
    return returnSF_TT(Name)


print "\n\n\n\n\n\n######### TT Scale factor  DiLepton #########"
for mt in MTRange:
    NameTT='TotalRootForLimit_PreSelection_Excess_MuJet_LQMass%s_HighDPhi_ttbarCRDiLep_Iso.root'%mt
    print "######### ",mt, " #########"
    SF_TT(NameTT)

print "\n\n\n\n\n\n######### TT Scale factor  SingleLepton #########"
for mt in MTRange:
    NameTT='TotalRootForLimit_PreSelection_Excess_MuJet_LQMass%s_HighDPhi_ttbarCRSingleLep_Iso.root'%mt
    print "######### ",mt, " #########"
    SF_TT(NameTT)



print "\n\n\n\n\n\n######### W Scale factor  DiLepton #########"
for mt in MTRange:
    NameW= 'TotalRootForLimit_PreSelection_Excess_MuJet_LQMass%s_HighDPhi_Iso.root'%mt
    NameTT='TotalRootForLimit_PreSelection_Excess_MuJet_LQMass%s_HighDPhi_ttbarCRDiLep_Iso.root'%mt
    print "######### ",mt, " #########"
    returnSF_W(NameW,SF_TT(NameTT))


print "\n\n\n\n\n\n######### W Scale factor  SingleLepton #########"
for mt in MTRange:
    NameW= 'TotalRootForLimit_PreSelection_Excess_MuJet_LQMass%s_HighDPhi_Iso.root'%mt
    NameTT='TotalRootForLimit_PreSelection_Excess_MuJet_LQMass%s_HighDPhi_ttbarCRSingleLep_Iso.root'%mt
    print "######### ",mt, " #########"
    returnSF_W(NameW,SF_TT(NameTT))

#
#
#def SF_TT_SingleLep():
#    return returnSF_TT('TotalRootForLimit_PreSelection_Excess_MuJet_LQMass_HighMT_HighDPhi_ttbarCRSingleLep_Iso.root')
#
#
#def SF_TT_DiLep():
#    return returnSF_TT('TotalRootForLimit_PreSelection_Excess_MuJet_LQMass_HighMT_HighDPhi_ttbarCRDiLep_Iso.root')
#
#
#def SF_W_SingleLep():
#    return returnSF_W('TotalRootForLimit_PreSelection_Excess_MuJet_LQMass_MT50To150_HighDPhi_Iso.root',SF_TT_SingleLep())
#
#
#def SF_W_DiLep():
#    return returnSF_W('TotalRootForLimit_PreSelection_Excess_MuJet_LQMass_MT50To150_HighDPhi_Iso.root',SF_TT_DiLep())
#

