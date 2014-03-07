/*
Frequently, you'll want to create a child process and set up one end of a pipe as its standard input or standard output. 
Using the dup2 call, you can equate one file descriptor with another. 
For example, to redirect a process's standard input to a file descriptor fd, use this line: dup2 (fd, STDIN_FILENO); 
The symbolic constant STDIN_FILENO represents the file descriptor for the standard input, which has the value 0. 
The call closes standard input and then reopens it as a duplicate of fd so that the two may be used interchangeably. 
Equated file descriptors share the same file position and the same set of file status flags.
 Thus, characters read from fd are not reread from standard input.
This program uses dup2 to send the output from a pipe to the SORT command.
After creating a pipe, the program forks.The parent process prints some strings to the pipe.
The child process attaches the read file descriptor of the pipe to its standard input using dup2. 
It then executes the sort program.

SORT: sort reads lines of text from standard input, 
sorts them into alphabetical order, and prints them to standard output.
*/

//Redirect Output from a Pipe with dup2
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h>
int main () 
{
	int fds[2]; pid_t pid;

	/* Create a pipe. File descriptors for the two ends of the pipe are placed in fds. */
	pipe (fds);

	/* Fork a child process. */ 
	pid = fork ();
	if (pid == (pid_t) 0) 
	{
		/* This is the child process. Close our copy of the write end of the file descriptor. */
		close (fds[1]);
		/* Connect the read end of the pipe to standard input. */ 
		dup2 (fds[0], STDIN_FILENO);
		/* Replace the child process with the “sort” program. */ 
		execlp ("sort", "sort", 0);
	}
	else 
	{
		/* This is the parent process. */
		FILE* stream;
		/* Close our copy of the read end of the file descriptor. */ 
		close (fds[0]);
		/* Convert the write file descriptor to a FILE object, and write
		to it. */
		stream = fdopen (fds[1], "w");
		fprintf (stream,"This is a test.\n");
		fprintf (stream,"Hello, world.\n");
		fprintf (stream,"My dog has fleas.\n");
		fprintf (stream,"This program is great.\n");
		fprintf (stream,"One fish, two fish.\n");
		fflush (stream);
		close (fds[1]);
		/* Wait for the child process to finish. */ 
		waitpid (pid, NULL, 0);
	}
	return 0;
}