# -*- coding=utf-8 -*-  
# python2.7
import os
from Config import *

import time


config = Config()

# create a new sub directory
config.set_xml_csv_sub_dir('temperature_profile/')

config.set_simulation_duration(100)
config.set_relative_deadline(1000)

ton = [100  ,100,   100,   100];
config.set_kernel_ton(ton);

base_value = 0
# config.is_save_result = False
control = [0, 0, 1]
for x in range(0, 20):	
	new_value = base_value + x*1
	# change the value
	toff = [new_value, new_value, new_value, new_value]
	config.set_kernel_toff(toff);

	# give the prefix for the files
	config.set_xml_csv_file_prefix('toff' + str(new_value))
	config.run_all_kernels(control, 100)