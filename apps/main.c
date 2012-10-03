/*
 * main.c
 */

#include "frame802154.h"
#include "framer-802154.h"
#include "memb.h"
#include "netstack.h"
#include "lowpan.h"
#include "bsp.h"
#include "bsp_leds.h"
#include "bsp_buttons.h"

void main(void) {
	 int i;
	 netstack_init();
	 BSP_Init();
	 while(1)
	 {
		 BSP_TURN_OFF_LED1();
		 if(BSP_BUTTON1()||BSP_BUTTON2())
		 {
			 BSP_TURN_ON_LED1();
			 for(i=0;i<1000;i++)
				 BSP_DELAY_USECS(1000);
		 }
	 }
}
