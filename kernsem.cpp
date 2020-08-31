#include "kernsem.h"
#include "list.h"
#include "pcb.h"
#include "schedule.h"
#include <stdlib.h>
#include <iostream.h>

KernelSem::~KernelSem() {
	while (list->first != 0) 
	{
		PCB* p = list->first->pcb;
		signal(0);
		p->retval = 1;
	}
}

KernelSem::KernelSem(int init) {
	list = new List();
	val = init;
	if (val < 0)
		val = 0;  
}

int KernelSem::wait(Time maxTimeToWait) 
{
	if (--val < 0) 
	{
		running->mySem = this;
		list->put(running);
		running->status = BLOCKED;
		if (maxTimeToWait > 0) sortedPcb->putSorted(running, maxTimeToWait);
		dispatch();

		return running->retval;
	}
	else return 1;
}

int KernelSem::signal(int n) 
{
	if (n<0) return n;
	
	if (n==0){
		if (val++ < 0) {
			PCB* p = list->get();
			if (p) {
				Scheduler::put(p);
				sortedPcb->takeSorted(p);
				p->status = READY;
				p->mySem = 0;
				p->retval = 1;
			}
		}
		return n;
	}
	int ret = 0; 
	while(n!=0 || list->first != 0){
		if (val++ < 0) 
		{ 
			PCB* p = list->get();
			if (p) {
				Scheduler::put(p);
				sortedPcb->takeSorted(p);
				p->status = READY;
				p->mySem = 0;
				p->retval = 1;
				ret++;
			}
		n--;
		}
	}
	return ret;
	
}

