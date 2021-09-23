#ifndef JOBLOG_H
#define JOBLOG_H



class JobLog {
private:
   int workerId;

   int taskId;

public:
   JobLog(int _workerid, int _taskid);

   int getWorkerId();

   int getTaskId();

   bool isMatch(int _workerid, int _taskid);
};


#endif
