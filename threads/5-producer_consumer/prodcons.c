#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <error.h>

#define MAX_LINE_LENGTH 4096
#define LINE_BUF_SIZE 42

#define T_EUID 1000

// vim: ts=4

char line_buf[LINE_BUF_SIZE][MAX_LINE_LENGTH];
int b_count = 0;
int done_exit = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t new_data = PTHREAD_COND_INITIALIZER;

void* unpriv_consumer() {
	int b_out = 0;
	char local_line[MAX_LINE_LENGTH]; // local single line storage variable

	// drop consumer threads privileges (this affects *only* unpriv_consumer())
	long ret = syscall(SYS_setresuid, T_EUID, T_EUID, T_EUID);
	if (ret != 0) {
		perror("Failed to set euid for worker");
	}

	while (1) {
		if (pthread_mutex_lock(&lock) != 0) {// acquire lock
			perror("lock failed");
			return NULL;
		}

		while (b_count == 0) {               // nothing in buffer -> wait
			printf(" c: wait: buffer empty\n");
			if (done_exit) {                 // producer signaled to exit, skip waiting and flush the buffer
				b_count--;                   // buffer didn't advance (since there was no new data) -> adjust
				break;
			}
			pthread_cond_wait(&new_data, &lock);
			printf(" c:   go: b_count: %i\n", b_count);
		}

		if (b_count < 0) {                   // producer signaled to quit and the buffer is empty -> exit
			printf(" c: done\n");
			return NULL;
		}

		// consume
		strcpy(local_line, line_buf[b_out]); // copy data to local_line for later (outside lock) processing
		b_count--;                           // step buffer count down
		pthread_cond_signal(&not_full);      // signal producer that the circular buffer has again free space
		pthread_mutex_unlock(&lock);         // end of critical section, release lock

		b_out ++;                            // step reading position
		b_out = b_out % LINE_BUF_SIZE;       // rotate @ LINE_BUF_SIZE
		// emit
		printf("%s", local_line);
	}

	return NULL;
}

int main() {
	pthread_t thread;
	pthread_create(&thread, NULL, unpriv_consumer, NULL);
	int b_in = 0;

	// open the log file as root
	FILE* file = fopen("./test.log", "r");
	if (file == NULL) {
		printf("Failed to open the log file.\n");
		return 1;
	}

	while (1) {
		if (pthread_mutex_lock(&lock) != 0) {// acquire lock
			perror("lock failed");
			break;
		}
		while (b_count == LINE_BUF_SIZE) {
			printf("p : wait: buffer full\n");
			pthread_cond_wait(&not_full, &lock);
			printf("p :   go: b_count: %i\n", b_count);
		}
		// produce
		if (!fgets(line_buf[b_in], MAX_LINE_LENGTH, file)) {
			// returned NULL: EOF or ERROR
			// signal consumer to flush remaining items from the buffer and to exit
			done_exit = 1;                   // rise exit flag
			pthread_cond_signal(&new_data);  // send new_data signal so that consumer quits waiting and works for flush & exit
			pthread_mutex_unlock(&lock);     // release lock
			break;
		}
		b_count++;
		pthread_cond_signal(&new_data);
		pthread_mutex_unlock(&lock);

		b_in++;
		b_in = b_in % LINE_BUF_SIZE;

	}

	// close the log file
	fclose(file);

	// Wait for the thread to finish
	pthread_join(thread, NULL);

	return 0;
}

