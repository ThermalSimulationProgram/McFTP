from Config import *

import os

os.system('sudo cpupower -c all frequency-set -g userspace')



config = Config();
config.stage_number = 4;
config.duration_value = 200;

U = 1;
task_num = 5;
base = 0;

period = 100;
pboo_num = 50;
test_num_for_each_pboo = 1;
control = [1]

#heat up
config.duration_value = 300;
for i in range(0, 1):
	new_value = base + i*1
	config.set_xml_csv_sub_dir('testpbooptm-'+str(new_value)+'/')
	for x in xrange(0, test_num_for_each_pboo):
		config.set_task_set(task_num, period, U)
		tons = [];
		toffs=[];
		for j in range(0, config.stage_number):
			tons.append(new_value)
			toffs.append(0)

		config.kernel_ton_value = str(tons)
		config.kernel_toff_value = str(toffs)
		config.set_xml_csv_file_prefix('random'+str(x));
		config.run_all_kernels(control, 100)

config.duration_value = 100;
# varying periods
for i in range(0, pboo_num):
	new_value = base + i*2
	config.set_xml_csv_sub_dir('testpbooptm-'+str(new_value)+'/')
	for x in xrange(0, test_num_for_each_pboo):
		config.set_task_set(task_num, period, U)
		tons = [];
		toffs=[];
		for j in range(0, config.stage_number):
			tons.append(50)
			toffs.append(new_value)

		config.kernel_ton_value = str(tons)
		config.kernel_toff_value = str(toffs)
		config.set_xml_csv_file_prefix('random'+str(x));
		config.run_all_kernels(control, 100)
















