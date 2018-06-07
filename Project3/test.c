#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include "test.h"

#define PAGE_SIZE (getpagesize()/sizeof(int))

extern int index_array[];

int main()
{
    struct rusage usage;
    struct stat buf;
    int file_size;

    long max_rss;
    long major_page_fault;
    long minor_page_fault;

    /* test program starts */
    system("./syslog.sh page fault test program starts !");


    /* open a big file for page fault testing */
    int fd = open("input.log", O_RDONLY);
    if( fd == -1 )
    {
        perror("failed to open input.log\n");
        exit(EXIT_FAILURE);
    }
    /* get the file size */
    fstat(fd, &buf);
    file_size = buf.st_size;

    /* map the big file into process address space*/
    int *map = mmap(0, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
        close(fd);
        perror("Error mmapping the input file");
        exit(EXIT_FAILURE);
    }

    /* access the big file */
    int i;
    for (i = 0; i < INDEX_LENGTH; ++i) {
        printf("%d\n", map[ index_array[i] * PAGE_SIZE] ); 
    }
    
    /* unmap the big file */
    if (munmap(map, file_size) == -1) {
        perror("Error un-mmapping the input file");
    }


    close(fd);
    /*  test program ends  */
    system("./syslog.sh page fault test program ends !");

    /* runtime statistics */
    getrusage(RUSAGE_SELF, &usage);
    printf("# of major pagefault: %ld\n", usage.ru_majflt);
    printf("# of minor pagefault: %ld\n", usage.ru_minflt);
    printf("# of resident set size: %ld KB\n", usage.ru_maxrss);

    return 0;
}
