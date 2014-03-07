#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

//char length[1024];
ssize_t read_from_file =1;
ssize_t read_from_pipe;
ssize_t written_to_pipe;
int mypipe[2];

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
		/*reads from stream, length characters, 100 bytes at a time and stores it in the array buffer, 
		and assaigns read the return value of fread*/
		read_from_pipe = fread(buffer,100, 1, stream);//read_from_pipe contains the amount of characters read from pipe
		fprintf(stdout,"%s",buffer);
		printf("\nREAD: %zd characters\n", read_from_pipe);
		printf("\nARRAY contains: %s\n", buffer);
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
		written_to_pipe = fwrite(array, 100, 1, stream);//write to stream//instead of array it was file
	}
	printf("\nCHILD WROTE: %zd characters\n", written_to_pipe);/*DEBUG CODE*/
	//fprintf (stream, "\nhello, world!\n");/*DEBUG CODE*/
	//fprintf (stream, "goodbye, world!\n");/*DEBUG CODE*/
	fclose (stream);
}

void read_file(const char *input)//char
{
	puts("\nChild Process in read_file!");
	char buffer[1024];
	FILE* fin;//declare a FILE pointer pointing to the input file
	fin = fopen(input, "r");//opens file for reading purposes
	printf("Before loop\n");/*DEBUG CODE*/
	printf("\nARRAY contains: %s\n", buffer);/*DEBUG CODE*/
	while(!feof(fin) && !ferror(fin) && read_from_file >0)//while no errors and end of file hasnt been reached
	{
		printf("During loop\n");/*DEBUG CODE*/
		//reads from fin, length characters, 100 bytes at a time and stores it in the array mypipe, and assaigns read the return value of fread
		read_from_file = fread(buffer,30, 10, fin);//read contains the amount of characters read from input files
		printf("child read: %zd characters from file", read_from_file);
		printf("\nARRAY contains: %s\n", buffer);/*DEBUG CODE*/
	}
	printf("After loop\n");/*DEBUG CODE*/
	printf("\nARRAY contains: %s\n", buffer);/*DEBUG CODE*/
	fclose(fin);//close input so content in buffer can be succesfully written
	write_pipe(mypipe[1], buffer);
}



int main_program(const char *input)
{
	//pid_t getpid (void): The getpid function returns the process ID of the current process.
	//pid_t getppid (void): The getppid function returns the process ID of the parent of the current process.
	int status;
	pid_t pid;

	// Create the pipe.
	//puts the file descriptors for the reading and writing ends of the pipe (respectively) into filedes[0] and filedes[1]
	//0 is for input, and 1 is for output

	pipe(mypipe);

	//Create the child process.
	pid = fork(); //creates a child

	if(pid == 0)
	{
		// This is the child process.
		//Dont close the other end, because it uses it to write to the parent
		puts("Child Process!");
		printf("Child PID is %ld\n", (long) getpid());
		printf("CHILD: I will read the input file!.\n");
		//close (mypipe[1]);
		read_file(input);
		//write_pipe(mypipe[1]);
		return EXIT_SUCCESS;
	}

	else if(pid < 0)//if fork fails it returns -1, but just in case I accounted for everything
	{
		// The fork failed.
		puts("Fork failed!!");
		fprintf(stderr, "Fork failed.\n");
		return EXIT_FAILURE;
	}

	else
	{
		// This is the parent process.
		puts("Parent Process!");
		close(mypipe[1]);//close unused side
		printf("Parent PID is %ld\n", (long) getpid());
		printf("I will now wait for my child to exit.\n");
		wait(&status); // wait for child to exit, and store its status
        printf("Child's exit code is: %d\n", WEXITSTATUS(status));
        printf("Now going to read from pipe!!\n");
		read_pipe(mypipe[0]);
		printf("Goodbye!\n");
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