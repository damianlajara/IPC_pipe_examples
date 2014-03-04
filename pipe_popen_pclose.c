/*
The call to popen creates a child process executing the sort command, 
replacing calls to pipe, fork, dup2, and execlp.The second argument, “w”, 
indicates that this process wants to write to the child process.
The return value from popen is one end of a pipe; the other end is connected to the child process’s standard input.
After the writing fin- ishes, pclose closes the child process’s stream, waits for the process to terminate, and returns its status value.
The first argument to popen is executed as a shell command in a subprocess run- ning /bin/sh.
The shell searches the PATH environment variable in the usual way to find programs to execute. 
If the second argument is “r”, the function returns the child process’s standard output stream so that the parent can read the output. 
If the second argument is “w”, the function returns the child process’s standard input stream so that the parent can send data.
If an error occurs, popen returns a null pointer.
Call pclose to close a stream returned by popen.After closing the specified stream, pclose waits for the child process to terminate.
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main () 
{
	FILE* stream = popen ("sort", "w");
	fprintf (stream, "This is a test.\n");
	fprintf (stream, "Hello, world.\n");
	fprintf (stream, "My dog has fleas.\n");
	fprintf (stream, "This program is great.\n");
	fprintf (stream, "One fish, two fish.\n");
	return pclose (stream);
}