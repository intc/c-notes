			/* POSIX: read(), pipe(), fork(), dup2(), STDOUT_FILENO */
#include <unistd.h>
			/* system(), exit() */
#include <stdlib.h>
			/* printf(), fflush(), perror() */
#include <stdio.h>
			/* waitpid() */
#include <sys/wait.h>
#define _R_ 0
#define _W_ 1

int main(void)
{
	int des_p[2];
	if( pipe(des_p) == -1 ) {
		perror("pipe() failed");
		return 1;
	}

			/* 1st fork, producer */
	int p_pid = 0;
	if( (p_pid = fork()) == 0 ) {
			/* Producer don't use read end of the pipe. Let's close it. */
		close(des_p[_R_]);
			/* Close STDOUT_FILENO and point it to des_p[_W_]. */
		if ( dup2(des_p[_W_], STDOUT_FILENO) == -1 )
			perror ("dup2() failed");
			/* This will now enter the pipe. */
		printf("Attached stdout (%i) to piped fd (%i)\n",
				STDOUT_FILENO, des_p[_W_]);
			/* Flushing stdout guarantees chronological output. */
		fflush(stdout);
		if ( system("echo \"ok\"") != 0 )
			perror("system(cmd); cmd exitted with error status. system() call");
		close(des_p[_W_]);
		exit(0);
	}

			/* 2nd fork, consumer */
	int c_pid = 0;
	if( (c_pid = fork()) == 0 ) {
			/* Let's make sure we have only one write end open. */
		close(des_p[_W_]);

		char bc='\0'; int rv=0;
		while ( (rv = read(des_p[_R_], &bc, 1)) > 0 )
			fprintf(stdout,"%c", bc);
		if ( rv < 0 ) perror ("read() failed");

		exit(0);
	}

			/* Not closing _W_ will prevent c_pid from exitting. */
	close(des_p[_W_]); close(des_p[_R_]);
			/* Wait for the child processes to exit. */
	fprintf(stdout,"Waiting p_pid\n"); waitpid(p_pid, NULL, 0);
	fprintf(stdout,"Waiting c_pid\n"); waitpid(c_pid, NULL, 0);

	return 0;
}

/*   Links
 *   + File descriptor: https://en.wikipedia.org/wiki/File_descriptor
 *
 *   Notes
 *   POSIX Programmer's manual is available on Linux as man pages (0p, 1p, 3p).
 *
 *   (C) Antti Antinoja, 2019
 */
