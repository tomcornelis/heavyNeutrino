#include "heavyNeutrino/multilep/interface/GenTools.h"

//include ROOT classes
#include "TLorentzVector.h"

const reco::GenParticle* GenTools::getFirstMother(const reco::GenParticle& gen, const std::vector<reco::GenParticle>& genParticles){
    return (gen.numberOfMothers() == 0) ? nullptr : &genParticles[gen.motherRef(0).key()];
}

const reco::GenParticle* GenTools::getMother(const reco::GenParticle& gen, const std::vector<reco::GenParticle>& genParticles){
    const reco::GenParticle* mom = getFirstMother(gen, genParticles);
    if(!mom) return 0;
    else if(mom->pdgId() == gen.pdgId()) return getMother(*mom, genParticles);
    else return mom;
}

//void GenTools::setDecayChain(const reco::GenParticle& gen, const std::vector<reco::GenParticle>& genParticles, std::set<int>& list, std::vector<int> *vect){
void GenTools::setDecayChain(const reco::GenParticle& gen, const std::vector<reco::GenParticle>& genParticles, std::set<int>& list){
   //  for(auto& i : list) std::cout << "list---> "<<i << std::endl;
    //if((list.empty() or list.find(gen.pdgId())==list.end()) and gen.pdgId() != 2212) {list.insert(gen.pdgId()); if(vect) vect->push_back(gen.pdgId());}
    // DO NOT ADD QUARKS AND GLUONS! (std::abs(gen.pdgId())<=6 or gen.pdgId()==21)
    if((list.empty() or list.find(gen.pdgId())==list.end()) and gen.pdgId()!=2212 and std::abs(gen.pdgId())>6 and gen.pdgId()!=21) list.insert(gen.pdgId());
    // if((list.empty() or list.find(gen.pdgId())==list.end()) and gen.pdgId()!=2212 and std::abs(gen.pdgId())>6 and gen.pdgId()!=21) {
    //   list.insert(gen.pdgId()); if(vect) vect->push_back(gen.pdgId());
    // }
    //std::cout<<"in setdecaychain: gen.pdgId()"<<gen.pdgId()<<std::endl;
    if(gen.numberOfMothers() > 1) setDecayChain(genParticles[gen.motherRef(1).key()], genParticles, list);
    if(gen.numberOfMothers() > 0) setDecayChain(genParticles[gen.motherRef(0).key()], genParticles, list);
  //  for(auto& i : list) std::cout << "list  2   ---> "<<i << std::endl;

}

bool GenTools::hasOnlyIncomingGluonsInChain(const reco::GenParticle& gen, const std::vector<reco::GenParticle>& genParticles){
    if(gen.pdgId()==21){
      std::set<int> chain;
      GenTools::setDecayChain(gen, genParticles, chain);
      if(std::any_of(chain.cbegin(), chain.cend(), [](const int entry){return abs(entry) < 7;})) return false; // found a gluon with a quark in the decay chain
    }
    if(gen.numberOfMothers() > 1 and not hasOnlyIncomingGluonsInChain(genParticles[gen.motherRef(1).key()], genParticles)) return false;
    if(gen.numberOfMothers() > 0 and not hasOnlyIncomingGluonsInChain(genParticles[gen.motherRef(0).key()], genParticles)) return false;
    return true;
}

bool GenTools::bosonInChain(const std::set<int>& chain){ // what is the point of finding a majorana HN here? and why not Dirac HN?
   return std::any_of(chain.cbegin(), chain.cend(), [](const int entry){ return (abs(entry) > 22 && abs(entry) < 26) || (abs(entry) == 9900012);});
}
 
bool GenTools::bBaryonInChain(const std::set<int>& chain){
   return std::any_of(chain.cbegin(), chain.cend(), [](const int entry){ return (abs(entry)/1000)%10 == 5;});
}

bool GenTools::bMesonInChain(const std::set<int>& chain){
   return std::any_of(chain.cbegin(), chain.cend(), [](const int entry){ unsigned mod = abs(entry)%10000; return mod >= 500 && mod < 600;});
}

bool GenTools::cBaryonInChain(const std::set<int>& chain){
   return std::any_of(chain.cbegin(), chain.cend(), [](const int entry){ return (abs(entry)/1000)%10 == 4;});
}

bool GenTools::cMesonInChain(const std::set<int>& chain){
    return std::any_of(chain.cbegin(), chain.cend(), [](const int entry){ unsigned mod = abs(entry)%10000; return mod >= 400 && mod < 500;});
}

bool GenTools::sBaryonInChain(const std::set<int>& chain){
    return std::any_of(chain.cbegin(), chain.cend(), [](const int entry){ return (abs(entry)/1000)%10 == 3;});
}

