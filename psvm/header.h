#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdatomic.h>
#include <sys/mman.h>       // mmap
#include <fcntl.h>
#include <errno.h>          // errno
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

struct Data {
  atomic_int state;
};

typedef struct main_inputs {
        pid_t pid;
        void *remote_ptr;
        ssize_t buffer_length;
} data_input;

#define DATA_SIZE (sizeof(struct Data))

#endif  /* PROTOCOL_H */

#define two			2
#define col			1024
#define meg_row			1024
#define gig_row			1048576
#define two_gig_row		(long long int)(two * gig_row)
#define four_gig_row		(two*two_gig_row)
#define lli			(long long int)
#define gig_size		(gig_row*col)
#define two_gig_size    	(two*gig_size)
#define four_gig_size		(two*two_gig_size)
#define eight_gig_size  	(two*four_gig_size)
#define shm_writer_file		"Write_finish"
#define shm_reader_file		"Read_finish"
#define shm_file_creat_mod	(O_CREAT | O_RDWR)
#define shm_file_use_mod	(O_RDWR)
#define shm_prov_prot		(PROT_READ | PROT_WRITE) 	// Provider is the 2nd program to run
#define shm_prov_flags		MAP_SHARED			// Provider is the 2nd program to run
#define shm_cons_prot						// Provider is the first program to run
#define shm_cons_flags						// Provider is the first program to run
#define psvm_writer		"process_vm_writev"
#define psvm_reader		"process_vm_readv"

void set_cpu_scheduler (int cpu_no, int priority);

int psvm_error_handler (ssize_t nread2);

void get_inputs (data_input *input_var, int argc, char **argv);

void* shm_builder (int file_mode, int prot, int flags, char *shm_file_name);

void print_results (char* psvm, ssize_t nread, struct timespec start, struct timespec finish);