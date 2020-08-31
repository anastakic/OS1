#ifndef _thread_h_
#define _thread_h_

typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;
typedef unsigned int Time; // time, x 55ms
const Time defaultTimeSlice = 2; // default = 2*55ms
typedef int ID;


//**********
typedef unsigned SignalId;
typedef void (*SignalHandler)();
//**********


class PCB; // Kernel's implementation of a user's thread

class Thread {
public:
	void start();
	void waitToComplete();
	virtual ~Thread();
	
	//static void sleep (Time timeToSleep);
	
	ID getId();
	static ID getRunningId();
	static Thread* getThreadById(ID id);
	
	///////////////////////////////////////////////////////////////////
	//////														///////
	//////				ZA DRUGI DEO PROJEKTA					///////
	//////														///////
	///////////////////////////////////////////////////////////////////
	
	ID getParentId(); //////??????
	
	
	void signal(SignalId signal);
	void registerHandler(SignalId signal, SignalHandler handler);
	void unregisterAllHandlers(SignalId id);
	void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);
	
	void blockSignal(SignalId signal);
	static void blockSignalGlobally(SignalId signal);
	void unblockSignal(SignalId signal);
	static void unblockSignalGlobally(SignalId signal);
	
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	
	
protected:
	friend class PCB;
	Thread (StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
	virtual void run() {}

public:
	PCB* myPCB;
};

void dispatch();

#endif
