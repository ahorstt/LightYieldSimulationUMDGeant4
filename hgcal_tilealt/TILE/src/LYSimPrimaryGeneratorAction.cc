//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id$
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "Analysis.hh"

#include "LYSimPrimaryGeneratorAction.hh"
#include "LYSimDetectorConstruction.hh"
#include "LYSimPrimaryGeneratorMessenger.hh"

#include "Randomize.hh"

#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4RandomDirection.hh"
#include "G4TransportationManager.hh"
#include "G4PhysicalConstants.hh"

#include <CLHEP/Units/PhysicalConstants.h>


//#include "G4OpticalPhoton.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LYSimPrimaryGeneratorAction::LYSimPrimaryGeneratorAction(LYSimDetectorConstruction* det)
{
  std::cout<<"[LYSIM] entering LYSIMPrimaryGeneratorAction"<<std::endl;
    fDetector = det;
    fprimarygeneratorMessenger = new LYSimPrimaryGeneratorMessenger(this);
    particleSource = new G4GeneralParticleSource();
    //particleSource = new G4ParticleGun(1);

    source000_toggle=false;
    angle000_toggle=false;

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = G4OpticalPhoton::OpticalPhotonDefinition();

    particleSource->SetParticleDefinition(particle);
    particleSource->SetParticleTime(0.0*ns);
    G4ThreeVector point1 (0.*mm,0.*mm,0.*mm);
    particleSource->SetParticlePosition(point1);
    //particleSource->SetParticleEnergy(2.95*eV);
    //particleSource->SetParticleMomentumDirection(G4ThreeVector (0.,0.,1.));
    G4ThreeVector meme =particleSource->GetParticlePosition();
    std::cout<<"[LYSIM] set default particle position to "<<meme.x()<<","<<meme.y()<<","<<meme.z()<<std::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LYSimPrimaryGeneratorAction::~LYSimPrimaryGeneratorAction()
{
    delete particleSource;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LYSimPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    if (particleSource->GetParticleDefinition()->GetParticleName() == "opticalphoton")
    {
        SetOptPhotonPolar();
    }
    //G4ThreeVector pos = particleSource->GetParticlePosition();
    //std::cout << "XYZXYZXYZ " << pos.x()<<" "<<pos.y()<<" "<<pos.z() << std::endl;

    std::string name="World";
    int icnt=0;
    G4ThreeVector point1(0.*mm,0.*mm,0.*mm);
    if(!source000_toggle) {
      while((name!="Rod")&&(icnt<10)) {
	if(icnt!=0) std::cout<<"rethrowing since name is "<<name<<" at coord "<<point1.x()<<","<<point1.y()<<","<<point1.z()<<std::endl;
	G4double xx = fDetector->GetScintSizeX()*(-0.5+G4UniformRand());
	G4double yy = fDetector->GetScintSizeY()*(-0.5+G4UniformRand());
	G4double zz = fDetector->GetScintThickness()*(-0.5+G4UniformRand());
	point1.setX(xx);
	point1.setY(yy);
	point1.setZ(zz);

	// check if it is in scintillator

	G4VPhysicalVolume* pv =
	  G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->LocateGlobalPointAndSetup(point1, (const G4ThreeVector*)0,false, true );
	name=pv->GetName();
	icnt++;

      }
      if(icnt==10) std::cout<<"Danger Danger Will Robinson"<<std::endl;
    }
    particleSource->SetParticlePosition(point1);
    //
    // set photon direction randomly
    G4ThreeVector Direction(0.,0.,1.);
    if(!angle000_toggle) {
      double phi = pi*G4UniformRand();
      double costheta = -1+2.*G4UniformRand();
      double sintheta= sqrt(1.-costheta*costheta);
      Direction.setX(cos(phi)*sintheta);
      Direction.setY(sin(phi)*sintheta);
      Direction.setZ(costheta);
    }

    //particleSource->SetParticleMomentumDirection(Direction);
			    

    //std::cout<<" photon position is "<<point1.x()<<","<<point1.y()<<","<<point1.z()<<" and direction is "<<Direction.x()<<","<<Direction.y()<<","<<Direction.z()<<std::endl;

    particleSource->GeneratePrimaryVertex(anEvent);

    //Analysis
    //Analysis::GetInstance()->AddPhotonCount(1);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//Photon polarization is randomized
void LYSimPrimaryGeneratorAction::SetOptPhotonPolar()
{
    G4double angle = G4UniformRand() * 360.0*deg;
    SetOptPhotonPolar(angle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void LYSimPrimaryGeneratorAction::SetOptPhotonPolar(G4double angle)
{
    if (particleSource->GetParticleDefinition()->GetParticleName() == "opticalphoton")
    {
        G4ThreeVector normal (1., 0., 0.);
        G4ThreeVector kphoton = particleSource->GetParticleMomentumDirection();
        G4ThreeVector product = normal.cross(kphoton);
        G4double modul2       = product*product;

        G4ThreeVector e_perpend (0., 0., 1.);
        if (modul2 > 0.) e_perpend = (1./std::sqrt(modul2))*product;
        G4ThreeVector e_paralle    = e_perpend.cross(kphoton);

        G4ThreeVector polar = std::cos(angle)*e_paralle + std::sin(angle)*e_perpend;
        particleSource->SetParticlePolarization(polar);
    }

}

const G4ThreeVector LYSimPrimaryGeneratorAction::GetSourcePosition()
{
    G4ThreeVector pos = particleSource->GetParticlePosition();
    return pos;
}



//....oooOO-1OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
