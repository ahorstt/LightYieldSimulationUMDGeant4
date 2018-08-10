
#ifndef LYSimPrimaryGeneratorAction_h
#define LYSimPrimaryGeneratorAction_h

#include "LYSimDetectorConstruction.hh"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

class G4GeneralParticleSource;
class G4Event;
class LYSimDetectorConstruction;
class LYSimPrimaryGeneratorAction;
class LYSimPrimaryGeneratorMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class LYSimPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    LYSimPrimaryGeneratorAction(LYSimDetectorConstruction*);
   ~LYSimPrimaryGeneratorAction();

  public:
    void GeneratePrimaries(G4Event*);


  void SetSource000Toggle (G4bool toggle) {source000_toggle = toggle;}
  G4bool GetSource000Toggle () {return source000_toggle;}


  void SetAngle000Toggle (G4bool toggle) {angle000_toggle = toggle;}
  G4bool GetAngle000Toggle () {return angle000_toggle;}


    void SetOptPhotonPolar();
    void SetOptPhotonPolar(G4double);
    const G4ThreeVector GetSourcePosition();

  private:
  LYSimPrimaryGeneratorMessenger* fprimarygeneratorMessenger;  

  G4GeneralParticleSource* particleSource;
  //G4ParticleGun* particleSource;
  LYSimDetectorConstruction* fDetector;
  G4bool source000_toggle;
  G4bool angle000_toggle;
  //G4double PhotonEnergy;
    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*LYSimPrimaryGeneratorAction_h*/
