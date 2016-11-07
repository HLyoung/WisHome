
#ifndef _TTHREAD_H_
#define _TTHREAD_H_

#include <pthread.h>

class tThread{
public:
	enum THREADSTATAS
	{
		IDLE,
		WORK,
		BUSY,
	};
public:
	tThread();
	virtual ~tThread();
	virtual void *run(void) = 0;
	virtual int start(void);
	virtual int cancel(void);
	pthread_t get_pid() const
	{
		return pid;
	}
protected:
	THREADSTATAS _thread_state;
private:
	pthread_t pid;
	static void * thread_entry(void* para);
};




#endif