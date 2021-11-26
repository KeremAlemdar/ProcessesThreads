
#include <stdlib.h>
#include <mqueue.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "shareddefs.h"
/* function declarations */
int getRequest(struct request *request, int *id);
int sendRequest(mqd_t *mq,struct request *request,int *id);
int getResult(mqd_t *mq, struct mq_attr *mq_attr, char* *bufptr, int *buflen, struct result* *resultptr);
int openMessageQueue(mqd_t *mq,char *name,int opentype);

int main ()
{	
	//REQUEST
	mqd_t mqctos;
	struct request request;
	//ANSWER
	mqd_t mqstoc;
	struct result *resultptr;
	struct mq_attr mq_attr;
	char *bufptr;
	int buflen;
	
	
	openMessageQueue(&mqstoc,MQSTOC,0);
	openMessageQueue(&mqctos,MQCTOS,1);
	int id = 0;
	while (1) {
		getRequest(&request,&id);
		sendRequest(&mqctos,&request,&id);
		if(strcmp(request.astr,"quit") == 0)
			break;
		getResult(&mqstoc,&mq_attr,&bufptr,&buflen,&resultptr);
	}
	if (id != 1)
		free(bufptr);
	mq_close(mqctos);
	mq_close(mqstoc);
	return 0;
}
int getRequest(struct request *request, int *id)
{
	char request_input[64];
	printf("Type the request you want\n");
	printf("Available requests are :\n");
	printf("count, count<start><end>\n");
	printf("avg, avg<start><end>\n");
	printf("max, range<start><end><K>\n");
	scanf("%[^\n]%*c",request_input);
	printf("---------------------------------------------------------\n");
	
	// fill struct request
	(*request).id = *id;	
	strcpy((*request).astr, request_input);
	return 1;
}
int sendRequest(mqd_t *mq,struct request *request,int *id)
{
	int n; // send verification variable
	
	// inputs (where, what, size)
	// send where : to mq(opened message queue)
	// send what : the item created &item
	// send size : sizeof the created struct item
	n = mq_send(*mq, (char *) *&request, sizeof(struct request), 0);
		
	if (n == -1) {
		perror ("mq_send failed\n");
		exit(1);
	}
		
	*id = *id + 1;
	return 1;
}
int getResult(mqd_t *mq, struct mq_attr *mq_attr, char* *bufptr, int *buflen, struct result* *resultptr)
{
	int m; // receive verification variable
	// RESULT WILL RECEIVED
	mq_getattr(*mq, &(*mq_attr));
	printf("mq maximum msgsize = %d\n", (int) (*mq_attr).mq_msgsize);
		
	/* allocate space for the buffet to store incoming message */
	*buflen = (*mq_attr).mq_msgsize;
	*bufptr = (char *) malloc(*buflen);
	m = mq_receive(*mq, (char *) *bufptr, *buflen, NULL);
	if (m == -1) {
		perror("mq_receive failed\n");
		exit(1);
	}
		
	*resultptr = (struct result *) *bufptr;
	
	//ANSWER IS NOW ARRAY
	printf("Result = \n");
	for(int i = 0; i < (*resultptr)->size; i++) {
	printf("%d\n", (*resultptr)->answer[i]);
	}
	printf("\n");
	
	return 1;
}
int openMessageQueue(mqd_t *mq,char *name,int opentype)
{
	//if(opentype == 0) {
		*mq = mq_open(name, O_RDWR);
		if (*mq == -1) {
			perror("can not open message queue\n");
			exit(1);
		}
		printf("%s message queue opened, mq id = %d\n", name, (int) *mq);// mq stores message queue id
	/*}
	if(opentype == 1) 
	{
		*mq = mq_open(name, O_RDWR | O_CREAT, 0666, NULL);
		if (*mq == -1) {
			perror("can not open message queue\n");
			exit(1);
		}
		printf("%s message queue opened, mq id = %d\n", name, (int) *mq);// mq stores message queue id
	}*/
	return 1;
}
	
