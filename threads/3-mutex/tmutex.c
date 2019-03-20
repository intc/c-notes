#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>             /* for sleep */
#include <stdlib.h>             /* for malloc */
#include <inttypes.h>           /* uintptr_t */

#define NUM_THREADS 5

typedef struct s_arr {
	int id;
	pthread_mutex_t *lock;
	int *shared;
} s_arr;

int *s_thread_worker(void **);

int main(void) {

	pthread_t thread_id[NUM_THREADS];
	s_arr *thread_arr[NUM_THREADS];
			/* Create mutex */
	pthread_mutex_t lock; pthread_mutex_init(&lock, NULL);

	int shared = NUM_THREADS;

	for ( int i = 0; i < NUM_THREADS; i++) {
		thread_arr[i] = malloc(sizeof(s_arr));
		thread_arr[i]->id = i;
		thread_arr[i]->lock = &lock;
		thread_arr[i]->shared = &shared;

		if ( pthread_create( &thread_id[i], NULL,
				(void *) s_thread_worker,
				(void **) &thread_arr[i] ) != 0 ) {
			perror("ERROR: Cerating thread");
			assert(0);
		}
	}
			/* Wait for the workers to exit */
	for ( int i = 0; i < NUM_THREADS; i++) {
		int *t_exit_var = NULL;
		if ( pthread_join(thread_id[i], (void **) &t_exit_var) == 0 ) {
			printf("thread 0x%.18lX exitting\n", (uintptr_t) thread_id[i]);
			printf("exit v 0x%.18X\n", *t_exit_var);
			printf("*thread_arr 0x%.18lX (i:%i)\n", (uintptr_t) thread_arr[i], i);
			free(t_exit_var);
			t_exit_var = NULL;
		} else {
			perror("ERROR: Joining thread");
			assert(0);
		}
	}
			/* Destroy the mutex */
	pthread_mutex_destroy(&lock);

	printf("\nmain(): All %d threads have terminated\n", NUM_THREADS);
	printf("Value of shared: %i\n", shared);
	
	return 0;
}

int *s_thread_worker(void **arg) {
	s_arr **pp = (s_arr **)arg;
	s_arr *thread_arr = *pp;
	int * exit_var = NULL;

	exit_var = malloc(sizeof(int));

	printf("thread 0x%.18lX id: %i shared: %i\n",
			(uintptr_t) pthread_self(), thread_arr->id, *thread_arr->shared);
#ifdef _NO_RACE
			/* Activate Mutual Exclution */
	if ( ( pthread_mutex_lock(thread_arr->lock) ) != 0 ) {
		perror("Failed to get lock");
		pthread_exit((void *) exit_var);
	}
#endif
	int temp = *thread_arr->shared;
	sleep(0.5);
	*thread_arr->shared = temp - 1;
#ifdef _NO_RACE
			/* Deactivate mutual Exclution */
	if ( ( pthread_mutex_unlock(thread_arr->lock) ) != 0 ) {
		perror("Failed to unlock");
		pthread_exit((void *) exit_var);
	}
#endif
	
	sleep(thread_arr->id);
			/* Copy last 32 bits from the address of this thread
			 * so we have a "meaningful" value for pthread_exit */
	*exit_var = (int) (pthread_self());
	
	printf("\nthread id %i @ 0x%.18lX awakening\n", thread_arr->id, (uintptr_t) pthread_self());
			/* free & reset stime which was allocated @main() */
	free(*pp);
	*pp=NULL;
	pthread_exit((void *) exit_var);
	return NULL;
}

/*   Links
 *   + http://www.csc.villanova.edu/~mdamian/threads/posixmutex.html
 *   + https://docs.oracle.com/cd/E19455-01/806-5257/6je9h032p/index.html
 *
 *   Notes
 *
 *   (C) Antti Antinoja, 2019
 */
