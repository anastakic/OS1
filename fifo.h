#ifndef _FIFO_H_
#define _FIFO_H_

typedef void (*SignalHandler)();
typedef unsigned SignalId;

class FIFO{
public:

	struct Elem 
	{
		SignalHandler hand;
		Elem* next;
		Elem(SignalHandler h)
		{
			hand = h;
			next = 0;
		}
		~Elem() { }
	};

	Elem *first, *last;

	FIFO()
	{
		first = 0;
		last = 0;
	}
	
	void put(SignalHandler hand);
	SignalHandler get();
	void swap(SignalHandler hand1, SignalHandler hand2);
	
	~FIFO();	

};

#endif
