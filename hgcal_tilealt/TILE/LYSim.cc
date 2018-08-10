#include "Analysis.hh"

//User initialization
#include "LYSimDetectorConstruction.hh"
#include "LYSimPhysicsList.hh"

//User action
#include "LYSimPrimaryGeneratorAction.hh"
#include "LYSimRunAction.hh"
#include "LYSimTrackingAction.hh"
#include "LYSimSteppingAction.hh"
#include "LYSimEventAction.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"


//Always include VI, UI
//#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
//#endif
//#ifdef G4UI_USE
#include "G4UIExecutive.hh"
//#endif

#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{

    // Construct the default run manager
    //
    G4RunManager * runManager = new G4RunManager;

    // Set mandatory initialization classes
    //
    // Detector construction
    LYSimDetectorConstruction* detector= new LYSimDetectorConstruction();
    //if (argc > 1) detector->SetDetectorType(std::atoi(argv[1]));
    //else detector->SetDetectorType(0);

    runManager->SetUserInitialization(detector);
    // Physics list
    LYSimPhysicsList *physlist = new LYSimPhysicsList();
    //if (detector->GetDetectorType()==1) physlist->SetHadProc(true);
    runManager-> SetUserInitialization(physlist);
    G4cout << "[LYSim] 33333" << G4endl;

    //Construct Analysis class
    Analysis::GetInstance()->SetDetector(detector);
    std::string outFileName= "Analysis";
    Analysis::GetInstance()->SetOutputFile(outFileName+".txt");
    Analysis::GetInstance()->SetROOTFile(outFileName+".root");
    LYSimPrimaryGeneratorAction* genaction = new LYSimPrimaryGeneratorAction(detector);
    Analysis::GetInstance()->SetGeneratorAction(genaction);

    // Set user action classes
    //    
    // Primary generator action
    runManager->SetUserAction(genaction);
    // Run action
    runManager->SetUserAction(new LYSimRunAction(detector));
    // Event action
    runManager->SetUserAction(new LYSimEventAction());
    // Tracking action
    runManager->SetUserAction(new LYSimTrackingAction());
    // Stepping action
    runManager->SetUserAction(new LYSimSteppingAction());

    // Initialize G4 kernel
    //
    runManager->Initialize();
    //#ifdef G4VIS_USE
    // Initialize visualization
    //G4VisManager* visManager = new G4VisExecutive();
    // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
    G4VisManager* visManager = new G4VisExecutive("Quiet");
    visManager->Initialize();
    //#endif

    // Get the pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    std::cout<<"[LYSIM] ARGC is "<<argc<<std::endl;
    for(int i=0;i<argc;i++) {
      std::cout<<  "argv["<<i<<"]="<<argv[i];
      
      if(i==0) {std::cout<<" is the file name"<<std::endl;}
      else if(i==1) {std::cout<<" is the macro to run (only works if at least 3 arguements supplied)"<<std::endl;}
      else if(i==2) {std::cout<<" anything here prevents the visualization"<<std::endl;}
      else if(i==3) {std::cout<<" is the output file name"<<std::endl;}
      else if(i==4) {std::cout<<" is the root file name"<<std::endl;}
      else {std::cout<<" invalid option"<<std::endl;}
      
    }



    // [0] is the exe name
    // [1] is photontest.mac
    // [2] is -novis
    // [3] is outputfile name
    // [4] is rootfile name
    if (argc == 3) {
        G4cout << "[LYSim] argv[2] is " << argv[2] <<G4endl;
        if(std::string(argv[2]) == "-novis") {
	  //G4UIExecutive* ui = new G4UIExecutive(argc, argv);
	  //ui->SessionStart();
            //UImanager->ApplyCommand("/control/execute init.mac");
            G4String command = "/control/execute ";
            G4String fileName = argv[1];
            UImanager->ApplyCommand(command+fileName);
            //delete ui;
        }
        else {
            G4cout << "[LYSim] run in batch mode" <<G4endl;
            // batch mode
            G4String command = "/control/execute ";
            G4String fileName = argv[1];
            UImanager->ApplyCommand(command+fileName);
        }
    }
    else if (argc == 4) {
        // batch mode
        G4String command = "/control/execute ";
        G4String fileName = argv[2];
        outFileName = argv[3];
        G4cout << "[LYSim] outFileName tag is " << outFileName <<G4endl;
        Analysis::GetInstance()->SetOutputFile(outFileName+".txt");
        Analysis::GetInstance()->SetROOTFile(outFileName+".root");
        UImanager->ApplyCommand(command+fileName);
    }
    else {
        // interactive mode : define UI session
        //#ifdef G4UI_USE
        G4UIExecutive* ui = new G4UIExecutive(argc, argv);
        //#ifdef G4VIS_USE
        UImanager->ApplyCommand("/control/execute init_vis.mac"); 
        //#else
        //UImanager->ApplyCommand("/control/execute init.mac"); 
        //#endif
        ui->SessionStart();
        delete ui;
        //#endif
    }

    // Job termination
    // Free the store: user actions, physics_list and detector_description are
    // owned and deleted by the run manager, so they should not be deleted 
    // in the main() program !

    //#ifdef G4VIS_USE
    delete visManager;
    //#endif
    delete runManager;

    return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
