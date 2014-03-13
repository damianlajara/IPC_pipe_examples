#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>//strlen

ssize_t read_from_file;
ssize_t read_from_pipe_c1;
ssize_t read_from_pipe_c2;
ssize_t written_to_pipe;

int child1[2];
int child2[2];

struct header{
   int id;
};

	void print_child1(FILE *stream_c1, char *read_buffer_c1, char *buf_c1, struct header *head/*, int *pipe_read_c1*/)
	{
		head->id = 1;
		read_from_pipe_c1 = fread(read_buffer_c1,1, 100, stream_c1);
		buf_c1 = (char *)malloc(read_from_pipe_c1*sizeof(char));
		strncpy(buf_c1,read_buffer_c1,read_from_pipe_c1);

		fprintf(stdout,"\nChild %d: %s",head->id,buf_c1);
		free(buf_c1);
	}

	void print_child2(FILE *stream_c2, char *read_buffer_c2, char *buf_c2, struct header *head/*, int *pipe_read_c2*/)
	{
		head->id =2;
		read_from_pipe_c2 = fread(read_buffer_c2,1, 100, stream_c2);//
		buf_c2 = (char *)malloc(read_from_pipe_c2*sizeof(char));
        strncpy(buf_c2,read_buffer_c2,read_from_pipe_c2);	

        fprintf(stdout,"\nChild %d: %s",head->id,buf_c2);
        free(buf_c2);
	}
    void read_pipe(int *pipe_read_c1, int *pipe_read_c2)
{
	struct header *head;
	char read_buffer_c1[1024];//temp to hold everything from pipe
	char read_buffer_c2[1024];

	int file_c1 = pipe_read_c1[0];
	int file_c2 = pipe_read_c2[0];

	FILE *stream_c1;
	FILE *stream_c2;

	char *buf_c1;
	char *buf_c2;

	head = (header *)malloc(sizeof(struct header));
	stream_c1 = fdopen (file_c1, "r");
	stream_c2 = fdopen (file_c2, "r");
	printf("\n");
	do //As soon as one of the pipes either reaches an error or reaches the end, the loop will stop
	{
		print_child1(stream_c1, read_buffer_c1, buf_c1, head);
		print_child2(stream_c2, read_buffer_c2, buf_c2, head);

	} while(!feof(stream_c1) && !ferror(stream_c1) && read_from_pipe_c1 > 0 && !feof(stream_c2) && !ferror(stream_c2) && read_from_pipe_c2 > 0);

	//if parent has not finished reading pipe for child1, then continue reading from it
	if(!feof(stream_c1) && !ferror(stream_c1) && read_from_pipe_c1 > 0)
	{
		printf("\nParent finished reading from Child2...will now continue to read from child1");
		while(!feof(stream_c1) && !ferror(stream_c1) && read_from_pipe_c1 > 0)
		{
			print_child1(stream_c1, read_buffer_c1, buf_c1, head);
		} 
	}
	//if parent has not finished reading pipe for child2, then continue reading from it
	else if(!feof(stream_c2) && !ferror(stream_c2) && read_from_pipe_c2 > 0)//or this one
	{
		printf("\nParent finished reading from Child1...will now continue to read from child2");
		while(!feof(stream_c2) && !ferror(stream_c2) && read_from_pipe_c2 > 0)
		{
			print_child2(stream_c2, read_buffer_c2, buf_c2, head);
		}
	}

	fclose(stream_c1);
	fclose(stream_c2);
}

void write_pipe(FILE *stream, char *temp,int bytes_read, long pid)
{
    int wrote = fwrite(temp,1, bytes_read, stream);//write to stream
    fprintf(stdout,"Child in write pipe with id: %ld wrote %d bytes\n",pid, wrote);/*DEBUG CODE*/
}

void read_write_from_file(const char *input, int *pipe_write, long pid)//char
{
	char temp[100];
	FILE* fin;//declare a FILE pointer pointing to the input file
	fin = fopen(input, "r");//opens file for reading purposes
	FILE *stream;
	stream = fdopen(pipe_write[1], "w");
	do
	{
		read_from_file = fread(temp,1, 100, fin);//read contains the amount of elements read from input files
		write_pipe(stream, temp, read_from_file, pid);

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

	if(pid == 0)//child 1
	{
		printf("CHILD1: my PID is %ld\n", (long) getpid());
		read_write_from_file(input, child1, (long) getpid());
	}

	else if(pid < 0)//if fork fails it returns -1, but just in case I accounted for everything
	{
		puts("Fork failed!!");
		fprintf(stderr, "Fork failed.\n");
		return EXIT_FAILURE;
	}

	else//parent
	{
		puts("\nParent Process!");
		close(child1[1]);//close unused side

		pid2 = fork();//create another child

		if(pid2 == 0)//child 2
		{
			printf("CHILD2: my PID is %ld\n", (long) getpid());
			read_write_from_file(input2, child2, (long) getpid());
		}

		else if(pid < 0)//if fork fails it returns -1, but just in case I accounted for everything
		{
			puts("Fork failed!!");
			fprintf(stderr, "Fork failed.\n");
			return EXIT_FAILURE;
		}

		else//parent
		{
			close(child2[1]);//close unused side
			read_pipe(child1, child2);
		}
	}
	return EXIT_SUCCESS;

}

int main(int argc, char **argv)
{
	main_program(argv[1],argv[2]);
	return 0;
}