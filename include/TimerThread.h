

#ifndef _TIMER_THREAD_H_
#define _TIMER_THREAD_H_

#include <stdio.h>
#include "tThread.h"
#include <list>
#include <mutex>


using namespace std;

struct Timer
{
	void *_args;
	int (*_callback)(void *);
	int _interval;
	int leftsecs;
	
	void open(int interval,int (*callback)(void *),void *args)
	{
		_args = args;
		_interval = interval;
		leftsecs = _interval;
		_callback = callback;
	}
	
	bool operator < (Timer _timer)
	{
		return _timer.leftsecs < this->leftsecs;
	}
	
	bool operator == (Timer _timer)
	{
		return _timer.leftsecs == this->leftsecs;
	}
};

class TimerThread : public tThread
{
public:
	static TimerThread *_instance;
	static TimerThread *get_instance();
	virtual void *run(void);
	virtual ~TimerThread();
	void Register(Timer* _timer);
	void unRegister(Timer* _timer);
private:
	TimerThread();
	std::list<Timer*> _timer_list;
	std::mutex _time_list_mutex;
};

extern long int  get_systime_clock();

#define TIMERMANAGER TimerThread::get_instance()




#endif