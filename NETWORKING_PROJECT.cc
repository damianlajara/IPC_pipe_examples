#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>//strlen

ssize_t read_from_file;
ssize_t read_from_pipe;
ssize_t written_to_pipe;
char read_buffer[1024];
int child1[2];
int child2[2];

void read_pipe(int *pipe_read)
{
	int file = pipe_read[0];
	FILE *stream;
	char *buf;
	stream = fdopen (file, "r");
	do
	{
		read_from_pipe = fread(read_buffer,1, 100, stream);
		buf = (char *)malloc(read_from_pipe*sizeof(char));
        strncpy(buf,read_buffer,read_from_pipe);
		fprintf(stdout,"%s",buf);
        free(buf);

	} while(!feof(stream) && !ferror(stream) && read_from_pipe > 0);

	fclose(stream);
}

void write_pipe(FILE *stream, char *temp,int bytes_read)
{
    int wrote = fwrite(temp,1, bytes_read, stream);//write to stream
    fprintf(stdout,"Child in write pipe wrote %d bytes\n",wrote);/*DEBUG CODE*/
}

void read_write_from_file(const char *input, int *pipe_write)//char
{
	//puts("\nChild Process in read_file!");
	char temp[100];
	FILE* fin;//declare a FILE pointer pointing to the input file
	fin = fopen(input, "r");//opens file for reading purposes
	FILE *stream;
	stream = fdopen(pipe_write[1], "w");
	do
	{
		read_from_file = fread(temp,1, 100, fin);//read contains the amount of elements read from input files
		write_pipe(stream, temp, read_from_file);

	} while(!feof(fin) && !ferror(fin) && read_from_file > 0);//while no errors and end of file hasnt been reached
	fclose(fin);//close input so content in buffer can be succesfully written
	fclose (stream);
	close(pipe_write[1]);
}

int main_program(const char *input, const char *input2)
{
	int status;
	pid_t pid;
	pid_t pid2;

	pipe(child1);
	pipe(child2);

	pid = fork(); //creates a child

	if(pid == 0)//child
	{
		//close(child1[1]);
		puts("Child1 Process!");
		printf("CHILD1: my PID is %ld\n", (long) getpid());
		printf("CHILD1: I will now read the input file and write it to the pipe!.\n");
		read_write_from_file(input, child1);
	}

	else if(pid < 0)//if fork fails it returns -1, but just in case I accounted for everything
	{
		puts("Fork failed!!");
		fprintf(stderr, "Fork failed.\n");
		return EXIT_FAILURE;
	}

	else//parent
	{
		puts("Parent1 Process!");
		close(child1[1]);//close unused side
		read_pipe(child1);//child 1

		
		pid2 = fork();//create another child
		if(pid2 == 0)
		{
			//close(child1[1]);
			puts("Child2 Process!");
			printf("CHILD2: my PID is %ld\n", (long) getpid());
			printf("CHILD2: I will now read the input file and write it to the pipe!.\n");
			read_write_from_file(input2,child2);
		}

		else if(pid < 0)//if fork fails it returns -1, but just in case I accounted for everything
		{
			puts("Fork failed!!");
			fprintf(stderr, "Fork failed.\n");
			return EXIT_FAILURE;
		}

		else//parent 2
		{
			puts("Parent2 Process!");
			close(child2[1]);//close unused side
			read_pipe(child2);//child 2
		}
		
	}
	return EXIT_SUCCESS;

}

int main(int argc, char **argv)
{
	main_program(argv[1],argv[2]);
	return 0;
}
