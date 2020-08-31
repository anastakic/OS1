#include "signlist.h"
#include "fifo.h"
#include "pcb.h"
#include <iostream.h>
#include <STDLIB.H>

void SignalList::processSignals(PCB* pcb) {
	Elem *tek = first, *prev = 0;

	while (tek != 0) 
	{
		if (pcb->blocked[tek->signal] == SIGNAL_UNBLOCKED && pcb->blockedGlobally[tek->signal] == SIGNAL_UNBLOCKED) 
		{
			if(pcb->myHandlers[tek->signal])
			{
				while (pcb->myHandlers[tek->signal]->first != 0) 
				{
					(pcb->myHandlers[tek->signal]->get())();
					if (tek->signal == 0) return;
				}
			}
		}
		//else {
			prev = tek;
			tek = tek->next;
		//}
	}
}


void SignalList::add(SignalId el) {
	Elem* elem = new Elem(el);
	if (first == 0)
		first = elem;
	else
		last->next = elem;
	last = elem;
}



SignalList::~SignalList() {
	while (first != 0) {
		Elem * old = first;
		first = first->next;
		old->signal = 0;
		delete old;
	}
}


