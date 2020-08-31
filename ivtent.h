#ifndef _IVTENT_H_
#define _IVTENT_H_

#include "pcb.h"

typedef unsigned char IVTNo;
class KernelEv;
class Event;

class IVTEntry {
public:
	
	IVTNo num;
	pInterrupt oldRout;							//pokazivac na staru prekidnu rutinu
	Event* myEvent;								//pok na dogadjaj
	
	IVTEntry(IVTNo ivtNo, pInterrupt newRout);	//prosledi se broj ulaza i pokazivac na novu prekidnu rutinu
	void signal(int callFlag);					//signal iz makroa
												//callFlag iz makroa: da li pozivati staru prekidnu rutinu oldRout
	~IVTEntry();
};

extern IVTEntry* IVTable[];

#endif
