

#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <iostream>

#include "TimerThread.h"

TimerThread *TimerThread::_instance = NULL;

TimerThread::TimerThread(){
}

TimerThread::~TimerThread(){
	
}


void *TimerThread::run(void)
{
	while(true)
	{
		list<Timer*>::iterator ite;
		_time_list_mutex.lock();
		for(ite = this->_timer_list.begin();ite != this->_timer_list.end();ite++)
		{
			(*ite)->leftsecs --;
			if((*ite)->leftsecs == 0)
			{
				(*ite)->_callback((*ite)->_args);
				(*ite)->leftsecs = (*ite)->_interval;
			}
		}
		_time_list_mutex.unlock();				
		sleep(1);
	}
	
	return (void *)0;
}

void TimerThread::Register(Timer* _timer)
{	

	std::lock_guard<std::mutex> lg(_time_list_mutex);   //this means we can`t regist a timer in another timer`s handler.
	_timer_list.push_back(_timer);

}

void TimerThread::unRegister(Timer* _timer)
{
//	std::lock_guard<std::mutex> lg(_time_list_mutex);   //without a lock is not safe ,but if we locked this,we can`t unregist a timer in another timer,this is embarassed.
	std::list<Timer*>::iterator ite = _timer_list.begin();
	for(;ite != _timer_list.end();ite++)
		if(*ite == _timer){
			_timer_list.erase(ite);
			return;
			}
}

TimerThread *TimerThread::get_instance()
{
	if(_instance == NULL)
	{
		_instance = new TimerThread();
	}
	
	return _instance;
}


long int  get_systime_clock()
{
	struct timeval now;
	gettimeofday(&now,NULL);
	return now.tv_sec*1000 + now.tv_usec/1000;
}