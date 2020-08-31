#ifndef _KERNSEM_H_
#define _KERNSEM_H_

typedef unsigned int Time;

class List;
void update();

class KernelSem {
public:

	int val;
	List* list; 
	
	~KernelSem();
	KernelSem(int init);
	int wait(Time maxTimeToWait);
	int signal(int n);
};

#endif
