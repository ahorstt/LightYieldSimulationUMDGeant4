#ifndef LYSimPrimaryGeneratorMessenger_h
#define LYSimPrimaryGeneratorMessenger_h

#include "globals.hh"
#include "G4UImessenger.hh"

#include "LYSimPrimaryGeneratorAction.hh"


class G4UIdirectory;
class G4UIcmdWithABool;
class G4UIcmdWithAString;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;
class G4UIcmdWithoutParameter;

class LYSimPrimaryGeneratorMessenger : public G4UImessenger
{
public:

    LYSimPrimaryGeneratorMessenger(LYSimPrimaryGeneratorAction* );
    ~LYSimPrimaryGeneratorMessenger();
 
    void SetNewValue(G4UIcommand*, G4String);

private:

  LYSimPrimaryGeneratorAction* Paction;
    G4UIdirectory* detDir;

  G4UIcmdWithABool*           SetSource000Cmd;
  G4UIcmdWithABool*           SetAngle000Cmd;



};

#endif
