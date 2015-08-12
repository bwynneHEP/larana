/////////////////////////////////////////////////////////////////////////////
/// Class:       MCTruthT0Matching
/// Module Type: producer
/// File:        MCTruthT0Matching_module.cc
///
/// Author:         Thomas Karl Warburton
/// E-mail address: k.warburton@sheffield.ac.uk
///
/// Generated at Wed Mar 25 13:54:28 2015 by Thomas Warburton using artmod
/// from cetpkgsupport v1_08_04.
///
/// This module accesses the Monte Carlo Truth information stored in the ART
/// event and matches that with a track. It does this by looping through the
/// tracks in the event and looping through each hit in the track.
/// For each hit it uses the backtracker service to work out the charge which
/// each MCTruth particle contributed to the total charge desposited for the
/// hit.
/// The MCTruth particle which is ultimately assigned to the track is simply
/// the particle which deposited the most charge.
/// It then stores an ART anab::T0 object which has the following variables;
/// 1) Generation time of the MCTruth particle assigned to track, in ns.
/// 2) The trigger type used to assign T0 (in this case 2 for MCTruth)
/// 3) The Geant4 TrackID of the particle (so can access all MCTrtuh info in
///     subsequent modules).
/// 4) The track number of this track in this event.
///
/// The module has been extended to also associate an anab::T0 object with a
/// recob::Shower. It does this following the same algorithm, where
/// recob::Track has been replaced with recob::Shower. 
///
/// The module takes a reconstructed track as input.
/// The module outputs an anab::T0 object
/////////////////////////////////////////////////////////////////////////////

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/FindManyP.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Event.h" 
#include "art/Persistency/Common/Ptr.h" 
#include "art/Persistency/Common/PtrVector.h" 
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Framework/Services/Optional/TFileService.h" 
#include "art/Framework/Services/Optional/TFileDirectory.h"

#include "art/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <memory>
#include <iostream>
#include <map>
#include <iterator>

// LArSoft
#include "Geometry/Geometry.h"
#include "Geometry/PlaneGeo.h"
#include "Geometry/WireGeo.h"
#include "AnalysisBase/T0.h"
#include "RecoBase/Hit.h"
#include "RecoBase/SpacePoint.h"
#include "RecoBase/Track.h"
#include "RecoBase/Shower.h"
#include "RecoBase/OpFlash.h"
#include "Utilities/AssociationUtil.h"
#include "Utilities/LArProperties.h"
#include "Utilities/DetectorProperties.h"
#include "SimulationBase/MCParticle.h"
#include "SimulationBase/MCTruth.h"
#include "MCCheater/BackTracker.h"
#include "RawData/ExternalTrigger.h"
#include "SimpleTypesAndConstants/PhysicalConstants.h"
#include "AnalysisBase/ParticleID.h"

// ROOT
#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"

namespace lbne {
  class PhotonCounterT0Matching;
}

