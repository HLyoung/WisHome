

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
		long int start_clock = get_systime_clock();
		this->_timer_list.sort();
		list<Timer>::iterator ite;

		std::unique_lock<std::mutex> ul(_time_list_mutex,std::defer_lock);
		ul.lock();
		for(ite = this->_timer_list.begin();ite != this->_timer_list.end();ite++)
		{
			ite->leftsecs --;
			if(ite->leftsecs == 0)
			{
				ite->_callback(ite->_args);
				ite->leftsecs = ite->_interval;
			}
		}
		ul.unlock();
		
		long int  end_clock = get_systime_clock();
		
		sleep(1);
	}
	
	return (void *)0;
}

void TimerThread::Register(Timer _timer)
{	

	std::lock_guard<std::mutex> lg(_time_list_mutex);
	_timer_list.push_back(_timer);

}

void TimerThread::unRegister(Timer _timer)
{
	std::lock_guard<std::mutex> lg(_time_list_mutex);
	_timer_list.remove(_timer);
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