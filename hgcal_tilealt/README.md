# HGcal Tile
GEANT4-based simulation of CE-H HGCal optical tile

## compiling

  git clone https://github.com/saraheno/hgcal_tile.git

  cd hgcal_tile

  ./g4env.csh

  mkdir build

  cd build 

  cmake -DWITH_GEANT4_UIVIS=ON -DGeant4_DIR=$G4LIB ../TILE

  make


## running
  ./LYSIM photontest.mac [-novis] [output_filename] [root_filename}

    (if running without -novis
         /run/beamOn 5

## run the job on condor.

edit the condor-*.* so that they point to your areas, not mine

submit with

condor_submit condor_jobs.jdl

see if it is running with

condor_q -submitter your-user-name  look at Analysis.root 

