#!/usr/bin/python
import os, sys
import shlex, subprocess
from datetime import datetime, date
import time
import shutil
import numpy as np   
#sys.path.append(os.path.abspath(os.path.curdir))
from random import randint

JobTime = datetime.now()
fTag = JobTime.strftime("%Y%m%d_%H%M%S")
dirname = "jobs/%s_%s"
#%(sTag,fTag)
DetType = "2" #0:Tile 1:Rod 2:Thin Rod 3:Disc 4:Mirrored Disc
if DetType == "1" or DetType == "5":
	Thickness = "1"
else:
	Thickness = ".4"

logFile = "0606.log"
Rad = "Unirr"

try:
    os.makedirs(dirname)
except:
    pass

ProdTag = "Run_PlanarDist5000-10milChopped"
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
#Arr = [3.0538, 3.0463, 3.0388, 3.0314, 3.024, 3.0166, 3.0093, 3.002, 2.9948, 2.9876]
#3.5424, 3.4925, 3.444, 3.3968, 3.3509, 3.3062, 3.2627, 3.2204, 3.1791, 3.1388]
#,]
#3.1309, 3.123, 3.1152, 3.1074, 3.0996, 3.0919, 3.0842, 3.0765, 3.0689, 3.0613
#for q in range(0,len(Arr)):

 #initial absorption length NOTE: if using 1 or 10 make sure to use 10.0 or 1.0 as there are other instances of the strings
 # 1 and 10 in the file
AbsIN = 0
EnIN = 3.5424

#total number of jobs being submitted 
jobNUM = 100

#final absorption length  
AbsFI = jobNUM
EnFI = 2.7552


#array that contains the different abs lengths being tested                          
Arr = np.linspace(AbsIN,AbsFI,num=jobNUM)
EnArr = np.linspace(EnIN,EnFI,num=jobNUM)

def random_N_digit(n):
    range_start = 10**(n-1)
    range_end = (10**n)-1
    return randint(range_start, range_end)

#Random Numbers to start with (1: 9 digit, 2: 8 digit) make sure these are in photontestplanar too
RandS1 = 498333355
RandS2 = 72626948

for q in range(len(Arr)):

    #Replacement Random Numbers
    RandF1 = random_N_digit(9)
    RandF2 = random_N_digit(8)
    
    if DetType == "5":
    	sTag = "DataCollectionPlanar{}MirroredRod{}".format(Rad,q)
    if DetType == "4":
    	sTag = "DataCollectionPlanar{}Mirrored{}".format(Rad,q)
    if DetType == "3":
    	sTag = "DataCollectionPlanar{}Disc{}".format(Rad,q)
    if DetType == "2":
    	sTag = "DataCollectionPlanar{}ThinRod{}".format(Rad,q)
    if DetType == "1":
    	sTag = "DataCollectionPlanar{}Rod{}".format(Rad,q)
    if q % 200 + 1 == 1 and q != 0:
    	#time.sleep(2400)
    	command = ["condor_q ahorst"]
    	p = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
        text= p.stdout.readlines()
        #print text
        #print(type(text))
        abort_after = 60 * 60
        start = time.time()       
	while any(" 0 running" not in s for s in text):
        	time.sleep(100)
        	delta = time.time() - start
        	command = ["condor_q ahorst"]
        	p = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
        	text = p.stdout.readlines()
       		if any(" 0 running" in s for s in text):
        		break
         	if delta >= abort_after:
        		break
        continue
    else:
        # Creating new file       

	if Thickness == ".4":
		shutil.copy2('photontestPlanar.mac', 'photest' '%s' '.mac' % q)
	else:
		shutil.copy2('photontestPlanarRod.mac', 'photest' '%s' '.mac' % q)



        # Reading in the file                                                            
        with open('photest' '%s' '.mac' % q, 'r') as file :
            filedata = file.read()
        # Replacing the target string
	    a =" "
	    j = str(RandF1)+a+str(RandF2)
	    #i = str(Initial)
	    i = str(RandS1)+a+str(RandS2)
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
    
        #Make Sure Correct Numbers Are Replaced Next Loop
        RandF1 = RandS1
        RandF2 = RandS2
#command = ["condor_q ahorst"]
#p = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
#text= p.stdout.readlines()
#abort_after = 120 * 60
#start = time.time() 
#while any(" 0 running" not in s for s in text):
#        time.sleep(100)
#        delta = time.time() - start
#        command = ["condor_q ahorst"]
#        p = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
#        text = p.stdout.readlines()
#       	if any(" 0 running" in s for s in text):
#		if DetType == "3":
#			shutil.move("/home/ahorst/UMDSRDGEStudy-build/DataCollectionPlanar{}Disc*.root".format(Rad), "/home/ahorst/UMDSRDGEStudy-build/Absdata/Run_PlanarDist2000-10milChopped/Rod115Irr/DataCollectionPlanarIrrDisc*.root")
#			os.remove("/home/ahorst/UMDSRDGEStudy-build/photest*.mac")
#			os.remove("/home/ahorst/UMDSRDGEStudy-build/DataCollectionPlanar{}Disc*.txt".format(Rad))
#			break
#		if DetType == "1":
#			shutil.move("/home/ahorst/UMDSRDGEStudy-build/DataCollectionPlanar{}Rod*.root".format(Rad), "/home/ahorst/UMDSRDGEStudy-build/Absdata/Run_PlanarDist2000-10milChopped/Rod115Irr/DataCollectionPlanarIrrRod*.root")
#			os.remove("/home/ahorst/UMDSRDGEStudy-build/photest*.mac")
#			os.remove("/home/ahorst/UMDSRDGEStudy-build/DataCollectionPlanar{}Rod*.txt".format(Rad))
#			break
#		if DetType == "2":
#			shutil.move("/home/ahorst/UMDSRDGEStudy-build/DataCollectionPlanar{}Mirrored*.root".format(Rad), "/home/ahorst/UMDSRDGEStudy-build/Absdata/Run_PlanarDist2000-10milChopped/Rod115Irr/DataCollectionPlanarIrrMirrored*.root")
#			os.remove("/home/ahorst/UMDSRDGEStudy-build/photest*.mac")
#			os.remove("/home/ahorst/UMDSRDGEStudy-build/DataCollectionPlanar{}Mirrored*.txt".format(Rad))
#        		break
#		
#        if delta >= abort_after:
#        	break
#	continue
