#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

//char length[1024];
ssize_t read_from_file;
ssize_t read_from_pipe;
int mypipe[2];
//int written_to_pipe;
// Read characters from the pipe and echo them to stdout.

/*Parent*/
void read_pipe(int file)
{
	puts("\nParent Process in read_pipe!!");
	ssize_t read_pipe;
	char buffer[1024];
	int c;
	FILE *stream;
	stream = fdopen (file, "r");
	while(!feof(stream) && !ferror(stream))//while no errors and end of file hasnt been reached
	{
		//reads from stream, length characters, 100 bytes at a time and stores it in the array mypipe, and assaigns read the return value of fread
		read_pipe = fread(buffer,100, 1, stream);//read contains the amount of characters read from pipe
		fprintf(stdout,"%s",buffer);

		//c = getc(buffer);//stream
		//putchar(c);
		//printf("child read: %d\n characters from pipe", read);
	}
	fclose(stream);
}

/*Child*/

void write_pipe(int file, char array[])//fds[1]
{
	puts("\nChild Process in write_pipe!");
	//fwrite(mypipe[1],100, 1, fout);

	FILE *stream;
	stream = fdopen(file, "w");
	for(int i = 0; i< read_from_file; ++i) 
	{
		/*written_to_pipe = */fwrite(array, 100, 1, stream);//write to stream//instead of array it was file
	}
	fprintf (stream, "\nhello, world!\n");
	fprintf (stream, "goodbye, world!\n");
	//printf("child wrote: %d\n characters from pipe", written_to_pipe);
	fclose (stream);
}

void read_file(const char *input)//char
{
	puts("\nChild Process in read_file!");
	char buffer[1024];
	ssize_t read_file;
	FILE* fin;//declare a FILE pointer pointing to the input file
	fin = fopen(input, "r");//opens file for reading purposes

	while(!feof(fin) && !ferror(fin))//while no errors and end of file hasnt been reached
	{
		//reads from fin, length characters, 100 bytes at a time and stores it in the array mypipe, and assaigns read the return value of fread
		read_file = fread(buffer,100, 1, fin);//read contains the amount of characters read from input files
		printf("child read: %zd characters from file", read);
	}//mypipe[0] holds everything in file
	read_from_file = read_file;
	fclose(fin);//close input so content in buffer can be succesfully written
	write_pipe(mypipe[1], buffer);
}



int main_program(const char *input)
{

	//pid_t getpid (void): The getpid function returns the process ID of the current process.
	//pid_t getppid (void): The getppid function returns the process ID of the parent of the current process.
	pid_t pid;
	//int mypipe[2];

	// Create the pipe.
	//puts the file descriptors for the reading and writing ends of the pipe (respectively) into filedes[0] and filedes[1]
	//0 is for input, and 1 is for output

	pipe(mypipe);

	//Create the child process.
	pid = fork(); //creates a child

	if(pid == 0)
	{
		// This is the child process.
		//Close other end first.
		puts("Child Process!");
		//close (mypipe[1]);
		read_file(input);
		//write_pipe(mypipe[1]);
		return EXIT_SUCCESS;
	}

	else if(pid < 0)
	{
		// The fork failed.
		puts("Fork failed!!");
		fprintf(stderr, "Fork failed.\n");
		return EXIT_FAILURE;
	}

	else
	{
		// This is the parent process.
		//Close other end first.
		puts("Parent Process!");
		printf("PARENT: I will now wait for my child to exit.\n");
        wait(&status); // wait for child to exit, and store its status
        printf("PARENT: Child's exit code is: %d\n", WEXITSTATUS(status));
        printf("PARENT: Now going to read from pipe!!\n");
		close(mypipe[1]);//close unused side
		read_pipe(mypipe[0]);
		printf("PARENT: Goodbye!\n");
		return EXIT_SUCCESS;
	}

	//read_pipe();
	//write_pipe();
	return EXIT_SUCCESS;

}

int main(int argc, char **argv)
{
	main_program(argv[1]);

	return 0;
}