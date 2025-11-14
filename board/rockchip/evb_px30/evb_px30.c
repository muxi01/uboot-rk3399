/*
 * (C) Copyright 2017 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <asm/io.h>

DECLARE_GLOBAL_DATA_PTR;
#define SAMPLE_TIME 		300
#define SAMPLE_INTERVAL		10


int rockchip_is_key_pressed(const char *gpio_name)
{
	int gpio_bank,gpio_pin;
	int delay=0;
	int press_cnt=0,release_cnt=0;
	gpio_rockchip_get_gpio(gpio_name,&gpio_bank,&gpio_pin);
	// gpio_rockchip_set_mux(gpio_bank,gpio_pin,0);
	gpio_rockchip_set_intput(gpio_bank,gpio_pin);
	// gpio_rockchip_set_pull(gpio_bank,gpio_pin,3);

	for(delay=0;delay<SAMPLE_TIME;delay+=SAMPLE_INTERVAL) {
		__udelay(1000 * SAMPLE_INTERVAL);
		if(0 == gpio_rockchip_get_value(gpio_bank,gpio_pin)) {
			press_cnt++;
		}
		else {
			release_cnt++;
		}
	}
	printf("%s: press cnt:%d release cnt:%d\n",gpio_name,press_cnt,release_cnt);
	release_cnt++;
	if((press_cnt > release_cnt)  && ((press_cnt / release_cnt) > 3)) {
		return 1;
	}
	return 0;
}


int rk_board_download(void)
{
	if(rockchip_is_key_pressed("GPIO0_B7")) {
		printf("go to fastboot mode\n");
	//	env_set("preboot", "setenv preboot; fastboot usb 0");
	//	run_command("fastboot usb 0", 0);
	}
	else if(rockchip_is_key_pressed("GPIO1_C2")) {
		printf("go to download mode\n");
	//	env_set("preboot", "setenv preboot; download");
	}
	else if(rockchip_is_key_pressed("GPIO1_B0")){
		printf("go to maskrom mode\n");
	//	writel(BOOT_BROM_DOWNLOAD, CONFIG_ROCKCHIP_BOOT_MODE_REG);
	//	do_reset(NULL, 0, 0, NULL);
	//	while(1);
	}
	return 0;
}
EXPORT_SYMBOL(rk_board_download);


int rk_board_late_init(void)
{
	return 0;
}

