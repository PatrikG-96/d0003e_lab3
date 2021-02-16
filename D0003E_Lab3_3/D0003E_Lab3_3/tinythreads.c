#include <setjmp.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "tinythreads.h"
#include "functions.h"


#define CLOCK			8000000
#define PRESCALER		1024
#define ICYCLE			20
#define NULL            0
#define DISABLE()       cli()
#define ENABLE()        sei()
#define STACKSIZE       80
#define NTHREADS        4
#define SETSTACK(buf,a) *((unsigned int *)(buf)+8) = (unsigned int)(a) + STACKSIZE - 4; \
                        *((unsigned int *)(buf)+9) = (unsigned int)(a) + STACKSIZE - 4

struct thread_block {
    void (*function)(int);   // code to run
    int arg;                 // argument to the above
    thread next;             // for use in linked lists
    jmp_buf context;         // machine state
    char stack[STACKSIZE];   // execution stack space
};

struct thread_block threads[NTHREADS];

struct thread_block initp;

thread freeQ   = threads;
thread readyQ  = NULL;
thread current = &initp;

int initialized = 0;


ISR(PCINT1_vect) {

	if (PINB >> 7 == 0) {
		spawn(button, 0);
	}
}

ISR(TIMER1_COMPA_vect) {
	spawn(blink, 0);
}
     

static void initialize(void) {
    int i;
    for (i=0; i<NTHREADS-1; i++)
        threads[i].next = &threads[i+1];
    threads[NTHREADS-1].next = NULL;

    initialized = 1;
}


static void enqueue(thread p, thread *queue) {
	p->next = *queue;
	*queue = p;
}

static thread dequeue(thread *queue) {
    thread p = *queue;
    if (*queue) {
        *queue = (*queue)->next;
    } else {
        // Empty queue, kernel panic!!!
        while (1) ;  // not much else to do...
    }
    return p;
}

static void dispatch(thread next) {
    if (setjmp(current->context) == 0) {
        current = next;
        longjmp(next->context,1);
    }
}

void spawn(void (* function)(int), int arg) {
    thread newp;

    DISABLE();
    if (!initialized) initialize();

    newp = dequeue(&freeQ);
    newp->function = function;
    newp->arg = arg;
    newp->next = NULL;
    if (setjmp(newp->context) == 1) {
        ENABLE();
        current->function(current->arg);
        DISABLE();
        enqueue(current, &freeQ);
        dispatch(dequeue(&readyQ));
    }
    SETSTACK(&newp->context, &newp->stack);

    enqueue(newp, &readyQ);
    ENABLE();
	yield();
}

void yield(void) {
	DISABLE();
	thread t = dequeue(&readyQ);
	enqueue(current, &readyQ);
	dispatch(t);
	ENABLE();
}

void lock(mutex *m) {
	DISABLE();
	if (!m->locked) {
		m->locked = 1;					//Lock the mutex block
	}
	else {
		enqueue(current, &(m->waitQ));  //Block locked - enqueue in waitQ
		dispatch(dequeue(&readyQ));		//Dispatch the next thread in readyQ
	}
	ENABLE();
}

void unlock(mutex *m) {
	DISABLE();
	if(m->waitQ) {
		enqueue(current, &readyQ);
		dispatch(dequeue(&(m->waitQ)));
	}
	else
	{
		m->locked = 0;
	}
	ENABLE();
}
