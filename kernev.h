#ifndef _KERNEV_H_
#define _KERNEV_H_

#include "event.h"

class PCB;

class KernelEv {
public:
	
	PCB* myPCB;

	IVTNo num;
	Event* myEvent;

	int wasSignal;
	
	KernelEv(Event* myEv, IVTNo ivtNo);
	void wait();
	void signal();
	~KernelEv();
	
};

#endif
