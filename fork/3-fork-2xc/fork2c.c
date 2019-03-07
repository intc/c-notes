#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>

#define _R_ 0
#define _W_ 1
#define MSG_SZ 201
#define MSG_MAX MSG_SZ - 1
#define _A_TEST

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

			/* Finalize */
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
			/* This child don't use Read end of the pipe. Let's close it. */
	close(des_p[_R_]);
	snprintf(msg, msgl, "Producer pid: %i\n", getpid());
	producer_write(des_p[_W_], msg);
			/* Anything written to stdout by system or execvp will
			 * enter *write* end of des_p. */
	stdout_connect_to(des_p[_W_]);

	if ( _system ) {
		producer_write(des_p[_W_], "Producer: Running ls -al with system()!\n");
		int sys_r = system("ls -al");
		snprintf(msg, msgl, "Producer: system exitted with %i\n", sys_r);
		producer_write(des_p[_W_], msg);
			/* Closing the write end of pipe will create an EOF on the receiving end. */
		if ( ! _exec ) close(des_p[_W_]);
		sleep(2);
	}
			/* execvp has to be done last since execvp replaces the child
			 * with new process image. see "man 3 execvp" */
	if( _exec ){
		producer_write(des_p[_W_], "Producer: Running ls -al with execvp()!\n");
			/* Close writing end of pipe since the output will be handed over to
			 * execvp process. */
		close(des_p[_W_]);
		char *const prog1[] = { "ls", "-al", 0};
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
#ifdef _A_TEST
			/* -A- */
	printf("\n");
#endif
			/* close stdout */
	close(STDOUT_FILENO);
			/* Open STDOUT_FILENO as fd */
	if ( dup2(fd, STDOUT_FILENO) == -1 )
		perror ("dup2 failed");
#ifdef _A_TEST
			/* Why this outputs ONLY when there is printf to stdout
			 * BEFORE we close & dup2 ??? */
	printf("Attached stdout (%i) to piped fd %i\n", STDOUT_FILENO, fd);
#endif
}

/*   Links
 *   + wait / waitpid: https://webdocs.cs.ualberta.ca/~tony/C379/C379Labs/Lab3/wait.html
 *
 *   Notes
 *
 *   (C) Antti Antinoja
 */
