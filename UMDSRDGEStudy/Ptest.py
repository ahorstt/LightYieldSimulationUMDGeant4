import os, sys
import subprocess

Abs = 72
counter = 1
command = ['./LYSim 1 photontest.mac presults_0505_c >& pmess_0505_c.log']
for counter in range (1,100):
    Abs = Abs + 0.05
    # Read in the file
    with open('photontest.mac', 'r') as file :
        filedata = file.read()

        # Replace the target string
    AbsPlusSome = Abs + 0.05
    j = str(AbsPlusSome)
    i = str(Abs)
    filedata = filedata.replace(i, j)

    # Write the file out again
    with open('photontest.mac', 'w') as file:
        file.write(filedata)

    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
    process.wait()
    print process.returncode

