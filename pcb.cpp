#include <iostream.h>
#include <stdlib.h>

#include "pcb.h"
#include "idleth.h"
#include "schedule.h"
#include "kernsem.h"
//#include "list.h"
#include "signlist.h"
#include "fifo.h"

#include <dos.h>


void tick();

int PCB::blockedGlobally[16] = {SIGNAL_UNBLOCKED};

PCB* mainpcb;
IdleTh* idleTh;
PCB* idlepcb;
UserMainTh* userMainTh;
int PCB::userMainRetval;

ID nextId = 1;
pInterrupt oldTimer = 0;

PCB* running = 0;
List* allPcb = 0;
List* sortedPcb = 0;  //za sleep!!!!

int IDLE = 1, NEW = 2, READY = 3, BLOCKED = 4, RUNNING = 5, SLEEPY = 6, COMPLETED = 7;

Time counter = 0;


///////////////////////////////////////////////////////////////////
//////*************************************************************

int SIGNAL_BLOCKED = 1, SIGNAL_UNBLOCKED = 0;

//////*************************************************************
///////////////////////////////////////////////////////////////////


int main(int argc, char* argv[]) 
{
	PCB::init(argc, argv);
	userMainTh->waitToComplete();
	PCB::restore();
	return PCB::userMainRetval;

}

void PCB::init(int argc, char** argv) {
	lock
	nextId = 1;
	allPcb = new List();
	sortedPcb = new List();
	mainpcb = new PCB(-1);

	unsigned int ttsp, ttss;
	asm{
		mov ttsp, sp
		mov ttss, ss
	}
	mainpcb->rsp = ttsp;
	mainpcb->rss = ttss;

	running = mainpcb;
	idlepcb = 0;
	idleTh = new IdleTh();
	idleTh->start();

	userMainTh = new UserMainTh(argc, argv);
	userMainTh->start();

	#ifndef BCC_BLOCK_IGNORE
	oldTimer = getvect(8);
	setvect(8, timer);
	#endif

	unlock
}

void PCB::restore() {
	delete userMainTh;
	#ifndef BCC_BLOCK_IGNORE
	setvect(8, oldTimer);
	#endif
	idleTh->stop();
	delete idleTh;
	delete mainpcb;
	delete allPcb;
	delete sortedPcb;

	unlock
}




void interrupt dispatch_intr() {
// interrupt rutina - auto push:
// psw, cs, ip - hardverski, asm naredba int, kao u ort2 int
// ax, bx, cx, dx, es, ds, si, di, bp - softverski, 9 x asm push generisano

// 1. nacin
	static unsigned int tss, tsp, tbp;
	asm {
	#ifndef BCC_BLOCK_IGNORE
		mov tbp, bp
		mov tss, ss
		mov tsp, sp
	}
	running->rbp = tbp;
	running->rss = tss;
	running->rsp = tsp;

	if (running->status == RUNNING && running != idlepcb) {	//trenutnu stavljam da bude spremna
		running->status = READY;		//i stavljam je u scheduler
		Scheduler::put(running);
	}
	running = Scheduler::get();			//uzmem novu iz schedulera
	if (running != 0) {					//ako uspesno uzme neku nit
		running->status = RUNNING;      //postavim da bude running
	}
	else {
		running = idlepcb;			//ako nema nijedne u scheduleru onda ostavim idle !!!!
	}

	tbp = running->rbp;
	tss = running->rss;
	tsp = running->rsp;
	asm {
		mov bp, tbp
		mov ss, tss
		mov sp, tsp
	}
	#endif
	counter = running->timeSlice;
	lock
	if(running != idlepcb)
	running->processSignals();
	unlock

}

PCB::PCB(int n){
	timeSlice = defaultTimeSlice;
	stackSize = defaultStackSize >>1;
	myThread = 0;
	id = n;
	/*
	if (stackSize < 65536UL)
			stackSize = 65536UL;
	*/
	parent = 0;
	listSignals = new SignalList();
	for (int i = 0; i < 16; i++) {
		blocked[i] = SIGNAL_UNBLOCKED;
		myHandlers[i] = new FIFO();
	}
	myHandlers[0]->put(signal0Handler);
}

