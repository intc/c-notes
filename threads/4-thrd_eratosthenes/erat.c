#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>             /* for sleep */
#include <stdlib.h>             /* for malloc */
#include <inttypes.h>           /* uintptr_t, strtoul */
#include <string.h>             /* strncpy */
#include <math.h>				/* floor, sqrt */

#define O_M 0
#define O_X 1
#define O_T 2

typedef struct _args {
	unsigned long min;
	unsigned long max;
	int num_w_threads;
	char strings[3][3];
} _args;

typedef struct _wargs {
	unsigned long min;
	unsigned long max;
} _wargs;

typedef struct _slist {
	struct _slist *next;
	unsigned long val;
} _slist;

#define SZ_WARGS sizeof(_wargs)
#define SZ_SLIST sizeof(_slist)

#define MIN_W_GAP 5

char usage[]=" Usage:\n\
 erat -m [integer] -x [integet] (-t [integer])\n\
 \n\
 \n\
 -m, start value >=5, positive odd integer value\n\
 -x, end value, positive integer value\n\
 -t, number of threads to use, positive integer value, optional (default: 1)\n";

int *th_eratosthenes(void **);
int get_args(int, char **, _args *);
void print_args(_args *);

int main(int argv, char **argc) {
	_args args;

	get_args(argv, argc, &args);
	print_args(&args);

	_wargs *w_args[args.num_w_threads];
	pthread_t w_thread[args.num_w_threads];
			/* prepare to devide the given min to max range (gap) for
			 * individual worker threads */
    unsigned long gap = args.max - args.min;
	unsigned long w_gap = gap / args.num_w_threads;
	unsigned long reminder = gap % args.num_w_threads;
	unsigned long w_start = args.min;

	if ( w_gap < MIN_W_GAP ) {
		fprintf(stderr, "Minimum gap (max - min) is %i\n", MIN_W_GAP);
		fprintf(stderr, "Please use less threads or provide larger range (max - min)! Exitting");
		return(1);
	}

			/* Starting up the workers */
	for ( int i = 0; i < args.num_w_threads; i++ ) {
		w_args[i] = malloc(SZ_WARGS);
		
		if ( i == 0 ) {
			/* 1st worker (takes the reminder portion of the devided gap) */
			w_args[i]->min = w_start;
			w_args[i]->max = w_start + w_gap + reminder;
		} else {
			/* Rest of the workers */
			unsigned long min = w_start + 1;
			/* Ensure that min is an odd number */
			if ( ! (min%2) ) min++;
			w_args[i]->min = min;
			w_args[i]->max = w_start + w_gap;
		}
		w_start = w_args[i]->max;
			/* Start this worker */
		if ( pthread_create( &w_thread[i], NULL,
				(void *) th_eratosthenes,
				(void **) &w_args[i] ) != 0 ) {
			perror("ERROR: Cerating thread");
			assert(0);
		}
	}

			/* Wait for the workers to exit */
	for ( int i = 0; i < args.num_w_threads; i++ ) {
		_slist *wl_root = NULL, *wl_node = NULL;
		if ( pthread_join(w_thread[i], (void **) &wl_root) == 0 ) {
			printf("thread 0x%.16lX exitting\n", (uintptr_t) w_thread[i]);
			if (wl_root) {
				wl_node = wl_root; int go = 1;
				while ( go ) {
					printf("%lu\n", wl_node->val);
					_slist *tl = wl_node;
					if ( wl_node->next )
						wl_node = wl_node->next;
					else
						go = 0;
					free(tl);
				}
			} else {
				printf("This thread didn't find any primes\n");
			}
			wl_root = NULL;
		} else {
			perror("ERROR: Joining thread");
			assert(0);
		}
	}
	
	printf("\nmain(): All %d threads have terminated\n", args.num_w_threads);
	
	return 0;
}

