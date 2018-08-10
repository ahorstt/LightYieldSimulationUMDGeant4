#ifndef LYSimDetectorMessenger_h
#define LYSimDetectorMessenger_h

#include "globals.hh"
#include "G4UImessenger.hh"

#include "LYSimDetectorConstruction.hh"
#include "Analysis.hh"

class G4UIdirectory;
class G4UIcmdWithABool;
class G4UIcmdWithAString;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;
class G4UIcmdWithoutParameter;

class LYSimDetectorMessenger : public G4UImessenger
{
public:

    LYSimDetectorMessenger(LYSimDetectorConstruction* );
    ~LYSimDetectorMessenger();
 
    void SetNewValue(G4UIcommand*, G4String);

private:

    LYSimDetectorConstruction*   Detector;
    Analysis* analysis;

    G4UIdirectory* detDir;

    G4UIcmdWithoutParameter*   	UpdateCmd;
  G4UIcmdWithABool*           SetWrappingCmd;
  G4UIcmdWithADouble*         SetRefIndexCmd;
    G4UIcmdWithADoubleAndUnit*  SetScintThicknessCmd;
    G4UIcmdWithADoubleAndUnit*  SetScintSizeXCmd;
    G4UIcmdWithADoubleAndUnit*  SetScintSizeYCmd;
    G4UIcmdWithADoubleAndUnit*  SetScintPMTGapThicknessCmd;
    G4UIcmdWithADoubleAndUnit*  SetTileAbsLengthCmd;
    G4UIcmdWithADoubleAndUnit*  SetInducedAbsLengthCmd;


};

#endif