PCB::PCB(StackSize stSize, Time tmSlice, Thread* thread) {
	if (idlepcb == 0)
		idlepcb = this;
	timeSlice = tmSlice;
	myThread = thread;
	id = nextId++;

	stackSize = stSize >> 1;
	//stackSize = stSize;
	//if (stackSize > 65536UL)
	//	stackSize = 65536UL;



	///////******
	if (running == 0){
		parent = 0;
		listSignals = new SignalList();
		for (int i = 0; i < 16; i++) {
			blocked[i] = SIGNAL_UNBLOCKED;
			//PCB::blockedGlobally[i] = SIGNAL_UNBLOCKED;
			myHandlers[i] = new FIFO();
		}
		myHandlers[0]->put(signal0Handler);
	
	}
	
	else {
		
		parent = running;
		
		listSignals = new SignalList();
		SignalList::Elem* tek = parent->listSignals->first;
		while(tek != 0){
			listSignals->add(tek->signal);
			tek = tek->next;
		}
		
		for (int i = 0; i < 16; i++) {
			blocked[i] = parent->blocked[i];
			myHandlers[i] = parent->myHandlers[i];
		}
		
	}
	///////******


	status = NEW;
	waitingList = new List();
	stack = 0;
	//if (id == 1) idle = this;
	allPcb->put(this);
	mySem = 0;
	myEv = 0;
}


void PCB::wrapper(){//Thread* thread) {
	
	running->myThread->run();
	lock
	
	///////************************************************

	if(running->parent){
		running->parent->myThread->signal(1);
	}

	///////************************************************


	running->status = COMPLETED;


	///////************************************************

	running->myThread->signal(2);

	///////************************************************


	while (running->waitingList->first) {
		PCB* pcb = running->waitingList->get();
		if(pcb->status != COMPLETED){
			pcb->status = READY;
			Scheduler::put(pcb);
		}
	}
	
	
	///////************************************************
	if (running->listSignals != 0) {
		delete running->listSignals;
		running->listSignals = 0;
	}
	///////************************************************

	
	unlock
	dispatch();
}

void PCB::createStack() {
#ifndef BCC_BLOCK_IGNORE
	stack = new unsigned[stackSize];

	stack[stackSize - 1] = 0x200;
	stack[stackSize - 2] = FP_SEG(PCB::wrapper);
	stack[stackSize - 3] = FP_OFF(PCB::wrapper);

	rss = FP_SEG(&stack[stackSize - 12]);
	rsp = FP_OFF(&stack[stackSize - 12]);
	rbp = rsp;


#endif
}



PCB::~PCB() {
	delete waitingList;
	delete[] stack;
	allPcb->take(this);
}



void interrupt timer(...) {

	(*oldTimer)();
	update();

	tick();

	if (running->timeSlice) {
		if (counter)
			counter--;
		if (!counter){
			dispatch();
		}
	}
}


void update() {

	if (sortedPcb->first && sortedPcb->first->time)
		--sortedPcb->first->time;

	while (1) {
		// ako ima neko za budjenje
		if (sortedPcb->first && !sortedPcb->first->time) {
			
			List::Elem* el = sortedPcb->first;
			sortedPcb->first = sortedPcb->first->next;
			if (sortedPcb->first == 0)
				sortedPcb->last = 0;
			PCB* pcb = el->pcb;
			delete el;

			// probudi nit
			pcb->status = READY;
			pcb->mySem->list->take(pcb);
			++pcb->mySem->val;
			pcb->mySem = 0;
			pcb->retval = 0; //nit probudjena zbog isteka vremena, vraca se 0
			Scheduler::put(pcb);
			/////

		} else
			return;
	}
}




///////////////////////////////////////////////////////////////////
//////														///////
//////				ZA DRUGI DEO PROJEKTA					///////
//////														///////
///////////////////////////////////////////////////////////////////



void PCB::processSignals() {
	listSignals->processSignals(this);
}

void PCB::signal0Handler() {

	if (running->parent != 0)
		running->parent->myThread->signal(1);

	running->status = COMPLETED;
	running->myThread->signal(2);

	while (running->waitingList->first) {
		PCB* pcb = running->waitingList->get();
		if(pcb->status != COMPLETED){
			pcb->status = READY;
			Scheduler::put(pcb);
		}
	}

	if (running->stack != 0) {
		delete[] running->stack;
		running->stack = 0;
	}
	
	if (running->listSignals != 0) {
		delete running->listSignals;
		running->listSignals = 0;
	}
	
	unlock
	
	dispatch();
}


ID PCB::getParentId() {
	return parent->myThread->getId();
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
