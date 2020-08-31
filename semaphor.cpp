#include "semaphor.h"
#include "pcb.h"
#include "kernsem.h"

Semaphore::Semaphore(int init) 
{
	lock
	myImpl = new KernelSem(init);
	unlock
}

Semaphore::~Semaphore() 
{
	lock
	while (myImpl->val)
		signal();
	//signal(myImpl->val);
	delete myImpl;
	unlock
}


int Semaphore::wait(Time maxTimeToWait) 
{
	lock
	int ret = myImpl->wait(maxTimeToWait);
	unlock
	return ret;
}

int Semaphore::signal(int n) 
{
	lock
	int ret = myImpl->signal(n);
	unlock
	return ret;
}

int Semaphore::val() const 
{
	return myImpl->val;
}

