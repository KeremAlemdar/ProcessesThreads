#include <stdlib.h>
#include <mqueue.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "shareddefs.h"



int getRequest(mqd_t *mq, struct mq_attr *mq_attr, char* *bufptr, int *buflen, struct request* *requestptr)
{
	int n; // recieve verification variable
	n = mq_receive(*mq, (char *) *bufptr, *buflen, NULL);
	if (n == -1) {
		perror("mq_receive failed\n");
		exit(1);
	}
	
	*requestptr = (struct request *) *bufptr;
	return 1;
}
int allocateSpace(mqd_t *mq, struct mq_attr *mq_attr, char* *bufptr, int *buflen)
{
	mq_getattr(*mq, &*mq_attr);
	printf("mq maximum msgsize = %d\n", (int) (*mq_attr).mq_msgsize);
	
	/* allocate large enough space for the buffer to store an incoming message */
	*buflen = (*mq_attr).mq_msgsize;
	*bufptr = (char *) malloc(*buflen);
	return 1;
}
int openMessageQueue(mqd_t *mq,char *name,int opentype)
{
		*mq = mq_open(name, O_RDWR | O_CREAT, 0666, NULL);
		if (*mq == -1) {
			perror("can not open message queue\n");
			exit(1);
		}
		printf("%s message queue opened, mq id = %d\n", name, (int) *mq);// mq stores message queue id
	//}
	return 1;
}
int executeSumCount(char* filename,int start, int end, int rangeExist, int operationType,int *answer, int *count_output) 
{
	int number = 0;
	FILE *file;
	int sum = 0;
	int count = 0;
	int max = INT_MIN;
	// r means read
	file = fopen(filename, "r");
	if (file == NULL) {
		perror("Error in file opening\n");
		exit(1);
	}
	fscanf(file, "%d\n", &number);
	if (operationType == 1 || operationType == 2) {
		while(!feof (file))
		{
			if(rangeExist == 1) {
				if(number >= start && number <= end) {
					count++;
					sum = sum + number;
				}
				fscanf(file, "%d", &number);
			}
			else {
				count++;
				sum = sum + number;
				fscanf(file, "%d", &number);
			}
		}
		
		if(operationType == 1)
			*answer = sum;
		if(operationType == 2)
			*answer = count;
	}
	else if (operationType == 3) {
		while(!feof (file))
		{
			if(max < number) {
				max = number;
			}
			count++;
			fscanf(file, "%d", &number);
		}
		*answer = max;
	}
	*count_output = count;
	
	
	fclose(file);
	return 1;
}

