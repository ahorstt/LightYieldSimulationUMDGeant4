#include "LYSimDetectorConstruction.hh"
#include "LYSimDetectorMessenger.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4UnitsTable.hh"
#include "G4SubtractionSolid.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4Torus.hh"
#include "G4Trap.hh"
#include "G4RotationMatrix.hh"
#include "G4IntersectionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4SDManager.hh"
#include "G4GeometryManager.hh"
#include "G4SolidStore.hh"
#include "G4RegionStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"

using std::cos;
using std::sin;
using std::tan;
using std::atan;
using std::exp;
using namespace CLHEP;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LYSimPMTSD* LYSimDetectorConstruction::fPMTSD = NULL;

LYSimDetectorConstruction::LYSimDetectorConstruction()
    : G4VUserDetectorConstruction() 
{ 
    fdetectorMessenger = new LYSimDetectorMessenger(this);

    //    detType = 0; // 0: Tile; 1: rod
    solidWorld = NULL;
    logicWorld = NULL;
    physWorld = NULL;
    logicWWW=NULL;

    fVacuum = fAir = fSiO2 = fPolystyrene = fPolycarbonate = fLYSO = fGaAs = fBialkali = NULL;
    fSCSN81 = fEJ200 = fEJ260 = NULL;
    fScintPmtGapMat = NULL;
    fFiberCore = fFiberInnerCladding = fFiberOuterCladding = NULL;
    fTyvekOpSurface = fIdealTyvekOpSurface = fUnifiedTyvekOpSurface = fUnifiedIdealTyvekOpSurface = NULL;
    fESROpSurface = NULL;
    fPolishedOpSurface = fIdealPolishedOpSurface = NULL;
    fMirrorOpSurface = fIdealMirrorOpSurface = NULL;
    RefIndex = 1.4;
    fUpdated = false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LYSimDetectorConstruction::~LYSimDetectorConstruction()
{ 
    if (fdetectorMessenger) delete fdetectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* LYSimDetectorConstruction::Construct()
{  
    SetDefaults();
    DefineMaterials();
    DefineSurfaces();
    return ConstructDetector();
}

void LYSimDetectorConstruction::SetDefaults()
{
    //Set default parameters
    //fiber_hole_toggle = true;
    wrapping_toggle = true;
    fScintPmtGapMat = fWater;
    // Geometry parameters
    //
    scint_sizeX = 29.65*mm;
    scint_sizeY = 29.65*mm;
    scint_thickness = 2.98*mm;//3.7*mm for SCSN81
    Photocat_sizeX = 1.3*mm;
    Photocat_sizeY = 1.3*mm;
    Photocat_thickness = 0.1*mm; //arbirary thickness for Photocathode
    ScintPMT_gap = 2.0*mm; //rough estimate of air gap between scintillator and PMT face
    wrapgap=0.1*mm;


    tileAbsLength = 380*cm;
    inducedMuTile = 1.e-20/cm;
    readoutCorner = 1;

    //world volume just needs to be big enough to accomodate everything
    world_sizeX = 3*(scint_sizeX);
    world_sizeY = 3*(scint_sizeY);
    world_sizeZ = 30*(scint_thickness);


}

G4VPhysicalVolume* LYSimDetectorConstruction::ConstructDetector()
{
    // Option to switch on/off checking of volumes overlaps
    //
    G4bool checkOverlaps = true;

    //
    // World
    //
    G4Box* solidWorld =
        new G4Box("World",                                                  //its name
                  0.5*world_sizeX, 0.5*world_sizeY, 0.5*world_sizeZ);     //its size
    G4LogicalVolume* logicWorld =
        new G4LogicalVolume(solidWorld, //its solid
                            fAir,       //its material
                            "World");   //its name
    G4VPhysicalVolume* physWorld =
        new G4PVPlacement(0,                     //no rotation
                          G4ThreeVector(),       //at (0,0,0)
                          logicWorld,            //its logical volume
                          "World",               //its name
                          0,                     //its mother  volume
                          false,                 //no boolean operation
                          0,                     //copy number
                          checkOverlaps);        //overlaps checking

    //logicWorld -> SetVisAttributes(new G4VisAttributes(white));
    logicWorld -> SetVisAttributes(G4VisAttributes::Invisible); //MakeInvisible
    logicWWW=logicWorld;


    ///////////////////////////////////////////////
    // wrapping
    /////////////////////////////////////////////////

    if(wrapping_toggle) {    
      G4Box* solidWrap =
	new G4Box("WrapBox",                                           //its name
		  0.5*scint_sizeX+wrapgap, 0.5*scint_sizeY+wrapgap, 0.5*scint_thickness+wrapgap);     //its size
      G4LogicalVolume* logicWrap =
	new G4LogicalVolume(solidWrap,   //its solid
			    fAir,     //its material
			    "Wrap");     //its name
      logicWWW=logicWrap;

      G4ThreeVector WrapOffset(0, 0, 0);
      G4VPhysicalVolume* physWrap = 
	new G4PVPlacement(0,
			  WrapOffset,
			  logicWrap,
			  "Wrap",
			  logicWorld,
			  false,
			  0,
			  checkOverlaps);

      G4LogicalBorderSurface* WrapAirSurface = 
	new G4LogicalBorderSurface("WrapAirSurface",
				   physWrap,
				   physWorld,
				   fESROpSurface);

        //Wrap visualization attributes
      G4VisAttributes * WrapVisAtt = new G4VisAttributes(G4Colour(0.,1.,0.));
      WrapVisAtt->SetForceWireframe(true);
      WrapVisAtt->SetVisibility(true);
      logicWrap->SetVisAttributes(WrapVisAtt);
    
    }
/*
        ////////////////////////////////////////////
        //// tile
        ////////////////////////////////////////////
    G4Box* solidRod =
      new G4Box("RodBox",                                           //its name
		0.5*scint_sizeX, 0.5*scint_sizeY, 0.5*scint_thickness);     //its size
    G4LogicalVolume* logicRod =
      new G4LogicalVolume(solidRod,   //its solid
			  fEJ200,     //its material
			  "Rod");     //its name


    G4ThreeVector RodOffset(0, 0, 0);
    G4VPhysicalVolume* physRod = 
      new G4PVPlacement(0,
                              RodOffset,
                              logicRod,
                              "Rod",
                              logicWWW,
                              false,
                              0,
                              checkOverlaps);

    G4LogicalBorderSurface* RodAirSurface = 
            new G4LogicalBorderSurface("RodAirSurface",
                                       physRod,
                                       physWorld,
                                       fPolishedOpSurface);

        //Rod visualization attributes
    G4VisAttributes * RodVisAtt = new G4VisAttributes(G4Colour(1.,1.,0.));
    RodVisAtt->SetForceWireframe(true);
    RodVisAtt->SetVisibility(true);
    logicRod->SetVisAttributes(RodVisAtt);


    ///////////////////////////////////////////////////////
    //  dimple
    /////////////////////////////////////////////////////////
    
    G4double Dsize(7.225*mm);
    G4double Din(1.6*mm);
    //Sphere diameter for 6mm diameter and 1.6mm depth = 7.225mm diameter sphere (intersecting chords thm)
    //    G4Box* solidDimple =
    //new G4Box("Dimple",                                           //its name
    //	       5.*Dsize,5.*Dsize,Dsize);     //its size
    G4Sphere* solidDimple = new G4Sphere("Dimple",0.,Dsize,0.,2.*pi,pi/2.,pi);
    G4LogicalVolume* logicDimple =
      new G4LogicalVolume(solidDimple,   //its solid
			  fAir,     //its material
			  "Dimple");     //its name

    G4double zdisp = 0.5*scint_thickness+Dsize-Din;
    G4ThreeVector DimpleOffset(0, 0, zdisp);
    G4VPhysicalVolume* DimpleRod = 
      new G4PVPlacement(0,
                              DimpleOffset,
                              logicDimple,
                              "Dimple",
                              logicRod,
                              false,
                              0,
                              checkOverlaps);


        //Rod visualization attributes
    G4VisAttributes * DimpleVisAtt = new G4VisAttributes(G4Colour(0.,1.,1.));
    DimpleVisAtt->SetForceWireframe(true);
    DimpleVisAtt->SetVisibility(true);
    logicDimple->SetVisAttributes(DimpleVisAtt);
*/
/*
    ///////////////////////////////////////////////////////
    // Subtracted Dimple Version
    /////////////////////////////////////////////////////////

    G4Box* solidRod =
      new G4Box("RodBox",                                           //its name
		0.5*scint_sizeX, 0.5*scint_sizeY, 0.5*scint_thickness);     //its size


    G4ThreeVector RodOffset(0, 0, 0);

    G4Box* solidSub =
      new G4Box("SubBox",              //its name
		1*cm, 1*cm, 1*cm);     //its size
    G4double SubDis(2.9875*mm);

        G4double Dsize(3.6125*mm);
    G4double Din(1.6*mm);
    G4Sphere* solidDimple = new G4Sphere("Dimple",0.,Dsize,0.,2.*pi,pi/2.,pi);

    G4double zdisp = 0.5*scint_thickness+Dsize-Din;

    G4SubtractionSolid *Calice = new G4SubtractionSolid("Calice",solidRod,solidDimple,0,G4ThreeVector(0, 0, zdisp));

    G4LogicalVolume* logicCalice =
      new G4LogicalVolume(Calice,   //its solid
			  fEJ200,     //its material
			  "Rod");     //its name

    G4VPhysicalVolume* physRod = 
      new G4PVPlacement(0,
                              RodOffset,
                              logicCalice,
                              "Rod",
                              logicWWW,
                              false,
                              0,
                              checkOverlaps);

    G4LogicalBorderSurface* RodAirSurface = 
            new G4LogicalBorderSurface("RodAirSurface",
                                       physRod,
                                       physWorld,
                                       fPolishedOpSurface);

        //Rod visualization attributes
    G4VisAttributes * RodVisAtt = new G4VisAttributes(G4Colour(1.,1.,0.));
    RodVisAtt->SetForceWireframe(true);
    RodVisAtt->SetVisibility(true);
    logicCalice->SetVisAttributes(RodVisAtt);
*/
    ///////////////////////////////////////////////////////
    // Subtracted Dimple Version 2
    /////////////////////////////////////////////////////////

    G4Box* solidRod =
      new G4Box("RodBox",                                           //its name
		0.5*scint_sizeX, 0.5*scint_sizeY, 0.5*scint_thickness);     //its size
    G4LogicalVolume* logicRod =
      new G4LogicalVolume(solidRod,   //its solid
			  fEJ200,     //its material
			  "Rod");     //its name


    G4ThreeVector RodOffset(0, 0, 0);
    G4VPhysicalVolume* physRod = 
      new G4PVPlacement(0,
                              RodOffset,
                              logicRod,
                              "Rod",
                              logicWWW,
                              false,
                              0,
                              checkOverlaps);

    G4LogicalBorderSurface* RodAirSurface = 
            new G4LogicalBorderSurface("RodAirSurface",
                                       physRod,
                                       physWorld,
                                       fPolishedOpSurface);

        //Rod visualization attributes
    G4VisAttributes * RodVisAtt = new G4VisAttributes(G4Colour(1.,1.,0.));
    RodVisAtt->SetForceWireframe(true);
    RodVisAtt->SetVisibility(true);
    logicRod->SetVisAttributes(RodVisAtt);

    G4double BSiz(10.0*mm);
    G4double SubDis(2.0125*mm);
    G4Box* solidSub =
      new G4Box("SubBox",              //its name
		BSiz, BSiz, SubDis);     //its size
    
    G4double Dsize(3.6125*mm);
    G4double Din(1.6*mm);
    G4Sphere* solidDimple = new G4Sphere("Dimple",0.,Dsize,0.,2.*pi,pi/2.,pi);

    G4double zdisp = 0.5*scint_thickness+Dsize-Din;

    G4SubtractionSolid *DimpleAir = new G4SubtractionSolid("DimpleAir",solidDimple,solidSub);
    G4LogicalVolume* logicDimple =
      new G4LogicalVolume(DimpleAir,   //its solid
			  fAir,     //its material
			  "Dimple");     //its name

    G4ThreeVector DimpleOffset(0, 0, zdisp);
    G4VPhysicalVolume* DimpleRod = 
      new G4PVPlacement(0,
                              DimpleOffset,
                              logicDimple,
                              "Dimple",
                              logicRod,
                              false,
                              0,
                              checkOverlaps);
    G4VisAttributes * DimpleVisAtt = new G4VisAttributes(G4Colour(0.,1.,1.));
    DimpleVisAtt->SetForceWireframe(true);
    DimpleVisAtt->SetVisibility(true);
    logicDimple->SetVisAttributes(DimpleVisAtt);

        ////////////////////////////////////////////
        // PMT : FIXME --> change to use HPK R6091
        ////////////////////////////////////////////
/*
    G4Box* solidPhotocat =
      new G4Box("Photocathode",
		0.5*Photocat_sizeX,0.5*Photocat_sizeY,0.5*Photocat_thickness);

    G4LogicalVolume* logicPhotocat =
      new G4LogicalVolume(solidPhotocat,
                                fBialkali,
                                "Photocathode");

    G4RotationMatrix* rotPhotocat = new G4RotationMatrix;
    rotPhotocat->rotateX(0*rad);
    rotPhotocat->invert();
    G4double aPMT = 0.5*scint_thickness+ScintPMT_gap;
    G4ThreeVector transPhotocat(0.*mm, 0.*mm, aPMT);

    G4VPhysicalVolume* physPhotocat = 
      new G4PVPlacement(rotPhotocat,
                              transPhotocat,
                              logicPhotocat,
                              "Photocathode",
                              logicWWW,
                              false,
                              0,
                              checkOverlaps);

    G4OpticalSurface* PMTOpSurface = new G4OpticalSurface("PMT_Surface");
    G4LogicalSkinSurface* PMTSurface = new G4LogicalSkinSurface("name",logicPhotocat,PMTOpSurface);

    PMTOpSurface -> SetType(dielectric_metal);
    PMTOpSurface -> SetModel(unified);

    G4MaterialPropertiesTable *OpSurfaceProperty = new G4MaterialPropertiesTable();

        //Renyuan's measured Q.E. for Hamamatsu R2059
    const G4int numentries = 2;
    G4double energies[numentries] = 
            {1.0*eV, 6.0*eV};
    G4double reflectivity[numentries] = 
            {0.0, 0.0};
    G4double perfectefficiency[numentries] =
            {1.0, 1.0};

    OpSurfaceProperty -> AddProperty("REFLECTIVITY",energies,reflectivity,numentries);
    OpSurfaceProperty -> AddProperty("EFFICIENCY",energies,perfectefficiency,numentries);

    PMTOpSurface -> SetMaterialPropertiesTable(OpSurfaceProperty);
    if(!fPMTSD)
      {
	fPMTSD = new LYSimPMTSD("/LYSimPMT");
	G4SDManager* sdman = G4SDManager::GetSDMpointer();
	sdman->AddNewDetector(fPMTSD);
      }

        //Photocathode visualization attributes
    G4VisAttributes * PhotocatVisAtt = new G4VisAttributes(G4Colour(0.7,0.7,0.7));
    PhotocatVisAtt->SetForceWireframe(false);
    PhotocatVisAtt->SetVisibility(true);
    logicPhotocat->SetVisAttributes(PhotocatVisAtt);
*/
        ////////////////////////////////////////////
        // SiPM
        ////////////////////////////////////////////

    G4Box* solidSiPM =
      new G4Box("SiPM",
		0.5*Photocat_sizeX,0.5*Photocat_sizeY,0.5*Photocat_thickness);

    G4LogicalVolume* logicSiPM =
      new G4LogicalVolume(solidSiPM,
                                fBialkali, //Change
                                "SiPM");
    G4RotationMatrix* rotSiPM = new G4RotationMatrix;
    rotSiPM->rotateX(0*rad);
    rotSiPM->invert();
    G4double aSiPM = 0.5*scint_thickness+0.5*Photocat_thickness;
    G4ThreeVector transSiPM(0.*mm, 0.*mm, aSiPM);

    G4VPhysicalVolume* physSiPM = 
      new G4PVPlacement(rotSiPM,
                              transSiPM,
                              logicSiPM,
                              "SiPM",
                              logicWWW,
                              false,
                              0,
                              checkOverlaps);

    G4OpticalSurface* SiPMOpSurface = new G4OpticalSurface("SiPM_Surface");
    G4LogicalSkinSurface* SiPMSurface = new G4LogicalSkinSurface("name",logicSiPM,SiPMOpSurface);

    SiPMOpSurface -> SetType(dielectric_metal);
    SiPMOpSurface -> SetModel(unified);

    G4MaterialPropertiesTable *SiPMOpSurfaceProperty = new G4MaterialPropertiesTable();

    //From Meeting on HB SiPM selection (James Hirschauer)
    const G4int SiPMnumentries = 49;
    G4double SiPMenergies[SiPMnumentries] = 
            {3.269640158*eV, 3.191037408*eV, 3.128716912*eV, 3.102662139*eV, 3.069524008*eV, 3.034012326*eV, 3.001068157*eV, 
		2.959583079*eV, 2.901064107*eV, 2.838801776*eV, 2.779808831*eV, 2.721566033*eV, 2.667612494*eV, 2.615095968*eV, 
		2.563388259*eV, 2.515844086*eV, 2.469260675*eV, 2.423979937*eV, 2.379744327*eV, 2.337228676*eV, 2.296532193*eV, 
		2.257220927*eV, 2.219239034*eV, 2.182957454*eV, 2.147148186*eV, 2.112470008*eV, 2.079619445*eV, 2.047338607*eV, 
		2.015906691*eV, 1.987528043*eV, 1.959370034*eV, 1.930211969*eV, 1.902680667*eV, 1.875153451*eV, 1.84878577*eV, 
		1.823292126*eV, 1.799066012*eV, 1.775177229*eV, 1.751266161*eV, 1.728338166*eV, 1.705491878*eV, 1.683828975*eV, 
		1.662853866*eV, 1.643031375*eV, 1.623272564*eV, 1.603516479*eV, 1.584163112*eV, 1.565103929*eV, 1.548241716*eV};
    G4double SiPMreflectivity[SiPMnumentries] = 
            {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    G4double quantumefficiency[SiPMnumentries] =
            {0.00080151, 0.003873974, 0.009957452, 0.025682901, 0.070237974, 0.150458664, 0.218876073, 0.239857707, 0.250435989, 
		0.259508637, 0.264540183, 0.266889254, 0.270669968, 0.273577562, 0.272402437, 0.270462795, 0.268157907, 0.264243881, 
		0.25831461, 0.252029247, 0.245123015, 0.237845409, 0.230874024, 0.224981219, 0.220313658, 0.21616835, 0.211206974, 
		0.205552053, 0.199704762, 0.193696557, 0.187884494, 0.182241421, 0.176095301, 0.168483378, 0.16291645, 0.157493577, 
		0.152319279, 0.146859841, 0.141516867, 0.135704067, 0.129615124, 0.123742916, 0.11736541, 0.11184209, 0.105217764, 
		0.097865669, 0.091000102, 0.084681686, 0.080451249};

    SiPMOpSurfaceProperty -> AddProperty("REFLECTIVITY",SiPMenergies,SiPMreflectivity,SiPMnumentries);
    SiPMOpSurfaceProperty -> AddProperty("EFFICIENCY",SiPMenergies,quantumefficiency,SiPMnumentries);

    SiPMOpSurface -> SetMaterialPropertiesTable(SiPMOpSurfaceProperty);
    if(!fPMTSD)
      {
	fPMTSD = new LYSimPMTSD("/LYSimPMT");
	G4SDManager* sdman = G4SDManager::GetSDMpointer();
	sdman->AddNewDetector(fPMTSD);
      }

        //Photocathode visualization attributes
    G4VisAttributes * SiPMVisAtt = new G4VisAttributes(G4Colour(0.7,0.7,0.7));
    SiPMVisAtt->SetForceWireframe(false);
    SiPMVisAtt->SetVisibility(true);
    logicSiPM->SetVisAttributes(SiPMVisAtt);

    return physWorld;

}


G4VSolid* LYSimDetectorConstruction::ConstructTileSolid (const G4String& name, 
                                                         G4double angle1,
                                                         G4double angle2,
                                                         G4double Dx2,
                                                         G4double Dy,
                                                         G4double Dz,
                                                         G4ThreeVector& center)
{
    G4double x2offset = -Dy*tan(angle1);
    //G4double Dx1 = Dx2 + x2offset;
    G4double Dx1 = Dx2 + x2offset + Dy*tan(angle2);

    G4ThreeVector centerOfGravity(0, 0, 0);
    corners[0] = G4ThreeVector(0., 0., 0.);
    corners[1] = G4ThreeVector(Dx1,          0., 0.);
    corners[2] = G4ThreeVector(x2offset,     Dy, 0.);
    corners[3] = G4ThreeVector(Dx2+x2offset, Dy, 0.);
    corners[4] = G4ThreeVector(0.,           0., Dz);
    corners[5] = G4ThreeVector(Dx1,          0., Dz);
    corners[6] = G4ThreeVector(x2offset,     Dy, Dz);
    corners[7] = G4ThreeVector(Dx2+x2offset, Dy, Dz);

    for(int i = 0; i < 8; i++){
        centerOfGravity += corners[i];
    }
    centerOfGravity /= 8;

    for(int i = 0; i < 8; i++){
        corners[i] -= centerOfGravity;
    }

    center = centerOfGravity;

    G4VSolid* solidTile = new G4Trap(name, corners);

    return solidTile;
}

void LYSimDetectorConstruction::DefineMaterials()
{
    // Get nist material manager
    G4NistManager* nist = G4NistManager::Instance();
    // Get elements
    G4Element *C = nist->FindOrBuildElement("C");
    G4Element *H = nist->FindOrBuildElement("H");
    G4Element *Si = nist->FindOrBuildElement("Si");
    G4Element *O = nist->FindOrBuildElement("O");
    G4Element *K = nist->FindOrBuildElement("K");
    G4Element *Sb = nist->FindOrBuildElement("Sb");
    G4Element *Rb = nist->FindOrBuildElement("Rb");
    G4Element *Cs = nist->FindOrBuildElement("Cs");
    G4Element *Lu = nist->FindOrBuildElement("Lu");
    G4Element *Y = nist->FindOrBuildElement("Y");
    G4Element *Ce = nist->FindOrBuildElement("Ce");
    G4Element *La = nist->FindOrBuildElement("La");
    G4Element *Br = nist->FindOrBuildElement("Br");
    G4Element *Na = nist->FindOrBuildElement("Na");
    G4Element *I = nist->FindOrBuildElement("I");
    G4Element *Tl = nist->FindOrBuildElement("Tl");

    fVacuum = nist->FindOrBuildMaterial("G4_VACUUM");
    fAir = nist->FindOrBuildMaterial("G4_AIR");
    fSiO2 = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    fPolystyrene = nist->FindOrBuildMaterial("G4_POLYSTYRENE");
    fPolycarbonate = nist->FindOrBuildMaterial("G4_POLYCARBONATE");
    fGaAs = nist->FindOrBuildMaterial("G4_GALLIUM_ARSENIDE");
    fPyrex = nist->FindOrBuildMaterial("G4_Pyrex_Glass");
    fWater = nist->FindOrBuildMaterial("G4_WATER");

    //Bialkali definition:
    //Ref: http://hypernews.slac.stanford.edu/HyperNews/geant4/get/AUX/2013/03/11/12.39-85121-chDetectorConstruction.cc
    {
        fBialkali = new G4Material("Bialkali", 4.28*g/cm3, 3, kStateSolid);
        fBialkali->AddElement(K,  13.3*perCent);
        fBialkali->AddElement(Cs, 45.2*perCent);
        fBialkali->AddElement(Sb, 41.5*perCent); 
    }

    //Air definition
    {
        /*
          const G4int nEntries = 50;

          //Photon energy corresponds to ~620nm, ~611nm, ... ~357nm wavelengths
          G4double PhotonEnergy[nEntries] =
          {2.00*eV,2.03*eV,2.06*eV,2.09*eV,2.12*eV,
          2.15*eV,2.18*eV,2.21*eV,2.24*eV,2.27*eV,
          2.30*eV,2.33*eV,2.36*eV,2.39*eV,2.42*eV,
          2.45*eV,2.48*eV,2.51*eV,2.54*eV,2.57*eV,
          2.60*eV,2.63*eV,2.66*eV,2.69*eV,2.72*eV,
          2.75*eV,2.78*eV,2.81*eV,2.84*eV,2.87*eV,
          2.90*eV,2.93*eV,2.96*eV,2.99*eV,3.02*eV,
          3.05*eV,3.08*eV,3.11*eV,3.14*eV,3.17*eV,
          3.20*eV,3.23*eV,3.26*eV,3.29*eV,3.32*eV,
          3.35*eV,3.38*eV,3.41*eV,3.44*eV,3.47*eV};

          G4double RefractiveIndexAir[nEntries] =
          { 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
          1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
          1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
          1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
          1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00};
        */
        const G4int nEntries = 2;

        G4double PhotonEnergy[nEntries] = {1.0*eV, 6.0*eV};
        G4double RefractiveIndexAir[nEntries] = {1.0, 1.0};

        G4MaterialPropertiesTable* MPTAir = new G4MaterialPropertiesTable();
        MPTAir->AddProperty("RINDEX", PhotonEnergy, RefractiveIndexAir, nEntries);

        fAir->SetMaterialPropertiesTable(MPTAir);
    }

    //Silicon Dioxide (SiO2)
    {
        const G4int nEntries = 50;
        //Photon energy corresponds to ~620nm, ~611nm, ... ~357nm wavelengths
        G4double PhotonEnergy[nEntries] =
            {2.00*eV,2.03*eV,2.06*eV,2.09*eV,2.12*eV,
             2.15*eV,2.18*eV,2.21*eV,2.24*eV,2.27*eV,
             2.30*eV,2.33*eV,2.36*eV,2.39*eV,2.42*eV,
             2.45*eV,2.48*eV,2.51*eV,2.54*eV,2.57*eV,
             2.60*eV,2.63*eV,2.66*eV,2.69*eV,2.72*eV,
             2.75*eV,2.78*eV,2.81*eV,2.84*eV,2.87*eV,
             2.90*eV,2.93*eV,2.96*eV,2.99*eV,3.02*eV,
             3.05*eV,3.08*eV,3.11*eV,3.14*eV,3.17*eV,
             3.20*eV,3.23*eV,3.26*eV,3.29*eV,3.32*eV,
             3.35*eV,3.38*eV,3.41*eV,3.44*eV,3.47*eV};

        G4double RefractiveIndexFiber[nEntries] = 
            { 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
              1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
              1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
              1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
              1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60};

        G4double AbsWLSFiber[nEntries] =
            { 5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,
              5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,
              5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,1.10*m,
              1.10*m,1.10*m,1.10*m,1.10*m,1.10*m,1.10*m, 1.*mm, 1.*mm, 1.*mm, 1.*mm,
              1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm};

        G4double EmissionWLSFiber[nEntries] =
            { 0.05, 0.10, 0.30, 0.50, 0.75, 1.00, 1.50, 1.85, 2.30, 2.75,
              3.25, 3.80, 4.50, 5.20, 6.00, 7.00, 8.50, 9.50, 11.1, 12.4,
              12.9, 13.0, 12.8, 12.3, 11.1, 11.0, 12.0, 11.0, 17.0, 16.9,
              15.0, 9.00, 2.50, 1.00, 0.05, 0.00, 0.00, 0.00, 0.00, 0.00,
              0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00};

        G4MaterialPropertiesTable* MPTFiber = new G4MaterialPropertiesTable();
        MPTFiber->AddProperty("RINDEX", PhotonEnergy, RefractiveIndexFiber, nEntries)->SetSpline(true);

        //if (wls_toggle)
        //{
        //    MPTFiber->AddProperty("WLSABSLENGTH",PhotonEnergy,AbsWLSFiber,nEntries)->SetSpline(true);
        //    MPTFiber->AddProperty("WLSCOMPONENT",PhotonEnergy,EmissionWLSFiber,nEntries)->SetSpline(true);
        //    MPTFiber->AddConstProperty("WLSTIMECONSTANT", 0.5*ns);
        //}
        fSiO2->SetMaterialPropertiesTable(MPTFiber);
    }

    //Polystyrene
    {
        const G4int nEntries = 50;
        //Photon energy corresponds to ~620nm, ~611nm, ... ~357nm wavelengths
        G4double PhotonEnergy[nEntries] =
            {2.00*eV,2.03*eV,2.06*eV,2.09*eV,2.12*eV,
             2.15*eV,2.18*eV,2.21*eV,2.24*eV,2.27*eV,
             2.30*eV,2.33*eV,2.36*eV,2.39*eV,2.42*eV,
             2.45*eV,2.48*eV,2.51*eV,2.54*eV,2.57*eV,
             2.60*eV,2.63*eV,2.66*eV,2.69*eV,2.72*eV,
             2.75*eV,2.78*eV,2.81*eV,2.84*eV,2.87*eV,
             2.90*eV,2.93*eV,2.96*eV,2.99*eV,3.02*eV,
             3.05*eV,3.08*eV,3.11*eV,3.14*eV,3.17*eV,
             3.20*eV,3.23*eV,3.26*eV,3.29*eV,3.32*eV,
             3.35*eV,3.38*eV,3.41*eV,3.44*eV,3.47*eV};

        G4double RefractiveIndexPS[nEntries] =
            { 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50,
              1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50,
              1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50,
              1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50,
              1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50};

        G4double AbsPS[nEntries] =
            {2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,
             2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,
             2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,
             2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,
             2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm};

        G4double ScintilFast[nEntries] =
            {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
             0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
             0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
             1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
             1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

        // Add entries into properties table
        G4MaterialPropertiesTable* MPTPolystyrene = new G4MaterialPropertiesTable();
        MPTPolystyrene->AddProperty("RINDEX",PhotonEnergy,RefractiveIndexPS,nEntries);
        MPTPolystyrene->AddProperty("ABSLENGTH",PhotonEnergy,AbsPS,nEntries);
        MPTPolystyrene->AddProperty("FASTCOMPONENT",PhotonEnergy, ScintilFast,nEntries);
        MPTPolystyrene->AddConstProperty("SCINTILLATIONYIELD",10./keV);
        MPTPolystyrene->AddConstProperty("RESOLUTIONSCALE",1.0);
        MPTPolystyrene->AddConstProperty("FASTTIMECONSTANT", 10.*ns);

        fPolystyrene->SetMaterialPropertiesTable(MPTPolystyrene);

        // Set the Birks Constant for the Polystyrene scintillator
        fPolystyrene->GetIonisation()->SetBirksConstant(0.126*mm/MeV);
    }

    //Pyrex glass
    {
        const G4int nEntries = 2;
        G4double PhotonEnergy[nEntries] = {1.0*eV, 6.0*eV};
        G4double RefractiveIndexPyrex[nEntries] = {1.52, 1.52};
        G4double AbsLengthPyrex[nEntries] = {10*m, 10*m};

        G4MaterialPropertiesTable* MPTPyrex = new G4MaterialPropertiesTable();
        MPTPyrex->AddProperty("RINDEX",PhotonEnergy,RefractiveIndexPyrex,nEntries);
        MPTPyrex->AddProperty("ABSLENGTH",PhotonEnergy,AbsLengthPyrex,nEntries);

        fPyrex->SetMaterialPropertiesTable(MPTPyrex);
    }

    //Water (Coupling between Scintillator and PMT window)
    {
        const G4int nEntries = 2;
        G4double PhotonEnergy[nEntries] = {1.0*eV, 6.0*eV};
        G4double RefractiveIndexWater[nEntries];
        for (int i = 0; i <nEntries; i++)
        {
            RefractiveIndexWater[i] = RefIndex;
        }
        G4double AbsLengthWater[nEntries] = {10*m, 10*m};

        G4MaterialPropertiesTable* MPTWater = new G4MaterialPropertiesTable();
        MPTWater->AddProperty("RINDEX",PhotonEnergy,RefractiveIndexWater,nEntries);
        MPTWater->AddProperty("ABSLENGTH",PhotonEnergy,AbsLengthWater,nEntries);

        fWater->SetMaterialPropertiesTable(MPTWater);

        G4cout << "[LYSim] Refractive Index of Scintillator-PMT gap set to " << RefIndex << G4endl;
    }

    /*
      const G4int numentrieslysolal = 55;
      G4double lysoenergieslal[numentrieslysolal] = 
      {        3.542*eV, 3.493*eV, 3.444*eV, 3.397*eV, 3.351*eV, 
      3.306*eV, 3.263*eV, 3.220*eV, 3.179*eV, 3.139*eV, 
      3.100*eV, 3.061*eV, 3.024*eV, 2.988*eV, 2.952*eV, 
      2.917*eV, 2.883*eV, 2.850*eV, 2.818*eV, 2.786*eV, 
      2.755*eV, 2.725*eV, 2.695*eV, 2.666*eV, 2.638*eV, 
      2.610*eV, 2.583*eV, 2.556*eV, 2.530*eV, 2.505*eV, 
      2.480*eV, 2.455*eV, 2.431*eV, 2.407*eV, 2.384*eV, 
      2.362*eV, 2.339*eV, 2.317*eV, 2.296*eV, 2.275*eV, 
      2.254*eV, 2.234*eV, 2.214*eV, 2.194*eV, 2.175*eV, 
      2.156*eV, 2.138*eV, 2.119*eV, 2.101*eV, 2.084*eV, 
      2.066*eV, 2.049*eV, 2.033*eV, 2.016*eV, 2.000*eV };
      G4double lysolal[numentrieslysolal] =
      {0.001*cm, 0.0022387211*cm, 0.0050118723*cm, 0.0112201845*cm, 0.0251188643*cm, 0.0562341325*cm, 0.1258925412*cm, 0.2818382931*cm, 0.6309573445*cm, 1.4125375446*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm, 50*cm}; //scenario
    */
    /*
    //Pre-05/01/2013
    const G4int numentrieslysolal = 10;
    G4double lysoenergieslal[numentrieslysolal] = 
    {
    3.26*eV, 3.18*eV, 3.10*eV, 3.02*eV, 2.95*eV, 
    2.82*eV, 2.48*eV, 2.21*eV, 1.94*eV, 1.55*eV,
    };
    G4double lysolal[numentrieslysolal] = 
    {
    0.*cm, 4.*cm, 20.*cm, 60.*cm, 75.*cm, 
    150.*cm, 330.*cm, 550.*cm, 640.*cm, 640.*cm
    };
    */

    //LYSO
    {
        //------------------------------
        // common LYSO
        G4Material* LYSOtemplate = new G4Material("LYSOtemplate", 7.1*g/cm3, 5, kStateSolid);
        LYSOtemplate->AddElement(Lu, 71.43*perCent);
        LYSOtemplate->AddElement(Y, 4.03*perCent);
        LYSOtemplate->AddElement(Si, 6.37*perCent);
        LYSOtemplate->AddElement(O, 18.14*perCent);
        LYSOtemplate->AddElement(Ce, 0.02*perCent); // cooke2000

        // LYSO
        if(!fLYSO){fLYSO = new G4Material("LYSO", LYSOtemplate->GetDensity(), LYSOtemplate, kStateSolid);}

        //LYSO Sellmeier fit from Sasha
        G4double n0 = 1.43923e0;
        G4double n1 = 3.67622e-1;
        G4double lambda1 = 2.95130e2;

        const G4int numentriesrefindex = 35;
        //wavelength array in nm
        G4double lysowavelength[numentriesrefindex] = 
            {
                360, 370, 380, 390, 400, 
                410, 420, 430, 440, 450, 
                460, 470, 480, 490, 500, 
                510, 520, 530, 540, 550, 
                560, 570, 580, 590, 600, 
                610, 620, 630, 640, 650, 
                660, 670, 680, 690, 700
            };

        G4double lysoenergies[numentriesrefindex];
        for (int i = 0; i<numentriesrefindex; i++)
        {
            lysoenergies[i] = 1239.842 / lysowavelength[i] * eV;
        }

        G4double lysorefindex[numentriesrefindex];
        for (int i = 0; i<numentriesrefindex; i++)
        {
            lysorefindex[i] = sqrt(1 + pow(n0, 2.0) + pow(n1, 2.0) / (1 - pow(lambda1, 2.0) / pow(lysowavelength[i], 2.0)));
        }

        G4double lysoconstrefindex[numentriesrefindex];
        for (int i = 0; i<numentriesrefindex; i++)
        {
            lysoconstrefindex[i] = 1.82;
        }

        const G4int numentrieslal = 2;
        G4double energieslal[numentrieslal] = {1.0*eV, 6.0*eV};
        G4double lal[numentrieslal] = {200*cm, 200*cm};

        //Light Absorption Length
        //From 1mm sample transmission data
        const G4int numentrieslysolal = 10;
        G4double lysoenergieslal[numentrieslysolal] = 
            {3.351*eV, 3.263*eV, 3.179*eV, 3.100*eV, 3.024*eV, 2.952*eV, 2.883*eV, 2.695*eV, 2.384*eV, 2.066*eV};
        G4double lysolal[numentrieslysolal] =
            {0.025*cm, 0.1*cm, 1*cm, 4*cm, 6*cm, 7*cm, 8*cm, 9*cm, 10*cm, 12*cm};


        //Scintillation emission spectrum (fast component)
        //Gamma-ray emission
        const G4int numentriesemissiongamma = 16;
        G4double lysoenergiesemissiongamma[numentriesemissiongamma] = 
            {
                3.44*eV, 3.26*eV, 3.18*eV, 3.10*eV, 3.02*eV, 
                2.95*eV, 2.88*eV, 2.82*eV, 2.70*eV, 2.58*eV, 
                2.48*eV, 2.38*eV, 2.30*eV, 2.21*eV, 2.14*eV, 
                1.82*eV
            };
        G4double lysoemissiongamma[numentriesemissiongamma] = 
            {
                0.00, 0.06, 0.28, 0.72, 1.40, 
                2.00, 2.20, 2.06, 1.48, 0.94, 
                0.60, 0.40, 0.30, 0.20, 0.10, 
                0.00
            };

        //Photoluminescence (theta = 10 deg)
        const G4int numentrieslysoemission = 21;
        G4double lysoenergiesemission[numentrieslysoemission] = 
            {
                3.54*eV, 3.35*eV, 3.26*eV, 3.18*eV, 3.13*eV, 
                3.10*eV, 3.02*eV, 2.95*eV, 2.88*eV, 2.82*eV, 
                2.76*eV, 2.70*eV, 2.64*eV, 2.58*eV, 2.53*eV, 
                2.48*eV, 2.43*eV, 2.38*eV, 2.30*eV, 2.21*eV, 
                2.00*eV
            };
        G4double lysoemission[numentrieslysoemission] = 
            {
                0, 0.26, 1.26, 2.14, 2.2, 
                2.16, 2.04, 1.9, 1.64, 1.3, 
                0.9, 0.62, 0.38, 0.26, 0.14, 
                0.1, 0.08, 0.06, 0.04, 0.02, 
                0
            };

        G4MaterialPropertiesTable* lysoprop = new G4MaterialPropertiesTable();
        lysoprop->AddProperty("FASTCOMPONENT", lysoenergiesemission, lysoemission, numentrieslysoemission);
        lysoprop->AddProperty("RINDEX",        lysoenergies, lysorefindex, numentriesrefindex);
        lysoprop->AddProperty("ABSLENGTH",     lysoenergieslal, lysolal,  numentrieslysolal); //scenario
        lysoprop->AddConstProperty("SCINTILLATIONYIELD",32./keV); // saint-gobain
        lysoprop->AddConstProperty("RESOLUTIONSCALE",1.0);
        lysoprop->AddConstProperty("FASTTIMECONSTANT",41.0*ns); // saint-gobain
        lysoprop->AddConstProperty("YIELDRATIO",1.0);
        fLYSO->SetMaterialPropertiesTable(lysoprop);
    }

    //fiberCore Y11 (http://kuraraypsf.jp/psf/index.html)
    {
        if (!fFiberCore) {
            fFiberCore = new G4Material("FiberCorePS", 1.05*g/cm3, 2, kStateSolid);
            fFiberCore->AddElement(C, 92.26*perCent);
            fFiberCore->AddElement(H,  7.74*perCent);
        }

        //fFiberCore = nist->FindOrBuildMaterial("G4_POLYSTYRENE");


        //Fiber material definition

        G4double baseAbsLength = 350.0*cm;
        G4double baseMu = 1 / baseAbsLength;
        //G4double inducedMu = GetInducedMuFiber();
        //G4double mu = baseMu + inducedMu;
        G4double mu = baseMu;
        G4double absLength = 1 / mu;

        G4cout << "[LYSim] Fiber abs length set to " << G4BestUnit(absLength, "Length") << G4endl;
        const G4int NUMENTRIES = 2;
        G4double PhotonEnergyFiberCore[NUMENTRIES] = {1.0*eV, 6.0*eV};
        G4double RefractiveIndexFiberCore[NUMENTRIES] = {1.59, 1.59};
        G4double AbsLengthFiberCore[NUMENTRIES] = {absLength, absLength};

        //FIXME: findout Y11 parameters
        const G4int NUMENTRIES1 = 91;
        G4double PhotonEnergy_WLS_ABS_FiberCore[NUMENTRIES1] = 
            {        1.776*eV, 1.794*eV, 1.807*eV, 1.821*eV, 1.832*eV, 
                     1.851*eV, 1.868*eV, 1.876*eV, 1.887*eV, 1.890*eV, 
                     1.902*eV, 1.908*eV, 1.917*eV, 1.926*eV, 1.932*eV, 
                     1.941*eV, 1.947*eV, 1.959*eV, 1.969*eV, 1.981*eV, 
                     1.994*eV, 2.004*eV, 2.010*eV, 2.020*eV, 2.027*eV, 
                     2.030*eV, 2.040*eV, 2.047*eV, 2.054*eV, 2.061*eV, 
                     2.071*eV, 2.081*eV, 2.088*eV, 2.099*eV, 2.110*eV, 
                     2.135*eV, 2.154*eV, 2.161*eV, 2.177*eV, 2.212*eV, 
                     2.244*eV, 2.273*eV, 2.285*eV, 2.302*eV, 2.311*eV, 
                     2.320*eV, 2.333*eV, 2.359*eV, 2.377*eV, 2.391*eV, 
                     2.410*eV, 2.424*eV, 2.443*eV, 2.458*eV, 2.478*eV, 
                     2.513*eV, 2.572*eV, 2.594*eV, 2.616*eV, 2.632*eV, 
                     2.649*eV, 2.666*eV, 2.678*eV, 2.695*eV, 2.701*eV, 
                     2.731*eV, 2.749*eV, 2.768*eV, 2.792*eV, 2.811*eV, 
                     2.824*eV, 2.831*eV, 2.850*eV, 2.877*eV, 2.904*eV, 
                     2.910*eV, 2.931*eV, 2.952*eV, 2.980*eV, 3.017*eV, 
                     3.046*eV, 3.069*eV, 3.092*eV, 3.123*eV, 3.155*eV, 
                     3.212*eV, 3.271*eV, 3.315*eV, 3.378*eV, 3.454*eV, 
                     3.522*eV};

        G4double WLS_ABSLENGTH_FiberCore[NUMENTRIES1] = 
            {        71.2971*cm, 117.49*cm, 146.611*cm, 181.757*cm, 211.883*cm, 
                     224.937*cm, 207.866*cm, 204.854*cm, 188.787*cm, 174.728*cm, 
                     155.649*cm, 139.582*cm, 128.536*cm, 131.548*cm, 141.59*cm, 
                     152.636*cm, 167.699*cm, 185.774*cm, 198.828*cm, 204.854*cm, 
                     200.837*cm, 187.782*cm, 165.69*cm, 123.515*cm, 85.3556*cm, 
                     67.2803*cm, 61.2552*cm, 63.2636*cm, 69.2887*cm, 86.3598*cm, 
                     111.464*cm, 139.582*cm, 156.653*cm, 168.703*cm, 178.745*cm, 
                     177.741*cm, 166.695*cm, 160.669*cm, 152.636*cm, 144.603*cm, 
                     136.569*cm, 129.54*cm, 119.498*cm, 108.452*cm, 99.4142*cm, 
                     88.3682*cm, 82.3431*cm, 84.3515*cm, 81.3389*cm, 74.3096*cm, 
                     65.272*cm, 56.2343*cm, 42.1757*cm, 31.1297*cm, 22.0921*cm, 
                     11.046*cm, 1.64583*cm, 0.51974*cm, 0.214673*cm, 0.121914*cm, 
                     0.0742481*cm, 0.0539618*cm, 0.0416667*cm, 0.0337031*cm, 0.0298338*cm, 
                     0.0277388*cm, 0.029216*cm, 0.0309561*cm, 0.0321661*cm, 0.0317524*cm, 
                     0.0301988*cm, 0.0278955*cm, 0.0261243*cm, 0.025*cm, 0.0261936*cm, 
                     0.0282951*cm, 0.0321661*cm, 0.0347711*cm, 0.0387255*cm, 0.0404713*cm, 
                     0.0418432*cm, 0.046801*cm, 0.0536685*cm, 0.0671769*cm, 0.0822918*cm, 
                     0.109722*cm, 0.147388*cm, 0.205729*cm, 0.308594*cm, 0.448864*cm, 
                     0.548611*cm};

        const G4int NUMENTRIES2 = 42;
        G4double PhotonEnergy_WLS_Em_FiberCore[NUMENTRIES2] = 
            {        1.993*eV, 2.029*eV, 2.070*eV, 2.109*eV, 2.153*eV, 
                     2.187*eV, 2.222*eV, 2.246*eV, 2.271*eV, 2.305*eV, 
                     2.331*eV, 2.353*eV, 2.366*eV, 2.384*eV, 2.394*eV, 
                     2.407*eV, 2.417*eV, 2.431*eV, 2.445*eV, 2.460*eV, 
                     2.475*eV, 2.490*eV, 2.510*eV, 2.520*eV, 2.535*eV, 
                     2.546*eV, 2.562*eV, 2.572*eV, 2.583*eV, 2.594*eV, 
                     2.605*eV, 2.616*eV, 2.627*eV, 2.644*eV, 2.661*eV, 
                     2.666*eV, 2.678*eV, 2.689*eV, 2.701*eV, 2.719*eV, 
                     2.749*eV, 2.780*eV };

        G4double WLS_Emission_FiberCore[NUMENTRIES2] = 
            {        0.00505051, 0.012626, 0.0252525, 0.035353, 0.0555556, 
                     0.0782828, 0.126263, 0.164141, 0.222222, 0.270202, 
                     0.315657, 0.373737, 0.444444, 0.515152, 0.580808, 
                     0.65404, 0.719697, 0.762626, 0.792929, 0.777778, 
                     0.747475, 0.70202, 0.686869, 0.69697, 0.739899, 
                     0.787879, 0.858586, 0.919192, 0.969697, 1, 
                     0.984848, 0.924242, 0.815657, 0.64899, 0.517677, 
                     0.39899, 0.287879, 0.186869, 0.103535, 0.0530303, 
                     0.0151515, 0 };

        G4MaterialPropertiesTable* MPTFiberCore = new G4MaterialPropertiesTable();
        MPTFiberCore->AddProperty("RINDEX",PhotonEnergyFiberCore,RefractiveIndexFiberCore,NUMENTRIES);
        MPTFiberCore->AddProperty("ABSLENGTH",PhotonEnergyFiberCore,AbsLengthFiberCore,NUMENTRIES);
        MPTFiberCore->AddProperty("WLSABSLENGTH",PhotonEnergy_WLS_ABS_FiberCore,WLS_ABSLENGTH_FiberCore,NUMENTRIES1);
        MPTFiberCore->AddProperty("WLSCOMPONENT",PhotonEnergy_WLS_Em_FiberCore,WLS_Emission_FiberCore,NUMENTRIES2);
        MPTFiberCore->AddConstProperty("WLSTIMECONSTANT", 0.5*ns);
        fFiberCore->SetMaterialPropertiesTable(MPTFiberCore);
    }

    //fFiberInnerCladding
    {
        //--------------------------------------------------
        // Y11(200) inner clade PMMA
        //--------------------------------------------------
        if (!fFiberInnerCladding)
        {
            fFiberInnerCladding = new G4Material("FiberInnerCladding", 1.190*g/cm3, 3, kStateSolid);
            fFiberInnerCladding->AddElement(C, 60.57*perCent);
            fFiberInnerCladding->AddElement(H,  8.05*perCent);
            fFiberInnerCladding->AddElement(O, 31.38*perCent);
        }
        const G4int NUMENTRIES3 = 2;
        G4double PhotonEnergyFiberInnerCladding[NUMENTRIES3] = {1.0*eV, 6.0*eV};
        G4double RefractiveIndexFiberInnerCladding[NUMENTRIES3] = {1.49, 1.49};
        G4double AbsFiberInnerCladding[NUMENTRIES3] = {20.0*m,20.0*m};

        G4MaterialPropertiesTable* MPTFiberInnerCladding = new G4MaterialPropertiesTable();
        MPTFiberInnerCladding->AddProperty("RINDEX",PhotonEnergyFiberInnerCladding,RefractiveIndexFiberInnerCladding,NUMENTRIES3);
        MPTFiberInnerCladding->AddProperty("ABSLENGTH",PhotonEnergyFiberInnerCladding,AbsFiberInnerCladding,NUMENTRIES3);
        fFiberInnerCladding->SetMaterialPropertiesTable(MPTFiberInnerCladding);
    }

    //fFiberOuterCladding
    {
        //--------------------------------------------------
        // Double Cladding (fluorinated polyethylene)  Note: Y11(200) used Fluorinated polymer (FP)
        //--------------------------------------------------
        if (!fFiberOuterCladding)
        {
            fFiberOuterCladding = new G4Material("FiberOuterCladding", 1.43*g/cm3, 2, kStateSolid);
            fFiberOuterCladding->AddElement(C, 85.63*perCent);
            fFiberOuterCladding->AddElement(H, 14.37*perCent);
        }
        const G4int NUMENTRIES4 = 2;
        G4double PhotonEnergyFiberOuterCladding[NUMENTRIES4] = {1.0*eV, 6.0*eV};
        G4double RefractiveIndexFiberOuterCladding[NUMENTRIES4] = {1.42, 1.42};
        G4double AbsFiberOuterCladding[NUMENTRIES4] = {20.0*m,20.0*m};

        G4MaterialPropertiesTable* MPTFiberOuterCladding = new G4MaterialPropertiesTable();
        MPTFiberOuterCladding->AddProperty("RINDEX",PhotonEnergyFiberOuterCladding,RefractiveIndexFiberOuterCladding,NUMENTRIES4);
        MPTFiberOuterCladding->AddProperty("ABSLENGTH",PhotonEnergyFiberOuterCladding,AbsFiberOuterCladding,NUMENTRIES4);
        fFiberOuterCladding->SetMaterialPropertiesTable(MPTFiberOuterCladding);
    }

    //SCSN81
    {
        fSCSN81 = nist->FindOrBuildMaterial("G4_POLYSTYRENE");
        const G4int nEntries = 2;
        G4double PhotonEnergy[nEntries] = {1.0*eV, 6.0*eV};
        G4double RefractiveIndex[nEntries] = {1.59, 1.59};

        G4double baseAbsLength = GetTileAbsLength();
        G4double baseMu = 1 / baseAbsLength;
        G4double inducedMu = GetInducedMuTile();
        G4double mu = baseMu + inducedMu;
        G4double absLength = 1 / mu;

        G4cout << "[LYSim] [SCSN81] Tile abs length set to " << G4BestUnit(absLength, "Length") << G4endl;
        G4double AbsLength[nEntries] = {absLength, absLength};
        // Add entries into properties table
        G4MaterialPropertiesTable* MPT = new G4MaterialPropertiesTable();
        MPT->AddProperty("RINDEX",PhotonEnergy,RefractiveIndex,nEntries);
        MPT->AddProperty("ABSLENGTH",PhotonEnergy,AbsLength,nEntries);
        fSCSN81->SetMaterialPropertiesTable(MPT);

        G4cout << fSCSN81 << G4endl;
    }
 //EJ200
    {
        //fEJ200 = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"); // H:8.5%; C: 91.5%; density = 1.032 g/cm^3
        fEJ200 = new G4Material("EJ200", 1.023*g/cm3, 2, kStateSolid);
        fEJ200->AddElement(C, 91.53*perCent);
        fEJ200->AddElement(H, 8.47*perCent);
	
       	
        //const G4int nEntries = 41;
        /*G4double PhotonEnergy[nEntries] = 
          {2.25433247*eV, 2.27501442*eV, 2.29607937*eV, 2.31753806*eV, 2.33940162*eV,
           2.36168164*eV, 2.38439012*eV, 2.40753953*eV, 2.43114286*eV, 2.45521358*eV,
           2.47976572*eV, 2.50481386*eV, 2.53037318*eV, 2.55645951*eV, 2.58308929*eV,
           2.61027971*eV, 2.63804864*eV, 2.66641475*eV, 2.69539752*eV, 2.72501727*eV,
           2.75529524*eV, 2.78625362*eV, 2.81791559*eV, 2.85030543*eV, 2.88344851*eV,
           2.91737144*eV, 2.95210205*eV, 2.98766954*eV, 3.02410454*eV, 3.06143916*eV,
           3.09970715*eV, 3.13894395*eV, 3.17918682*eV, 3.22047496*eV, 3.26284963*eV,
           3.30635429*eV, 3.35103476*eV, 3.39693934*eV, 3.44411906*eV, 3.49262777*eV,
           3.54252246*eV};
	
	G4double RefractiveIndex[nEntries] = 
	 {1.58, 1.58, 1.58, 1.58, 1.58,
	  1.58, 1.58, 1.58, 1.58, 1.58,
	  1.58, 1.58, 1.58, 1.58, 1.58,
	  1.58, 1.58, 1.58, 1.58, 1.58,
	  1.58, 1.58, 1.58, 1.58, 1.58,
	  1.58, 1.58, 1.58, 1.58, 1.58,
	  1.58, 1.58, 1.58, 1.58, 1.58,
	  1.58, 1.58, 1.58, 1.58, 1.58,
	  1.58};
	
	G4double AbsLength[nEntries] = 
	  {124.0151808*cm, 121.5151144*cm, 111.5082173*cm, 111.5001357*cm, 109.3711504*cm,
           107.6920217*cm, 104.7432022*cm, 102.5760019*cm, 97.4190864*cm, 89.49243424*cm,
           86.97876601*cm, 78.90244211*cm, 72.94223244*cm, 69.24306758*cm, 62.16439407*cm,
           59.79116519*cm, 59.55295166*cm, 59.21266478*cm, 59.10102979*cm, 58.6349355*cm,
           57.40205548*cm, 57.11877278*cm, 57.11812027*cm, 54.38217272*cm, 46.74583771*cm,
           36.87944033*cm, 25.63972462*cm, 13.25810175*cm, 4.230161766*cm, 1.021088981*cm,
           0.307094977*cm, 0.15251478*cm, 0.144117313*cm, 0.144066445*cm, 0.143662647*cm,
           0.143359894*cm, 0.143032223*cm, 0.142956159*cm, 0.142906311*cm, 0.142855274*cm,
           0.142805293*cm};
	
	
	const G4int nEntries = 2;
	G4double PhotonEnergy[nEntries]= {1.0*eV,6.0*eV};
	G4double RefractiveIndex[nEntries] = {1.58, 1.58};
	 
        G4double baseAbsLength = GetTileAbsLength();
        G4double baseMu = 1 / baseAbsLength;
        G4double inducedMu = GetInducedMuTile();
        G4double mu = baseMu + inducedMu;
        G4double absLength = 1 / mu;
	
	G4cout << "[LYSim] [EJ200] Tile abs length set to " << G4BestUnit(absLength, "Length") << G4endl;
  	
	G4double AbsLength[nEntries] = {absLength, absLength};
	*/
		
	//4 mm irradiated
	/*
	const G4int nEntries = 60;

	G4double PhotonEnergy[nEntries] = 	  
	  {3.542*eV, 3.493*eV, 3.444*eV, 3.397*eV, 3.351*eV, 3.306*eV, 3.263*eV, 3.246*eV, 3.237*eV, 3.229*eV, 
	  3.220*eV, 3.212*eV, 3.204*eV, 3.195*eV, 3.187*eV, 3.179*eV, 3.171*eV, 3.163*eV, 3.155*eV, 3.147*eV, 
	  3.139*eV, 3.131*eV, 3.123*eV, 3.115*eV, 3.107*eV, 3.100*eV, 3.092*eV, 3.084*eV, 3.077*eV, 3.061*eV, 
	  3.046*eV, 3.024*eV, 2.988*eV, 2.952*eV, 2.917*eV, 2.883*eV, 2.850*eV, 2.818*eV, 2.786*eV, 2.755*eV, 
	  2.725*eV, 2.695*eV, 2.666*eV, 2.638*eV, 2.610*eV, 2.583*eV, 2.556*eV, 2.530*eV, 2.505*eV, 2.480*eV, 
	  2.455*eV, 2.431*eV, 2.407*eV, 2.384*eV, 2.362*eV, 2.339*eV, 2.317*eV, 2.296*eV, 2.275*eV, 2.254*eV};
	
	G4double RefractiveIndex[nEntries] = 
	  {
	    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 
	    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	    1.58, 1.58, 1.58, 1.58
	  };

	G4double AbsLength[nEntries] = 
	 {0.55800*mm, 0.56400*mm, 0.56200*mm, 0.56400*mm, 0.56400*mm, 0.56200*mm, 0.57300*mm, 0.58200*mm, 0.59100*mm, 
	  0.60900*mm, 0.62700*mm, 0.64500*mm, 0.67300*mm, 0.71800*mm, 0.76300*mm, 0.82000*mm, 0.89100*mm, 0.98200*mm, 
	  1.08600*mm, 1.22000*mm, 1.38900*mm, 1.59900*mm, 1.85800*mm, 2.19300*mm, 2.62000*mm, 3.15300*mm, 3.83900*mm, 
	  4.73900*mm, 5.86400*mm, 9.09800*mm, 13.9680*mm, 23.8580*mm, 39.5250*mm, 50.0130*mm, 60.7720*mm, 70.4490*mm, 
	  78.8850*mm, 86.6010*mm, 96.8360*mm, 106.602*mm, 115.300*mm, 128.116*mm, 141.749*mm, 151.683*mm, 159.770*mm, 
	  170.497*mm, 189.118*mm, 196.214*mm, 210.116*mm, 231.530*mm, 239.115*mm, 258.689*mm, 275.084*mm, 288.386*mm, 
	  304.507*mm, 326.255*mm, 326.255*mm, 347.733*mm, 366.133*mm, 387.123*mm};
	*/
	
        //4mm Unirr
	
	const G4int nEntries = 57;

	G4double PhotonEnergy[nEntries] = 	
	 {
	   3.542405514*eV, 3.492512479*eV, 3.444005361*eV, 3.396827205*eV, 3.350924135*eV, 3.306245147*eV, 3.262741921*eV, 3.220368649*eV, 
	   3.179081872*eV, 3.170951228*eV, 3.162862066*eV, 3.154814071*eV, 3.146806929*eV, 3.138840329*eV, 3.130913965*eV, 3.123027531*eV,
	   3.115180729*eV, 3.107373258*eV, 3.099604825*eV, 3.091875137*eV, 3.084183905*eV, 3.076530844*eV, 3.068915668*eV, 3.061338099*eV,
	   3.053797857*eV, 3.046294668*eV, 3.038828260*eV, 3.031398362*eV, 3.024004707*eV, 2.987570916*eV, 2.952004595*eV, 2.917275129*eV,
	   2.883353326*eV, 2.850211333*eV, 2.817822568*eV, 2.786161640*eV, 2.755204289*eV, 2.724927319*eV, 2.695308543*eV, 2.666326731*eV,
	   2.637961553*eV, 2.610193537*eV, 2.583004021*eV, 2.556375113*eV, 2.530289653*eV, 2.504731172*eV, 2.479683860*eV, 2.455132535*eV,
	   2.431062608*eV, 2.407460058*eV, 2.384311404*eV, 2.361603676*eV, 2.339324396*eV, 2.317461551*eV, 2.296003574*eV, 2.274939321*eV,
	   2.254258055*eV
	 };  

		
	G4double RefractiveIndex[nEntries] = 
	 {
	    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 
	    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	    1.58
	 };

	G4double AbsLength[nEntries] = 
	 {
	   0.5743617028*mm, 0.5767800495*mm, 0.5773794872*mm, 0.5780135803*mm, 0.5789505759*mm, 0.582016903*mm, 0.5830349115*mm, 0.5975989105*mm, 0.729882704*mm,
	   0.7881381067*mm, 0.8633691117*mm, 0.9585952443*mm, 1.079257244*mm, 1.228571927*mm, 1.419603272*mm, 1.660828418*mm, 1.972327621*mm, 2.372104668*mm,
	   2.896569657*mm, 3.584178546*mm, 4.500608885*mm, 5.707467479*mm, 7.335457013*mm, 9.476300059*mm, 12.39047472*mm, 16.11698146*mm, 21.00255358*mm, 
	   26.86579025*mm, 34.26737294*mm, 76.31922807*mm, 105.842978*mm, 125.2249369*mm, 137.1644668*mm, 145.9649899*mm, 152.6773991*mm, 162.9471828*mm, 
	   170.5827376*mm, 177.8360165*mm, 188.3857227*mm, 203.2702981*mm, 206.9511627*mm, 209.597121*mm, 218.800288*mm, 228.2574561*mm, 241.3501381*mm,
	   252.1649689*mm, 267.0613552*mm, 280.9645483*mm, 293.2338149*mm, 304.009777*mm, 324.5206616*mm, 339.7711016*mm, 357.9664491*mm, 370.3212829*mm,
	   393.1346279*mm, 426.5560315*mm, 434.4316833*mm
	 };
	

        // Add entries into properties table
        G4MaterialPropertiesTable* MPTEJ200 = new G4MaterialPropertiesTable();
        MPTEJ200->AddProperty("RINDEX",PhotonEnergy,RefractiveIndex,nEntries);
        MPTEJ200->AddProperty("ABSLENGTH",PhotonEnergy,AbsLength,nEntries);
        MPTEJ200->AddConstProperty("SCINTILLATIONYIELD",10./keV);
        MPTEJ200->AddConstProperty("ELECTRONSCINTILLATIONYIELD",10./keV); // default: electron ELJEN
        MPTEJ200->AddConstProperty("ALPHASCINTILLATIONYIELD",100./MeV); // alpha particle
        MPTEJ200->AddConstProperty("RESOLUTIONSCALE",1.0);
        MPTEJ200->AddConstProperty("FASTSCINTILLATIONRISETIME",0.9*ns); // Rise time
        MPTEJ200->AddConstProperty("FASTTIMECONSTANT",2.1*ns); // Decay time
        //MPTEJ200->AddProperty("FASTCOMPONENT",PhotonEnergy,Scints,nEntries);
        //MPTEJ200->AddConstProperty("YIELDRATIO",1.0);//Fast component/Tot scint
        fEJ200->SetMaterialPropertiesTable(MPTEJ200);

        // FIXME: Set the Birks Constant for the EJ200 scintillator
        fEJ200->GetIonisation()->SetBirksConstant(0.126*mm/MeV);
        G4cout << fEJ200 << G4endl;
    }

    //EJ260
    {
        //fEJ260 = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"); // H:8.5%; C: 91.5%; density = 1.032 g/cm^3
        fEJ260 = new G4Material("EJ260", 1.023*g/cm3, 2, kStateSolid);
        fEJ260->AddElement(C, 91.49*perCent);
        fEJ260->AddElement(H, 8.51*perCent);

        const G4int nEntries = 2;
        G4double PhotonEnergy[nEntries] = {1.0*eV, 6.0*eV};
        G4double RefractiveIndex[nEntries] = {1.58, 1.58};

        G4double baseAbsLength = GetTileAbsLength();
        G4double baseMu = 1 / baseAbsLength;
        G4double inducedMu = GetInducedMuTile();
        G4double mu = baseMu + inducedMu;
        G4double absLength = 1 / mu;

        G4cout << "[LYSim] [EJ260] Tile abs length set to " << G4BestUnit(absLength, "Length") << G4endl;
        G4double AbsLength[nEntries] = {absLength, absLength};
        // Add entries into properties table
        G4MaterialPropertiesTable* MPTEJ260 = new G4MaterialPropertiesTable();
        MPTEJ260->AddProperty("RINDEX",PhotonEnergy,RefractiveIndex,nEntries);
        MPTEJ260->AddProperty("ABSLENGTH",PhotonEnergy,AbsLength,nEntries);
        MPTEJ260->AddConstProperty("SCINTILLATIONYIELD",9.2/keV); // ELJEN
        //MPTEJ260->AddConstProperty("ELECTRONSCINTILLATIONYIELD",9.2/keV); // default: electron
        //MPTEJ260->AddConstProperty("ALPHASCINTILLATIONYIELD",10.12/keV); // alpha particle
        MPTEJ260->AddConstProperty("RESOLUTIONSCALE",1.0);
        MPTEJ260->AddConstProperty("FASTSCINTILLATIONRISETIME",1.3*ns); // Rise time
        MPTEJ260->AddConstProperty("FASTIMECONSTANT",9.2*ns);// Decay time
        //MPTEJ260->AddConstProperty("YIELDRATIO",1.0);
        fEJ260->SetMaterialPropertiesTable(MPTEJ260);

        // FIXME: Set the Birks Constant for the EJ260 scintillator
        fEJ260->GetIonisation()->SetBirksConstant(0.126*mm/MeV);
        G4cout << fEJ260 << G4endl;
    }

}


   
void LYSimDetectorConstruction::DefineSurfaces()
{
    {
        //////////////////////////////////
        //Realistic Crystal-Tyvek surface
        //////////////////////////////////
        fTyvekOpSurface = new G4OpticalSurface("TyvekOpSurface");
        fTyvekOpSurface->SetType(dielectric_LUT);
        fTyvekOpSurface->SetModel(LUT);
        fTyvekOpSurface->SetFinish(polishedtyvekair);

        const G4int num = 2;
        G4double Ephoton[num] = {1.5*eV, 8.0*eV};
        G4double Reflectivity[num] = {0.979, 0.979};

        G4MaterialPropertiesTable *MPT = new G4MaterialPropertiesTable();

        MPT->AddProperty("REFLECTIVITY", Ephoton, Reflectivity, num);

        fTyvekOpSurface->SetMaterialPropertiesTable(MPT);
    }

    {
        //////////////////////////////////
        //ESR surface
        //////////////////////////////////
        fESROpSurface = new G4OpticalSurface("ESROpSurface");
        fESROpSurface->SetType(dielectric_LUT);
        fESROpSurface->SetModel(LUT);
        fESROpSurface->SetFinish(polishedvm2000air);

        const G4int num = 2;
        G4double Ephoton[num] = {1.5*eV, 8.0*eV};
        G4double Reflectivity[num] = {0.985, 0.985};
	//source: https://www.osti.gov/servlets/purl/1184400

        G4MaterialPropertiesTable *MPT = new G4MaterialPropertiesTable();

        MPT->AddProperty("REFLECTIVITY", Ephoton, Reflectivity, num);

        fESROpSurface->SetMaterialPropertiesTable(MPT);
    }

    {
        //////////////////////////////////
        //Ideal Crystal-Tyvek surface
        //////////////////////////////////
        fIdealTyvekOpSurface = new G4OpticalSurface("IdealTyvekOpSurface");
        fIdealTyvekOpSurface->SetType(dielectric_LUT);
        fIdealTyvekOpSurface->SetModel(LUT);
        fIdealTyvekOpSurface->SetFinish(polishedtyvekair);
    }

    {
        //////////////////////////////////
        //Unified Tyvek surface
        //////////////////////////////////
        fUnifiedTyvekOpSurface = new G4OpticalSurface("UnifiedTyvekOpSurface");
        fUnifiedTyvekOpSurface->SetType(dielectric_dielectric);
        fUnifiedTyvekOpSurface->SetModel(unified);
        fUnifiedTyvekOpSurface->SetFinish(groundbackpainted);
        fUnifiedTyvekOpSurface->SetSigmaAlpha(1.3*degree);

        const G4int NUM = 2;
        G4double pp[NUM] = {2.0*eV, 6.0*eV};
        G4double specularlobe[NUM] = {1.0, 1.0};
        G4double specularspike[NUM] = {0., 0.};
        G4double backscatter[NUM] = {0., 0.};
        G4double rindex[NUM] = {1.0, 1.0};
        G4double reflectivity[NUM] = {0.979, 0.979};
        G4double efficiency[NUM] = {0.0, 0.0};

        G4MaterialPropertiesTable* SMPT = new G4MaterialPropertiesTable();

        SMPT->AddProperty("RINDEX",pp,rindex,NUM);
        SMPT->AddProperty("SPECULARLOBECONSTANT",pp,specularlobe,NUM);
        SMPT->AddProperty("SPECULARSPIKECONSTANT",pp,specularspike,NUM);
        SMPT->AddProperty("BACKSCATTERCONSTANT",pp,backscatter,NUM);
        SMPT->AddProperty("REFLECTIVITY",pp,reflectivity,NUM);
        SMPT->AddProperty("EFFICIENCY",pp,efficiency,NUM);

        fUnifiedTyvekOpSurface -> SetMaterialPropertiesTable(SMPT);

    }


    {
        //////////////////////////////////
        //Unified Ideal Tyvek surface
        //////////////////////////////////
        fUnifiedIdealTyvekOpSurface = new G4OpticalSurface("UnifiedIdealTyvekOpSurface");
        fUnifiedIdealTyvekOpSurface->SetType(dielectric_dielectric);
        fUnifiedIdealTyvekOpSurface->SetModel(unified);
        fUnifiedIdealTyvekOpSurface->SetFinish(groundbackpainted);
        fUnifiedIdealTyvekOpSurface->SetSigmaAlpha(1.3*degree);

        const G4int NUM = 2;
        G4double pp[NUM] = {2.0*eV, 6.0*eV};
        G4double specularlobe[NUM] = {1.0, 1.0};
        G4double specularspike[NUM] = {0., 0.};
        G4double backscatter[NUM] = {0., 0.};
        G4double rindex[NUM] = {1.0, 1.0};
        G4double reflectivity[NUM] = {1.0, 1.0};
        G4double efficiency[NUM] = {0.0, 0.0};

        G4MaterialPropertiesTable* SMPT = new G4MaterialPropertiesTable();

        SMPT->AddProperty("RINDEX",pp,rindex,NUM);
        SMPT->AddProperty("SPECULARLOBECONSTANT",pp,specularlobe,NUM);
        SMPT->AddProperty("SPECULARSPIKECONSTANT",pp,specularspike,NUM);
        SMPT->AddProperty("BACKSCATTERCONSTANT",pp,backscatter,NUM);
        SMPT->AddProperty("REFLECTIVITY",pp,reflectivity,NUM);
        SMPT->AddProperty("EFFICIENCY",pp,efficiency,NUM);

        fUnifiedIdealTyvekOpSurface -> SetMaterialPropertiesTable(SMPT);
    }

    {
        //////////////////////////////////
        //Realistic polished surface
        //////////////////////////////////
        fPolishedOpSurface = new G4OpticalSurface("PolishedOpSurface");//, unified);
        fPolishedOpSurface->SetType(dielectric_dielectric);
        fPolishedOpSurface->SetModel(unified);
        fPolishedOpSurface->SetFinish(ground); // necessary even for polished surfaces to enable UNIFIED code
        fPolishedOpSurface->SetSigmaAlpha(1.3 * degree); // Janecek2010

        G4MaterialPropertiesTable *PolishedOpSurfaceProperty = new G4MaterialPropertiesTable();

        const G4int NUM = 2;
        G4double pp[NUM] = {1.0*eV, 6.0*eV};
        G4double specularlobe[NUM] = {1.0, 1.0};
        G4double specularspike[NUM] = {0., 0.};
        G4double backscatter[NUM] = {0., 0.};

        PolishedOpSurfaceProperty->AddProperty("SPECULARLOBECONSTANT",pp,specularlobe,NUM);
        fPolishedOpSurface->SetMaterialPropertiesTable(PolishedOpSurfaceProperty);
    }

    {
        //////////////////////////////////
        //Ideal polished surface
        //////////////////////////////////
        fIdealPolishedOpSurface = new G4OpticalSurface("IdealOpSurface");
        fIdealPolishedOpSurface->SetType(dielectric_dielectric);
        fIdealPolishedOpSurface->SetModel(glisur);
        fIdealPolishedOpSurface->SetFinish(polished);
    }

    {
        //////////////////////////////////
        //Mirror surface
        //////////////////////////////////
        fMirrorOpSurface = new G4OpticalSurface("MirrorOpSurface");
        fMirrorOpSurface->SetType(dielectric_metal);
        fMirrorOpSurface->SetFinish(polished);
        fMirrorOpSurface->SetModel(unified);

        G4MaterialPropertiesTable *MirrorOpSurfaceProperty = new G4MaterialPropertiesTable();
        const G4int NUM = 2;
        G4double pp[NUM] = {1.0*eV, 6.0*eV};
        G4double reflectivity[NUM] = {0.9, 0.9};
        MirrorOpSurfaceProperty->AddProperty("REFLECTIVITY",pp,reflectivity,NUM);
        fMirrorOpSurface->SetMaterialPropertiesTable(MirrorOpSurfaceProperty);
    }

    {
        //////////////////////////////////
        //Ideal mirror surface
        //////////////////////////////////
        fIdealMirrorOpSurface = new G4OpticalSurface("MirrorOpSurface");
        fIdealMirrorOpSurface->SetType(dielectric_metal);
        fIdealMirrorOpSurface->SetFinish(polished);
        fIdealMirrorOpSurface->SetModel(unified);

        G4MaterialPropertiesTable *IdealMirrorOpSurfaceProperty = new G4MaterialPropertiesTable();
        const G4int NUM = 2;
        G4double pp[NUM] = {1.0*eV, 6.0*eV};
        G4double reflectivity[NUM] = {1.0, 1.0};
        IdealMirrorOpSurfaceProperty->AddProperty("REFLECTIVITY",pp,reflectivity,NUM);
        fIdealMirrorOpSurface->SetMaterialPropertiesTable(IdealMirrorOpSurfaceProperty);
    }

    {
        //////////////////////////////////
        //Absorbing surface
        //////////////////////////////////
        fAbsorbingOpSurface = new G4OpticalSurface("AbsorbingOpSurface");
        fAbsorbingOpSurface->SetType(dielectric_dielectric);
        fAbsorbingOpSurface->SetFinish(groundfrontpainted);
        fAbsorbingOpSurface->SetModel(unified);

        G4MaterialPropertiesTable *AbsorbingOpSurfaceProperty = new G4MaterialPropertiesTable();
        const G4int NUM = 2;
        G4double pp[NUM] = {1.0*eV, 6.0*eV};
        G4double reflectivity[NUM] = {0.0, 0.0};
        AbsorbingOpSurfaceProperty->AddProperty("REFLECTIVITY",pp,reflectivity,NUM);
        fAbsorbingOpSurface->SetMaterialPropertiesTable(AbsorbingOpSurfaceProperty);
    }
}


void LYSimDetectorConstruction::UpdateGeometry()
{

    // clean-up previous geometry
    G4GeometryManager::GetInstance()->OpenGeometry();

    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();
    G4LogicalSkinSurface::CleanSurfaceTable();
    G4LogicalBorderSurface::CleanSurfaceTable();
    G4SurfaceProperty::CleanSurfacePropertyTable();
    DefineMaterials();
    DefineSurfaces();
    G4RunManager::GetRunManager()->DefineWorldVolume(ConstructDetector());
    G4cout << "[LYSim] Setting induced absorption coefficient = " << inducedMuTile << " [cm^-1]" << G4endl;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
    //G4RunManager::GetRunManager()->PhysicsHasBeenModified();

    //G4RegionStore::GetInstance()->UpdateMaterialList(physWorld);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
