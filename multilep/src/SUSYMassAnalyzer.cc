#include <exception>
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "SimDataFormats/GeneratorProducts/interface/GenLumiInfoHeader.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "heavyNeutrino/multilep/interface/SUSYMassAnalyzer.h"


SUSYMassAnalyzer::SUSYMassAnalyzer(const edm::ParameterSet& iConfig, multilep* multilepAnalyzer, LheAnalyzer* lheAnalyzer):
      multilepAnalyzer(multilepAnalyzer), 
      lheAnalyzer(lheAnalyzer)
{};


void SUSYMassAnalyzer::beginJob(TTree* outputTree, edm::Service<TFileService>& fs){
    if( !multilepAnalyzer->isSUSY() ) return;    //only run this module on SUSY samples

    //Counter to determine the amount of events for every SUSY mass point
    //Note, too small binning is used to be sure the binning is smaller than the sample's mass point separation
    //There is no way to access the amount of mass points or there splitting while running over the sample!!!
    //Center each bin at integer masses to avoid edge effects
    hCounterSUSY = fs->make<TH2D>( "hCounterSUSY", "SUSY Events counter", 2001, -0.5, 2000.5, 1501, -0.5, 1500.5 );

    //Store SUSY particle masses for event
    outputTree->Branch("_mChi1", &_mChi1, "_mChi1/D");
    outputTree->Branch("_mChi2", &_mChi2, "_mChi2/D");
}


void SUSYMassAnalyzer::beginLuminosityBlock(const edm::LuminosityBlock& iLumi, const edm::EventSetup& iEventSetup){

    //Extract model string corresponding to this lumi block.
    edm::Handle<GenLumiInfoHeader> genHeader = getHandle(iLumi, multilepAnalyzer->genLumiInfoToken);
    std::string model = genHeader->configDescription(); 

    //Extract mass values from model string 
    for(unsigned m = 0; m < 2; ++m){
        std::string::size_type pos = model.find_last_of("_");
        if(m == 0){
            _mChi1 = std::stod(model.substr(pos + 1)); 
            model.erase(pos, model.size());
        } else{

            //Some models only define the mass of the heavy SUSY particles, and assume a 1 GeV mass for Chi1_0 (see for instance SLHA table here: https://cms-pdmv.cern.ch/mcm/edit?db_name=requests&prepid=SUS-RunIIAutumn18FSPremix-00096&page=0 )
            try{
                _mChi2 = std::stod(model.substr(pos + 1));
            } catch ( std::exception ){
                _mChi2 = _mChi1;
                _mChi1 = 1.;
            }
        }
    }
}


void SUSYMassAnalyzer::analyze(const edm::Event& iEvent){
    hCounterSUSY->Fill( _mChi2, _mChi1, lheAnalyzer->getWeight() );
}
