#ifndef _LIST_H_
#define _LIST_H_

#include "thread.h"
class PCB;

class List {
public:
	
	struct Elem 
	{
		Elem* next;
		PCB* pcb;
		Time time;

		Elem(PCB* p, Time t=0) 
		{
			next = 0;
			pcb = p;
			time = t;
		}

	};
	
	Elem *first, *last;
	
	List()
	{
		first = 0;
		last = 0;
	}

	PCB* get();
	void take(PCB* pcb);
	void put(PCB* pcb);
	void putSorted(PCB* pcb, Time time);
	void takeSorted(PCB* pcb);
	Thread* get(ID id);
	~List();
};

#endif
