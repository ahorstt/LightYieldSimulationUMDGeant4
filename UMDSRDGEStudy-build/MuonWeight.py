#!/usr/bin/python
import os, sys
import shlex, subprocess
from datetime import datetime, date, time
from time import sleep
import shutil
import numpy as np   
#sys.path.append(os.path.abspath(os.path.curdir))

JobTime = datetime.now()
fTag = JobTime.strftime("%Y%m%d_%H%M%S")
dirname = "jobs/%s"%(fTag)
DetType = "0" #Tile
logFile = "0606.log"

try:
    os.makedirs(dirname)
except:
    pass

ProdTag = "Run4_20170606"
OutDir  = "/home/ahorst/UMDSRDGEStudy-build/Absdata"
WorkDir = "/home/ahorst/UMDSRDGEStudy-build"

try:
    os.makedirs(OutDir)
except:
    pass

try:
    os.makedirs(OutDir+"/"+ProdTag)
except:
    pass

try:
    os.makedirs(OutDir+"/"+ProdTag+"/logs")
except:
    pass



#########################################
#make sure OutDir is the same in main.cc
#########################################
condor_script_template = """
universe = vanilla
Executable = ./CMS.sh
+IsLocalJob = true
Should_transfer_files = NO
Requirements = TARGET.FileSystemDomain == "privnet"
Output = %(OUTDIR)s/%(MYPREFIX)s/logs/%(FILENAME)s_sce_$(cluster)_$(process).stdout
Error  = %(OUTDIR)s/%(MYPREFIX)s/logs/%(FILENAME)s_sce_$(cluster)_$(process).stderr
Log    = %(OUTDIR)s/%(MYPREFIX)s/logs/%(FILENAME)s_sce_$(cluster)_$(process).condor
Arguments = %(WORKDIR)s %(INPUT)s %(FILENAME)s %(DETTYPE)s
Queue 1
 
"""
######################%(WORKDIR)s %(INPUT)s %(FILENAME)s %(DETTYPE)s###################
# %(OUTDIR)s/%(MYPREFIX)s/

#starting wavelenght
#Initial = 3.0538
#array that contains the different wavelengths being tested                          
Arr = [16000, 18000, 20000, 22000, 24000, 26000, 28000, 30000, 32000,
 34000, 36000, 38000, 40000, 42000, 44000, 46000, 48000, 50000, 52000, 54000,
 56000, 58000, 60000, 62000, 64000, 66000, 68000, 70000, 72000, 74000, 76000, 
78000, 80000, 82000, 84000, 86000, 88000, 90000]

#for q in range(0,len(Arr)):

 #initial absorption length NOTE: if using 1 or 10 make sure to use 10.0 or 1.0 as there are other instances of the strings
 # 1 and 10 in the file
PhotonIN = 5000

#total number of jobs being submitted 
jobNUM = 1


#array that contains the different abs lengths being tested                          
#Arr = np.linspace(AbsIN,AbsFI,num=jobNUM)
#EnArr = np.linspace(EnIN,EnFI,num=jobNUM)

for q in range(len(Arr)):
#for q in range(10):
    
    sTag = "DataCollectionMuonTotalEneUnirr{0}[{1}]".format(q,Arr[q])
    # Creating new file                                                              
    shutil.copy2('photontestMuonWeight.mac', 'photest' '%s' '.mac' % q)

    # Reading in the file                                                            
    with open('photest' '%s' '.mac' % q, 'r') as file :
        filedata = file.read()

    # Replacing the target string   
    a =" "
    j = str(Arr[q])
    #i = str(Initial)
    i = str(PhotonIN)
    filedata = filedata.replace(i, j)

    # Writing out the new file                                                       
    with open('photest' '%s' '.mac' % q, 'w') as file:
        file.write(filedata)

    # Defining the infile                                                            
    InFile = 'photest' '%s' '.mac' % q 
    
    kw = {}

    kw["MYPREFIX"]  = ProdTag
    kw["WORKDIR"]   = WorkDir
    kw["OUTDIR"]    = OutDir
    kw["INPUT"]     = InFile
    kw["FILENAME"]  = sTag
    kw["DETTYPE"]   = DetType
    kw["LOGFILE"]   = logFile

    script_str = condor_script_template % kw
    f = open("%s/condor_jobs_%s_G4Sim.jdl"%(dirname,sTag), 'w')
    f.write(script_str)
    f.close()

    condorcmd = "condor_submit %s/condor_jobs_%s_G4Sim.jdl"%(dirname,sTag)
    print 'condorcmd: ', condorcmd
    print ('Executing condorcmd %s' % str(q))

    p=subprocess.Popen(condorcmd, shell=True)
    p.wait()
   
        
    print "\n"
    print "Histos output dir: %s/%s"%(OutDir,ProdTag)

