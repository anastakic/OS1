#ifndef _SIGNLIST_H_
#define _SIGNLIST_H_

typedef unsigned SignalId;
//#include "thread.h"

class PCB;
class SignalList{
public:
	struct Elem 
	{
		SignalId signal;
		Elem* next;
		Elem(SignalId i)
		{
			signal = i;
			next = 0;
		}
		~Elem() {}
	};

	Elem *first, *last;

	SignalList()
	{
		first = 0;
		last = 0;
	}
	
	void processSignals(PCB* pcb);
	void add(SignalId el);
	
	~SignalList();

};

#endif
