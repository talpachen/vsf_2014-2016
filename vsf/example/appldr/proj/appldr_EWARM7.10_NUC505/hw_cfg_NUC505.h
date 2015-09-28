/**************************************************************************
 *  Copyright (C) 2008 - 2012 by Simon Qian                               *
 *  SimonQian@SimonQian.com                                               *
 *                                                                        *
 *  Project:    Versaloon                                                 *
 *  File:       hw_cfg_VSFCore_STM32.h                                    *
 *  Author:     SimonQian                                                 *
 *  Versaion:   See changelog                                             *
 *  Purpose:    hardware configuration file for VSFCore_STM32             *
 *  License:    See license                                               *
 *------------------------------------------------------------------------*
 *  Change Log:                                                           *
 *      YYYY-MM-DD:     What(by Who)                                      *
 *      2008-11-07:     created(by SimonQian)                             *
 *      2008-11-22:     rewrite GPIO_Dir(by SimonQian)                    *
 **************************************************************************/

#define APP_MAIN_ADDR					0

// USB
#define USB_PULLUP_PORT					0xFF
#define USB_PULLUP_PIN					0

#define KEY_PORT						IFS_DUMMY_PORT
#define KEY_PIN							0

// bcm wifi
#define BCM_PORT						BCM_BUS_TYPE_SPI
#define BCM_SPI_PORT					1
#define BCM_SPI_CS_PORT					1
#define BCM_SPI_CS_PIN					10
#define BCM_SPI_FREQ					9000
#define BCM_RST_PORT					2
#define BCM_RST_PIN						14
#define BCM_EINT_PORT					2
#define BCM_EINT_PIN					5
#define BCM_EINT						0x52

#define BCM_PWRCTRL_PORT				IFS_DUMMY_PORT
#define BCM_PWRCTRL_PIN					0

//#define BCM_CFG_FWADDR					0x00020000

// LED24
#define LED24							{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},\
										{IFS_DUMMY_PORT, 0, IFS_DUMMY_PORT, 0},