#include "ivtent.h"
#include "pcb.h"

#include "kernev.h"
#include "event.h"

#include <dos.h>

IVTEntry* IVTable[256] = {0}; 


IVTEntry::IVTEntry(IVTNo ivtNo, pInterrupt newRout) 
{
	lock
	num = ivtNo;
	IVTable[num] = this;
#ifndef BCC_BLOCK_IGNORE
	oldRout = getvect(num);
	setvect(num, newRout);
#endif
	unlock
}

void IVTEntry::signal(int callFlag) 
{
	if (callFlag != 0 && oldRout != 0)
		(*oldRout)();
	if (myEvent != 0)
		myEvent->signal();
	dispatch();
}


IVTEntry::~IVTEntry() 
{
	lock
#ifndef BCC_BLOCK_IGNORE
	setvect(num, oldRout);
#endif
	if (oldRout != 0)
		(*oldRout)();
	unlock
}
