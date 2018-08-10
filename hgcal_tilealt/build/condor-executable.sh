#!/bin/bash

cd /home/ahorst/hgcal_tile/build/

export UNIQUE_ID=sce
export CONDOR_PROCESS=sce
export RUN_DIR=`pwd`
export PARAMETER_SET=photontest.mac


START_TIME=`/bin/date`
echo "started at $START_TIME"

echo ""
echo "parameter set:"
echo "UNIQUE_ID: $UNIQUE_ID"
echo "CONDOR_PROCESS: $CONDOR_PROCESS"
echo "RUN_DIR: $RUN_DIR"
echo "PARAMETER_SET: $PARAMETER_SET"



#
# setup software environment at UMD
#
cd $RUN_DIR


export USERBASE=`pwd`
export ARCH=x86_64-slc6-gcc46-opt
export ARCH2=x86_64-slc6-gcc46-opt
export ARCHd=x86_64-slc6-gcc46-dbg
source /cvmfs/sft.cern.ch/lcg/external/gcc/4.6.3/x86_64-slc6/setup.sh
export QTHOME=/cvmfs/sft.cern.ch/lcg/external/qt/4.8.4/${ARCH}/
export DAWNHOME=/afs/cern.ch/sw/lcg/external/dawn/3_88a/x86_64-slc5-gcc43-opt/
export G4DAWNFILE_DEST_DIR=${USERBASE}/DawnFiles/
export HEPMC_DIR=/cvmfs/sft.cern.ch/lcg/external/HepMC/2.06.08/${ARCH}/
export FASTJET_INSTALL=/cvmfs/sft.cern.ch/lcg/external/fastjet/3.0.3/${ARCH}/
export BOOSTSYS=/cvmfs/cms.cern.ch/slc6_amd64_gcc462/external/boost/1.47.0-cms



export path=$path:/cvmfs/sft.cern.ch/lcg/external/CMake/2.8.6/${ARCH}/bin
export path=$path:/cvmfs/sft.cern.ch/lcg/external/expat/2.0.1/${ARCH}


export G4BASE=/data/users/eno
cd /data/users/eno/geant4.9.6.p04-install/share/Geant4-9.6.4/geant4make/
source geant4make.sh
cd ${USERBASE}

cd /cvmfs/sft.cern.ch/lcg/external/ROOT/5.34.00/${ARCH}/root/bin
source thisroot.sh
cd ${USERBASE}


export XERCESCROOT=/cvmfs/sft.cern.ch/lcg/external/XercesC/3.1.1/${ARCH}/
export PYTHONDIR=/cvmfs/sft.cern.ch/lcg/external/Python/2.7.3/${ARCH}/
export PYTHONPATH=${PYTHONDIR}:${ROOTSYS}/lib
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$XERCESCROOT/lib:$HEPMC_DIR/lib:$USERBASE/userlib/lib:$USERBASE/analysis/lib:$FASTJET_INSTALL/lib:$BOOSTSYS/lib:$ROOTSYS/lib:$PYTHONDIR/lib
export path=$DAWNHOME/bin:$path:$FASTJET_INSTALL/bin:$BOOSTSYS



#
# run 
#

./LYSim $PARAMETER_SET -novis >> junk.log

exitcode=$?


#
# end run
#

echo ""
END_TIME=`/bin/date`
echo "finished at $END_TIME"
exit $exitcode