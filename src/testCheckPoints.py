import os;
import time;


for i in range(0, 5):
	for slicev in range(10000, 200000, 10000):
		command = './testsem -s ' + str(slicev);
		print command
		os.system(command);
		time.sleep(1)