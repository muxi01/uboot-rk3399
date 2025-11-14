/*
 * SPDX-License-Identifier:     GPL-2.0+
 *
 * (C) Copyright 2020 Rockchip Electronics Co., Ltd
 */

#include <common.h>
#include <dwc3-uboot.h>
#include <usb.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_USB_DWC3
static struct dwc3_device dwc3_device_data = {
	.maximum_speed = USB_SPEED_HIGH,
	.base = 0xfe500000,
	.dr_mode = USB_DR_MODE_PERIPHERAL,
	.index = 0,
	.dis_u2_susphy_quirk = 1,
	.usb2_phyif_utmi_width = 16,
};

int usb_gadget_handle_interrupts(void)
{
	dwc3_uboot_handle_interrupt(0);
	return 0;
}

int board_usb_init(int index, enum usb_init_type init)
{
	return dwc3_uboot_init(&dwc3_device_data);
}
#endif

#define SAMPLE_TIME 		300
#define SAMPLE_INTERVAL		10
int rockchip_is_key_pressed(const char *gpio_name)
{
	int gpio_bank,gpio_pin;
	int delay=0;
	int press_cnt=0,release_cnt=0;
	gpio_rockchip_get_gpio(gpio_name,&gpio_bank,&gpio_pin);
	gpio_rockchip_set_mux(gpio_bank,gpio_pin,0);
	gpio_rockchip_set_intput(gpio_bank,gpio_pin);
	gpio_rockchip_set_pull(gpio_bank,gpio_pin,3);
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
	if(release_cnt == 0)  {
		return 1;
	}
	else if((press_cnt > release_cnt)  && ((press_cnt / release_cnt) > 3)) {
		return 1;
	}
	return 0;
}

int rk_board_download(void)
{
	if(rockchip_is_key_pressed("GPIO1_A0")) {
		printf("go to fastboot mode\n");
	//	env_set("preboot", "setenv preboot; fastboot usb 0");
	//	run_command("fastboot usb 0", 0);
	}
	if(rockchip_is_key_pressed("GPIO3_A6")) {
		printf("go to download mode\n");
	//	env_set("preboot", "setenv preboot; download");
	}
	if(rockchip_is_key_pressed("GPIO1_B0")){
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
	return rk_board_download();
}

