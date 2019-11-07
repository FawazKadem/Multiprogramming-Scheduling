#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

/*
Three queues implemented with LL per input line.
One of these queues (one representing SJF) is a priority queue

Algorithm
- For each queue given in input file:
  - Create three queues for FCFS, SJF, and RR
  - Extract time quantum and queue number
  - For each process in the queue in input file:
      - Enqueue the process to FCFS and RR
      - SortedEnqueue the process to SJF
  - For each queue
        - Dequeue them while tracking wait times
        - For RR, enqueue node again if not finished after TQ time.
        - Print results to output file and to screen
*/

// Implement queue with LL

//LL
struct node
{
    int data[2]; //first is count second is time
    struct node *next;
};
typedef struct node node;

// Queue
struct queue
{
    int count;
    node *front;
    node *rear;
};
typedef struct queue queue;

void initialize(queue *q)
{
    q->count = 0;
    q->front = NULL;
    q->rear = NULL;
}
int getCount(queue *q){
  return q->count;
}
int isempty(queue *q)
{
    return getCount(q) == 0;
}

void enqueue(queue *q, int pCount, int pTime)
{
    node *tmp;
    tmp = malloc(sizeof(node));
    tmp->data[0] = pCount;
    tmp->data[1] = pTime;
    tmp->next = NULL;
    if(!isempty(q))
    {
        q->rear->next = tmp;
        q->rear = tmp;
    }
    else
    {
        q->front = q->rear = tmp;
    }
    q->count++;
}

int * dequeue(queue *q)
{
    node *tmp;
    static int n[2];
    memcpy(n, q->front->data, sizeof(n));
    tmp = q->front;
    q->front = q->front->next;
    q->count--;
    free(tmp);
    return(n);
}



void sortedEnqueue(queue *q, int pCount, int pTime) {
  if (isempty(q)) {
    enqueue(q, pCount, pTime);
    return;
  }

  node* head = q->front;
  node* tmp = malloc(sizeof(node));
  tmp->data[0] = pCount;
  tmp->data[1] = pTime;

  if (head->data[1] > pTime) {
    tmp->next = (q->front);
    q->front = tmp;

  }
  else {
    while (head->next != NULL && head->next->data[1] < pTime) {
      head = head->next;
    }

    tmp->next = head->next;
    head->next = tmp;
  }
  q->count++;
}

