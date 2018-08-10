#ifndef LYSimEVENTACTION_HH
#define LYSimEVENTACTION_HH


#include "G4UserEventAction.hh"
#include "G4String.hh"
#include <iostream>
#include <fstream>
class G4Event;

//User event action class. Prepares new event in analysis code at beginning of event.
class LYSimEventAction : public G4UserEventAction
{
public:
    //! Default constructor
    LYSimEventAction();
    //! Default destructor
    virtual ~LYSimEventAction() {};
    //! Beginning of event
    void BeginOfEventAction(const G4Event* anEvent);
    //! Digitize hits and store information
    void EndOfEventAction(const G4Event* anEvent);

    void AddEdep(G4double depenergy) {fTotalEdep += depenergy; }
    G4double GetEdep() const { return fTotalEdep; }
private:
    G4double fTotalEdep;
};

#endif /* TEST4EVENTACTION_HH */
