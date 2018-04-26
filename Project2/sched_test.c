#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <sched.h>
#include <getopt.h>
#include <ctype.h>
#include <limits.h>

#include <sys/time.h>
#include <sys/resource.h>
//#include <sys/syscall.h>

#define SCHED_NORMAL 0
#define SCHED_FIFO 1

#define SYS_weighted_rr_getquantum 337
#define SYS_weighted_rr_setquantum 338

#define START_CHAR 97

struct thread_args {
  int tid;
  int prio;
  char mychar;
};

int sched_policy, quantum, old_quantum, num_threads, buffer_size;
int total_num_chars;
char *val_buf;
int val_buf_pos = 0;
pthread_t *threads;

void fail(char* msg)
{
    printf("%s\n", msg);
    exit(-1);
}

void *run(void *arg) 
{
    int i;
    struct thread_args *my_args = (struct thread_args*) arg;
    
    //+ write characters to the val_bufs
    // nchars xxx
    for(i = 0; i<3; i++)
    {
        printf("Thread %c is running\n", my_args->mychar);
        clock_t start = clock();
        while ((clock() - start) / CLOCKS_PER_SEC < 0.5);
    }

    free(my_args);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    struct sched_param param;
    struct thread_args *targs;
    pthread_attr_t attr;
    int i;
    char cur;
    
    if( argc > 2 )
        fail("Invalid arguments count");
    
    sched_policy = SCHED_NORMAL;
    if( argc == 2 ) {
        if( strcmp(argv[1], "SCHED_FIFO") == 0 )
            sched_policy = SCHED_FIFO;
        else
            fail("Invalid scheduling policy");
    }

    if (sched_policy == SCHED_FIFO)
    {
        printf ("Setting policy to SCHED_FIFO\n");
        param.sched_priority = sched_get_priority_min(sched_policy);
        if ( sched_setscheduler(getpid(), sched_policy, &param) == -1)
        {
            perror("sched_setscheduler");
            fail("sched_setscheduler fail");
        };
    }
    
    num_threads = 2;
    
    //+ create and start each thread
    if ( (threads = malloc(num_threads*sizeof(pthread_t))) == NULL )
        fail("malloc(num_threads) fail");
        
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for (i = 0; i < num_threads; i++)
    {
        targs = malloc(sizeof(*targs));
        targs->tid    = i;
        targs->prio   = 1;
        targs->mychar = i + 1 + '0';

        printf ("Thread %i was created\n", i + 1);
        pthread_create(&threads[i], &attr, run, (void *)targs);
    }

    //+ wait for all threads to complete
    for (i = 0; i < num_threads; i++) 
    {
        pthread_join(threads[i], NULL);
    }

    //+ clean up and exit
    pthread_attr_destroy(&attr);
    pthread_exit (NULL);
}
