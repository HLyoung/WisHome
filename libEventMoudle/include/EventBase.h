

#ifndef _EVENT_BASE_H_
#define _EVENT_BASE_H_

#include <event.h>
#include <pthread.h>
#include <stdlib.h>
#include <malloc.h>


class EventBase
{
public:
	EventBase();
	~EventBase();
	int addEvent(struct event *events);
	int delEvent(struct event *events);
	int startRun();
	bool getStatus();
	void setStatus(bool Run);
private:
    struct event_base *base;
	pthread_mutex_t event_mutex;
	bool isRun;
};
#endif