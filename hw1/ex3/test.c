/* test.c
 * ------
 * Test POSIX threads' and custom barrier.
 *
 * Developed by George Z. Zachos
 */

// #define USE_PTHREAD_BAR

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>
#include <sys/time.h>
#ifndef USE_PTHREAD_BAR
	#include "gz_barrier.h"
#endif

#define NTHREADS	5
#define ITERATIONS	100

/* Global definitions */
typedef struct thrarg_s {
	int tid;
} thrarg_t;

typedef struct timeval timeval_t;

/* Function prototypes */
void *thrfunc(void *arg);

#ifdef USE_PTHREAD_BAR
pthread_barrier_t bar;
#else
barrier_t bar;
#endif

int main(void)
{
	int       i;
	pthread_t tid[NTHREADS];
	thrarg_t  arg[NTHREADS];
	timeval_t tv1, tv2;
	double    elapsed_time;

#ifdef USE_PTHREAD_BAR
	pthread_barrier_init(&bar, NULL, NTHREADS);
#else
	barrier_init(&bar, NTHREADS);
#endif

	/* Start timing */
	gettimeofday(&tv1, NULL);
	for (i = 0; i < NTHREADS; i++)
	{
		(arg+i)->tid = i;
		pthread_create(tid+i, NULL, thrfunc, ((void *)(arg+i)));
	}
	for (i = 0; i < NTHREADS; i++)
		pthread_join(tid[i], NULL);
	/* End timing */
	gettimeofday(&tv2, NULL);

	elapsed_time =  (tv2.tv_sec - tv1.tv_sec) +
			(tv2.tv_usec - tv1.tv_usec)*1.0E-6;

#ifdef USE_PTHREAD_BAR
	pthread_barrier_destroy(&bar);
#else
	barrier_destroy(&bar);
#endif

#ifdef USE_PTHREAD_BAR
	printf("\nPTHREADS' Barrier: nthr = %d\ttime: %lf sec.\n",
		NTHREADS, elapsed_time);
#else
	printf("\nGZachos' Barrier: nthr = %d\ttime: %lf sec.\n",
		NTHREADS, elapsed_time);

#endif

	return EXIT_SUCCESS;
}


void *thrfunc(void *arg)
{
	int i, j, a,
	    tid = (int) (((thrarg_t *)arg)->tid);

	for (i = 0; i < ITERATIONS; i++)
	{
		for (j = 0, a = 0; j < (INT_MAX >> 5); j += (tid+1))
			a += j / (2 + a);
#ifdef USE_PTHREAD_BAR
		pthread_barrier_wait(&bar);
#else
		barrier_wait(&bar);
#endif
	}
	return NULL;
}
