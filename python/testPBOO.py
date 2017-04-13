from Config import *

import os

os.system('sudo cpupower -c all frequency-set -g userspace')



config = Config();
config.duration_value = 200;

U = 0.5;
task_num = 5;
base = 0.4;

period = 100;
pboo_num = 10;
test_num_for_each_pboo = 1;
control = [1]
# varying periods
for i in range(0, pboo_num):
	new_value = base + i*0.2
	config.set_xml_csv_sub_dir('testpbooptm-'+str(new_value)+'/')
	for x in xrange(0, test_num_for_each_pboo):
		config.set_task_set(task_num, period, U)
		tons = [];
		for j in range(0, config.stage_number):
			tons.append(new_value)

		config.kernel_ton_value = str(tons)
		config.kernel_toff_value = str(tons)
		config.set_xml_csv_file_prefix('random'+str(x));
		config.run_all_kernels(control, 1)
















