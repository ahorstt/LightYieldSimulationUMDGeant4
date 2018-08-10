#!/usr/bin/python
import os, sys
import errno
import shlex 
import subprocess
from datetime import datetime, date
import time
import shutil
import numpy as np   
import pylab as pl
#sys.path.append(os.path.abspath(os.path.curdir))

JobTime = datetime.now()
fTag = JobTime.strftime("%Y%m%d_%H%M%S")
sTag = "DataCollectionhgcal"
dirname = "jobs/%s_%s"%(sTag,fTag)
logFile = "0607.log"

ProdTag = "RunHgcal_2018"
OutDir  = "/home/ahorst/hgcal_tile/build/Absdata"
WorkDir = "/home/ahorst/hgcal_tile/build/"

try:
    os.makedirs(WorkDir+dirname)
except:
    pass

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
Executable = condor-executable.sh
should_transfer_files = NO
Requirements = TARGET.FileSystemDomain == "privnet"
Error  = %(OUTDIR)s/%(MYPREFIX)s/logs/%(FILENAME)s_sce_$(cluster)_$(process).stderr
Log    = %(OUTDIR)s/%(MYPREFIX)s/logs/%(FILENAME)s_sce_$(cluster)_$(process).condor
Output = %(OUTDIR)s/%(MYPREFIX)s/logs/%(FILENAME)s_sce_$(cluster)_$(process).stdout
Arguments = %(INPUT)s %(OUTDIR)s %(FILENAME)s
Queue
"""
#Log    = /dev/null
#Output = /dev/null
######################%(WORKDIR)s %(INPUT)s %(FILENAME)s %(DETTYPE)s###################
# %(OUTDIR)s/%(MYPREFIX)s/


 #initial absorption length NOTE: if using 1 or 10 make sure to use 10.0 or 1.0 as there are other instances of the strings
 # 1 and 10 in the file
AbsIN = -2.0
AbsIN2 = -2.0

#final absorption length  
AbsFI = 2.0
AbsFI2 = 2.0

#total number of jobs being submitted (squared)
jobNUM = 2
jobNUM2 = 2

#function for linspace that produces numbers spaced according to a sine function
def sinspace(start, stop, num=50):
    ls = np.linspace(-np.pi / 2, np.pi / 2, num) #get linearly interpolated angles
    ss = np.sin(ls) / 2 + 0.5 #take sin, transform to fit the range b/w [0,1]
    return ss * (stop - start) + start #transform desired range

#array that contains the different abs lengths being tested                          
Arr = np.linspace(AbsIN,AbsFI,num=jobNUM2)
Arr2 = np.linspace(AbsIN2,AbsFI2,num=jobNUM2)

q=0
a = False
for x in range(len(Arr)):
	for y in range(len(Arr2)):
		if q >= 0:
                	# Creating new file                                                              
                	shutil.copy2('photontestHisto.mac', 'photest' '%s' '.mac' % q)

	        	# Reading in the file                                                            
	        	with open('photest' '%s' '.mac' % q, 'r') as file :
	        		filedata = file.read()

	        	# Replacing the target string   
	        	a =" "
	        	j = str(Arr[x])+a+str(Arr2[y])+a+str(0.)
	        	i = str(AbsIN)+a+str(AbsIN2)+a+str(0.)
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
                	kw["LOGFILE"]   = logFile
			
			filename = "%s/condor_jobs_%s_G4Sim.jdl"%(dirname,sTag)
                	script_str = condor_script_template % kw

                	f = open(filename,'w')
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
            
#                	if q % 200 + 1 == 1:
#                    		#time.sleep(2400)
#                    		command = ["condor_q ahorst"]
#                    		p = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
#                    		text= p.stdout.readlines()
#                    		#print text
#                    		#print(type(text))
#                    		abort_after = 60 * 60
#                    		start = time.time()
#                    		while any(" 0 running" not in s for s in text):
#                        		time.sleep(100)
#                        		delta = time.time() - start
#                        		command = ["condor_q ahorst"]
#                        		p = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
#                        		text = p.stdout.readlines()
#                        		if any(" 0 running" in s for s in text):
#                        	    		break
#                        		if delta >= abort_after:
#                        	    		break
#                    		continue
#            	    	else:
#                 		# Creating new file
#               			shutil.copy2('photontestHisto.mac', 'photest' '%s' '.mac' % q)
#
#
#                		# Reading in the file
#                		with open('photest' '%s' '.mac' % q, 'r') as file :
#                		    filedata = file.read()
#
#                		# Replacing the target string
#                		a =" "
#                		j = str(Arr[x])+a+str(Arr2[y])+a+str(0)
#                		#i = str(Initial)
#                		i = str(AbsIN)+a+str(AbsIN2)+a+str(0)
#                		filedata = filedata.replace(i, j)
#	
#                		# Writing out the new file
#                		with open('photest' '%s' '.mac' % q, 'w') as file:
#                		    file.write(filedata)
#
#                		# Defining the infile
#                		InFile = 'photest' '%s' '.mac' % q
#                		q+=1
