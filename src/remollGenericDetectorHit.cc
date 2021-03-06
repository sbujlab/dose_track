#include "remollGenericDetectorHit.hh"

G4Allocator<remollGenericDetectorHit> remollGenericDetectorHitAllocator;

remollGenericDetectorHit::remollGenericDetectorHit(G4int det, G4int copy){
    fDetID  = det;
    fCopyID = copy;

    f3X = G4ThreeVector(-1e9, -1e9, -1e9);
    f3P = G4ThreeVector(-1e9, -1e9, -1e9);
    f3V = G4ThreeVector(-1e9, -1e9, -1e9);

    fLastPos = G4ThreeVector(-1e9, -1e9, -1e9);   // NEW
    fDeltaE  = -1.0;                              // NEW
    fDeltaEDep  = -1.0;                           // NEW
    fDeltaTh = -1.0;                              // NEW
    
    fP  = -1.0;
    fE  = -1.0;
    fM  = -1.0;

    fTrID  = -1;
    fPID   = (G4int) 1e9;
    fmTrID = -1;
    fEdep = 0;
    fGen   = 1;
}

remollGenericDetectorHit::~remollGenericDetectorHit(){
}

remollGenericDetectorHit::remollGenericDetectorHit(const remollGenericDetectorHit &right) : G4VHit(){
    // copy constructor

    fDetID  = right.fDetID;
    fCopyID = right.fCopyID;
    f3X     = right.f3X;
    f3P     = right.f3P;
    f3V     = right.f3V;


    fLastPos= right.fLastPos; // NEW
    fDeltaE = right.fDeltaE;  // NEW
    fDeltaEDep = right.fDeltaEDep;  // NEW
    fDeltaTh= right.fDeltaTh; // NEW
    fP      = right.fP;
    fE      = right.fE;
    fM      = right.fM;

    fTrID   = right.fTrID;
    fPID    = right.fPID;
    fmTrID  = right.fmTrID;
    fEdep   = right.fEdep;
    fGen    = right.fGen;
}

const remollGenericDetectorHit& remollGenericDetectorHit::operator =(const remollGenericDetectorHit &right){
    (*this) = right;
    return *this;
}

G4int remollGenericDetectorHit::operator==(const remollGenericDetectorHit &right ) const {
    return (this==&right) ? 1 : 0;
}
