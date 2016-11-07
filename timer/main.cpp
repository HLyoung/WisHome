

#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>


#include "TimerThread.h"

using namespace std;

int handle_timeout(void *)
{
	long int  time = get_systime_clock();
	printf("time out,%ld \n",time);
	return 0;
}

int main()
{
	Timer _timer;
	_timer.open(1,handle_timeout,NULL);
	TIMERMANAGER->Register(_timer);
	TIMERMANAGER->start();
	getchar();
	return 0;
}