#ifndef CTHREAD_H_
#define CTHREAD_H_ 


#include <string.h>
#include <thread>

class CThread{
	
private:
	std::thread m_Thread;
	std::thread::id m_ThreadID;
	bool m_Detach;
	char *m_ThreadName;

protected:
	static void ThreadFunction(void*);
	
public:
	CThread();
	CThread(bool detach);
	virtual ~CThread(){};
	virtual void Run(void) = 0;

	void SetThreadName(char *thrname){strcpy(m_ThreadName,thrname);}
	char *GetThreadName(void){return m_ThreadName;}
	std::thread::id  GetThreadID(void){return m_ThreadID;}

	void Detach(void);
	void Join(void);
	std::thread::id Self(void);
};

#endif //CTHREAD_H_
