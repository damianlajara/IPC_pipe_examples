#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>//strlen

ssize_t read_from_file;
ssize_t read_from_pipe;
ssize_t written_to_pipe;
int mypipe[2];

/*Parent*/
void read_pipe(int file)
{
	puts("\nParent Process in read_pipe!!");
	char buffer[1024];
	FILE *stream;
	stream = fdopen (file, "r");
	printf("\nPARENT: in read_pipe before loop, Array contains: %s\n", buffer);/*DEBUG CODE*/
	do
	{
		read_from_pipe = fread(buffer,100, 1, stream);
		fprintf(stdout,"%s",buffer);
		//fflush (stdout);
		printf("\nPARENT: in read_pipe during loop, Array contains: %s\n", buffer);/*DEBUG CODE*/

	} while(!feof(stream) && !ferror(stream) && read_from_pipe > 0);

	printf("\nPARENT: in read_pipe after loop, Array contains: %s\n", buffer);/*DEBUG CODE*/

	/*
	 * Here is Problem 1! For some reason, buffer[] is empty, when its supposed to be filled with what's in the stream
	 */

	fclose(stream);
}

/*Child*/

void write_pipe(int file, char *array)//mypipe[1]
{
	close(mypipe[0]);
	puts("\nChild Process in write_pipe!");
	//fwrite(mypipe[1],100, 1, fout);

	FILE *stream;
	stream = fdopen(file, "w");
	for(int i = 0; i < read_from_file; ++i)
	{
		written_to_pipe = fwrite(array, 100, 1, stream);//write to stream
		//fflush (stream);
		printf("CHILD: In write_pipe ARRAY contains: %s\n", array);/*DEBUG CODE*/
	}

	fclose (stream);
}

void read_file(const char *input)//char
{
	puts("\nChild Process in read_file!");
	char buffer[1024];
	char temp[1024];
	FILE* fin;//declare a FILE pointer pointing to the input file
	fin = fopen(input, "r");//opens file for reading purposes
	strcpy(temp, "");//initialize temp array
	printf("CHILD: Before loop Array contains: %s\n", temp);/*DEBUG CODE*/
	do
	{
		//printf("\nDuring loop in read_file");/*DEBUG CODE*/
		read_from_file = fread(temp,100, 1, fin);//read contains the amount of elements read from input files
		//printf(" ARRAY contains: %s", temp);/*DEBUG CODE*/
		strcat(buffer ,temp);
		//fflush (fin);
	} while(!feof(fin) && !ferror(fin) && read_from_file > 0);//while no errors and end of file hasnt been reached

	/*
	 * Here is Problem 2! For some reason, after having read the file, there is some extra text taken from towards the end
	 * of the file, and is being unwantingly read by this function!
	 */

	fclose(fin);//close input so content in buffer can be succesfully written
	printf("CHILD: After loop Array contains: %s\n", buffer);/*DEBUG CODE*/
	
	write_pipe(mypipe[1], buffer);
}



int main_program(const char *input)
{
	int status;
	pid_t pid;

	pipe(mypipe);
	pid = fork(); //creates a child

	if(pid == 0)
	{
		//close(mypipe[1]);
		puts("Child Process!");
		printf("CHILD: my PID is %ld\n", (long) getpid());
		printf("CHILD: I will now read the input file!.\n");
		read_file(input);
		return EXIT_SUCCESS;
	}

	else if(pid < 0)//if fork fails it returns -1, but just in case I accounted for everything
	{
		puts("Fork failed!!");
		fprintf(stderr, "Fork failed.\n");
		return EXIT_FAILURE;
	}

	else
	{
		/*
		if wifexited() is true, it means child exited succesfully, 
		and will only then show its exit status with wexitstatus()
		*/

			puts("Parent Process!");
			close(mypipe[1]);//close unused side
			printf("PARENT: my PID is %ld\n", (long) getpid());
			printf("PARENT: I will now wait for my child to exit.\n");
			wait(&status); // wait for child to exit, and store its status
			if(WIFEXITED(status))//if child exited normally
			{
	       	 	printf("PARENT: Child's exit code is: %d\n", WEXITSTATUS(status));
	       		printf("PARENT: Now going to read from pipe!!\n");
				read_pipe(mypipe[0]);
				printf("PARENT: Goodbye!\n");
				return EXIT_SUCCESS;
			}
			else
			{
				printf("\nParent will now exit, due to bad child exit code!\n");
				return EXIT_FAILURE;
			}
	}
	return EXIT_SUCCESS;

}

int main(int argc, char **argv)
{
	main_program(argv[1]);

	return 0;
}