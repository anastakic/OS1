#ifndef _event_h_
#define _event_h_

#include "ivtent.h"

typedef unsigned char IVTNo;
class KernelEv;

class Event {
public:
	Event (IVTNo ivtNo);
	~Event ();

	void wait ();
	
protected:
	friend class KernelEv;
	friend class IVTEntry;
	
	void signal(); // can call KernelEv

private:
	KernelEv* myImpl;
};



#define PREPAREENTRY(num,callFlag) \
void interrupt rout##num(...); \
IVTEntry entry##num(num, &rout##num); \
void interrupt rout##num(...) { \
	entry##num.signal(callFlag); \
}


#endif



