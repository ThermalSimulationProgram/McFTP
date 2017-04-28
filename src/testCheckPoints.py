import os;
import time;


for i in range(0, 5):
	for slicev in range(10, 52, 2):
		command = './testsem -s ' + str(slicev);
		print command
		os.system(command);
		time.sleep(1)