#include "Analysis.hh"

#include "LYSimRunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"

#include "LYSimDetectorConstruction.hh"
#include <iostream>
#include "g4root.hh"

LYSimRunAction::LYSimRunAction(LYSimDetectorConstruction* ipDetectorConstruction)
{
//     runMessenger = new LYSimRunActionMessenger(this);
    pDetectorConstruction = ipDetectorConstruction;
    G4AnalysisManager::Instance();
}

LYSimRunAction::~LYSimRunAction()
{
//     delete runMessenger;
    delete G4AnalysisManager::Instance();
    delete Analysis::GetInstance();
}

void LYSimRunAction::BeginOfRunAction(const G4Run* aRun)
{
    Analysis::GetInstance()->PrepareNewRun(aRun);
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    outFileName = Analysis::GetInstance()->GetROOTFileName();
    G4cout << "Output filename: " << outFileName << G4endl;
    man->OpenFile(outFileName.c_str());
    man->SetFirstHistoId(1);

    // Create histogram(s) (avoid non-integer bins)
    if (pDetectorConstruction->GetDetectorType()==1) {
        man->CreateH1("h1","Optical photons energy [eV]", //histoID,histo name 
                      500,0.,5.); //bins' number, xmin, xmax
        man->CreateH1("h2","Number of detected photons per event",
                      5000,0.,5000.); //bins' number, xmin, xmax
        man->CreateH1("h3","Total optical photons energy deposited per event [eV]",
                      500,0.,500.); //bins' number, xmin, xmax
        man->CreateH1("h4","Muon energy deposited per step [MeV]",
                      100,0.,10); //bins' number, xmin, xmax
        man->CreateH1("h5","Total Muon total energy[MeV]",
                      100,0.,1000000.); //bins' number, xmin, xmax
    } else {
        man->CreateH1("h1","Optical photons energy [eV]", //histoID,histo name 
                      500,0.,5.); //bins' number, xmin, xmax
        man->CreateH1("h2","Number of detected photons per event",
                      5000,0.,5000.); //bins' number, xmin, xmax
        man->CreateH1("h3","Total optical photons energy deposited per event [eV]",
                      500,0.,500.); //bins' number, xmin, xmax
        man->CreateH1("h4","Muon energy deposited [MeV]",
                      100,0.,10); //bins' number, xmin, xmax
        man->CreateH1("h5","Total Muon total energy[MeV]",
                      100,0.,1000000.); //bins' number, xmin, xmax
    }
}

void LYSimRunAction::EndOfRunAction(const G4Run* aRun)
{
    Analysis::GetInstance()->EndOfRun(aRun);
}
