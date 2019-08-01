/****************************************************************************
 * (C) 2019-2020 - DSLab @ Iran University of Science and Technology
 ****************************************************************************
 *
 *      File: vmsplice-splice/main.c
 *      Authors: Amir Hossein Sorouri - Sina Mahmoodi
 *
 * Description: 
 */

#define _GNU_SOURCE /* See feature_test_macros(7) */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include "../vmsplice.h"

#define SPLICERS_ROW_SIZE     ((1ull) << 10)
#define SPLICERS_COL_SIZE     ((1ull) << 20)

const unsigned long int memSize = ( SPLICERS_ROW_SIZE * SPLICERS_COL_SIZE );


/*
 * Lorem Ipsum
 */
static int ipcb_parse_option(int argc, char *argv[])
{
        int opt, index = 1, vmSpliceFlag;

        while ((opt = getopt(argc, argv, ":gu")) != -1) {
                switch (opt) {
                        case 'u':
                                vmSpliceFlag = SPLICE_F_MOVE;
                                index++;
                                break;
                        case 'g':
                                printf("Gift\n");
                                vmSpliceFlag = SPLICE_F_GIFT;
                                index++;
                                break;
                        default:
                                return -1;
                }
        }
    	for(; optind <= argc; optind++){
        	printf("extra arguments: %s\n", argv[optind-1]);
    	}
        return vmSpliceFlag;
}


/*
 *  Lorem Ipsum
 */
int
main() {
        ssize_t nread;
        char* name;
	struct timeval start, end;
        int pip[2], fd[2], childPipe[2];
	pid_t   childOnePid;

	if (pipe(childPipe) < 0 || pipe(pip) < 0 || pipe(fd) < 0)
                exit(1);
        char** data = ipcb_fake_data_generator(SPLICERS_ROW_SIZE, SPLICERS_COL_SIZE);

        childOnePid = ipcb_fork();
        if(childOnePid == 0)
        {
                /* First Child process closes up input side of pipe. */
                name = "First Child";
                close(pip[0]);
                close(fd[0]);

                ipcb_get_time(&start, "\nfirst_child:start: "); /* Start. */

                nread = ipcb_vmsplicer(pip[1], data, SPLICERS_ROW_SIZE, SPLICERS_COL_SIZE, SPLICE_F_GIFT);
                
		ipcb_get_time(&end, "\nfirst_child:end: ");  /* End. */
               
                printf("---------------------------------------------\n");
                write(fd[1], &start, sizeof(start));
                write(fd[1], &end, sizeof(start));
                exit(0);
        }
        else
        {
        	pid_t childTwoPid;
        	childTwoPid = ipcb_fork();
	        if(childTwoPid == 0)
            	{
            		/* Second Child process closes up output side of pipe */
            		name = "Second Child";
           		close(fd[1]);
            		close(pip[1]);
            		close(childPipe[0]);

            		ipcb_get_time(&start, "\nsecond_child:start: "); /* Start. */

            		nread = ipcb_splicer(pip[0], STDOUT_FILENO, SPLICERS_ROW_SIZE, SPLICERS_COL_SIZE, SPLICE_F_MOVE);
            		
			ipcb_get_time(&end, "\nsecond_child:end: ");  /* End. */

            		printf("in %s: number of reads from the pipe = %ld\n", name, nread);
            		printf("---------------------------------------------\n");
            		struct timeval firstChildStart, firstChildEnd;
            		
            		read(fd[0], &firstChildStart, sizeof(start));
            		read(fd[0], &firstChildEnd, sizeof(start));
                        
			double result = printf("\nWriting Data into memory is done.\n");
	                printf("Time in microseconds: %ld microseconds\n",
        	                ((end.tv_sec - firstChildStart.tv_sec)*1000000L
                	        +end.tv_usec) - firstChildStart.tv_usec
                        	); // Added semicolon
                	printf("The frequency is eqals to(Mbps): %f .\n", (double)((2* memSize)/(result*1000000L)));

                        exit(0);
        	}
        	else {
            		/* Parent process closes up output side of pipe */
			name  = "Parent Process";
            		int childOneStatus, childTwoStatus;

            		waitpid(childOnePid, &childOneStatus, 0);
            		waitpid(childTwoPid, &childTwoStatus, 0);
            		
			if (childOneStatus == 0 && childTwoStatus == 0)  // Verify child process terminated without error.
            		{
                		printf("The child processes terminated normally.\n");
            		}
            		else{printf("The child process terminated with an error!.\n");}
                        
			ipcb_free_memory(data, SPLICERS_ROW_SIZE);
            		exit(0);
        	}
    	}
    	return 0;
}
