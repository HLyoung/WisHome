

#include "tThread.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <errno.h>

tThread::tThread(){
	
}

tThread::~tThread(){
	
}

void *tThread::thread_entry(void *para)
{
	tThread *pThread = static_cast<tThread*>(para);
	return pThread->run();
}

int tThread::start(void)
{
	if(pthread_create(&pid,0,thread_entry,static_cast<void *>(this))<0)
	{
		std::cout<<"start timer thread failed,errorMsg:%s "<<strerror(errno)<<std::endl;
		return -1;
	}
	pthread_detach(this->pid);
	return 0;
}

int tThread::cancel(void)
{
	pthread_cancel(this->pid);
	return 0;
}