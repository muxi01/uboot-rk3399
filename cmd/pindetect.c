// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2018, STMicroelectronics - All Rights Reserved
 */
#include <common.h>
#include <config.h>
#include <command.h>
#include <part.h>
#include <vsprintf.h>

#define LIMIT_DEVNAME	30
#define PIN_DETECT_ON	1

#if PIN_DETECT_ON

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
	int  part;
	int  number;
};

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


#define  GROUP_SIZE		20

/*skip of mmc2 gpio: 
	GPIO1_B[4..7] GPIO1_C[0..5]  GPIO1_C7

  skip of uart2 for debug:
	GPIO1_D[5..6] GPIO0_D[0..1]
*/
static struct gpio_describe all_gpio[GROUP_SIZE]={

	{.pGroup="GPIO0",.pPart="A",.pins={'0','1',2,3,4,5,'6','7'}},

	{.pGroup="GPIO0",.pPart="B",.pins={0,1,2,3,4,5,6,7}},
	{.pGroup="GPIO0",.pPart="C",.pins={0,1,2,3,4,5,6,7}},
	{.pGroup="GPIO0",.pPart="D",.pins={'0','1',2,3,4,5,6,7}},


	{.pGroup="GPIO1",.pPart="A",.pins={'0','1',2,3,4,5,6,7}},
	{.pGroup="GPIO1",.pPart="B",.pins={0,1,2,3,'4','5','6','7'}},
	{.pGroup="GPIO1",.pPart="C",.pins={'0','1','2','3','4','5',6,'7'}},
	{.pGroup="GPIO1",.pPart="D",.pins={0,1,2,3,4,'5','6',7}},

	{.pGroup="GPIO2",.pPart="A",.pins={0,1,2,3,4,5,6,7}},
	{.pGroup="GPIO2",.pPart="B",.pins={0,1,2,3,4,5,6,7}},
	{.pGroup="GPIO2",.pPart="C",.pins={0,1,2,3,4,5,6,7}},
	{.pGroup="GPIO2",.pPart="D",.pins={0,1,2,3,4,5,6,7}},

	{.pGroup="GPIO3",.pPart="A",.pins={0,1,2,3,4,5,6,7}},
	{.pGroup="GPIO3",.pPart="B",.pins={0,1,2,3,4,5,6,7}},
	{.pGroup="GPIO3",.pPart="C",.pins={0,1,2,3,4,5,6,7}},
	{.pGroup="GPIO3",.pPart="D",.pins={0,1,2,3,4,5,6,7}},

	{.pGroup="GPIO4",.pPart="A",.pins={0,1,2,3,4,5,6,7}},
	{.pGroup="GPIO4",.pPart="B",.pins={0,1,2,3,4,5,6,7}},
	{.pGroup="GPIO4",.pPart="C",.pins={0,1,2,3,4,5,6,7}},
	{.pGroup="GPIO4",.pPart="D",.pins={0,1,2,3,4,5,6,7}},

};


int gpio_rk3xxx_set_mux(int bank, int pin, int mux);
int gpio_rk3xxx_set_output(int bank, int pin, int mux);
int gpio_rk3xxx_set_value(int bank, int pin, int mux);
int gpio_rk3xxx_get_value(int bank, int pin);
int gpio_rk3xxx_set_intput(int bank, int pin);


static int  count=0;
static int  tick =0;
static struct gpio_info gpios[GROUP_SIZE*8];


static void get_gpio_by_name(const char *name,int *part,int *number)
{
	//GPIO0_A1
	*part =name[4]-'0';
	*number =(name[6]- 'A') * 8 + (name[7] - '0');
}

static void create_gpio_map(void)
{
	struct gpio_describe *pdescribe;
	struct gpio_info *pgpio;

	count=0;
	memset(gpios,0,sizeof(gpios));
	for(int i=0;i<GROUP_SIZE;i++)
	{
		pdescribe = &all_gpio[i];
		for(int k=0;k<8;k++){
			if((pdescribe->pins[k] < 0) || (pdescribe->pins[k] > 7)){
				continue;
			}
			pgpio=&gpios[count++];
			sprintf(pgpio->name,"%s_%s%d",pdescribe->pGroup,pdescribe->pPart,pdescribe->pins[k]);
			get_gpio_by_name(pgpio->name, &(pgpio->part), &(pgpio->number));
			printf("%s.%d: %s\n",__FUNCTION__,__LINE__,pgpio->name);
			gpio_rk3xxx_set_mux(pgpio->part,pgpio->number, 0);
			gpio_rk3xxx_set_output(pgpio->part,pgpio->number, 1);
		}
	}
}

static void simulate_set_bsp(int bsp)
{
	tick =(1000*1000 + (bsp / 2)) / bsp;
}

void inline simulate_uart_delay(int percent)
{
	__udelay(tick);
}

static void simulate_uart_send(char *str,int part,int number)
{
	char data;
	while(*str){
		data =*str++;
		gpio_rk3xxx_set_value(part,number, 0);
		simulate_uart_delay(10);
		for(int i=0;i<8;i++){
			gpio_rk3xxx_set_value(part,number,data & 0x01);
			simulate_uart_delay(10);
			data >>=1;
		}
		gpio_rk3xxx_set_value(part,number, 1);	//stop
		simulate_uart_delay(10);
	}
}

static int do_pin_detect(struct cmd_tbl *cmdtp, int flag, int argc,
		     char *const argv[])
{
	char io_name[16];
	int triger=0;
	unsigned long bps =4800;
	
	if(argc > 1) {
		bps = simple_strtoul(argv[1], NULL, 0);
	}
	printf("show GPIO name on itself bps=%ld\n",bps);
	create_gpio_map();
	simulate_set_bsp(bps);
	while(get_serial_input(50) < 0){
		for(int i=0;i<count;i++){
			if(gpios[i].name[0] == 'G'){
				sprintf(io_name,"%s \n",gpios[i].name);
				// gpio_rk3xxx_set_value(gpios[i].part,gpios[i].number,triger & 0x01);
				simulate_uart_send(io_name,gpios[i].part,gpios[i].number);
				// printf("%d.%s   : part %d  number %d\n",i,gpios[i].name,gpios[i].part,gpios[i].number);
			}
		}
		triger++;
	}
	return 0;
}

#else
static int do_pin_detect(struct cmd_tbl_t *cmdtp, int flag, int argc,
		     char *const argv[])
{
	return 0;
}
#endif

U_BOOT_CMD(pindetect, CONFIG_SYS_MAXARGS, 1, do_pin_detect,
	"the GPIO name is displaied on itself in UART at bsp 4800 [defalut] ",
	"\nexample: pindetect <bps>  - all pins name on itself gpio\n"
);
