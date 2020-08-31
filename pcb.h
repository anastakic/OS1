#ifndef _PCB_H_
#define _PCB_H_

#include "thread.h"
#include "list.h"

#define lock asm pushf; asm cli;
#define unlock asm popf;

typedef void interrupt (*pInterrupt)(...); //pokazivac na prekidnu rutinu

class KernelSem;
class IdleTh;
class UserMainTh;
class KernelEv;
class SignalList;
class FIFO;
class List;

class PCB {
public:
	
	Time timeSlice;
	StackSize stackSize;
	unsigned int rss, rsp, rbp;
	unsigned* stack;

	int status;
	
	
	ID id;
	int retval;

	static int userMainRetval;

	Thread* myThread;
	List* waitingList;
	

	KernelSem* mySem;
	KernelEv* myEv;
	
	
	PCB(StackSize stSize, Time tmSlice, Thread* thread = 0);
	PCB(int n);

	static void wrapper();//Thread* thread);
	void createStack();

	~PCB();
	

	static void init(int argc, char** argv);
	static void restore();
	
	
//////////////////////////////////////////////////////////////
/////*********************************************************
	
	void processSignals();
	ID getParentId();
	
	SignalList* listSignals;
	PCB* parent;
	
	FIFO* myHandlers[16];


	int blocked[16];
	static int blockedGlobally[16];

	static void signal0Handler();

	
/////**********************************************************
///////////////////////////////////////////////////////////////
};



//////////////////////////////////////////////////////////////
/////*********************************************************

extern int SIGNAL_BLOCKED, SIGNAL_UNBLOCKED;


/////**********************************************************
///////////////////////////////////////////////////////////////

extern PCB* mainpcb;
extern IdleTh* idleTh;
extern PCB* idlepcb;
extern UserMainTh* userMainTh;


extern ID nextId;
extern pInterrupt oldTimer;

extern PCB* running;

extern List* allPcb;
extern List* sortedPcb;  //za sleep!!!!

extern int IDLE, NEW, READY, BLOCKED, RUNNING, SLEEPY, COMPLETED;

void interrupt dispatch_intr();
void interrupt timer(...);
void update();


int userMain(int argc, char *args[]);
int main(int argc, char *args[]);



#endif
