#include "includes.h"
#include "BswDrv_Watchdog.h"


#define WATCH_DOG_ENABLE        0

void BswDrv_WatchDogInit(void) 
{	
#if WATCH_DOG_ENABLE
    /* enable IRC40K */
    rcu_osci_on(RCU_IRC40K);
    /* wait till IRC40K is ready */
    while(SUCCESS != rcu_osci_stab_wait(RCU_IRC40K));
 	/* confiure FWDGT counter clock: 40KHz(IRC40K) / 64 = 0.625 KHz */
    fwdgt_config(0xff0, FWDGT_PSC_DIV64);   //6S后会产生复位
    
    fwdgt_counter_reload();
    fwdgt_enable();
#endif
}

void BswDrv_FeedWatchDog(void)
{
#if WATCH_DOG_ENABLE
    fwdgt_counter_reload();
#endif
}

