#!/usr/bin/python
import os, sys
import shlex 
import subprocess
from datetime import datetime, date
import time
import shutil
import numpy as np   
import time
import pylab as pl
#sys.path.append(os.path.abspath(os.path.curdir))

JobTime = datetime.now()
fTag = JobTime.strftime("%Y%m%d_%H%M%S")
sTag = "DataCollectionTile"
dirname = "jobs/%s_%s"%(sTag,fTag)
DetType = "0" #Tile
logFile = "0607.log"

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
Output = /dev/null
Error  = %(OUTDIR)s/%(MYPREFIX)s/logs/%(FILENAME)s_sce_$(cluster)_$(process).stderr
#Log    = %(OUTDIR)s/%(MYPREFIX)s/logs/%(FILENAME)s_sce_$(cluster)_$(process).condor
#Output = %(OUTDIR)s/%(MYPREFIX)s/logs/%(FILENAME)s_sce_$(cluster)_$(process).stdout
Log    = /dev/null
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
AbsIN = 0.0
AbsIN2 = 0.0

#final absorption length  
AbsFI = 0.0
AbsFI2 = 0.0

#total number of jobs being submitted (squared)
jobNUM = 1
jobNUM2 = 1

#function for linspace that produces numbers spaced according to a sine function
def sinspace(start, stop, num=50):
    ls = np.linspace(-np.pi / 2, np.pi / 2, num) #get linearly interpolated angles
    ss = np.sin(ls) / 2 + 0.5 #take sin, transform to fit the range b/w [0,1]
    return ss * (stop - start) + start #transform desired range

#array that contains the different abs lengths being tested                          
Arr = np.linspace(AbsIN2,AbsFI2,num=jobNUM2)
Arr2 = np.linspace(AbsIN2,AbsFI2,num=jobNUM2)

