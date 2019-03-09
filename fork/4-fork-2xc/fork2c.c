#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
			/* strnlen() */
#include <string.h>
			/* uintptr_t */
#include <inttypes.h>

#define _R_ 0
#define _W_ 1
#define MSG_SZ 201
#define MSG_MAX MSG_SZ - 1

const int _exec=1;
const int _system=1;

void producer_prnt(char *, ssize_t, ssize_t);
void producer_write(int, char *);
void stdout_connect_to(int);
void producer(int *);
void consumer(int *);

int main(void)
{
	int des_p[2];
	if( pipe(des_p) == -1 ) {
		perror("Pipe failed");
		return 1;
	}

			/* 1st fork, producer */
	int p_pid = 0;
	if( (p_pid = fork()) == 0 )
		producer(des_p);

			/* 2nd fork, consumer */
	int c_pid = 0;
	if( (c_pid = fork()) == 0 )
		consumer(des_p);

			/* Close the pipe since it's not accessed by the parent */
	close(des_p[_R_]); close(des_p[_W_]);

	fprintf(stderr, "  Parent: Waiting for %i\n", p_pid);
	fprintf(stderr, "  %i - ready!\n",
			waitpid(p_pid, NULL, 0));
	fprintf(stderr, "  Parent: Waiting for %i\n", c_pid);
	fprintf(stderr, "  %i - ready!\n",
			waitpid(c_pid, NULL, 0));
	fprintf(stderr, "  All children closed. Exit. (main)\n");

	return 0;
}

void producer(int *des_p){
	const int msgl = 101; char msg[msgl];
			/* This child don't use read end of the pipe. Let's close it. */
	close(des_p[_R_]);
			/* No stdout redirection yet. We can use a function which can
			 * write directly to the pipe */
	snprintf(msg, msgl, "Producer pid: %i\n", getpid());
	producer_write(des_p[_W_], msg);
			/* After this stdout redirects to the pipe */
	stdout_connect_to(des_p[_W_]);

	if ( _system ) {
			/* Note the use of fflush to avoid issues caused by buffered
			 * stdout stream mixing order of our messages */
		printf("Producer: Running \"date -R\" via system()!\n"); fflush(stdout);
			/* system(cmd) uses fork() to create a child process (man 3 system)
			 * that executes shell which runs the "cmd". */
		int sys_r = system("date -R");
		printf("Producer: system exitted with %i\n", sys_r); fflush(stdout);
			/* Closing the write end of pipe will create an EOF on the receiving end. */
		if ( ! _exec ) close(des_p[_W_]);
		sleep(2);
	}
			/* execvp replaces the child with new process image. see "man 3 execvp" */
	if( _exec ){
		printf("Producer: Running \"date -R\" via execvp()!\n"); fflush(stdout);
			/* Close writing end of pipe since the output will be handed over to
			 * execvp process. */
		close(des_p[_W_]);
		char *const prog1[] = { "date", "-R", NULL};
		execvp(*prog1, prog1);
			/* Execution reaches this point only if execvp failed. */
		perror("execvp of ls failed");
	}
	exit(0);
}

void consumer(int *des_p){
	char *line = NULL; size_t len = 0;	ssize_t read = 0;
			/* Close write pipe. Failing to close this here causes
			 * getline to "hang" since the write end of the pipe
			 * remains open even the producer has exitted! */
	close(des_p[_W_]);
	printf("  Consumer pid: %i\n", getpid());
	FILE *fp_r = fdopen(des_p[_R_], "r");
	producer_prnt(line, len, read);
			/* Read lines from read end of the pipe.
			 * Note: Getline will handle allocation. */
	while ( (read = getline( &line, &len, fp_r)) != -1) {
		producer_prnt(line, len, read);
	}
	free(line); line = NULL; len = 0;
	producer_prnt(line, len, read);
	printf("  Closing consumer!\n");
	exit(0);
}

void producer_prnt(char *line, ssize_t len, ssize_t read){
	printf("  a:0x%.16lx - ", (uintptr_t)line);
	printf("r:%.3li - ", read);
	printf("l:%li - ", len);
	if(line) printf("%s", line); else printf("nil string\n");
}

void producer_write(int fd, char *msg) {
	if ( write(fd, msg, strnlen(msg, MSG_MAX)) == -1 )
		perror("write failed");
}

void stdout_connect_to(int fd) {
			/* Close stdout and assoiate it with the write end of the pipe */
	if ( dup2(fd, STDOUT_FILENO) == -1 ) {
		perror ("dup2() failed");
		exit(1);
	}
}

/*   Links
 *   + wait / waitpid: https://webdocs.cs.ualberta.ca/~tony/C379/C379Labs/Lab3/wait.html
 *
 *   Notes
 *   I read some notes that in Unix stdout and stdin are line buffered while in terminal
 *   and block buffered otherwise (like here when working with pipes). fflush() seems to
 *   keep things in order.
 *
 *   One could also use setbuf family of functions (see "man setbuf") to disable buffering.
 *
 *   (C) Antti Antinoja, 2019
 */
