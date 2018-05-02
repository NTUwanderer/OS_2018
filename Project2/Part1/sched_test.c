#define _GNU_SOURCE
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
    cpu_set_t  mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    sched_setaffinity(0, sizeof(mask), &mask);

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
        param.sched_priority = sched_get_priority_max(sched_policy);
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
        
    for (i = 0; i < num_threads; i++)
    {
        targs = malloc(sizeof(*targs));
        targs->mychar = i + 1 + '0';

        pthread_attr_t attr;
        pthread_attr_init(&attr);

        struct sched_param param;
        // increasing order
        if (sched_policy == SCHED_FIFO)
            param.sched_priority = sched_get_priority_max(SCHED_FIFO) - i;
            // param.sched_priority = sched_get_priority_min(SCHED_FIFO) + i;
        else
            param.sched_priority = 0;

        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&attr, sched_policy);
        pthread_attr_setschedparam(&attr, &param);

        printf ("Thread %i was created\n", i + 1);
        pthread_create(&threads[i], &attr, run, (void *)targs);
        pthread_attr_destroy(&attr);
    }

    //+ wait for all threads to complete
    for (i = 0; i < num_threads; i++) 
    {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    //+ clean up and exit
    pthread_attr_destroy(&attr);
    pthread_exit (NULL);
}
