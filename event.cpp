#include "event.h"
#include "kernev.h"
#include "pcb.h"



Event::Event(IVTNo ivtNo) 
{
	lock
	myImpl = new KernelEv(this, ivtNo);
	unlock
}

Event::~Event() 
{
	lock
	signal();
	unlock
}

void Event::wait() 
{
	lock
	myImpl->wait();
	unlock
}

void Event::signal() 
{
	lock
	myImpl->signal();
	unlock
}

