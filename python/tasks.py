
import random

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
	def __init__(self, _period, _utilization, _nstage):
		self.period = _period
		self.u = _utilization
		self.nstage = _nstage

		wcets = []
		for i in range(0, _nstage):
			wcets.append(_period*_utilization)
		self.wcets = wcets

			

		


def createTaskSet(period, utilization, _nstage):
	task_num = random.randint(1, 10);

	vectU = UUniFast(task_num, utilization);
	taskSet = [];

	for i in xrange(0, task_num):
		newtask = Task(period, vectU[i], _nstage)
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


