import os;
import time;


for i in range(0, 30):
	for slicev in range(10000, 1010000, 10000):
		command = './testsem -s ' + str(slicev);
		print command
		os.system(command);
	