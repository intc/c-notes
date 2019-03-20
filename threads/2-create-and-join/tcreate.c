#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>             /* for sleep */
#include <stdlib.h>             /* for malloc */
#include <inttypes.h>           /* uintptr_t */

#define NUM_THREADS 5

int *s_thread_worker(void **);

int main(void) {

	pthread_t thread_id[NUM_THREADS];
	int *stime_arr[NUM_THREADS];

	for ( int i = 0; i < NUM_THREADS; i++) {
		stime_arr[i] = malloc(sizeof(int));
		*stime_arr[i] = i+1;
		printf("*stime 0x%.18lX (%i sec.)\n", (uintptr_t) stime_arr[i], *stime_arr[i]);

		if ( pthread_create( &thread_id[i], NULL,
				(void *) s_thread_worker,
				(void **) &stime_arr[i] ) != 0 ) {
			perror("ERROR: Cerating thread");
			assert(0);
		}
	}
			/* Wait for the workers to exit */
	for ( int i = 0; i < NUM_THREADS; i++) {
		int *t_exit_val = NULL;
		if ( pthread_join(thread_id[i], (void **) &t_exit_val) == 0 ) {
			printf("thread 0x%.18lX exitting\n", (uintptr_t) thread_id[i]);
			printf("exit v 0x%.18X\n", *t_exit_val);
			printf("*stime 0x%.18lX (i:%i)\n", (uintptr_t) stime_arr[i], i);
			free(t_exit_val);
			t_exit_val = NULL;
		} else {
			perror("ERROR: Joining thread");
			assert(0);
		}
	}
	
	printf("\nmain(): All %d threads have terminated\n", NUM_THREADS);
	
	return 0;
}

int *s_thread_worker(void **int_arg) {
	int **pp = (int **)int_arg;
	int sleeptime = **pp;
	int * exit_val = NULL;

	exit_val = malloc(sizeof(int));

	printf("thread 0x%.18lX sleeping %d seconds ...\n",
			(uintptr_t) pthread_self(), sleeptime);
	
	sleep(sleeptime);
			/* Copy last 32 bits from the address of this thread
			 * so we have a "meaningful" value for pthread_exit */
	*exit_val = (int) (pthread_self());
	
	printf("\nthread 0x%.18lX awakening\n", (uintptr_t) pthread_self());
	printf("*stime 0x%.18lX %i\n", (uintptr_t) *pp, sleeptime);
			/* free & reset stime which was allocated @main() */
	free(*pp);
	*pp=NULL;
	pthread_exit((void *) exit_val);
	return NULL;
}

/*   Links
 *   + http://www.cs.rpi.edu/academics/courses/fall04/os/c6/index.html
 *
 *   Notes
 *   This is largely based on the link above. Added freeing up the worker argument
 *   allocated in the main function (r20).
 *
 *   Also added some printf's along the way to see that the pointers are correctly
 *   dealed with.
 *
 *   (C) Antti Antinoja, 2019
 */
