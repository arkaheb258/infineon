/*
 * cal_conf.h - CANopen Library Configuration
 *
 * Copyright (C) 1998-2013  by port GmbH  Halle(Saale)/Germany
 *
 *------------------------------------------------------------
 * This file was generated by the CANopen Design Tool V2.2.53.0
 * on 2015/01/21 at 10:42:00.
 */

/**
 * \file cal_conf.h
 * \author port GmbH
 *
 * This module contains the configuration
 * of the CANopen Library for the CANopen device.
 */



#ifndef CAL_CONF_H
#define CAL_CONF_H	1
#define CO_CONFIG_DESIGNTOOL_VERSION 0x0202

/* active configuration : 0 */
#define CO_CONFIG_USE_TARGET_0 1


/*
 * General Settings
 */
#define CONFIG_CAN_ERROR_HANDLING      	1
#define CONFIG_SLAVE                   	1
#define CO_LIB_INIT_VAR			
#define CO_DT_INIT_VAR			
#define CO_LIB_UNINIT_VAR		
#define CONFIG_CAN_OBJECTS             	11

/*
 * Hardware configuration 0: 0
 */


/*
 * Code Maturity Level Options
 */


/*
 * CPU Setup
 */
#ifdef CO_CONFIG_USE_TARGET_0
#define CONFIG_COLIB_TIMER                  	1
#define CONFIG_CPU_FAMILY_XE166             	1
#define CONFIG_CPU_TYPE_XE164               	1
#define CONFIG_TIMER_INC                    	10
#define CONFIG_TIMER_ISR_NUMBER             	_interrupt(0x21)
#define CONFIG_TIMER_ISR_REGISTERBANK       	/* */

# ifdef DEF_HW_PART
#  include <cpu_xe166.h>
# endif /* DEF_HW_PART */

#endif /* CO_CONFIG_USE_TARGET_0*/


/*
 * CAN Controller Setup
 */
#ifdef CO_CONFIG_USE_TARGET_0
#define CONFIG_CAN_FAMILY_MULTICAN          	1
#define CONFIG_COLIB_BUFFER                 	1
#define CONFIG_COLIB_FLUSHMBOX              	1
#define CONFIG_FLUSHMBOX_READ_MAXMSG        	10
#define CONFIG_RX_BUFFER_SIZE               	10
#define CONFIG_TX_BUFFER_SIZE               	10
#define CONFIG_CAN_START_TYPE               	1
#define CONFIG_CAN_ACCESS_MEM_MAP           	1
#define CONFIG_CAN_ISR_NUMBER               	_interrupt(0x40)
#define CONFIG_CAN_ISR_REGISTERBANK         	/* */
#define CONFIG_CAN_MULTICAN_NODE            	1
#define CONFIG_CAN_REGISTER_OFFSET          	1
#define CONFIG_CAN_TYPE_XE164               	1
#define CONFIG_CAN_T_CLK                    	33
#define CONFIG_CAN_USE_DIRECTTRANSFER       	1
#define CONFIG_STANDARD_IDENTIFIER          	1
#define CONFIG_CAN_ADDR                     	{(void _huge *) 0x200000ul}
#endif /* CO_CONFIG_USE_TARGET_0*/


/*
 * Compiler Setup
 */
#ifdef CO_CONFIG_USE_TARGET_0
#define CONFIG_ALIGNMENT                    	2
#define CONFIG_COMPILER_TASKING_C166        	1
#  include <co_tasking.h>
#endif /* CO_CONFIG_USE_TARGET_0 */




/*
 * CANopen Services
 */
#define CONFIG_HEARTBEAT_CONSUMER      	2
#define CONFIG_HEARTBEAT_PRODUCER      	1
#define CONFIG_MAPPING_CNT             	5
#define CONFIG_NON_VOLATILE_MEM        	1
#define CONFIG_PDO_EVENTTIMER          	1
#define CONFIG_PDO_PRODUCER            	2
#define CONFIG_RPDO_MAPPING_CNT        	0
#define CONFIG_SDO_SERVER              	1
#define CONFIG_SEG_SDO                 	1
#define CONFIG_SYNC_CONSUMER           	1
#define CONFIG_TIME_CONSUMER           	1
#define CONFIG_TPDO_MAPPING_CNT        	5
/*
 * Additional CANopen Settings
 */
#define CONFIG_CONST_OBJDIR            	1
#define CONFIG_CO_ERR_LED              	1
#define CONFIG_CO_LED                  	1
#define CONFIG_CO_RUN_LED              	1
#define CONFIG_USER_CAN_MSG            	1

/* 
 * special DEFINE part - don't edit here, it is changed by the DesignTool 
 */
#define CO_CONFIG_SPECIAL_START	1
#include <dave\XE16xxREGS.H>

#define HW_KPS	0
#define HW_UPT	1
#define HW_UZS	0

#define IEN PSW_IEN
#define CONFIG_NO_PRINTF 1

#include <co_stru.h>
#ifdef DEF_OBJ_DIC
typedef struct {
    UNSIGNED8	numOfEntries;	/* number of entries */
    UNSIGNED32	map[3];	/* mapping entries */
} STORAGE3_T;
#endif


#define CO_CONFIG_SPECIAL_END	1

/* 
 * user specific part - will not be changed by the DesignTool
 */
#define CO_CONFIG_USER_SPEC_START	1

#define CO_CONFIG_USER_SPEC_END	1

#endif /* CAL_CONF_H */
