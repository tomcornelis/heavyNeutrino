#ifndef GenTools_H
#define GenTools_H

//include c++ libarary classes
#include <set>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

namespace GenTools{
    const reco::GenParticle* getFirstMother(const reco::GenParticle&, const std::vector<reco::GenParticle>&);
    const reco::GenParticle* getMother(const reco::GenParticle&, const std::vector<reco::GenParticle>&);
    //return decay chain for a particle;
    //void setDecayChain(const reco::GenParticle& gen, const std::vector<reco::GenParticle>& genParticles, std::set<int>& list, std::vector<int> *vect = nullptr);
    void setDecayChain(const reco::GenParticle& gen, const std::vector<reco::GenParticle>& genParticles, std::set<int>& list);
    bool hasOnlyIncomingGluonsInChain(const reco::GenParticle& gen, const std::vector<reco::GenParticle>& genParticles);

    //scan decay chain for certain types of particles
    bool bosonInChain(const std::set<int>&);
    bool bBaryonInChain(const std::set<int>&);
    bool bMesonInChain(const std::set<int>&);
    bool cBaryonInChain(const std::set<int>&);
    bool cMesonInChain(const std::set<int>&);
    bool sBaryonInChain(const std::set<int>&);
    bool sMesonInChain(const std::set<int>&);
    bool lightBaryonInChain(const std::set<int>&);
    bool lightMesonInChain(const std::set<int>&);
    bool pi0InChain(const std::set<int>&);
    bool photonInChain(const std::set<int>&);
    bool udsInChain(const std::set<int>&);
    bool tauInChain(const std::set<int>&);
    //find the provenance of a particle using the contents of its decayChain
    unsigned provenance(const reco::GenParticle*, const std::vector<reco::GenParticle>&);
    unsigned provenanceCompressed(const reco::GenParticle*, const std::vector<reco::GenParticle>&, bool isPrompt);

    //check whether photon comes from ME in conversion
    unsigned provenanceConversion(const reco::GenParticle*, const std::vector<reco::GenParticle>&);

    bool isPrompt(const reco::GenParticle&, const std::vector<reco::GenParticle>&); //function to check if particle is prompt TO BE USED INSTEAD OF CMSSW BUILTIN
    bool passParentage(const reco::GenParticle& gen, const std::vector<reco::GenParticle>& genParticles);
    double getMinDeltaR(const reco::GenParticle& p, const std::vector<reco::GenParticle>& genParticles, float ptCut=5);

    const reco::GenParticle* geometricMatch(const reco::Candidate& reco, const std::vector<reco::GenParticle>& genParticles, const bool differentId=false);
    bool considerForMatching(const reco::Candidate& reco, const reco::GenParticle& gen, const bool differentId);

    //function to check if particle is prompt TO BE USED INSTEAD OF CMSSW BUILTIN
    bool isPrompt(const reco::GenParticle&, const std::vector<reco::GenParticle>&);
    bool isPromptFinalState(const reco::GenParticle&, const std::vector<reco::GenParticle>&);
    bool isPromptDecayed(const reco::GenParticle&, const std::vector<reco::GenParticle>&);

    //get CMSSW gen match for lepton (displaced specific ???)
    template<typename Lepton> const reco::GenParticle* getCMSSWGenMatch(const Lepton& lepton){
        return lepton.genParticle();
    }
}
#endif
