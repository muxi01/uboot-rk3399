/*
* Copyright (C) 2016 The Android Open Source Project
*
* SPDX-License-Identifier: BSD-2-Clause
*/

#include <common.h>
#include <fastboot.h>
#ifdef CONFIG_UDP_FUNCTION_FASTBOOT
#include <net/fastboot.h>
#endif

#define LOG_PRINT(fmt,...)  // pr_err(fmt,##__VA_ARGS__)


void fastboot_fail(const char *reason, char *response)
{
	const char *fail_str = "FAIL";
	strncpy(response, fail_str, FASTBOOT_RESPONSE_LEN);
	strncat(response, reason, FASTBOOT_RESPONSE_LEN - strlen(fail_str) - 1);
}

void fastboot_okay(const char *reason, char *response)
{
	const char *okay_str = "OKAY";
	strncpy(response, okay_str, FASTBOOT_RESPONSE_LEN);
	strncat(response, reason, FASTBOOT_RESPONSE_LEN - strlen(okay_str) - 1);
}

void timed_send_info(ulong *start, const char *msg)
{
#ifdef CONFIG_UDP_FUNCTION_FASTBOOT
	/* Initialize timer */
	if (*start == 0) {
		*start = get_timer(0);
	}
	ulong time = get_timer(*start);
	/* Send INFO packet to host every 30 seconds */
	if (time >= 30000) {
		*start = get_timer(0);
		fastboot_send_info(msg);
	}
#endif
}


int fb_copy_images(const char * part, void *buffer, unsigned int  len)
{
	uint32_t img_addr;
	if(fastboot_oem_mode == OEM_MODE_DTB)
	{
		img_addr =env_get_hex("fdt_addr_r", 0); 
		if(img_addr ){
			memcpy((void *)img_addr,buffer,len);
			printf("\ndownloading of %d bytes to debug_dtb 0x%08x\n",len,img_addr);
			return 0;
		}
	}
	if(fastboot_oem_mode == OEM_MODE_KERNEL)
	{
		img_addr =env_get_hex("kernel_addr_r", 0);
		if(img_addr ){
			memcpy((void *)img_addr,buffer,len);
			printf("\ndownloading of %d bytes to debug_kernel 0x%08x\n",len,img_addr);
			return 0;
		} else {
			printf("\nthe partition(%s) is not exist or it's address(%d) less than %d at environment\n",part,img_addr,CONFIG_FASTBOOT_BUF_ADDR);
		}
	}
	if(fastboot_oem_mode == OEM_MODE_INITRD)
	{
		img_addr =env_get_hex("ramdisk_addr_r", 0);
		if(img_addr ){
			memcpy((void *)img_addr,buffer,len);
			printf("\ndownloading of %d bytes to debug_kernel 0x%08x\n",len,img_addr);
			return 0;
		} else {
			printf("\nthe partition(%s) is not exist or it's address(%d) less than %d at environment\n",part,img_addr,CONFIG_FASTBOOT_BUF_ADDR);
		}
	}
	
	return -1;
}
