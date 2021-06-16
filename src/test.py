import os
import sys
import subprocess
import numpy as np

if __name__ == "__main__":
    
    depth = np.arange(2, 20, 2).astype(np.int)
    train_size = 0.7
    information_gain = np.arange(0.005, 0.05, 0.005).astype(np.float)
    minsamples = np.arange(0, 20, 2).astype(np.int)

    data = "tic-tac-toe"
    if len(sys.argv) > 1:
        data = sys.argv[1]

    max_cr = 0.0
    for d in depth:
        for ig in information_gain:
            for s in minsamples:
                command = "main.exe -data {} -ts 0.7 -d {} -ig {} -s {}".format(data, str(d), str(ig), str(s))

                process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
                process.wait()
                command_output = process.stdout.read().decode('utf-8')
                idx = command_output.find("Correct rate")
                cr = float(command_output[idx + 15 : ])
                
                if cr > max_cr:
                    max_cr = cr
                    print(command, cr)

                command = command + " -R"

                process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
                process.wait()
                command_output = process.stdout.read().decode('utf-8')
                idx = command_output.find("Correct rate")
                cr = float(command_output[idx + 15 : ])
                
                if cr > max_cr:
                    max_cr = cr
                    print(command, cr)

# process = subprocess.Popen("main.exe -data tic-tac-toe -d 100 -ts 0.7 -ig 0.01 -s 0", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
# process.wait()
# command_output = process.stdout.read().decode('utf-8')

# idx = command_output.find("Correct rate")
# cr = float(command_output[idx + 15: ])

# print(cr)