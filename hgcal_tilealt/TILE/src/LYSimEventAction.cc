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
    G4PrimaryVertex* primaryVertex = anEvent->GetPrimaryVertex();
    G4PrimaryParticle* primaryParticle = primaryVertex->GetPrimary();
    G4double ke = primaryParticle->GetKineticEnergy();
    if ( anEvent->GetEventID() % 100 == 0 )
    {
        G4cout<<"Starting Event: "<<anEvent->GetEventID()<<G4endl;
    }
    Analysis::GetInstance()->PrepareNewEvent(anEvent);
}

void LYSimEventAction::EndOfEventAction(const G4Event* anEvent)
{
    Analysis::GetInstance()->EndOfEvent(anEvent);
}