q=0
a = False
for x in range(len(Arr)):
	for y in range(len(Arr2)):

        	if (-2.4<Arr[x]<2.4 and 4.5<Arr2[y]<4.7) or (-2.4<Arr[x]<2.4 and -4.7<=Arr2[y]<-4.5):
            		a = True
        	elif (-2.40<Arr2[y]<2.40 and -4.7<Arr[x]<-4.5):
            		a = True
        	elif (-5.0<Arr2[y]<2.40 and 4.5<Arr[x]<4.7):
            		a = True
        	elif (1.9<Arr2[y]<2.90 and 4.3<Arr[x]<4.5) or (1.9<Arr2[y]<2.90 and -4.5<Arr[x]<-4.3) or (-2.90<Arr2[y]<-1.9 and -4.5<Arr[x]<-4.3):
            		a = True
        	elif (2.5<Arr2[y]<3.00 and 4.1<Arr[x]<4.3) or (2.5<Arr2[y]<3.00 and -4.3<Arr[x]<-4.1) or (-3.00<Arr2[y]<-2.5 and -4.3<Arr[x]<-4.1):
            		a = True
        	elif (2.9<Arr2[y]<3.4 and 3.85<Arr[x]<4.10) or (2.9<Arr2[y]<3.4 and -4.10<Arr[x]<-3.85) or (-3.4<Arr2[y]<-2.9 and -4.10<Arr[x]<-3.85):
            		a = True
        	elif (3.2<Arr2[y]<3.50 and 3.65<Arr[x]<3.85) or (3.2<Arr2[y]<3.50 and -3.85<Arr[x]<-3.65) or (-3.50<Arr2[y]<-3.2 and -3.85<Arr[x]<-3.65) or (-3.50<Arr2[y]<-3.2 and 3.65<Arr[x]<3.85):
            		a = True
        	elif (3.30<Arr2[y]<3.60 and 3.5<Arr[x]<3.65) or (3.30<Arr2[y]<3.60 and -3.65<Arr[x]<-3.5) or (-3.60<Arr2[y]<-3.30 and -3.65<Arr[x]<-3.5) or (-3.60<Arr2[y]<-3.30 and 3.5<Arr[x]<3.65):
            		a = True		
        	elif (3.5<Arr2[y]<3.80 and 3.3<Arr[x]<3.6) or (3.5<Arr2[y]<3.80 and -3.6<Arr[x]<-3.3) or (-3.80<Arr2[y]<-3.5 and -3.6<Arr[x]<-3.3) or (-3.80<Arr2[y]<-3.5 and 3.3<Arr[x]<3.6):
            		a = True	
        	elif (3.6<Arr2[y]<4.0 and 3.15<Arr[x]<3.4) or (3.6<Arr2[y]<4.0 and -3.4<Arr[x]<-3.15) or (-4.0<Arr2[y]<-3.6 and -3.4<Arr[x]<-3.15) or (-4.0<Arr2[y]<-3.6 and 3.15<Arr[x]<3.4):
            		a = True	
        	elif (3.8<Arr2[y]<4.1 and 2.85<Arr[x]<3.15) or (3.8<Arr2[y]<4.1 and -3.15<Arr[x]<-2.85) or (-4.1<Arr2[y]<-3.8 and -3.15<Arr[x]<-2.85) or (-4.1<Arr2[y]<-3.8 and 2.85<Arr[x]<3.15):
            		a = True	
        	elif (3.9<Arr2[y]<4.3 and 2.5<Arr[x]<3.0) or (3.9<Arr2[y]<4.3 and -3.0<Arr[x]<-2.5) or (-4.3<Arr2[y]<-3.9 and -3.0<Arr[x]<-2.5) or (-4.3<Arr2[y]<-3.9 and 2.5<Arr[x]<3.0):
            		a = True	
        	elif (4.1<Arr2[y]<4.4 and 2.4<Arr[x]<2.6) or (4.1<Arr2[y]<4.4 and -2.6<Arr[x]<-2.4) or (-4.4<Arr2[y]<-4.1 and -2.6<Arr[x]<-2.4) or (-4.4<Arr2[y]<-4.1 and 2.4<Arr[x]<2.6):
            		a = True	
		elif (4.35<Arr2[y]<4.55 and 1.9<Arr[x]<2.6) or (4.35<Arr2[y]<4.55 and -2.6<Arr[x]<-1.9) or (-4.55<Arr2[y]<-4.35 and -2.6<Arr[x]<-1.9) or (-4.55<Arr2[y]<-4.35 and 1.9<Arr[x]<2.6):
            		a = True
		else:
        		a = False

			if q >= 1: # and q <= 300:
                		# Creating new file                                                              
                		shutil.copy2('photontest.mac', 'photest' '%s' '.mac' % q)

	        		# Reading in the file                                                            
	        		with open('photest' '%s' '.mac' % q, 'r') as file :
	        		    filedata = file.read()

	        		# Replacing the target string   
	        		a =" "
	        		j = str(Arr[x])+a+str(Arr2[y])+a+str(0)
	       			#i = str(Initial)
	        		i = str(AbsIN)+a+str(AbsIN2)+a+str(0)
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
                
                		q+=1
                
                		print "\n"
                		print "Histos output dir: %s/%s"%(OutDir,ProdTag)
            
                		if q % 100 + 1 == 1:
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
               			shutil.copy2('photontest.mac', 'photest' '%s' '.mac' % q)

                		# Reading in the file
                		with open('photest' '%s' '.mac' % q, 'r') as file :
                		    filedata = file.read()

                		# Replacing the target string
                		a =" "
                		j = str(Arr[x])+a+str(Arr2[y])+a+str(0)
                		#i = str(Initial)
                		i = str(AbsIN)+a+str(AbsIN2)+a+str(0)
                		filedata = filedata.replace(i, j)
	
                		# Writing out the new file
                		with open('photest' '%s' '.mac' % q, 'w') as file:
                		    file.write(filedata)

                		# Defining the infile
                		InFile = 'photest' '%s' '.mac' % q
                		q+=1
		if a == True:
			# Creating new file
               		shutil.copy2('photontest.mac', 'photest' '%s' '.mac' % q)

                	# Reading in the file
                	with open('photest' '%s' '.mac' % q, 'r') as file :
                	    filedata = file.read()

                	# Replacing the target string
                	a =" "
                	j = str(Arr[x])+a+str(Arr2[y])+a+str(0)
                	#i = str(Initial)
                	i = str(AbsIN)+a+str(AbsIN2)+a+str(0)
                	filedata = filedata.replace(i, j)

                	# Writing out the new file
                	with open('photest' '%s' '.mac' % q, 'w') as file:
                	    file.write(filedata)

                	# Defining the infile
                	InFile = 'photest' '%s' '.mac' % q
                	q+=1
