#ifndef CJOB_H_
#define CJOB_H_


#include <thread>
#include <stdio.h>

#include "CThread.h"

class CJob
{
private:
	int m_JobNo;
	char *m_JobName;
	CThread *m_pWorkThread;

public:
	CJob(void);
	virtual ~CJob(){}
	
	int GetJobNo(void)const{return m_JobNo;}
	void SetJobNo(int jobno){m_JobNo = jobno;}
	char *GetJobName(void) const {return m_JobName;}
	void SetJobName(char *jobname);
	CThread *GetWorkThread(void){ return m_pWorkThread; }
    void     SetWorkThread ( CThread *pWorkThread ){
        m_pWorkThread = pWorkThread;
    }
	virtual void Run(void *ptr) = 0;
};

#endif  //CJOB_H_
