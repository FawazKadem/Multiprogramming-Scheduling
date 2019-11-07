#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

/*

*/
int x, y, z;
void *calc_sum(void *num) {
  z = x + y;
  return NULL;
}

int main()
{
    pid_t pid;
    pthread_t calcThread;

    x = 10;
    y = 20;
    z = 0;


    pid = fork();

    if (pid<0)
    {
      perror("Fork unsuccessful");
      exit(-1);
    }

    if (pid>0)
    {
      wait(NULL); //wait for child to perform z = x + y
      printf("%d\n", z); //Expected 0 b/c child doesn't share data with main
      //create thread
      if (pthread_create(&calcThread, NULL, calc_sum, NULL)) {
        printf("Thread creation unsuccessful\n");
        exit(-1);
      }
      pthread_join(calcThread, NULL);
      printf("%d\n", z); //Expceted: 30 b/c of data sharing between thread & main


    } else
    {
      //child
      z = x + y;


    }
return 0;

}
