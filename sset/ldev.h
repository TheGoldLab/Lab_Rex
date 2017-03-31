/*
 *-----------------------------------------------------------------------*
 * NOTICE:  This code was developed by the US Government.  The original
 * versions, REX 1.0-3.12, were developed for the pdp11 architecture and
 * are in the public domain.  This version, REX 4.0, is a port of the
 * original version to the Intel 80x86 architecture.  This version is
 * copyright (C) 1992 by the National Institutes of Health, Laboratory
 * of Sensorimotor Research, Bldg 10 Rm 10C101, 9000 Rockville Pike,
 * Bethesda, MD, 20892, (301) 496-9375.  All rights reserved.
 *-----------------------------------------------------------------------*
 */

/*
 * This header contains the device definitions that are specific
 * to each laboratory. 
 *
 * Updated 6/6/03 by jig for new lab settings
 */

/*
 *-----------------------------------------------------------------------*
 *		    Laboratory Specific Device Definitions
\ *-----------------------------------------------------------------------*
 */

/* in case rigs are different... */
#define RIG1 1
#define RIG2 2

/* Shadlen lab Fancy TTL  */
#define FTTL1			Dio_id(PCDIO_DIO, 4, 0x1)
#define FTTL2			Dio_id(PCDIO_DIO, 4, 0x2)
#define FTTL1_2		Dio_id(PCDIO_DIO, 4, 0x3)
#define FTTL3			Dio_id(PCDIO_DIO, 4, 0x4)
#define FTTL4			Dio_id(PCDIO_DIO, 4, 0x8)
#define FTTL5			Dio_id(PCDIO_DIO, 4, 0x10)
#define FTTL6			Dio_id(PCDIO_DIO, 4, 0x20)
#define FTTL7			Dio_id(PCDIO_DIO, 4, 0x40)
#define FTTL8			Dio_id(PCDIO_DIO, 4, 0x80)

/* REWARD + BEEP */
#define REW			FTTL2
#define REWBEEP	FTTL2
#define BEEP		FTTL3

/* Joystick button */
#define JOYBUT		Dio_id(PCDIO_DIO, 6, 0x1)

/*
 * GRASS electrical stimulator
 */
#define ELECTR1	    Dio_id(PCDIO_DIO, 4, 0x1)


/* 
** OLD STUFF 
**
** 
** Valve and Beeper for reward.  The second reward is a switch
** definition that we check to go to the second reward state.
** 6/6/96 defined DIMSWITCH as the 7th switch on port 0.  Para 502st3 
** (and subsequent paras) use this switch to override the value of 
** of the dim target luminance so that the target is simply turned off.  
** This allows the experimenter to switch between overlap and memory saccades
** by the flip of a switch
**
** #define REW2SWITCH 	Dio_id(PCDIO_DIO, 0, 0x80)
** #define DIMSWITCH		Dio_id(PCDIO_DIO, 0, 0x40)
** #define TAG0SWITCH	Dio_id(PCDIO_DIO, 0, 0x10)
**
**
**
** LED
**
** #define LED1	    Dio_id(PCDIO_DIO, 2, 0x01)
** #define LED2	    Dio_id(PCDIO_DIO, 2, 0x02)
** #define LED3	    Dio_id(PCDIO_DIO, 2, 0x04)
** #define LED4	    Dio_id(PCDIO_DIO, 2, 0x08)

** 
** Lamps for Backlight in Lab-g
**
**
** #define BACKLT	    Dio_id(PCDIO_DIO, 5, 0x08)
** #define BACKLT3	 Dio_id(PCDIO_DIO, 5, 0x20)
** 
** * 
** * Slide Projector
** *
** 
** #define PROJ	    Dio_id(PCDIO_DIO, 5, 0x10)
** 
*/
