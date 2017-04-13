#coding:utf-8


import os
import copy
import time
import csv
  
from xml_api import *
from dir_utils import *
from tasks import *

def readcsv(name):
	tempdata = []
	with open(name, 'rb') as csvfile:
		r = csv.reader(csvfile, delimiter = ',')
		for row in r:
			for d in row:
				tempdata.append(float(d))
	return tempdata

def heat_cpu(configin):
	config = copy.deepcopy(configin)
	config.set_simulation_duration(100)
	config.is_save_result = False
	config.set_xml_csv_file_prefix('heatup_cpu')
	config.set_kernel('aptm')
	config.run()
	# time.sleep(30)

	
class FilePath():
	def __init__(self, root_path, sub_dir = '', filename = 'default'):
		self.rootPath = root_path
		self.subDir = sub_dir
		self.filename = filename
		self.update_final_path()
		

	def update_final_path(self):
		self.finalPath = self.rootPath + self.subDir + self.filename

	def set_root_path(self, path):
		self.rootPath = path
		self.update_final_path()

	def set_sub_dir(self, subdir):
		self.subDir = subdir
		self.update_final_path()

	def set_file_name(self, name):
		self.filename = name
		self.update_final_path()

	def get_root_path(self):
		return self.rootPath

	def get_sub_dir_path(self):
		return self.rootPath + self.subDir

	def get_final_path(self):
		return self.finalPath

	def get_filename(self):
		return self.filename




class Config(object):
	"""docstring for config"""
	def __init__(self):
		self.kernel_type = 'invalid kernel'
		self.valid_kernels = ['pboo']
		self.is_save_result = True
		self.csv_path = FilePath('./result/csv/')
		self.xml_path = FilePath('./result/xml/')
		self.duration_value = 10
		self.duration_unit = "sec"
		self.stage_number = 3;
		self.task_num = 5;
		self.task_period = 100;
		self.task_u = 0.4;
		self.update_task_set();
		self.kernel_type = 'pboo'
		self.kernel_ton_value = '{100, 100, 100}'
		self.kernel_ton_unit = 'ms'
		self.kernel_toff_value = '{50, 50, 50}'
		self.kernel_toff_unit = 'ms'
		self.fixedFrequency = False
		self.fixedActive = False
		self.xmlfileprefix = ''
		self.csvfileprefix = ''

	def update_xml_csv_filenames(self):
		same_filename = self.xmlfileprefix + '_' + self.kernel_type.lower()
		self.xml_path.set_file_name(same_filename)
		self.csv_path.set_file_name(same_filename)

	def set_xml_csv_file_prefix(self, prefix):
		self.xmlfileprefix = prefix
		self.csvfileprefix = prefix
		if self.kernel_type in self.valid_kernels:
			self.update_xml_csv_filenames()

	def set_xml_csv_sub_dir(self, subdir):
		self.set_xml_sub_dir(subdir)
		self.set_csv_sub_dir(subdir)

	def set_xml_sub_dir(self, subdir):
		self.xml_path.set_sub_dir(subdir)

	def set_csv_sub_dir(self, subdir):
		self.csv_path.set_sub_dir(subdir)

	def set_kernel(self, kernel_type):

		if kernel_type.lower() in self.valid_kernels:
			self.kernel_type = kernel_type.lower()
			self.update_xml_csv_filenames()
		else:
			print "Illegal kernel type input!"

	def update_task_set(self):
		self.taskSet = createTaskSet(self.task_num, self.task_period, self.task_u, self.stage_number)

	def set_task_set(self, task_num, period, utilization):
		self.task_num = task_num;
		self.task_period = period;
		self.task_u = utilization
		self.update_task_set()
	'''***********************************************'''

	
	def run_all_kernels(self, control = [1], sleeplength = 60):
		# sleeplength = 60
		# sleeplength = 0
		index = 0
		for kernel in self.valid_kernels:
			if control[index] > 0:
				time.sleep(sleeplength)
				self.set_kernel(kernel)
				self.run()
			index = index + 1




	def save_to_xml(self):

		if not self.kernel_type in self.valid_kernels:
			print "kernel type has not been set yet!"
			return

		make_dir(self.xml_path.get_root_path())
		make_dir(self.xml_path.get_sub_dir_path())
		make_dir(self.csv_path.get_root_path())
		make_dir(self.csv_path.get_sub_dir_path())

		tree = ElementTree()
		simulation = create_node('experiment', { 'name': self.csv_path.get_final_path() }, "")

		duration    = create_time_node('duration', self.duration_value, self.duration_unit);
		pipeline    = create_node('processor', {'core_number':str(self.stage_number), 'fixed_frequency':str(self.fixedFrequency), 'fixed_active':str(self.fixedActive)}, "")
		save_result = create_node('save_result', {'value':str(self.is_save_result)}, "")
		
		simulation.append(duration)
		simulation.append(save_result)
		simulation.append(pipeline)
	

		tasks     = create_node('tasks', {}, "")

		for task in self.taskSet:
			tasknode = task.to_xml_node()
			tasks.append(tasknode)

		simulation.append(tasks)

		scheduler = create_node('thermal_approach', {}, "")
		kernel    = create_node('kernel', {'type':self.kernel_type.upper()}, "")
		# if (self.kernel_ton_value < 1):
		# 	self.kernel_ton_value = self.kernel_ton_value * 1000
		# 	self.kernel_ton_unit = 'us'
		# if (self.kernel_toff_value < 1):
		# 	self.kernel_toff_value = self.kernel_toff_value * 1000
		# 	self.kernel_toff_unit = 'us'
		if ((self.kernel_type == 'pboo') or (self.kernel_type == 'ge')):
			ton  = create_time_node('ton', self.kernel_ton_value, self.kernel_ton_unit)
			toff = create_time_node('toff', self.kernel_toff_value, self.kernel_toff_unit)
			kernel.append(ton)
			kernel.append(toff)
		

		scheduler.append(kernel)
		simulation.append(scheduler)

		tree._setroot(simulation)

		saved_xml_name = self.xml_path.get_final_path() + '.xml'
		pretty_write_xml(tree, saved_xml_name)

	def run(self, flag = False):
		self.save_to_xml()
		main_to_xml_path = './' + self.xml_path.get_final_path()
		exe_command(main_to_xml_path, flag)
		






		
if __name__ == "__main__": 
	config_me = Config()
	config_me.save_to_xml()