bool GenTools::lightMesonInChain(const std::set<int>& chain){
    return std::any_of(chain.cbegin(), chain.cend(), [](const int entry){ unsigned mod = abs(entry)%10000; return (mod >= 100 && mod < 400) || entry == 21;});
}

bool GenTools::lightBaryonInChain(const std::set<int>& chain){
    return std::any_of(chain.cbegin(), chain.cend(),
            [](const int entry){
                if(abs(entry) == 2212) return false; // useless? there are no protons saved in the chain; actually those wo do appear you want to have here
                unsigned red = (abs(entry)/1000)%10; 
                return (red == 1 || red == 2); 
            });
}

bool GenTools::pi0InChain(const std::set<int>& chain){
    return chain.count(111);
}

bool GenTools::photonInChain(const std::set<int>& chain){
    return chain.count(22);
}

bool GenTools::tauInChain(const std::set<int>& chain){
    return chain.count(15) or chain.count(-15);
}

bool GenTools::udsInChain(const std::set<int>& chain){
    if(sBaryonInChain(chain))       return true;
    if(lightMesonInChain(chain))    return true;
    if(lightBaryonInChain(chain))   return true;
    return false;
}

//enumerated type to specify decay
enum decayType {
  W_L,       //  0
  W_T_L,     //  1
  W_B_L,     //  2
  W_B_C_L,   //  3
  W_B_C_T_L, //  4
  W_B_T_L,   //  5
  W_C_L,     //  6
  W_C_T_L,   //  7
  B_L,       //  8
  B_C_L,     //  9
  B_C_T_L,   // 10
  B_T_L,     // 11
  C_L,       // 12
  C_T_L,     // 13
  B_Baryon,  // 14
  C_Baryon,  // 15
  pi_0,      // 16
  photon_,   // 17
  F_L        // 18
};

unsigned GenTools::provenance(const reco::GenParticle* gen, const std::vector<reco::GenParticle>& genParticles){
    if(!gen) return F_L; 

    std::set<int> decayChain;
    setDecayChain(*gen, genParticles, decayChain);
    //first consider decays involving a boson
    if(bosonInChain(decayChain)){
      if(bMesonInChain(decayChain)){
        if(cMesonInChain(decayChain)){
          if(tauInChain(decayChain))  return W_B_C_T_L;
          else                        return W_B_C_L;
        }
        if(tauInChain(decayChain))    return W_B_T_L;
        else                          return W_B_L;
      }
      if(cMesonInChain(decayChain)){
        if(tauInChain(decayChain))    return W_C_T_L;
        else                          return W_C_L;
      }
      if(udsInChain(decayChain))      return pi_0;
      if(tauInChain(decayChain))      return W_T_L;
      else                            return W_L;
    }
    if(bMesonInChain(decayChain)){
      if(cMesonInChain(decayChain)){
        if(tauInChain(decayChain))    return B_C_T_L;
        else                          return B_C_L;
      }
      if(tauInChain(decayChain))      return B_T_L;
      else                            return B_L;
    }
    if(cMesonInChain(decayChain)){
      if(tauInChain(decayChain))      return C_T_L;
      else                            return C_L;
    }
    if(bBaryonInChain(decayChain))    return B_Baryon;
    if(cBaryonInChain(decayChain))    return C_Baryon;
    if(udsInChain(decayChain))        return pi_0;
    if(photonInChain(decayChain))     return photon_;
    return F_L; // in master branch
    return W_L; // in displaced branch
}

// TODO: check this definition is completely different from master branch!
unsigned GenTools::provenanceCompressed(const reco::GenParticle* gen, const std::vector<reco::GenParticle>& genParticles, bool isPrompt){
    if(isPrompt) return 0;
    if(!gen) return 4; // This is in the master

    std::set<int> decayChain;
    setDecayChain(*gen, genParticles, decayChain);
    if(bMesonInChain(decayChain) || bBaryonInChain(decayChain) ) return 1;          //lepton from heavy flavor decay
    if(cMesonInChain(decayChain) || cBaryonInChain(decayChain) ) return 2;          //lepton from c flavor decay
//    if(bosonInChain(decayChain) ) return 0;                                         //lepton from boson
//    if(!decayChain.empty()) return 3;                                               //light flavor fake
//    return 4;                                                                       //unkown origin
    if(udsInChain(decayChain))                                   return 3;  // light flavor fake
    if(!decayChain.empty())                                      return 0;  // prompt (boson not always in chain)
    return 4;                                                               //unkown origin
}

