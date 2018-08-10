#ifndef LYSimDetectorConstruction_h
#define LYSimDetectorConstruction_h 1

#include "LYSimPMTSD.hh"

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class LYSimDetectorMessenger;
class G4Box;
class G4LogicalVolume;
class G4Material;
class G4OpticalSurface;
class LYSimPMTSD;

class LYSimDetectorConstruction : public G4VUserDetectorConstruction
{
public:
    LYSimDetectorConstruction();
    virtual ~LYSimDetectorConstruction();
    virtual G4VPhysicalVolume* Construct();
    void UpdateGeometry();


    //Toggle Tyvek on or off
    void SetWrappingToggle (G4bool toggle) {wrapping_toggle = toggle;}
    G4bool GetWrappingToggle () {return wrapping_toggle;}

    //Set ref index of scint pmt gap
    void  SetRefIndex (G4double iRefIndex) {RefIndex = iRefIndex;}
    G4double GetRefIndex () {return RefIndex;}

    void  SetScintThickness (G4double iscint_thickness) {scint_thickness = iscint_thickness;}
    G4double GetScintThickness() {return scint_thickness;}

    void  SetScintSizeX (G4double iscint_sizeX) {scint_sizeX = iscint_sizeX;}
    G4double GetScintSizeX() {return scint_sizeX;}

    void  SetScintSizeY (G4double iscint_sizeY) {scint_sizeY = iscint_sizeY;}
    G4double GetScintSizeY() {return scint_sizeY;}

    void  SetScintPMTGapThickness (G4double ithickness) {ScintPMT_gap = ithickness;}
    G4double GetScintPMTGapThickness() {return ScintPMT_gap;}

    void  SetTileAbsLength (G4double iAbsLength) {tileAbsLength = iAbsLength;}
    G4double GetTileAbsLength() {return tileAbsLength;}

    void  SetInducedMuTile(G4double value) {inducedMuTile = value;}
    G4double GetInducedMuTile() {return inducedMuTile;}


    G4double GetMinZ() {return minZposition;}
    G4double GetMaxZ() {return maxZposition;}


private:
    //Subfunctions for cleaner code
    void DefineMaterials();
    void DefineSurfaces();
    void SetDefaults(); //*-*Doesn't seem to work right now
    G4VPhysicalVolume* ConstructDetector();
    G4VSolid* ConstructTileSolid(const G4String& name, 
                                 G4double angle1, //angle measured ccw from y axis for the side at -x
                                 G4double angle2, //angle measured ccw from y axis for the side at +x
                                 G4double Dx2, //length along x of side at y=+Dy
                                 G4double Dy, //length along y
                                 G4double Dz, //length along z
                                 G4ThreeVector& centerCoord); //coordinate of center of gravity w.r.t. corner 0 at -x, -y
    //Pointer to detector messenger class
    LYSimDetectorMessenger* fdetectorMessenger;

    G4bool fUpdated;

    //Contains the minimum and maximum Z-position of the previously defined volume
    //Use this variable to define volume successively along the z-axis
    //(max for +z direction, min for -z direction)
    //Set value after defining a physical volume
    G4double minZposition;
    G4double maxZposition;

    //Pointers to main volumes
    G4Box* solidWorld;
    G4LogicalVolume* logicWorld;
    G4VPhysicalVolume* physWorld;

  //pointer for wrapping
  G4LogicalVolume* logicWWW;

    //Pointers to materials
    G4Material* fVacuum;
    G4Material* fAir;
    G4Material* fSiO2;
    G4Material* fPolystyrene;
    G4Material* fPolycarbonate;
    G4Material* fFiberCore;
    G4Material* fFiberInnerCladding;
    G4Material* fFiberOuterCladding;
    G4Material* fLYSO;
    G4Material* fGaAs;
    G4Material* fPyrex;
    G4Material* fWater;
    G4Material* fSCSN81;
    G4Material* fEJ200;
    G4Material* fEJ260;
    G4Material* fScintPmtGapMat;
    G4Material* fBialkali;

    //Pointers to surfaces
    G4OpticalSurface* fTyvekOpSurface;
    G4OpticalSurface* fIdealTyvekOpSurface;
    G4OpticalSurface* fUnifiedTyvekOpSurface;
    G4OpticalSurface* fUnifiedIdealTyvekOpSurface;
    G4OpticalSurface* fESROpSurface;
    G4OpticalSurface* fPolishedOpSurface;
    G4OpticalSurface* fIdealPolishedOpSurface;
    G4OpticalSurface* fMirrorOpSurface;
    G4OpticalSurface* fIdealMirrorOpSurface;
    G4OpticalSurface* fAbsorbingOpSurface;

    //Pointers for access to Sensitive Detector
    static LYSimPMTSD* fPMTSD;

    //Geometry parameters
    //Important parameters marked with ////
    G4bool wrapping_toggle; //(Tyvek) wrapping around scintillator
  G4double RefIndex; //Refractive index of fiber or scint-PMT gap
    G4double world_sizeX;
    G4double world_sizeY;
    G4double world_sizeZ;
    G4double scint_sizeX;
    G4double scint_sizeY;
    G4double scint_thickness;
    G4double Photocat_sizeX;
    G4double Photocat_sizeY;
    G4double Photocat_thickness;
    G4double ScintPMT_gap; //thickness of gap between scintillator and PMT
  G4double wrapgap;  // distance between tile and wrapping
    G4double tileAbsLength;
    G4double inducedMuTile; //Radiation-induced absorption coefficient (cm^-1) in tile


    G4ThreeVector corners[8];           //stores the corner coordinates of the tile
    G4ThreeVector readout0, readout1;   //location of readout at corner 0/1
  //G4ThreeVector mirror0, mirror1;     //location of mirrors at corner 0/1
    G4int readoutCorner;                //determines location of fiber readout (0:corner 0, 1:corner 1)
    };

#endif

