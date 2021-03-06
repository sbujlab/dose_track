#include "remollGenericDetector.hh"
#include "G4SDManager.hh"
#include "G4ThreeVector.hh"

#include "remollVUserTrackInformation.hh"
#include "remollEvent.hh"
#include "remollGenericDetectorHit.hh"

#include <unordered_map>

remollGenericDetector::remollGenericDetector( G4String name, G4int detnum ) : G4VSensitiveDetector(name){
    char colname[255];

    fDetNo = detnum;
    assert( fDetNo > 0 );

//    fTrackSecondaries = false;
    fTrackSecondaries = true;

    sprintf(colname, "genhit_%d", detnum);
    collectionName.insert(G4String(colname));

    sprintf(colname, "gensum_%d", detnum);
    collectionName.insert(G4String(colname));

    fHCID = -1;
    fSCID = -1;
}

remollGenericDetector::~remollGenericDetector(){
}

void remollGenericDetector::Initialize(G4HCofThisEvent *){

    fHitColl = new remollGenericDetectorHitsCollection( SensitiveDetectorName, collectionName[0] );
    fSumColl = new remollGenericDetectorSumCollection ( SensitiveDetectorName, collectionName[1] );

    fSumMap.clear();
}

///////////////////////////////////////////////////////////////////////

G4bool remollGenericDetector::ProcessHits( G4Step *step, G4TouchableHistory *){
    G4bool badedep = false;
    G4bool badhit  = false;

    // Get touchable volume info
    G4TouchableHistory *hist = 
	      (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());
    //G4int  copyID = hist->GetVolume(1)->GetCopyNo();//return the copy id of the parent volume
    G4int  copyID = hist->GetVolume()->GetCopyNo();//return the copy id of the logical volume

    G4StepPoint *prestep = step->GetPreStepPoint();
    //G4StepPoint *poststep = step->GetPostStepPoint();
    G4Track     *track   = step->GetTrack(); // FIXME Track will store all of my interesting information from here on out.

    G4double edep = step->GetTotalEnergyDeposit();

    // We're just going to record primary particles and things
    // that have just entered our boundary
    //the following condition ensure that not all the hits are recorded. This will reflect in the energy deposit sum from the hits compared to the energy deposit from the hit sum detectors.
    badhit = true;
    if( track->GetCreatorProcess() == 0 || 
              (prestep->GetStepStatus() == fGeomBoundary && fTrackSecondaries)){
        badhit = false;
    }
    //badhit = false;

    //  Make pointer to new hit if it's a valid track
    remollGenericDetectorHit *thishit;
    if( !badhit ){
        thishit = new remollGenericDetectorHit(fDetNo, copyID);
        fHitColl->insert( thishit );
    } 

    //  Get pointer to our sum  /////////////////////////
    remollGenericDetectorSum *thissum = NULL;

    if( !fSumMap.count(copyID) ){
	      if( edep > 0.0 ){
            thissum = new remollGenericDetectorSum(fDetNo, copyID);
            fSumMap[copyID] = thissum;
            fSumColl->insert( thissum );
        } else {
            badedep = true;
	      }
    } else {
	      thissum = fSumMap[copyID];
    } 
    /////////////////////////////////////////////////////

    // Do the actual data grabbing

    if( !badedep ){
	      // This is all we need to do for the sum
        thissum->AddEDep( track->GetDefinition()->GetPDGEncoding(), prestep->GetPosition(), edep );
    }

    if( !badhit ){
	      // Hit
//	      G4cout << "Hit registered" << G4endl;
        thishit->f3X = prestep->GetPosition();
	      thishit->f3V = track->GetVertexPosition();
	      thishit->f3P = track->GetMomentum();

        // FIXME Plan:
        // get the data from the tracks user variables
        remollVUserTrackInformation* trackInfo = (remollVUserTrackInformation*)(track->GetUserInformation());
//        trackInfo->Print();
//        G4cout << "LastSigVertdE = " << trackInfo->GetLastSigVertdE() << G4endl;
//        G4cout << "LastSigVertdEDep = " << trackInfo->GetLastSigVertdEDep() << G4endl;
//        G4cout << "LastSigVertdTh = " << trackInfo->GetLastSigVertdTh() << G4endl;
//        G4cout << "LastSigVertdPos = " << trackInfo->GetLastSigVertPos() << G4endl;
        thishit->fDeltaE  = trackInfo->GetLastSigVertdE(); // NEW
        thishit->fDeltaEDep = trackInfo->GetLastSigVertdEDep(); // NEW
        thishit->fDeltaTh = trackInfo->GetLastSigVertdTh(); // NEW
        thishit->fLastPos = trackInfo->GetLastSigVertPos(); // NEW

	      thishit->fP = track->GetMomentum().mag();
	      thishit->fE = track->GetTotalEnergy();
	      thishit->fM = track->GetDefinition()->GetPDGMass();

	      thishit->fTrID  = track->GetTrackID();
	      thishit->fmTrID = track->GetParentID();
	      thishit->fPID   = track->GetDefinition()->GetPDGEncoding();
	      thishit->fEdep  = edep; 
	      // FIXME - Enumerate encodings
	      thishit->fGen   = (long int) track->GetCreatorProcess();
    }

    return !badedep && !badhit;
}

///////////////////////////////////////////////////////////////////////

void remollGenericDetector::EndOfEvent(G4HCofThisEvent*HCE) {
    G4SDManager *sdman = G4SDManager::GetSDMpointer();

    if(fHCID<0){ fHCID = sdman->GetCollectionID(collectionName[0]); }
    if(fSCID<0){ fSCID = sdman->GetCollectionID(collectionName[1]); }

    HCE->AddHitsCollection( fHCID, fHitColl );
    HCE->AddHitsCollection( fSCID, fSumColl );

    return;
}


