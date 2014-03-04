//Using a Pipe to Communicate with a Child Process

#include <stdlib.h> 
#include <stdio.h> 
#include <unistd.h>
/* Write COUNT copies of MESSAGE to STREAM, pausing for a second between each. */
void writer (const char* message, int count, FILE* stream) 
{
	for (; count > 0; --count) 
	{
		/* Write the message to the stream, and send it off immediately. */ 
		fprintf (stream, "%s\n", message);
		fflush (stream);
		/* Snooze a while. */
		sleep (1);
	} 
}

/* Read random strings from the stream as long as possible. */
void reader (FILE* stream) 
{
	char buffer[1024];
	/* Read until we hit the end of the stream. fgets reads until either a newline or the end-of-file. */ 
	while (!feof (stream) && !ferror (stream) && fgets (buffer, sizeof (buffer), stream) != NULL)
	fputs (buffer, stdout);
}

int main () 
{
	int fds[2]; 
	pid_t pid;
	
	pipe (fds); /* Create a pipe. File descriptors for the two ends of the pipe are placed in fds. */
	pid = fork (); /* Fork a child process. */
	if (pid == (pid_t) 0) 
	{
		/*This is the child process.*/
		printf("This is the child process\n");./
		FILE* stream;		
		/* Close our copy of the write end of the file descriptor. */
		close (fds[1]);
		/* Convert the read file descriptor to a FILE object, and read from it. */
		stream = fdopen (fds[0], "r"); reader (stream);
		close (fds[0]); 
	}
	else 
	{
		/* This is the parent process. */
		printf("This is the Parent process\n");
		FILE* stream;
		/* Close our copy of the read end of the file descriptor. */ 
		close (fds[0]);
		/* Convert the write file descriptor to a FILE object, and write to it. */
		stream = fdopen (fds[1], "w"); writer ("Hello, world.", 5, stream); close (fds[1]);
	}
	return 0;
}