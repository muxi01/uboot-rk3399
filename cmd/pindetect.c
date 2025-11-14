// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2018, STMicroelectronics - All Rights Reserved
 */
#include <common.h>
#include <config.h>
#include <command.h>
#include <part.h>
#include <vsprintf.h>
#include <common.h>
#include <command.h>
#include <dm.h>
#include <errno.h>
#include <stdio.h>
#include <dm/pinctrl.h>
#include <dm/uclass-internal.h>
#include <asm/io.h>
#include <asm/gpio.h>
#include <asm/arch/gpio.h>

struct gpio_describe {
	const char *pGroup;
	const char *pPart;
	const char  pins[8];
};

struct gpio_info {
	char name[16];
	int  bank;
	int  pin;
};

#define  PIN_MIN		0xa0
#define  PIN_MAX		0xa7

#define  ON_LINE_0			(PIN_MIN + 0)
#define  ON_LINE_1			(PIN_MIN + 1)
#define  ON_LINE_2			(PIN_MIN + 2)
#define  ON_LINE_3			(PIN_MIN + 3)
#define  ON_LINE_4			(PIN_MIN + 4)
#define  ON_LINE_5			(PIN_MIN + 5)
#define  ON_LINE_6			(PIN_MIN + 6)
#define  ON_LINE_7			(PIN_MIN + 7)

#define  OF_LINE_0			(0)
#define  OF_LINE_1			(1)
#define  OF_LINE_2			(2)
#define  OF_LINE_3			(3)
#define  OF_LINE_4			(4)
#define  OF_LINE_5			(5)
#define  OF_LINE_6			(6)
#define  OF_LINE_7			(7)

static int get_serial_input(uint32_t timeout)
{
	unsigned long time_start;
	int input;
	time_start = get_timer(0);
	for(;;)
	{
		if(tstc()) {
			input =getc();
			if(input > 0)
				return input;
		}
		if(get_timer(time_start) > timeout){
			return -1;
		}
	}
}




#ifdef CONFIG_ROCKCHIP_RK3399
#define  GROUP_SIZE		18
static struct gpio_describe gpio_table[GROUP_SIZE]={

	{.pGroup="GPIO0",.pPart="A",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6,ON_LINE_7}},
	{.pGroup="GPIO0",.pPart="B",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5}},

	{.pGroup="GPIO1",.pPart="A",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6,ON_LINE_7}},
	{.pGroup="GPIO1",.pPart="B",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6,ON_LINE_7}},
	{.pGroup="GPIO1",.pPart="C",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6,ON_LINE_7}},
	{.pGroup="GPIO1",.pPart="D",.pins={ON_LINE_0}},

	{.pGroup="GPIO2",.pPart="A",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6,ON_LINE_7}},
	{.pGroup="GPIO2",.pPart="B",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4}},
	{.pGroup="GPIO2",.pPart="C",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6,ON_LINE_7}},
	{.pGroup="GPIO2",.pPart="D",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4}},

	{.pGroup="GPIO3",.pPart="A",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6,ON_LINE_7}},
	{.pGroup="GPIO3",.pPart="B",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6,ON_LINE_7}},
	{.pGroup="GPIO3",.pPart="C",.pins={ON_LINE_0,ON_LINE_1}},
	{.pGroup="GPIO3",.pPart="D",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6,ON_LINE_7}},

	{.pGroup="GPIO4",.pPart="A",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6,ON_LINE_7}},
	{.pGroup="GPIO4",.pPart="B",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5}},
	{.pGroup="GPIO4",.pPart="C",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6,ON_LINE_7}},
	{.pGroup="GPIO4",.pPart="D",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6}},
};
#else 
#define  GROUP_SIZE		12
static struct gpio_describe gpio_table[GROUP_SIZE]={
	{.pGroup="GPIO0",.pPart="A",.pins={ON_LINE_0,ON_LINE_1}},

