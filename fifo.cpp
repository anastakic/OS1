#include "fifo.h"



SignalHandler FIFO::get()
{
	if (first != 0)
	{
		SignalHandler h = first->hand;
		Elem* pom = first;
		first = first->next;
		delete pom;
		return h;
	}
	else return 0;
}


void FIFO::put(SignalHandler h) 
{
	Elem* elem = new Elem(h);
	if (first == 0)	first = elem;
	else last->next = elem;
	last = elem;
}



void FIFO::swap(SignalHandler hand1, SignalHandler hand2)
{
	Elem* tek1 = first;
	Elem* tek2 = first;

	while (tek1 != 0) 
	{
		if (tek1->hand == hand1) break;
		tek1 = tek1->next;
	}
	
	while (tek2 != 0) 
	{
		if (tek2->hand == hand2) break;
		tek2 = tek2->next;
	}
	
	if (tek1 != 0 && tek2 != 0)
	{
		tek1->hand = hand2;
		tek2->hand = hand1;
	}
	
}


FIFO::~FIFO() 
{
	last = 0;
	while (first != 0) 
	{
		Elem* old = first;
		first = first->next;
		delete old;
	}
}
