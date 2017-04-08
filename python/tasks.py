
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
		self.period = _period
		self.u = _utilization
		self.nstage = _nstage
		self.name = _name
		self.type = "busy_wait"
		self.periodicity = "periodic"
		self.attached_cores = list(range(0, _nstage))


		wcets = []
		for i in range(0, _nstage):
			wcets.append( int(_period*_utilization+0.5) )
		self.wcets = wcets

	def to_xml_node(self):
		task_node = create_node(str(self.name), {'type':str(self.type), 'periodicity':self.periodicity, 'name':str(self.name)}, "")
		period = create_time_node("period", self.period, "ms");
		wcets = create_time_node("wcets", self.wcets, "ms");
		attached_cores = create_node("attached_cores", {'value':str(self.attached_cores)}, "");

		task_node.append(period)
		task_node.append(wcets)
		task_node.append(attached_cores)

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


