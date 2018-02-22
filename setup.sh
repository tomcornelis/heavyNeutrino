if [[ $CMSSW_BASE == *CMSSW_8* ]]
then
  # CMSSW_8_0_X checkout
  git checkout CMSSW_8_0_X
  eval `scram runtime -sh`
  cd $CMSSW_BASE/src

  # First EGM smearing
  git cms-merge-topic cms-egamma:EGM_gain_v1
  cd EgammaAnalysis/ElectronTools/data
  git clone -b Moriond17_gainSwitch_unc https://github.com/ECALELFS/ScalesSmearings.git 
  cd -

  # Setup for new electron and photon MVA
  git cms-merge-topic ikrav:egm_id_80X_v3_photons

else
  # CMSSW_9_4_X checkout
  eval `scram runtime -sh`
  cd $CMSSW_BASE/src

  # First EGM smearing
  git cms-merge-topic cms-egamma:EGM_94X_v1
  cd EgammaAnalysis/ElectronTools/data
  git clone https://github.com/ECALELFS/ScalesSmearings.git
  cd ScalesSmearings/
  git checkout Run2017_17Nov2017_v1

  # Electron ID
  git cms-merge-topic lsoffi:CMSSW_9_4_0_pre3_TnP
  git cms-merge-topic guitargeek:ElectronID_MVA2017_940pre3

  # Setting up new photon ID
  cd $CMSSW_BASE/external
  cd slc6_amd64_gcc630/
  git clone https://github.com/lsoffi/RecoEgamma-PhotonIdentification.git data/RecoEgamma/PhotonIdentification/data
  cd data/RecoEgamma/PhotonIdentification/data
  git checkout CMSSW_9_4_0_pre3_TnP
  cd $CMSSW_BASE/external
  cd slc6_amd64_gcc630/
  git clone https://github.com/lsoffi/RecoEgamma-ElectronIdentification.git data/RecoEgamma/ElectronIdentification/data
  cd data/RecoEgamma/ElectronIdentification/data
  git checkout CMSSW_9_4_0_pre3_TnP
  cd $CMSSW_BASE/src
fi

# Finally, compile
scram b -j 10