unsigned GenTools::provenanceConversion(const reco::GenParticle* photon, const std::vector<reco::GenParticle>& genParticles){
    //https://hypernews.cern.ch/HyperNews/CMS/get/susy-interpretations/192.html
    //99: not a photon
    //0: direct prompt photon (prompt and delta R with ME parton > 0.05)
    //1: fragmentation photon (prompt and delta R with ME parton < 0.05)
    //2: non-prompt photon
    if(!photon or photon->pdgId() != 22) return 99;
    if(!photon->isPromptFinalState())    return 2;
    if(photon->pt() < 10)                return 1;

    TLorentzVector photonVec(photon->px(), photon->py(), photon->pz(), photon->energy() );
    for(auto& parton : genParticles){

        //only compare photon to ME partons
        if(parton.status() != 23) continue;

        //make sure parton is a parton
        unsigned partonId = abs( parton.pdgId() );
        if( ! ( (partonId == 21) || (partonId > 0 && partonId < 7) ) ) continue;
        
        //check separation of photon to parton
        TLorentzVector partonVec(parton.px(), parton.py(), parton.pz(), parton.energy() );
        if( photonVec.DeltaR(partonVec) < 0.05){
            return 1;
        }
    }
    return 0;
}

bool GenTools::isPrompt(const reco::GenParticle& gen, const std::vector<reco::GenParticle>& genParticles){
    const reco::GenParticle* mom = getMother(gen, genParticles);
    if(abs(mom->pdgId()) == 15 && mom->isPromptDecayed()) return true;
    return (gen.isPromptFinalState() || gen.isPromptDecayed());
}
bool GenTools::isPromptFinalState(const reco::GenParticle& gen, const std::vector<reco::GenParticle>& genParticles){
    return (gen.isPromptFinalState() );
}
bool GenTools::isPromptDecayed(const reco::GenParticle& gen, const std::vector<reco::GenParticle>& genParticles){
    return (gen.isPromptDecayed());
}

/*
 * Check if only quarks, leptons, bosons or incoming gluons are in the parentagelist
 */
bool GenTools::passParentage(const reco::GenParticle& gen, const std::vector<reco::GenParticle>& genParticles){
    std::set<int> decayChain;
    setDecayChain(gen, genParticles, decayChain);
    if(decayChain.size()==0)                                                    return true;
    if(*(std::max_element(std::begin(decayChain), std::end(decayChain))) > 37)  return false;
    if(*(std::min_element(std::begin(decayChain), std::end(decayChain))) < -37) return false;
    if(not GenTools::hasOnlyIncomingGluonsInChain(gen, genParticles))           return false;
    return true;
}

/*
 * Minimum deltaR between a gen particle and other gen particles with pt > ptCut
 * This could be used in trying to select the madgraph phase space on pythia level
 * The madgraph run card often contains deltaR cuts, such that events with getMinDeltaR(ptCut=5)<0.2
 * are typically out of the phase space of the generated sample
 * [but this is based on tuning and agreement with other groups, so maybe room for more studies/tuning]
 */
double GenTools::getMinDeltaR(const reco::GenParticle& p, const std::vector<reco::GenParticle>& genParticles, float ptCut){
    double minDeltaR = 10;
    for(auto& q : genParticles){
        if(q.pt() < ptCut)                                                       continue;
        if(q.status() != 1)                                                      continue;
        if(fabs(p.pt()-q.pt()) < 0.0001)                                         continue; // same particle
        if(abs(q.pdgId()) == 12 or abs(q.pdgId()) == 14 or abs(q.pdgId()) == 16) continue;
        minDeltaR = std::min(minDeltaR, deltaR(p.eta(), p.phi(), q.eta(), q.phi()));
    }
    return minDeltaR;
}

/*
 * Geometric matching for leptons
 */
bool GenTools::considerForMatching(const reco::Candidate& reco, const reco::GenParticle& gen, const bool differentId){
    if(abs(gen.pdgId()) != 22 or !differentId){                //if genparticle is not a photon, or differentId=false
      if(abs(reco.pdgId()) != abs(gen.pdgId())) return false;  //only consider if gen particle has same pdgId as reco particle
    }
    if(abs(reco.pdgId()) == 15 && abs(gen.pdgId()) == 15) return gen.status() == 2 && gen.isLastCopy();
    return gen.status() == 1;
}

const reco::GenParticle* GenTools::geometricMatch(const reco::Candidate& reco, const std::vector<reco::GenParticle>& genParticles, const bool differentId){
    reco::GenParticle const* match = nullptr;
    TLorentzVector recoV(reco.px(), reco.py(), reco.pz(), reco.energy());
    double minDeltaR = 99999.;
    for(auto genIt = genParticles.cbegin(); genIt != genParticles.cend(); ++genIt){
        if(considerForMatching(reco, *genIt, differentId) ){
            TLorentzVector genV(genIt->px(), genIt->py(), genIt->pz(), genIt->energy());
            double deltaR = recoV.DeltaR(genV);
            if(deltaR < minDeltaR){
                minDeltaR = deltaR;
                match = &*genIt;
            }
        }
    } 
    if(minDeltaR > 0.2){
      if(!differentId) match = geometricMatch(reco, genParticles, true);
      else             return nullptr;
    }
    return match;
}
