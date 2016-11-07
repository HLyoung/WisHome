

#include "EventBase.h"

EventBase::EventBase():isRun(false)
{
	base = event_base_new();
	pthread_mutex_init(&event_mutex,NULL);
}

EventBase::~EventBase()
{
	pthread_mutex_lock(&event_mutex);
	event_base_free(base);
	pthread_mutex_unlock(&event_mutex);
	
	pthread_mutex_destroy(&event_mutex);
}

int EventBase::addEvent(struct event *events)
{
	int ret = 0;
	pthread_mutex_lock(&event_mutex);
	event_base_set(base,events);
	ret = event_add(events,NULL);
	pthread_mutex_unlock(&event_mutex);
	return ret;
}

int EventBase::delEvent(struct event* events)
{
	int ret = 0;
	pthread_mutex_lock(&event_mutex);
	ret = event_del(events);
	pthread_mutex_unlock(&event_mutex);
	return ret;
}

int EventBase::startRun()
{
	int ret = 0;
	pthread_mutex_lock(&event_mutex);
	if(!this->getStatus())
		ret = event_base_dispatch(base);
	setStatus(true);
	pthread_mutex_unlock(&event_mutex);
	return ret;
}
bool EventBase::getStatus()
{
	return isRun;
}

void EventBase::setStatus(bool Run)
{
	isRun = Run;
}