class lbne::PhotonCounterT0Matching : public art::EDProducer {
public:
  explicit PhotonCounterT0Matching(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  PhotonCounterT0Matching(PhotonCounterT0Matching const &) = delete;
  PhotonCounterT0Matching(PhotonCounterT0Matching &&) = delete;
  PhotonCounterT0Matching & operator = (PhotonCounterT0Matching const &) = delete; 
 PhotonCounterT0Matching & operator = (PhotonCounterT0Matching &&) = delete;

  // Required functions.
  void produce(art::Event & e) override;

  // Selected optional functions.
  void beginJob() override;
  void reconfigure(fhicl::ParameterSet const & p) override;


private:
  // Internal functions.....
  void TrackProp ( double TrackStart_X, double TrackEnd_X, double &TrackLength_X, double &TrackCentre_X,
		   double TrackStart_Y, double TrackEnd_Y, double &TrackLength_Y, double &TrackCentre_Y,
		   double TrackStart_Z, double TrackEnd_Z, double &TrackLength_Z, double &TrackCentre_Z,
		   double trkTimeStart, double trkTimeEnd, double &trkTimeLengh , double &trkTimeCentre,
		   double &TrackLength);
  double DistFromPoint ( double StartY, double EndY, double StartZ, double EndZ, double PointY, double PointZ );

  // Params got from fcl file.......
  std::string fTrackModuleLabel;
  std::string fShowerModuleLabel;
  std::string fHitsModuleLabel;
  std::string fFlashModuleLabel;
  std::string fTruthT0ModuleLabel;
  double fPredConst;
  double fPredSlope;
  int    fDriftTicks;

  // Variables used in module.......
  std::vector<double> trackStart;
  std::vector<double> trackEnd;
  double TrackLength_X, TrackCentre_X, BestTrackCentre_X;
  double TrackLength_Y, TrackCentre_Y, BestTrackCentre_Y;
  double TrackLength_Z, TrackCentre_Z, BestTrackCentre_Z;
  double trkTimeStart, trkTimeEnd, trkTimeLengh; 

  double trkTimeCentre, BesttrkTimeCentre;
  double TrackLength, BestTrackLength;
  double PredictedX, BestPredictedX;
  double TimeSepPredX, BestTimeSepPredX;
  double DeltaPredX, BestDeltaPredX;
  double minYZSep, BestminYZSep;
  double FitParam, BestFitParam;
  double FlashTime, BestFlashTime;
  double TimeSep, BestTimeSep;
  int BestFlash;
  int FlashTriggerType=1;
  
  double YZSep, MCTruthT0; 
  // Histograms in TFS branches
  TTree* fTree;
  TH2D* hPredX_T;
  TH2D* hPredX_PE;
  TH2D* hPredX_T_PE;
  TH2D* hdeltaX_deltaYZ;
  TH2D* hdeltaYZ_Length;
  TH2D* hFitParam_Length;
  TH2D* hPhotonT0_MCT0;
};


lbne::PhotonCounterT0Matching::PhotonCounterT0Matching(fhicl::ParameterSet const & p)
// :
// Initialize member data here, if know don't want to reconfigure on the fly
{
  // Call appropriate produces<>() functions here.
  produces< std::vector<anab::T0>               >();
  produces< art::Assns<recob::Track , anab::T0> >();
  produces< art::Assns<recob::Shower, anab::T0> > ();
  reconfigure(p);
}

void lbne::PhotonCounterT0Matching::reconfigure(fhicl::ParameterSet const & p)
{
  // Implementation of optional member function here.
  fTrackModuleLabel   = (p.get< std::string > ("TrackModuleLabel" ) );
  fShowerModuleLabel  = (p.get< std::string > ("ShowerModuleLabel") );
  fHitsModuleLabel    = (p.get< std::string > ("HitsModuleLabel"  ) );
  fFlashModuleLabel   = (p.get< std::string > ("FlashModuleLabel" ) );
  fTruthT0ModuleLabel = (p.get< std::string > ("TruthT0ModuleLabel"));
  fPredConst  = (p.get< double > ("PredictedConstant" ) );
  fPredSlope  = (p.get< double > ("PredictedSlope"    ) );
  fDriftTicks = (p.get< int    > ("DriftTicks"        ) );
  }

void lbne::PhotonCounterT0Matching::beginJob()
{
  // Implementation of optional member function here.
  art::ServiceHandle<art::TFileService> tfs;
  fTree = tfs->make<TTree>("PhotonCounterT0Matching","PhotonCounterT0");
  fTree->Branch("TrackCentre_X",&BestTrackCentre_X,"TrackCentre_X/D");
  fTree->Branch("PredictedX"   ,&BestPredictedX   ,"PredictedX/D");
  fTree->Branch("TrackTimeCent",&BesttrkTimeCentre,"TimeSepPredX/D");
  fTree->Branch("FlashTime"    ,&BestFlashTime    ,"FlashTime/D");
  fTree->Branch("TimeSep"      ,&BestTimeSep      ,"TimeSep/D");
  fTree->Branch("TimeSepPredX" ,&BestTimeSepPredX ,"TimeSepPredX/D");
  fTree->Branch("minYZSep"     ,&BestminYZSep     ,"minYZSep/D");
  fTree->Branch("FitParam"     ,&BestFitParam     ,"FitParam/D");
  fTree->Branch("MCTruthT0"    ,&MCTruthT0        ,"MCTruthT0/D");
  
  hPredX_T  = tfs->make<TH2D>("hPredX_T" ,"Predicted X from timing information against reconstructed X; Reconstructed X (cm); Predicted X (cm)", 30, 0, 300, 30, 0, 300 );
  hPredX_PE = tfs->make<TH2D>("hPredX_PE","Predicted X from PE information against reconstructed X; Reconstructed X (cm); Predicted X (cm)"    , 30, 0, 300, 30, 0, 300 );
  hPredX_T_PE = tfs->make<TH2D>("hPredX_T_PE", 
				"Predicted X position from time and PE information; Predicted X from timing information (cm); Predicted X from PE information",
				30, 0, 300, 30, 0, 300);
  hdeltaX_deltaYZ = tfs->make<TH2D>("hdeltaX_deltaYZ", 
				    "Difference between X predicted from PE's and T agaisnt distance of flash from track in YZ; Difference in X predicted from PE's and T (cm); Distance of flash from track in YZ (cm)",
				    20, 0, 200, 20, 0, 100);
  hdeltaYZ_Length = tfs->make<TH2D>("hdeltaYZ_Length",
				    "Distance of flash from track against track length; Distance from flash to track (cm); Track length (cm)",
				    20, 0, 100, 30, 0, 300); 
  hFitParam_Length = tfs->make<TH2D>("hFitParam_Length", "How fit correlates with track length; Fit correlation; Track Length (cm)", 50, 0, 250, 30, 0, 300);
  hPhotonT0_MCT0   = tfs->make<TH2D>("hPhotonT0_MCT0"  , "Comparing Photon Counter reconstructed T0 against MCTruth T0; Photon Counter T0 (ns); MCTruthT0 T0 (ns)", 100, -3200, 32000, 100, -3200, 32000);
}

void lbne::PhotonCounterT0Matching::produce(art::Event & evt)
{
  // Access art services...
  art::ServiceHandle<geo::Geometry> geom;
  art::ServiceHandle<util::LArProperties> larprop;
  art::ServiceHandle<util::DetectorProperties> detprop;
  art::ServiceHandle<util::TimeService> timeservice;
  art::ServiceHandle<cheat::BackTracker> bt;

  //TrackList handle
  art::Handle< std::vector<recob::Track> > trackListHandle;
  std::vector<art::Ptr<recob::Track> > tracklist;
  if (evt.getByLabel(fTrackModuleLabel,trackListHandle))
    art::fill_ptr_vector(tracklist, trackListHandle); 
  
  //ShowerList handle
  art::Handle< std::vector<recob::Shower> > showerListHandle;
  std::vector<art::Ptr<recob::Shower> > showerlist;
  if (evt.getByLabel(fShowerModuleLabel,showerListHandle))
    art::fill_ptr_vector(showerlist, showerListHandle); 
  
  //HitList Handle
  art::Handle< std::vector<recob::Hit> > hitListHandle;
  std::vector<art::Ptr<recob::Hit> > hitlist;
  if (evt.getByLabel(fHitsModuleLabel,hitListHandle))
      art::fill_ptr_vector(hitlist, hitListHandle);

  //FlashList Handle
  art::Handle< std::vector<recob::OpFlash> > flashListHandle;
  std::vector<art::Ptr<recob::OpFlash> > flashlist;
  if (evt.getByLabel(fFlashModuleLabel, flashListHandle))
    art::fill_ptr_vector(flashlist, flashListHandle);

  // Create anab::T0 objects and make association with recob::Track
  
  std::unique_ptr< std::vector<anab::T0> > T0col( new std::vector<anab::T0>);
  std::unique_ptr< art::Assns<recob::Track, anab::T0> > Trackassn( new art::Assns<recob::Track, anab::T0>);
  std::unique_ptr< art::Assns<recob::Shower, anab::T0> > Showerassn( new art::Assns<recob::Shower, anab::T0>);
 
  if (trackListHandle.isValid() && flashListHandle.isValid() ){
    //Access tracks and hits
    art::FindManyP<recob::Hit> fmtht(trackListHandle, evt, fTrackModuleLabel);
    art::FindMany<anab::T0>    fmtruth(trackListHandle, evt, fTruthT0ModuleLabel);

    size_t NTracks  = tracklist.size();
    size_t NFlashes = flashlist.size();
    
    std::cout << "There were " << NTracks << " tracks and " << NFlashes << " flashes in this event." << std::endl;
    
    // Now to access PhotonCounter for each track... 
    for(size_t iTrk=0; iTrk < NTracks; ++iTrk) { 
      std::cout << "\n New Track " << (int)iTrk << std::endl;
      BestFlashTime = BestFitParam = BestTrackCentre_X = BestTrackLength = 0;
      BestTimeSepPredX = BestPredictedX = BestDeltaPredX = BestminYZSep = MCTruthT0 = 0;
      bool ValidTrack = false;
      tracklist[iTrk]->Extent(trackStart,trackEnd); 
      std::vector< art::Ptr<recob::Hit> > allHits = fmtht.at(iTrk);
      size_t nHits = allHits.size();
      trkTimeStart = allHits[nHits-1]->PeakTime();
      trkTimeEnd   = allHits[0]->PeakTime();
      
      TrackProp ( trackStart[0], trackEnd[0], TrackLength_X, TrackCentre_X,
		  trackStart[1], trackEnd[1], TrackLength_Y, TrackCentre_Y,
		  trackStart[2], trackEnd[2], TrackLength_Z, TrackCentre_Z,
		  trkTimeStart , trkTimeEnd , trkTimeLengh , trkTimeCentre,
		  TrackLength);     

      std::cout << trackStart[0] << " " << trackEnd[0] << " " << TrackLength_X << " " << TrackCentre_X 
		<< "\n" << trackStart[1] << " " << trackEnd[1] << " " << TrackLength_Y << " " << TrackCentre_Y
		<< "\n" << trackStart[2] << " " << trackEnd[2] << " " << TrackLength_Z << " " << TrackCentre_Z
		<< "\n" << trkTimeStart  << " " << trkTimeEnd  << " " << trkTimeLengh  << " " << trkTimeCentre
		<< std::endl;
      // Loop over flashes
      for ( size_t iFlash=0; iFlash < NFlashes; ++iFlash ) {
	//Reset some flash specific quantities
	YZSep = minYZSep = 0;
	FlashTime = TimeSep = 0;
	PredictedX = TimeSepPredX = DeltaPredX = FitParam = 0;
	// Check flash could be caused by track...
	FlashTime = flashlist[iFlash]->Time();
	TimeSep = trkTimeCentre - FlashTime;
	if ( TimeSep < 0 || TimeSep > fDriftTicks ) continue;
	// Work out some quantities for this flash...
	PredictedX   = exp ( fPredConst + ( fPredSlope * flashlist[iFlash]->TotalPE() ) );
	TimeSepPredX = TimeSep * larprop->DriftVelocity() * 0.5;
	DeltaPredX   = fabs(TimeSepPredX-PredictedX);

	for ( size_t Point = 1; Point < tracklist[iTrk]->NumberTrajectoryPoints(); ++Point ) {
	  TVector3 NewPoint  = tracklist[iTrk]->LocationAtPoint(Point);
	  TVector3 PrevPoint = tracklist[iTrk]->LocationAtPoint(Point-1);
	  YZSep = DistFromPoint ( NewPoint[1], PrevPoint[1], NewPoint[2], PrevPoint[2], 
				  flashlist[iFlash]->YCenter(), flashlist[iFlash]->ZCenter());

	  if ( Point == 1 ) minYZSep = YZSep;
	  if ( YZSep < minYZSep ) minYZSep = YZSep;
	}
	FitParam = pow( ((DeltaPredX*DeltaPredX)+(minYZSep*minYZSep)), 0.5);
	//----FLASH INFO-----
	//std::cout << "\nFlash " << (int)iFlash << " " << TrackCentre_X << ", " << TimeSepPredX << " - " << PredictedX << " = " << DeltaPredX << ", " << minYZSep << " -> " << FitParam << std::endl; 
	//----Select best flash------
	//double YFitRegion = (-1 * DeltaPredX ) + 80;
	//if ( minYZSep > YFitRegion ) continue;
	if ( FitParam < BestFitParam || (int)iFlash == 0) {
	  ValidTrack        = true;
	  BestFlash         = (int)iFlash;
	  BestFitParam      = FitParam;
	  BestTrackCentre_X = TrackCentre_X;
	  BestTrackLength   = TrackLength;
	  BesttrkTimeCentre = trkTimeCentre;
	  BestTimeSepPredX  = TimeSepPredX;
	  BestPredictedX    = PredictedX;
	  BestDeltaPredX    = DeltaPredX;
	  BestminYZSep      = minYZSep;
	  BestFlashTime     = FlashTime;
	  BestTimeSep       = TimeSep;
	} // Find best Flash
      } // Loop over Flashes
      
      // ---- Now Make association and fill TTree/Histos with the best matched flash.....
      if (ValidTrack) {

	// -- Fill Histos -- 
	hPredX_T         ->Fill( BestTrackCentre_X, BestTimeSepPredX );
	hPredX_PE        ->Fill( BestTrackCentre_X, BestPredictedX );
	hPredX_T_PE      ->Fill( BestTimeSepPredX , BestPredictedX );
	hdeltaX_deltaYZ  ->Fill( BestDeltaPredX, BestminYZSep );
	hdeltaYZ_Length  ->Fill( BestminYZSep, BestTrackLength );
	hFitParam_Length ->Fill( BestFitParam, BestTrackLength );
	// ------ Compare Photon Matched to MCTruth Matched -------
	if ( fmtruth.isValid() ) {
	  std::vector<const anab::T0*> T0s = fmtruth.at((int)iTrk);
	  for ( size_t i=0; i<T0s.size(); ++i) {
	    MCTruthT0 = T0s[i]->Time() / (timeservice->TPCClock().TickPeriod()*1e3);
	    hPhotonT0_MCT0 ->Fill( BestFlashTime, MCTruthT0 );
	    //std::cout << "Size " << T0s.size() << " " << MCTruthT0 << " " << BestFlashTime << std::endl;	
	  }
	}
	// -- Fill TTree --
	fTree->Fill();
	//Make Association
	T0col->push_back(anab::T0(BestFlashTime,
				  FlashTriggerType,
				  (int)BestFlash,
				  (*T0col).size()
				  ));
	util::CreateAssn(*this, evt, *T0col, tracklist[iTrk], *Trackassn);
      } // Valid Track
    } // Loop over tracks   
  }
  /* // ------------------------------------------------- SHOWER STUFF -------------------------------------------------
  if (showerListHandle.isValid()){
    art::FindManyP<recob::Hit> fmsht(showerListHandle,evt, fShowerModuleLabel);
    // Now Loop over showers....
    size_t NShowers = showerlist.size();
    for (size_t Shower = 0; Shower < NShowers; ++Shower) {
      ShowerMatchID     = 0;
      ShowerID          = 0;
      ShowerT0          = 0;
      std::vector< art::Ptr<recob::Hit> > allHits = fmsht.at(Shower);
      
      std::map<int,double> showeride;
      for(size_t h = 0; h < allHits.size(); ++h){
	art::Ptr<recob::Hit> hit = allHits[h];
	std::vector<sim::IDE> ides;
	std::vector<sim::TrackIDE> TrackIDs = bt->HitToTrackID(hit);
	
	for(size_t e = 0; e < TrackIDs.size(); ++e){
	  showeride[TrackIDs[e].trackID] += TrackIDs[e].energy;
	}
      }
      // Work out which IDE despoited the most charge in the hit if there was more than one.
      double maxe = -1;
      double tote = 0;
      for (std::map<int,double>::iterator ii = showeride.begin(); ii!=showeride.end(); ++ii){
	tote += ii->second;
	if ((ii->second)>maxe){
	  maxe = ii->second;
	  ShowerID = ii->first;
	}
      }
      // Now have MCParticle trackID corresponding to shower, so get PdG code and T0 etc.
      const simb::MCParticle *particle = bt->TrackIDToParticle(ShowerID);
      ShowerT0 = particle->T();
      ShowerID = particle->TrackId();
      ShowerTriggerType = 1; // Using PhotonCounter as trigger, so tigger type is 1.
      
      T0col->push_back(anab::T0(ShowerT0,
				ShowerTriggerType,
				ShowerID,
				(*T0col).size()
				));
      util::CreateAssn(*this, evt, *T0col, showerlist[Shower], *Showerassn);    
    }// Loop over showers
  }
  */ 
  evt.put(std::move(T0col));
  evt.put(std::move(Trackassn));
  evt.put(std::move(Showerassn));
  
} // Produce
// ----------------------------------------------------------------------------------------------------------------------------
void lbne::PhotonCounterT0Matching::TrackProp ( double TrackStart_X, double TrackEnd_X, double &TrackLength_X, double &TrackCentre_X,
						double TrackStart_Y, double TrackEnd_Y, double &TrackLength_Y, double &TrackCentre_Y,
						double TrackStart_Z, double TrackEnd_Z, double &TrackLength_Z, double &TrackCentre_Z,
						double trkTimeStart, double trkTimeEnd, double &trkTimeLengh , double &trkTimeCentre,
						double &TrackLength) {
  
  TrackLength_X = fabs ( TrackEnd_X - TrackStart_X );
  if ( TrackStart_X < TrackEnd_X ) TrackCentre_X = TrackStart_X + 0.5*TrackLength_X;
  else TrackCentre_X = TrackStart_X - 0.5*TrackLength_X;
  
  TrackLength_Y = fabs ( TrackEnd_Y - TrackStart_Y );
  if ( TrackStart_Y < TrackEnd_Y ) TrackCentre_Y = TrackStart_Y + 0.5*TrackLength_Y;
  else TrackCentre_Y = TrackStart_Y - 0.5*TrackLength_Y;
  
  TrackLength_Z = fabs ( TrackEnd_Z - TrackStart_Z );
  if ( TrackStart_Z < TrackEnd_Z ) TrackCentre_Z = TrackStart_Z + 0.5*TrackLength_Z;
  else TrackCentre_Z = TrackStart_Z - 0.5*TrackLength_Z;

  trkTimeLengh   = trkTimeEnd - trkTimeStart;
  trkTimeCentre = trkTimeStart + 0.5*trkTimeLengh;

  TrackLength =  pow( pow((TrackEnd_X-TrackStart_X), 2) + pow((TrackEnd_Y-TrackStart_Y), 2) + pow((TrackEnd_Z-TrackStart_Z), 2) , 0.5);
  
  return;
}
// ----------------------------------------------------------------------------------------------------------------------------
double lbne::PhotonCounterT0Matching::DistFromPoint ( double StartY, double EndY, double StartZ, double EndZ, double PointY, double PointZ ) {
  double Length = hypot ( fabs( EndY - StartY), fabs ( EndZ - StartZ ) );
  //  double distance = (double)((point.x - line_start.x) * (line_end.y - line_start.y) - (point.y - line_start.y) * (line_end.x - line_start.x)) / normalLength;
  double distance = ( (PointZ - StartZ) * (EndY - StartY) - (PointY - StartY) * (EndZ - StartZ) ) / Length;
  return fabs(distance);
}
// ----------------------------------------------------------------------------------------------------------------------------
DEFINE_ART_MODULE(lbne::PhotonCounterT0Matching)
    
