// LYSimEventAction.cc

#include "LYSimEventAction.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"
#include "Analysis.hh"
#include "g4root.hh"

LYSimEventAction::LYSimEventAction()
{
G4AnalysisManager::Instance();
}


void LYSimEventAction::BeginOfEventAction(const G4Event* anEvent )
{
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    G4PrimaryParticle* primary = anEvent->GetPrimaryVertex(0)->GetPrimary(0);
    //G4cout << G4endl
    //     << ">>> Event " << anEvent->GetEventID() << " >>> Simulation truth : "
    //     << primary->GetG4code()->GetParticleName()
    //     << " " << primary->GetMomentum() << G4endl;
    G4PrimaryVertex* primaryVertex = anEvent->GetPrimaryVertex();
    G4PrimaryParticle* primaryParticle = primaryVertex->GetPrimary();
    G4double ke = primaryParticle->GetKineticEnergy();
    man->FillH1(5,ke); //total energy of source
    if ( anEvent->GetEventID() % 100 == 0 )
    {
        G4cout<<"Starting Event: "<<anEvent->GetEventID()<<G4endl;
    }
    Analysis::GetInstance()->PrepareNewEvent(anEvent);
    //Retrieve the ID for the hit collection
    //if ( hitsCollID == -1 )
    //	{
    //	G4SDManager * SDman = G4SDManager::GetSDMpointer();
    //	hitsCollID = SDman->GetCollectionID(hitsCollName);
    // 	}
    fTotalEdep = 0.;
}

void LYSimEventAction::EndOfEventAction(const G4Event* anEvent)
{
    Analysis::GetInstance()->EndOfEvent(anEvent);
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    fTotalEdep = GetEdep();
    if (fTotalEdep > 0.){
    man->FillH1(4,fTotalEdep); //total energy of source
    //G4cout << fTotalEdep << G4endl;
    }
}