int *th_eratosthenes(void **int_arg) {
	_wargs **pp = (_wargs **)int_arg;
	_wargs *w_args = *pp;
	_slist *wl_root = NULL, *wl_node = NULL, *wl_prev = NULL;

	printf("thread 0x%.16lX min: %lu max: %lu\n",
			(uintptr_t) pthread_self(), w_args->min, w_args->max);

			/* Assign value for the root node */
	if ( ( wl_root = malloc(SZ_SLIST) ) == NULL ) {
		perror("malloc error"); pthread_exit((void *) wl_root);
	}
	wl_root->val = w_args->min; wl_prev = wl_root;

			/* Fill the list with odd numbers for the w_gap range */
	long unsigned loop_min = w_args->min + 2;
	for ( long unsigned n = loop_min; n <= w_args->max; n = n + 2 ) {
		if ( ( wl_node = malloc(SZ_SLIST) ) == NULL ) {
			perror("malloc error"); pthread_exit((void *) wl_root);
		}
		wl_node->next = NULL; wl_prev->next = wl_node; wl_prev = wl_node;
		wl_node->val = n;
	}
	
	unsigned long sqrt_max = (unsigned long) floor( sqrt( w_args->max ) );

	for ( unsigned long y = 3; y <= sqrt_max ; y++ ) {
		while ( wl_root->val > y && (wl_root->val % y == 0) ) {
			/* Root node is not a prime. Eliminate. */
			if (wl_root->next) {
				_slist *newroot = wl_root->next; free(wl_root); wl_root = newroot;
			} else {
			/* If arrived here: This is the last node -> no primes in this w_gap */
				free(wl_root); wl_root = NULL;
				pthread_exit((void *) wl_root); /* <- POSSIBLE EXIT POINT! */
			}
		}
			/* Now we have a root node which is a prime number.
			 * Let's contune with the rest of the list */
		if ( wl_root->next ) {
			wl_prev = wl_root; wl_node = wl_root->next; int go = 1;
			while ( go ) {
				if ( wl_node->val > y && (wl_node->val % y == 0) ) {
			/* val is not a prime. Remove this node from the list.
			 * Note: wl_prev remains the same in this case. */
					_slist *tmp = wl_node;
					wl_prev->next = wl_node->next;
					if (wl_node->next)
						wl_node = wl_node->next->next;
					else
						wl_node = NULL;
					free(tmp);
				} else {
					wl_prev = wl_node;
					wl_node = wl_node->next;
				}
				if ( !wl_node ) go = 0;
			}
		}
	}
			/* free & reset w_args ( was allocated @main() ) NOTE: move these to main.*/
	free(*pp);
	*pp=NULL;
	pthread_exit((void *) wl_root);
	return NULL;
}

int get_args(int argv, char **argc, _args *args){

	int c = argv - 1;
	unsigned long val[3] = { 0, 0, 0 };
			/* Initialize cli options */
	strncpy(args->strings[O_M], "-m", 3);
	strncpy(args->strings[O_X], "-x", 3);
	strncpy(args->strings[O_T], "-t", 3);
			/* We need at least 4 argc elements. Also require even number of elements. */
	if ( c%2 == 1 || c<4 ) {
		printf(usage);
		exit(1);
	}
			/* Parse cli */
	while ( c > 0 ) {
		int found = 0;
		for ( int n = 0; n < 3; n++ ) {
			if ( strncmp(args->strings[n], argc[c-1], 10) == 0 ) {
				val[n] = strtoull(argc[c], NULL, 10);
				found = 1;
			}
			if (found) break;
		}
		if ( !found ) { fprintf(stderr, "Unknown option %s\n", argc[c-1]); exit (1); }
		c = c - 2;
	}
			/* Ensure that we have an odd number for min */
	if ( ! ( val[O_M] % 2 ) || val[O_M] < 5 ) { printf(usage); exit(1); }
			/* Assign parced values to corresponding args members. */
	if ( val[O_M] ) args->min = val[O_M]; else { printf(usage); exit(1); }
	if ( val[O_X] ) args->max = val[O_X]; else { printf(usage); exit(1); }
	if ( val[O_T] ) args->num_w_threads = (int) val[O_T]; else args->num_w_threads = 1;

	return 0;
}

void print_args(_args *args) {
	printf("min: %lu\n", args->min);
	printf("max: %lu\n", args->max);
	printf("threads: %u\n\n", args->num_w_threads);
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
