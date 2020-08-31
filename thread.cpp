#include <iostream.h>
#include <stdlib.h>
#include "thread.h"
#include "pcb.h"
#include "fifo.h"
#include "schedule.h"
#include "signlist.h"


void dispatch(){
	lock
	dispatch_intr();
	unlock
}

Thread::~Thread(){
	lock
	waitToComplete();
	delete myPCB;
	unlock
}

Thread::Thread(StackSize stackSize, Time timeSlice){
	lock
	myPCB = new PCB(stackSize, timeSlice, this);
	unlock
}

void Thread::start(){
	lock
	if (myPCB->status != NEW) 
	{
		unlock
		return;
	}
	
	myPCB->createStack();
	
	if (myPCB != idlepcb) 
	{ 
		myPCB->status = READY;
		Scheduler::put(myPCB);
	}
	else 
	{
		myPCB->status = IDLE;
	}
	unlock
}

void Thread::waitToComplete(){
	lock

	if (myPCB != running &&  myPCB != idlepcb && myPCB->status != COMPLETED) 
	{
		myPCB->waitingList->put(running);
		running->status = BLOCKED;
		lock
		cout<<"Wait to complete: "<<running->id<<endl;
		unlock
		dispatch();
	}

	unlock
}

/*
void Thread::sleep(Time timeToSleep) {
	lock
	if (running->status == IDLE) 
	{
			unlock
			return;
	}
	
	//if (running->status == COMPLETED) 
	//{
	//	unlock
	//	return;
	//}

	running->status = SLEEPY;
	
	if (timeToSleep > 0) {
		// sortirana lista
		sortedPcb->putSorted(running, timeToSleep);
	}
	dispatch(); 

	unlock
}

*/

ID Thread::getId(){
	//lock
	return myPCB->id;
	//unlock
}

ID Thread::getRunningId() {
	//lock
	return running->id;
	//unlock
}

Thread* Thread::getThreadById(ID id) {
	lock
	Thread* t = 0;
	if(id != -1){ t = allPcb->get(id);}
	unlock
	return t;
}



///////////////////////////////////////////////////////////////////
//////														///////
//////				ZA DRUGI DEO PROJEKTA					///////
//////														///////
///////////////////////////////////////////////////////////////////

ID Thread::getParentId() { return myPCB->parent->myThread->getId(); }


void Thread::signal(SignalId signal)
{
	lock
	if (signal == 2 && myPCB->status == COMPLETED )
	{	
		unlock
		return;
	}
	if(signal<16 && myPCB != running) myPCB->listSignals->add(signal);
	unlock
}


void Thread::registerHandler(SignalId signal, SignalHandler handler)
{
	if (signal < 16 && signal >= 0)
	{
		myPCB->myHandlers[signal]->put(handler);
	}
}

void Thread::unregisterAllHandlers(SignalId id)
{
	FIFO::Elem* pom = myPCB->myHandlers[id]->first;
	delete pom;
	if(id == 0) registerHandler(0, PCB::signal0Handler);
}

void Thread::swap(SignalId id, SignalHandler hand1, SignalHandler hand2)
{
	myPCB->myHandlers[id]->swap(hand1, hand2);
}


////// Signal blokiranje

void Thread::blockSignal(SignalId signal)
{
	if (signal < 16 && signal >= 0)
	{
		myPCB->blocked[signal] = SIGNAL_BLOCKED;
	}
}

void Thread::blockSignalGlobally(SignalId signal)
{
	if (signal < 16 && signal >= 0)
	{
		PCB::blockedGlobally[signal] = SIGNAL_BLOCKED;
	}
}

void Thread::unblockSignal(SignalId signal)
{
	if (signal < 16 && signal >= 0)
	{
		myPCB->blocked[signal] = SIGNAL_UNBLOCKED;
	}
}

void Thread::unblockSignalGlobally(SignalId signal)
{
	if (signal < 16 && signal >= 0)
	{
		PCB::blockedGlobally[signal] = SIGNAL_UNBLOCKED;
	}
}