int executeRange(int start, int end, char k, char* filename,int *arr_output,int *count_output) 
{
	int number = 0;
	FILE *file;
	int count = 0;
	int* arr;
	int index = 0;
	int counter = 0;
	// r means read
	file = fopen(filename, "r");
	if (file == NULL) {
		perror("Error in file opening\n");
		exit(1);
	}
	fscanf(file, "%d\n", &number);
	while(!feof (file))
	{
		if(number >= start && number <= end) {
			count++;
		}
		fscanf(file, "%d", &number);
	}
	arr = (int*)malloc((k+1) * sizeof(int));
	for(int l = 0; l < k; l++) {
	arr[l] = 0;
	}
	rewind(file);
	fscanf(file, "%d\n", &number);
	while(!feof (file))
	{
		index = counter;
		if(number >= start && number <= end) {
			if (index == 0)
			{	
				arr[0] = number;
				counter++;
			}
			else
			{
				if(arr[0] < number) {
					for(int m = counter-1; m >= 0; m--) {
						if(m < k) {
							arr[m+1] = arr[m];
						}
					}
					arr[0] = number;
					counter++;
				}
				else if(arr[0] > number) {
					for(int m = 1; m < counter+1; m++) {
						if(arr[m] == 0) {
							arr[m] = number;
							counter++;
							break;
						}
						if(arr[m] < number) {
							for(int j = counter-1; j > m; j--) {
								if(j < k) {
									arr[j] = arr[j+1];
								}
								arr[m] = number;
								counter++;
							}
						}
						if(arr[m] > number) {
						}	
						if(arr[m] == number) {
							break;
						}
					}
				}
				else {
				}
			}
		}
		fscanf(file, "%d", &number);
	}
	for(int i = 0; i < counter; i++){
		*arr_output = arr[i];
		arr_output++;
	}
	*count_output = (k < counter) ? k : counter;
	fclose(file);
	free(arr);
	return 1;
}
int execute(int numberOfString, char* substrings[], char* filename, int *answer_arr, int size)
{
	int answer_size = 0;
	int answer = -1;
	int operationType;
	int count = -1;
	int arr[1003];
	if (numberOfString == 1) 
	{
		if(strcmp(substrings[0],"avg") == 0) 
		{
			operationType = 1;

		}
		else if(strcmp(substrings[0],"count") == 0)
		{
			operationType = 2;
		}
		else if(strcmp(substrings[0],"max") == 0)
		{
			operationType = 3;
		}
		executeSumCount(filename,0,0,0,operationType,&answer,&count);
	}
	else if (numberOfString == 3)
	{
		if(strcmp(substrings[0],"avg") == 0)
		{
			operationType = 1;
		}
		else if(strcmp(substrings[0],"count") == 0)
		{
			operationType = 2;
		}
		executeSumCount(filename,atoi(substrings[1]),atoi(substrings[2]),1,operationType,&answer,&count);
	}
	else if (numberOfString == 4) 
	{
		if(strcmp(substrings[0],"range") == 0)
		{
			executeRange(atoi(substrings[1]),atoi(substrings[2]),atoi(substrings[3]),filename,arr,&count);
			operationType = 4;
			answer_size = atoi(substrings[3]);
		}
	}
	else
	{
	}
	*answer_arr = operationType; // [0]
	answer_arr++;
	*answer_arr = answer_size; // [1]
	answer_arr++;
	*answer_arr = answer; // [2]
	answer_arr++;
	*answer_arr = count; // [3]
	if(numberOfString == 4) {
		for(int j = 0; j < answer_size; j++) {
			answer_arr++;
			*answer_arr = arr[j];
		}
		answer_arr++;
		*answer_arr = -1;
	}
	else {
		answer_arr++;
		*answer_arr = -1; // [4]
	}
	return 1;
}
int calculateResult(struct result *result, struct request* *requestptr, int argc, char* argv[], int *id) 
{
	(*result).id = *id;
	// SPLITTING
	char* str = (*requestptr)->astr;
	char* substrings[4];
	int i = 0;
	
	char* currentWord = strtok (str, " ");
	while (currentWord != NULL)
	{
		substrings[i++] = currentWord;
		currentWord = strtok(NULL, " ");
	}
	int numberOfString = i;
	pid_t n; // stores process ids
	int fd[atoi(argv[1])][2];
	for(int pipeNumber = 0; pipeNumber < atoi(argv[1]); pipeNumber++)
	{
		if (pipe(fd[pipeNumber]) < 0)
		{
		printf("could not create pipe\n");
		exit (1);
		}
	}
	
	int pipe_results[10][1003];
	for(int childNumber = 0; childNumber < atoi(argv[1]); childNumber++) 
	{
		int answer_arr[1003];
		n = fork();
		if (n == 0) 
		{
			close (fd[childNumber][0]); // close the read end
			int sent_byte;
			execute(numberOfString, substrings, argv[childNumber+2],answer_arr,1003);
			for(int i = 0; i < answer_arr[1]+4; i++) {
				sent_byte = answer_arr[i];
				write (fd[childNumber][1], &sent_byte, sizeof(int));
			}
			close(fd[childNumber][1]);
			exit(0);
		}
		else 
		{
			close (fd[childNumber][1]); // close the read end
			
			int recv_byte;
			for(int i = 0; i < atoi(argv[1]); i++) {
				int j = 0;
				while (read(fd[childNumber][0], &recv_byte, sizeof(int)) > 0)
				{
					pipe_results[childNumber][j] = recv_byte;
					j++;
				}
				fflush (stdout);
				close(fd[childNumber][0]);
			}
			for(int i = 0; i < atoi(argv[1]); i++) {
				for(int j = 0; j < 4; j++) {
				}
				for(int j = 0; j < pipe_results[i][1]; j++) {
				}
			}
		}

	}
	for(int i = 0; i < atoi(argv[1]); i++)
		wait(NULL);
	// CALCULATE ALL CHILDS ANSWERS
	int sum = 0;
	int count = 0;
	int max = 0;
	int operationType = 0;
	
	for(int i = 0; i < atoi(argv[1]); i++)
	{

		if(pipe_results[i][0] == 1) 
		{
			sum = sum + pipe_results[i][2];
			count = count + pipe_results[i][3];
			operationType = 1;	
		}
		if(pipe_results[i][0] == 2) 
		{
			count = count + pipe_results[i][3];
			operationType = 2;
		}
		if(pipe_results[i][0] == 3) 
		{
			if(max < pipe_results[i][2]) {
				max = pipe_results[i][2];
			}
			operationType = 3;
		}
		if(pipe_results[i][0] == 4) 
		{
			operationType = 4;
		}
	}
	if(operationType == 4) {
		int general_ans[1000];
		int general_ans_reverse[1000];
		int i = 0;
		int counter[10] = {0,0,0,0,0,0,0,0,0,0};
		int counter_max[10] = {0,0,0,0,0,0,0,0,0,0};
		int sum_count = 0;
		int index = 0;
		int max = 0;
		for(int m = 0; m < atoi(argv[1]); m++) {
			counter_max[m] = pipe_results[m][3];
			sum_count = sum_count + pipe_results[m][3];
		}
		printf("SUM COUNT = %d,\n",sum_count);
		while (i != sum_count && i < pipe_results[0][1]) {
			index = 0;
			max = INT_MIN;
			int numberOfThemEqualToMin = 0;
			for(index = 0; index < atoi(argv[1]); index++) {
				if(max < pipe_results[index][counter[index]+4] && counter[index] != counter_max[index]) {
					max = pipe_results[index][counter[index]+4];
				}
			}
			for(index = 0; index < atoi(argv[1]); index++) {
				if(max == pipe_results[index][counter[index]+4]) {
					counter[index]++;
					numberOfThemEqualToMin++;
					if(numberOfThemEqualToMin > 1) {
					sum_count--;
					}
				}
			}
			general_ans[i] = max;
			i++;
		}
		for(int m = 0; m < i; m++) {
			general_ans_reverse[i-m-1] = general_ans[m];
		}
		for(int j = 0; j < i; j++) {
			(*result).answer[j] = general_ans_reverse[j];
		}
		(*result).size = i;
	}
	if(operationType == 1) 
	{
		(*result).answer[0] = sum/count;
		(*result).size = 1;
	}
	if(operationType == 2) 
	{
		(*result).answer[0] = count;
		(*result).size = 1;
	}
	if(operationType == 3) 
	{
		(*result).answer[0] = max;
		(*result).size = 1;
	}
	return 1;
}

int sendResult(mqd_t *mq, struct result *result, int *id)
{
	int m; // send verification
	m = mq_send(*mq, (char *) &*result, sizeof(struct result), 0);
	
	if (m == -1) {
		perror ("mq_send failed\n");
		exit(1);
	}
	
	id++;
	return 1;
}
int main(int argc, char* argv[])
{
	// REQUEST
	mqd_t mqctos;
	struct mq_attr mq_attr;
	struct request *requestptr;
	char *bufptr;
	int buflen;
	// ANSWER
	mqd_t mqstoc;
	struct result result;
	

	openMessageQueue(&mqstoc,MQSTOC,1);
	openMessageQueue(&mqctos,MQCTOS,0);
	allocateSpace(&mqctos,&mq_attr,&bufptr,&buflen);
	int id = 0;
	while (1) {
		getRequest(&mqctos,&mq_attr,&bufptr,&buflen,&requestptr);
		if(strcmp(requestptr->astr,"quit") == 0)
			break;
		calculateResult(&result, &requestptr, argc, argv, &id);
		sendResult(&mqstoc,&result,&id);
		sleep(1);
	}
	free(bufptr);
	mq_close(mqctos);
	mq_close(mqstoc);
	return 0;
}
