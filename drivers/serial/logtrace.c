#include <logtrace.h>
#include <common.h>
#include <command.h>
#include <amp.h>
#include <asm/byteorder.h>
#include <mapmem.h>
#include <asm/bootm.h>
#include <asm/secure.h>
#include <linux/compiler.h>


#define LOGTRACE_INIT_FLAG 0X12345678

static logtrace_t logtrace = {
    .buf = NULL,
    .buf_size = 0,
    .is_full = 0,
    .is_init = 0,
    .r_ptr = 0,
    .w_ptr = 0
};

logtrace_t *logtrace_get(void)
{
    return &logtrace;
}

int logtrace_push(char data) 
{
    logtrace_t *plog_trace = logtrace_get();
    if (LOGTRACE_INIT_FLAG != plog_trace->is_init)
        return -1;

    plog_trace->buf[plog_trace->w_ptr] = data;
    if (++plog_trace->w_ptr >= plog_trace->buf_size)
        plog_trace->w_ptr = 0;

    if (plog_trace->w_ptr == plog_trace->r_ptr) {
        plog_trace->is_full = 1;
        if (++plog_trace->r_ptr >= plog_trace->buf_size)
            plog_trace->r_ptr = 0;
    }

    return 0;
}

int logtrace_init(void) 
{
    uint32_t log_addr;
    uint32_t log_size;
    logtrace_t *plog_trace = logtrace_get();
    if(LOGTRACE_INIT_FLAG == plog_trace->is_init) {
        printf("logtrace init again\n");
    }
    log_addr =env_get_hex("log_addr_r", 0);
    log_size =env_get_hex("log_size_r", 0);
    if((log_addr > 0) && (log_size > 0)) {
        plog_trace->buf = (char *)(log_addr);
        plog_trace->buf_size = log_size;
        plog_trace->w_ptr = 0;
        plog_trace->r_ptr = 0;
        plog_trace->is_full = 0;
        plog_trace->is_init = LOGTRACE_INIT_FLAG;
        printf("logtrace init \n");
    }
    return 0;
}


int logtrace_pull(char *dest, int max_len) 
{
    logtrace_t *plog_trace = logtrace_get();
    if ((LOGTRACE_INIT_FLAG != plog_trace->is_init) || dest == NULL || max_len == 0){
        printf("logtrace pull failed 0x%x \n",plog_trace->is_init);
        return -1;
    }
    
    int read_len = 0;
    while (read_len < max_len && !((plog_trace->r_ptr == plog_trace->w_ptr) && !plog_trace->is_full)) {
        dest[read_len++] = plog_trace->buf[plog_trace->r_ptr];
        if (++plog_trace->r_ptr >= plog_trace->buf_size)
            plog_trace->r_ptr = 0;
        if (plog_trace->is_full)
            plog_trace->is_full = 0;
        if((plog_trace->buf[plog_trace->r_ptr] == '\n') || (plog_trace->buf[plog_trace->r_ptr] == '\r')){
            break;
        }
    }
    return (int)read_len;
}

int logtrace_print(fastboot_write_str fastboot_write)
{
    int result=0,slen=0,lines;
    char fb_buff[64];
    for(lines=0;lines<100;lines++) {
        strcpy(fb_buff,"INFO");
        result =logtrace_pull(&fb_buff[4], 50);
        if(result < 0) {
            slen =-1;
            break; 
        } 
        else if (result > 0) {
            fb_buff[5+ result] = '\0';
            if(fastboot_write !=NULL) {
                fastboot_write(fb_buff);
            }
            slen +=result;
        }
        else {
            break;
        }
    }
    return slen;
}
