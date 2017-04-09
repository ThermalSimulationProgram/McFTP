from Config import *


config = Config();


U = 0.5;
task_num = 5;
base = 30;

periods_num = 70;
test_num_for_each_period = 15;
control = [1]
# varying periods
for i in range(0, periods_num):
	new_value = base + i*1
	config.set_xml_csv_sub_dir('period-'+str(new_value)+'/')
	for x in xrange(0, test_num_for_each_period):
		config.set_task_set(task_num, new_value, U)
		config.set_xml_csv_file_prefix('random'+str(x));
		config.run_all_kernels(control, 0)



periods_num = 0;
test_num_for_each_period = 20;
control = [1]
config.fixedFrequency = True;
# varying periods
for i in range(0, periods_num):
	new_value = base + i*5
	config.set_xml_csv_sub_dir('fixedFrequencyperiod-'+str(new_value)+'/')
	for x in xrange(0, test_num_for_each_period):
		config.set_task_set(task_num, new_value, U)
		config.set_xml_csv_file_prefix('random'+str(x));
		config.run_all_kernels(control, 0)

periods_num = 0;
test_num_for_each_period = 20;
control = [1]
config.fixedFrequency = False;
config.fixedActive = True;
# varying periods
for i in range(0, periods_num):
	new_value = base + i*5
	config.set_xml_csv_sub_dir('fixedActiveperiod-'+str(new_value)+'/')
	for x in xrange(0, test_num_for_each_period):
		config.set_task_set(task_num, new_value, U)
		config.set_xml_csv_file_prefix('random'+str(x));
		config.run_all_kernels(control, 0)
