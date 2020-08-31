//===========================================================================//
//   Project: Projekat iz Operativnih sistema 1
//   File:    user.cpp
//   Date:    Jun 2019
//===========================================================================//
#include <iostream.h>
#include <stdlib.h>
#include <assert.h>

#include "bounded.h"
#include "intLock.h"
#include "keyevent.h"
#include "user.h"
#include "Event.h"
#include "semaphor.h"

//---------------------------------------------------------------------------//
//  Otkomentarisati ukoliko se testiraju signali
//---------------------------------------------------------------------------//
#define SIGNAL
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//  Ovo se menja u testu
//---------------------------------------------------------------------------//

Time TIME_SLICE = 2; //0 ili defaultTimeSlice;

int N = 3;            // 1 <= N <= 19

//---------------------------------------------------------------------------//

volatile int theEnd=0;

class Producer : public Thread {
public:
	Producer (BoundedBuffer* bb, char y, Time time_slice);
	virtual ~Producer() {waitToComplete(); }
    Thread* clone() const { return new Producer(myBuffer, x, time_slice_clone); }
protected:
	virtual void run ();
	char produce() {return x;}; // Produce an item

private:
	Time time_slice_clone;
	BoundedBuffer* myBuffer;
	char x;
	Semaphore sleep;
};



//---------------------------------------------------------------------------//
class Consumer : public Thread {
public:
	Consumer (BoundedBuffer* bb) : Thread(defaultStackSize, 0), myBuffer(bb), sleep(0) {}
	virtual ~Consumer() {waitToComplete(); }
	Thread* clone() const { return new Consumer(myBuffer); }
protected:
	virtual void run ();
	void consume(char p); // Consume an item

private:
	BoundedBuffer* myBuffer;
	Semaphore sleep;
};




//---------------------------------------------------------------------------//
// Korisnicki program mora obavezno da definise ovu f-ju
//---------------------------------------------------------------------------//
void tick(){
}

//---------------------------------------------------------------------------//


Producer::Producer (BoundedBuffer* bb, char y, Time time_slice) 
: Thread(defaultStackSize, time_slice),myBuffer(bb), x(y), sleep(0), time_slice_clone(time_slice) {}
	
void Producer::run () {
	while(!theEnd) {
		char d = produce();

		myBuffer->append(d);

		assert(1 != sleep.wait(10));
		//intLock
		//cout<<"RUN PRODUCERA"<<endl;
		//intUnlock
	}
	intLock
	cout<<"Izlazi iz run produsera"<<endl;
	intUnlock
}

//---------------------------------------------------------------------------//


void Consumer::consume(char p) {
	intLock
	cout<<p<<" ";
	intUnlock
} // Consume an item

void Consumer::run () {
		
	int i = 0;
	//cout<<"USAO U RUN CONSUMERA"<<endl;
	while(!theEnd) {
		char d = myBuffer->take();
		consume(d);

		if (i++ == 40) {
			//cout<<"OK"<<endl;

			assert(1 != sleep.wait(5));
			i = 0;

		}else {for(int j=0;j<200;j++);
		}
		//cout<<"RUN CONSUMERA"<<endl;

	}
 
	intLock
	cout<<endl<<"ESC pressed - empty the buffer!"<<endl;
	intUnlock
	
	while (myBuffer->fullCount()){
		char d = myBuffer->take();
		consume(d);
		dispatch();
	}
	
	
	intLock
	cout<<endl<<"Happy End"<<endl;
	intUnlock
}

#ifdef SIGNAL
Semaphore signalTest(0);
void signal1() {
	signalTest.signal();
}

#endif


int userMain (int argc, char* argv[])
{
	volatile int awd=0;
	//intLock
	//	cout<< "STIGLO OVDE 1";
	//intUnlock

	BoundedBuffer *buff;
	Consumer *con;
	



	intLock

	if(argc <2){
		cout<<"Invalid input!"<<endl;

		intUnlock
		return -1;
	}

	int buffSize = atoi(argv[1]);
	N = atoi(argv[2]);
	N = N>19 ? 19 : N;
	TIME_SLICE = atoi(argv[3]);

	if(buffSize<N) {
		cout<<"Number of Producers is larger than Buffer size!"<<endl;
		intUnlock
		return 1;
	}

	//dispatch();
	//cout<<running->id<<endl;

	//TRENUTNI RUNNING JE MAINPCB KOJI NE MOZE DA SE DOHVATI IZ LISTE!!!!!!

#ifdef SIGNAL
	Thread *current = Thread::getThreadById(Thread::getRunningId());
	//if(current){cout<<current->getId()<<endl;}
	current->registerHandler(1, signal1);
	current->blockSignal(1);

#endif



	buff = new BoundedBuffer(buffSize);
	Producer **pro = new Producer*[N];
	KeyboardEvent* kev;
	int i;
	


	con = new Consumer(buff);

	con->start();


	for (i=0; i<N; i++){
		pro[i] = new Producer(buff,'0'+i, TIME_SLICE);
#ifdef SIGNAL
		if (i==0)
		{

			pro[i]->registerHandler(5, signal1);

		}
#endif
		pro[i]->start();
	}
  
	kev = new KeyboardEvent(buff);
	intUnlock
	
	//cout<<"LINE 217"<<endl;
	kev->start();

#ifdef SIGNAL
	current->unblockSignal(1);
	Semaphore timer(0);

	timer.wait(10);

	if (N > 1) {
		pro[0]->signal(5);
		// Kill thread
		pro[0]->signal(0);

		//cout<<"I OVDE JE STIGAO"<<endl;

		//dispatch();
		signalTest.wait(0);
	}
#endif


	for (i=0; i<N; i++){
		//cout<<"SAD CE DELETE"<<endl;
		if(pro[i])
		delete pro[i];
	}

	cout<<"223 red"<<endl;
	delete [] pro;
	delete kev;
	cout<<"226 red"<<endl;

	delete con;
	cout<<"229 red"<<endl;

	delete buff;
	cout<<"232 red"<<endl;

	awd++;
	intLock
	cout<<"userMain finished!"<<endl;
	intUnlock
	return 0;
}
//---------------------------------------------------------------------------//
