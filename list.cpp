#include "list.h"
#include "pcb.h"


PCB* List::get() 
{
	if (first != 0){
		PCB* pcb = first->pcb;
		Elem* pom = first;
		first = first->next;
		delete pom;
		return pcb;
	}
	else return 0;
}



void List::take(PCB* pcb) 
{
	Elem* prev = 0;
	Elem* tek = first;

	while (tek != 0) 
	{
		if (tek->pcb == pcb) 
		{
			if (prev != 0) prev->next = tek->next;
			else first = tek->next;
			if (tek == last) last = prev;

			break;
		}
		prev = tek;
		tek = tek->next;
	}
	delete tek;
}



void List::put(PCB* pcb) 
{
	Elem* el = new Elem(pcb);
	if (first == 0) first = el;
	else last->next = el;
	last = el;
}



void List::putSorted(PCB* pcb, Time time) 
{
	Elem* el = new Elem(pcb, time);
	
	Elem* prev = 0;
	Elem* tek = first;

	while (tek != 0) {
		if (el->time > tek->time) {
			prev = tek;
			tek = tek->next;
			el->time -= prev->time;
		}
		else break;
	}
	
	if (prev != 0) prev->next = el;
	else first = el;
	
	if (tek != 0) {
		el->next = tek;
		tek->time -= el->time;
	}
	else last = el;
}



void List::takeSorted(PCB* pcb) 
{
	Elem* tek = first;
	Elem* prev = 0;
	while (tek != 0) {
		if (pcb == tek->pcb) {
			if (prev)
				prev->next = tek->next;
			else
				first = tek->next;
			if (tek->next)
				tek->next->time += tek->time;
			if (tek == last)
				last = prev;
			delete tek;
			return;
		}
		
		prev = tek;
		tek = tek->next;
	}
}


Thread* List::get(ID id) 
{
	if (first == 0)
		return 0;
	
	Elem* tek = first;
	while (tek != 0) {
		if (id == tek->pcb->id) {
			return tek->pcb->myThread;
		}
		
		tek = tek->next;
	}
	
	return 0;
}

List::~List()
{
	last = 0;
	while (first != 0) {
		Elem* old = first;
		first = first->next;
		delete old;
	}
}
