#include "kernev.h"
#include "schedule.h"
#include "pcb.h"
#include "ivtent.h"


KernelEv::KernelEv(Event* myEv, IVTNo ivtNo) 
{
	myEvent = myEv;
	num = ivtNo;
	wasSignal = 1;
	myPCB = running;
	
	if (IVTable[num] != 0) 
	{
		if (IVTable[num]->myEvent == 0) IVTable[num]->myEvent = myEvent;
	}
}

void KernelEv::wait() 
{
	if (myPCB == running) 
	{
		if (wasSignal) wasSignal = 0;
		else if (myPCB->myEv == 0) 
		{
			myPCB->status = BLOCKED;
			myPCB->myEv = this;
			dispatch();
		}
	}
}

void KernelEv::signal() 
{
	if (myPCB->myEv == this) 
	{ 
		myPCB->status = READY;
		Scheduler::put(myPCB);
		myPCB->myEv = 0;
	}
	else wasSignal = 1;
}


KernelEv::~KernelEv() {
	// odjavi se sa ivtentry
	if (IVTable[num] != 0) 
	{
		if (IVTable[num]->myEvent == myEvent)IVTable[num]->myEvent = 0;
	}
}
