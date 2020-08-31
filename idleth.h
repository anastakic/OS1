#ifndef _IDLETH_H_
#define _IDLETH_H_

#include "thread.h"
#include "pcb.h"

class IdleTh : public Thread {
public:
	int work;
	IdleTh() { work =1; }
	void stop() { work=0; }
	void run() 
	{
		while (work)
			dispatch();
	}
};


class UserMainTh : public Thread {
public:
	int retval;
		int argc;
		char** argv;

	UserMainTh(int argc, char** argv)
	{
		this->argc = argc;
		this->argv = argv;
	}

	~UserMainTh() { waitToComplete(); } 

	void run() 
	{
		PCB::userMainRetval = userMain(argc, argv);
	}

};

#endif
