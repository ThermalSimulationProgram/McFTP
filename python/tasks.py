
import random
from xml_api import *

def UUniFast(n, U):
	sumU = U;
	vectU = []
	for i in range(1, n):
		r = random.random()**(1.0/(n - i))
		nextSumU = sumU*r
		vectU.append(sumU - nextSumU)
		sumU = nextSumU

	vectU.append(sumU)
	return vectU




class Task(object):
	def __init__(self, _period, _utilization, _nstage, _name):
		self.name = _name
		self.type = "pipelined"
		self.periodicity = "periodic"
		self.task_load = "busywait"

		self.benchmark_name = "default"
		self.user_defined_load_id = 1;

		self.period = _period
		self.u = _utilization
		self.nstage = _nstage
		
		self.attached_cores = list(range(0, _nstage))
		
		

		wcets = []
		for i in range(0, _nstage):
			w = int(_period*_utilization+0.5) 
			if (w<1):
				w = 1
			wcets.append(w)
		self.wcets = wcets

	def to_xml_node(self):
		task_node = create_node(str(self.name), {'type':str(self.type), 'periodicity':self.periodicity, 'load_type':self.load_type,'name':str(self.name)}, "")
		period = create_time_node("period", self.period, "ms");
		wcets = create_time_node("wcets", self.wcets, "ms");
		attached_cores = create_node("attached_cores", {'value':str(self.attached_cores)}, "");



		task_node.append(period)
		task_node.append(wcets)
		task_node.append(attached_cores)

		if (self.load_type == 'benchmark'):
			benchmark = create_node("benchmark",{'name':self.benchmark_name},"");
			task_node.append(benchmark)
		if (self.load_type == 'user_defined'):
			userload = create_node("defined_load",{'index':str(self.user_defined_load_id)},"");
			task_node.append(userload)

		return task_node


			
def createTaskSet(task_num, period, utilization, _nstage):
	# task_num = random.randint(1, 10);

	vectU = UUniFast(task_num, utilization);
	taskSet = [];
	name_prefix = 'task'

	for i in xrange(0, task_num):
		name = name_prefix + str(i)
		newtask = Task(period, vectU[i], _nstage, name)
		taskSet.append(newtask)

	return taskSet





if __name__ == "__main__":
	taskset = createTaskSet(100, 0.7, 2)

	allu = 0
	for task in taskset:
		allu = allu + task.u
		print task.u
		print task.wcets

	print allu