//Program start
int main()
{
// queue *test;
// test = malloc(sizeof(queue));
// initialize(test);
// sortedEnqueue(test,2,12);
// sortedEnqueue(test,2,11);
// int *info = dequeue(test);
// printf("%d    %d\n",*info, *(info + 1));
//
// info = dequeue(test);
// printf("%d    %d\n",*info, *(info + 1));


FILE *ifp, *ofp;
ifp = fopen ("cpu_scheduling_input_file.txt","r");
ofp = fopen ("cpu_scheduling_output_file.txt","w");
int queueNum = 0;
if (ifp!=NULL && ofp != NULL)
{

  char singleLine[1024];
  fgets(singleLine, 1024, ifp);
  while(!feof(ifp)) {

    // Part a - Create queues, assign TQ, populate queues with processes
    int tq;
    int numProcesses;

    queue *fcfs;
    queue *sjf;
    queue *rr;

    fcfs = malloc(sizeof(queue));
    sjf = malloc(sizeof(queue));
    rr = malloc(sizeof(queue));

    initialize(fcfs);
    initialize(sjf);
    initialize(rr);


    char* split = strtok(singleLine, " ");
    split = strtok(NULL, " ");

    int queueNum = *split - '0';
    //printf("%d\n", queueNum);

    split = strtok(NULL, " ");
    split = strtok(NULL, " ");

    tq = *split - '0';
    //printf("%d\n", tq);

    split = strtok(NULL, " ");
    split = strtok(NULL, " ");

    int pCount = 0;
    int pTime;
    while(split != NULL) {
      pCount++;

      sscanf(split, "%d", &pTime);
      //printf("P Count: %d\n", pCount);
      //printf("P Time: %d\n", pTime);
      split = strtok(NULL, " ");
      split = strtok(NULL, " ");
      enqueue(fcfs, pCount, pTime);
      enqueue(rr, pCount, pTime);
      sortedEnqueue(sjf, pCount, pTime);

    }

    float totalWaitTime = 0.0;
    int runningWaitTime = 0;
    numProcesses = getCount(fcfs);
    int *info;

    char orderString[2048];
    strcpy(orderString, "Order of selection by CPU:\n");

    char waitString[2048];
    strcpy(waitString, "Individual waiting times for each process:\n");

    char* toAdd;
    // Part b - Run FCFS, SJF, RR scheduling algorithms on queues
    //dequeue fcfs
    while(!isempty(fcfs)) {
      info  = dequeue(fcfs);
      asprintf(&toAdd, "p%d ", *(info));
      strcat(orderString, toAdd);
      //printf("%s\n", orderString);

      totalWaitTime += runningWaitTime;
      asprintf(&toAdd, "p%d = %d\n", *(info), runningWaitTime);
      strcat(waitString, toAdd);
      runningWaitTime += *(info+1);

    }
    printf("Ready Queue %d Applying FCFS Scheduling:\n\n", queueNum);
    printf("%s\n\n%s\nAverage waiting time = %.1f\n\n", orderString, waitString, totalWaitTime/numProcesses);
    fprintf(ofp, "Ready Queue %d Applying FCFS Scheduling:\n\n", queueNum);
    fprintf(ofp, "%s\n\n%s\nAverage waiting time = %.1f\n\n", orderString, waitString, totalWaitTime/numProcesses);


    //reset and dequeue sjf
    strcpy(orderString, "Order of selection by CPU:\n");
    strcpy(waitString, "Individual waiting times for each process:\n");
    totalWaitTime=0;
    runningWaitTime=0;
    while(!isempty(sjf)) {
      info  = dequeue(sjf);
      asprintf(&toAdd, "p%d ", *(info));
      strcat(orderString, toAdd);
      //printf("%s\n", orderString);

      totalWaitTime += runningWaitTime;
      asprintf(&toAdd, "p%d = %d\n", *(info), runningWaitTime);
      strcat(waitString, toAdd);
      runningWaitTime += *(info+1);

    }
    printf("Ready Queue %d Applying SJF Scheduling:\n\n", queueNum);
    printf("%s\n\n%s\nAverage waiting time = %.1f\n\n", orderString, waitString, totalWaitTime/numProcesses);
    fprintf(ofp, "Ready Queue %d Applying SJF Scheduling:\n\n", queueNum);
    fprintf(ofp, "%s\n\n%s\nAverage waiting time = %.1f\n\n", orderString, waitString, totalWaitTime/numProcesses);


    //reset and dequeue RR
    strcpy(orderString, "Order of selection by CPU:\n");
    strcpy(waitString, "Turnaround times for each process:\n");
    runningWaitTime=0;
    int startTimes[numProcesses];

    for(int i = 0; i < numProcesses;i++) {
      startTimes[i] = -1;

    }

    while(!isempty(rr)) {
      info  = dequeue(rr);
      asprintf(&toAdd, "p%d ", *(info));
      strcat(orderString, toAdd);
      //printf("%s\n", orderString);
      if (startTimes[*info-1] < 0) {
        startTimes[*info-1] = runningWaitTime;
      }

      if (*(info+1) - tq > 0) {
        enqueue(rr, *(info), *(info+1)-tq);
        runningWaitTime+= tq;
      } else {
        runningWaitTime+= *(info+1);
        asprintf(&toAdd, "p%d = %d\n", *info, runningWaitTime - startTimes[*info-1]);
        strcat(waitString, toAdd);
      }

    }
    printf("Ready Queue %d Applying RR Scheduling:\n\n", queueNum);
    printf("%s\n\n%s\n", orderString, waitString);
    fprintf(ofp,"Ready Queue %d Applying RR Scheduling:\n\n", queueNum);
    fprintf(ofp,"%s\n\n%s\n", orderString, waitString);

    fgets(singleLine, 1024, ifp);

  }
  //fscanf(ifp,"Some String\n", &var);
  fclose (ifp);
  fclose (ofp);
}



return 0;

}
