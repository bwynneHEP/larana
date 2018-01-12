/////////////////////////////////////////////////////////////////////////////
/// Class:       MCParticleHitMatching
/// Module Type: producer
/// File:        MCParticleHitMatching_module.cc
///
/// Author:         Wesley Ketchum and Yun-Tse Tsai
/// E-mail address: wketchum@fnal.gov and yuntse@stanford.edu
///
/// This module uses the larsoft backtracker to match hits to truth-level
/// MCParticles (from LArG4 output). It originated from MCTruthT0Matching
/// module, and follows a similar strategy. All MCParticles matching the hit
/// are associated, with information on the amount of contributing energy and
/// number of electrons stored in assn metadata. 
/// 
/// Input: MCParticles (via Backtracker) and recob::Hit collection
/// Output: recob::Hit/simb::MCParticle assns, with BackTrackerHitMatchingData.
///
/////////////////////////////////////////////////////////////////////////////

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Event.h" 

#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <memory>
#include <iostream>
#include <map>
#include <iterator>

// LArSoft
#include "larana/T0Finder/AssociationsTools/IHitParticleAssociations.h"
#include "larana/T0Finder/AssociationsTools/DirectHitParticleAssns_tool.h"
#include "larana/T0Finder/AssociationsTools/IndirectHitParticleAssns_tool.h"

namespace t0 {
  class MCParticleHitMatching;
}

class t0::MCParticleHitMatching : public art::EDProducer {
public:
    explicit MCParticleHitMatching(fhicl::ParameterSet const & p);
    // The destructor generated by the compiler is fine for classes
    // without bare pointers or other resource use.

    // Plugins should not be copied or assigned.
    MCParticleHitMatching(MCParticleHitMatching const &) = delete;
    MCParticleHitMatching(MCParticleHitMatching &&) = delete;
    MCParticleHitMatching & operator = (MCParticleHitMatching const &) = delete;
    MCParticleHitMatching & operator = (MCParticleHitMatching &&) = delete;

    // Required functions.
    void produce(art::Event & e) override;

    // Selected optional functions.
    void beginJob() override;
    void reconfigure(fhicl::ParameterSet const & p) override;

private:
    
    // For keeping track of the replacement backtracker
    std::unique_ptr<IHitParticleAssociations> fHitParticleAssociations;
    bool                                      fOverrideRealData;      ///< if real data, tell it to run anyway (=0)

};


t0::MCParticleHitMatching::MCParticleHitMatching(fhicl::ParameterSet const & pset)
{
    reconfigure(pset);
    produces< art::Assns<recob::Hit , simb::MCParticle, anab::BackTrackerHitMatchingData > > ();
}

void t0::MCParticleHitMatching::reconfigure(fhicl::ParameterSet const & pset)
{
    fOverrideRealData     = pset.get<bool       >("OverrideRealData", false);
    // Get the tool for MC Truth matching
    const fhicl::ParameterSet& hitPartAssnsParams = pset.get<fhicl::ParameterSet>("HitParticleAssociations");
    
    if (hitPartAssnsParams.get<std::string>("tool_type") == "DirectHitParticleAssns")
    {
        fHitParticleAssociations = std::unique_ptr<IHitParticleAssociations>(new DirectHitParticleAssns(hitPartAssnsParams));
    }
    else
    {
        fHitParticleAssociations = std::unique_ptr<IHitParticleAssociations>(new IndirectHitParticleAssns(hitPartAssnsParams));
    }
}

void t0::MCParticleHitMatching::beginJob()
{
}

void t0::MCParticleHitMatching::produce(art::Event & evt)
{
    if(evt.isRealData() && !fOverrideRealData) return;
    std::unique_ptr<HitParticleAssociations> MCPartHitassn( new HitParticleAssociations);
    
    fHitParticleAssociations->CreateHitParticleAssociations(evt, MCPartHitassn.get());

    evt.put(std::move(MCPartHitassn));
} // Produce

DEFINE_ART_MODULE(t0::MCParticleHitMatching)
    
