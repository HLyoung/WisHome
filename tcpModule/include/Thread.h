

#ifndef _THREAD_H_
#define _THREAD_H_

#include "macro.h"
#include "libLog.h"
#include <mutex>
#include <condition_variable>
#include <ratio>
#include <chrono>
#include <thread>
#include <unistd.h>


typedef std::chrono::duration<int> second_type;
typedef std::chrono::duration<int,std::milli> milliseconds_type;
typedef std::chrono::duration<int,std::ratio<60*60>> hours_type;

class CThread
{
protected:
	enum
	{
		LOCK_DefaultTimeout = 10,/*Seconds*/
		LOCK_Peek = 5, /* Millisecond*/
		LOCK_SuspendPeek = 20 ,/*Millisecond*/
		IDLE_TimeAtom = 50 ,/*Millisecond*/
		IDLE_Idle = 1,
		SUSECOND = 1000
	};

public:
	CThread();
	virtual ~CThread();
	std::mutex killMutex;
	std::mutex killedMutex;
	std::mutex suspendMutex;
	
	std::unique_lock<std::mutex> killLck;
	std::unique_lock<std::mutex> killedLck;
	std::unique_lock<std::mutex> suspendLck;
	
	std::condition_variable  killVariable;
	std::condition_variable  killedVariable;
	std::condition_variable  suspendVariable;
	
public:
	std::thread::id  GetID();
	bool IsKilled();
	bool IsSuspend();
	virtual bool Destroy(bool bWait = false);
	virtual bool Resume();
	virtual bool Suspend();
	virtual bool Create(bool bSuspend = false);

protected:
	virtual void OnCycle(){}
	virtual void OnDestroy();
	virtual void OnResume();
	virtual void OnSuspend();
	virtual void OnCreate();
protected:
	static void Idle();
	static void Sleep(UINT32 seconds, UINT32 milliseconds );


private:	
	static void *Proc(void *OwnerPtr);
	std::thread::id  m_ID;
	
};



#endif