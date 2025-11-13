#ifndef __LOGTRACE_H__
#define __LOGTRACE_H__

#define NULL ((void *)0)

typedef int (*fastboot_write_str)(const char *string);

int logtrace_init(void) ;
int logtrace_push(char data) ;
int logtrace_pull(char *dest, int max_len);
int logtrace_print(fastboot_write_str fastboot_write);

typedef struct {
    char *buf;
    int buf_size;
    int w_ptr;
    int r_ptr;
    int is_full;
    unsigned int is_init;
}logtrace_t;


#endif