	{.pGroup="GPIO1",.pPart="A",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6,ON_LINE_7}},
	{.pGroup="GPIO1",.pPart="B",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6,ON_LINE_7}},
	{.pGroup="GPIO1",.pPart="C",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6,ON_LINE_7}},
	{.pGroup="GPIO1",.pPart="D",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6,ON_LINE_7}},

	{.pGroup="GPIO2",.pPart="A",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6}},

	{.pGroup="GPIO3",.pPart="A",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6,ON_LINE_7}},
	{.pGroup="GPIO3",.pPart="B",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6,ON_LINE_7}},
	{.pGroup="GPIO3",.pPart="C",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3}},


	{.pGroup="GPIO4",.pPart="A",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6,ON_LINE_7}},
	{.pGroup="GPIO4",.pPart="B",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6,ON_LINE_7}},
	{.pGroup="GPIO4",.pPart="C",.pins={ON_LINE_0,ON_LINE_1,ON_LINE_2,ON_LINE_3,ON_LINE_4,ON_LINE_5,ON_LINE_6,ON_LINE_7}},
	// {.pGroup="GPIO4",.pPart="D",.pins={ON_LINE_0}},
};
#endif 
// int gpio_rockchip_set_mux(int bank, int pin, int mux);
// int gpio_rockchip_set_output(int bank, int pin, int mux);
// int gpio_rockchip_set_value(int bank, int pin, int mux);
// int gpio_rockchip_get_value(int bank, int pin);
// int gpio_rockchip_set_intput(int bank, int pin);


static struct gpio_info gpios[GROUP_SIZE*8];

static int create_gpio_map(void)
{
	struct gpio_describe *pdescribe;
	struct gpio_info *pgpio;
	int gpio_cnt=0;
	memset(gpios,0,sizeof(gpios));
	for(int i=0;i<GROUP_SIZE;i++) {
		pdescribe = &gpio_table[i];
		for(int k=0;k<8;k++){
			if((pdescribe->pins[k] < PIN_MIN) || (pdescribe->pins[k] > PIN_MAX)){
				continue;
			}
			pgpio=&gpios[gpio_cnt++];
			sprintf(pgpio->name,"%s_%s%d",pdescribe->pGroup,pdescribe->pPart,(pdescribe->pins[k] - PIN_MIN));
			gpio_rockchip_get_gpio(pgpio->name, &(pgpio->bank), &(pgpio->pin));
			printf("%s.%d: %s\n",__FUNCTION__,__LINE__,pgpio->name);
			gpio_rockchip_set_mux(pgpio->bank,pgpio->pin, 0);
			gpio_rockchip_set_output(pgpio->bank,pgpio->pin, 1);
		}
	}
	return gpio_cnt;
}

static int  tick =0;
static void simulate_set_bsp(int bsp)
{
	tick =(1000*1000 + (bsp / 2)) / bsp;
}

void inline simulate_uart_delay(int percent)
{
	__udelay(tick);
}

static void simulate_uart_send(char *str,int bank,int pin)
{
	char data;
	while(*str){
		data =*str++;
		gpio_rockchip_set_value(bank,pin, 0);
		simulate_uart_delay(10);
		for(int i=0;i<8;i++){
			gpio_rockchip_set_value(bank,pin,data & 0x01);
			simulate_uart_delay(10);
			data >>=1;
		}
		gpio_rockchip_set_value(bank,pin, 1);
		simulate_uart_delay(10);
	}
}

static int do_pin_detect(struct cmd_tbl *cmdtp, int flag, int argc,
		     char *const argv[])
{
	char io_name[16];
	int  gpio_cnt;
	unsigned long bps =4800;
	
	if(argc > 1) {
		bps = simple_strtoul(argv[1], NULL, 0);
	}

	gpio_cnt =create_gpio_map();
	simulate_set_bsp(bps);
	printf("show GPIO name on itself with bps=%ld\n",bps);
	while(get_serial_input(50) < 0){
		for(int i=0;i<gpio_cnt;i++){
			if(gpios[i].name[0] == 'G'){
				sprintf(io_name,"%s \n",gpios[i].name);
				simulate_uart_send(io_name,gpios[i].bank,gpios[i].pin);
			}
		}
	}
	return 0;
}


U_BOOT_CMD(pindetect, CONFIG_SYS_MAXARGS, 1, do_pin_detect,
	"the GPIO name is displaied on itself in UART at bsp 4800 [defalut] ",
	"\nexample: pindetect <bps>  - all pins name on itself gpio\n"
);
