#include "LYSimPrimaryGeneratorMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

LYSimPrimaryGeneratorMessenger::LYSimPrimaryGeneratorMessenger(LYSimPrimaryGeneratorAction * Pact)
    : Paction(Pact)
{

    detDir = new G4UIdirectory("/LYSim/");
    detDir->SetGuidance(" Geometry Setup ");

  
    SetSource000Cmd = new G4UIcmdWithABool("/LYSim/SetSource000", this);
    SetSource000Cmd->SetGuidance("Set source position to 0,0,0 toggle true or false");
    SetSource000Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);


    SetAngle000Cmd = new G4UIcmdWithABool("/LYSim/SetAngle000", this);
    SetAngle000Cmd->SetGuidance("Set photon angle to perp to surface toggle true or false");
    SetAngle000Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

LYSimPrimaryGeneratorMessenger::~LYSimPrimaryGeneratorMessenger()
{
    delete detDir;
    delete SetSource000Cmd;
    delete SetAngle000Cmd;
}

void LYSimPrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command,G4String val)
{
    if( command == SetSource000Cmd ) {
        Paction->SetSource000Toggle(G4UIcmdWithABool::GetNewBoolValue(val));
    }
    if( command == SetAngle000Cmd ) {
        Paction->SetAngle000Toggle(G4UIcmdWithABool::GetNewBoolValue(val));
    }
}
