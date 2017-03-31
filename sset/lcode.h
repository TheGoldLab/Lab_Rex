/*
 *-----------------------------------------------------------------------*
 * NOTICE:  This code was developed by the US Government.  The original
 * versions, REX 1.0-3.12, were developed for the pdp11 architecture and
 * distributed without restrictions.  This version, REX 4.0, is a port of
 * the original version to the Intel 80x86 architecture.  This version is
 * distributed only under license agreement from the National Institutes 
 * of Health, Laboratory of Sensorimotor Research, Bldg 10 Rm 10C101, 
 * 9000 Rockville Pike, Bethesda, MD, 20892, (301) 496-9375.
 *-----------------------------------------------------------------------*
 */

/************* START OF STUFF LOCAL TO NEWSOME LAB **************/

/* I have added the parts of our old lcode.h here
 * MNS 9/14/93
 *
 * organization/version control (CVS checkin) Huk, JD, MM 04/02
 */

#define EYEB	(EYEH & EYEV)

/*
 * Ecode defines for handshaking.
 */
#define FAILCD	8191

/* index codes for parameters.  This allows all parameters
 * to use the same ecode space 7000-7999
 * but to be identified uniquely by the immediately
 * preceding 8000 ecode.
 * Try to keep these the same between paradigms in 600
 * series at least -- even though some codes are not
 * used.  This way we can use these same defines in
 * spikes.
 */
#define HEADCD			8000	/* marks beginning of a series
							 		* and should be followed by a series
							 		* of index 8000+n ECODE pairs.
							 		*/
#define I_FIXXCD		8001	/* fix x */
#define I_FIXYCD		8002	/* fix y */
#define I_STXCD		8003	/* stim center x */
#define I_STYCD		8004	/* stim center y */
#define I_STDIACD		8005	/* stim (aperture) diameter */
#define I_SPDCD		8006	/* speed */
#define I_VNVCD		8007	/* var novar */
#define I_TRG1XCD		8008	/* target iposition X */
#define I_TRG1YCD		8009	/* targ position Y */
#define I_DOTDIRCD	8010	/* pref dir */
#define I_COHCD		8011	/* correlation */
#define I_TRG2XCD		8012	/* target 2 X */
#define I_TRG2YCD		8013	/* target 2 Y */
#define I_SEEDCD		8014	/* seed range */
#define CHMADECD		8015	/* code to indicate choice was made */
#define EYEHLDCD		8016	/* code to indicate eye was held in wind. */
#define I_TRIALIDCD	8017	/* trial id number */
#define I_STIMDUR		8018	/* stimulus duration, new to para 502  */
#define I_DWCD			8019	/* DataWaves interrupt codes */
#define I_QXCD			8020	/* Attention Q */
#define I_QYCD			8021
#define I_GOXCD		8022	/* Go target */
#define I_GOYCD		8023
#define I_GOLUMCD		8024  /* luminance of Go target */
#define I_TRG1LCD		8025	/* Target 1 luminance */
#define I_TRG2LCD		8026	/* Target 2 luminance */
#define I_DTVARCD		8027	/* Random seed for dots7 	*/
#define I_DTNOVARCD	8028	/* NOVAR seed for dots7		*/

/* huk added the following, 040902 */
/* the following are for the 65* series of paradigms, MEM */
#define I_SEEDACD		8029
#define I_SEEDBCD		8030
#define I_STAIRCD		8031
#define I_FIXLCD		8032		/* FP luminance (or color); added by jd 11/21/02 for 730 */
#define I_TASKIDCD	8033
#define I_REWCONTCD	8034		/* reward contingency 0, 1 or 2, added Long 2-22-2007	*/
#define I_STOPSIGD	8035
#define I_REWLENCD	8036		/* reward length in ms , added bss 8-20-07 */

/* jig and mn added 4/22/10 */
#define I_FIXDCD     8037     /* fixation diameter */
#define I_CTGDCD     8038     /* choice target diameter */
#define I_CTGLCD     8039     /* choice target luminance */
#define I_PRNT       8040     /* Prob Rew task, num targets */
#define I_PRTI       8041     /* Prob Rew task, target i */
#define I_PRTD       8042     /* Prob Rew task, target distance */
#define I_PRTHR      8043     /* Prob Rew task, hazard rate */
#define I_PRTANG     8044     /* Prob Rew task, angle of max reward target */
#define I_PRTSTD     8045     /* Prob Rew task, std of reward distribution */
#define I_PRTTCHC    8046     /* Prob Rew task, chosen target */
#define I_PRTTREW    8047     /* Prob Rew task, rewarded target */

/*target probabilities for probabilistic reward task, mn added 1/18/11 */
#define I_PRTP0      8048     /* Prob Rew task, prob of best target */
#define I_PRTP1      8049     /* Prob Rew task, prob of 2nd best target */
#define I_PRTP2      8050     /* Prob Rew task, prob of 3rd best target */
#define I_PRTP3      8051     /* Prob Rew task, prob of 4th best target */
#define I_PRTP4      8052     /* Prob Rew task, prob of 5th best target */
#define I_PRTP5      8053     /* Prob Rew task, prob of 6th best target */
#define I_PRTP6      8054     /* Prob Rew task, prob of 7th best target */
#define I_PRTP7      8055     /* Prob Rew task, prob of 8th best target */
#define I_PRTP8      8056     /* Prob Rew task, prob of 9th best target */
#define I_PRTP9       8057    /* Prob Rew task, prob of 10th best target */
#define I_PRTPA       8058    /* Prob Rew task, prob of 11th best target */

/* jig and yl changed 3/24/14
**
** OLD:
** #define I_TESTIDCD   8037     * test type in dotsj2 task, added by TD, 2013-06-20 *
** #define I_RATE	      8038		* Index for the rate of motion change, adaption to motion change task, added by TD, 2014-01-29 *
** #define I_ANGLEO	   8039		* TD, 2014-02-18 *
** #define I_ANGLED1	   8040
** #define I_ANGLED2	   8041	
**
** NEW:
*/
#define I_TESTIDCD   8059     /* test type in dotsj2 task, added by TD, 2013-06-20 */
#define I_RATE	      8060		/* Index for the rate of motion change, adaption to motion change task, added by TD, 2014-01-29 */
#define I_ANGLEO	   8061		/* TD, 2014-02-18 */
#define I_ANGLED1	   8062
#define I_ANGLED2	   8063

/* added by jig 11/10/15 */
#define I_RATE1		8064
#define I_RATE2		8065
#define I_RATEFLAG1	8066
#define I_RATEFLAG2	8067

/* Added by jig 11/11/16 from rig1:
**
** OLD VALUES:
** #define I_DOTDENSITYCD	8040	/* ADDED BY LD 2015-07-08 */
** #define I_ESTIMCD			8037	/* estim type 0-5, added Long 09-07-2010	*/
** #define I_ESTIMCORCD 	8038  /* stim_corr_choice in estimPostSacTD.d, added TD 06-21-2012 */
** #define I_ESTIMDIRCD 	8039  /* stim_motion_dir  in estimPostSacTD.d, added TD 06-21-2012 */
*/
#define I_DOTDENSITYCD	8068		/* ADDED BY LD 2015-07-08 */
#define I_ESTIMCD			8069		/* estim type 0-5, added Long 09-07-2010	*/
#define I_ESTIMCORCD 	8070     /* stim_corr_choice in estimPostSacTD.d, added TD 06-21-2012 */
#define I_ESTIMDIRCD 	8071     /* stim_motion_dir  in estimPostSacTD.d, added TD 06-21-2012 */

/* special event codes for dot paradigms (lifted from 660.d) */
#define GORANDCD	4900		/* turn on dots */
#define GOCOHCD 	4901		/* turn on coherence */
#define ENDCD		4902		/* turn off dots */
#define ALLONCD	4903
#define ALLOFFCD	4904
#define CORRECTCD	4905
#define WRONGCD   4906
#define NOCHCD		4907
#define REW1CD		4908
#define LOOPCD		4909
#define PREFCD		4910
#define NULLCD		4911
/* not sure what this was at stanford... but at UW it is... */
#define ASKOFFCD	4912
#define FIX1CD 	4913  /* added for Matt, para 512 */
#define BIGREWCD 	4914 /* added for Matt, para 512 */ 
#define CHGFPCD 	4915  /* added for Jamie, para 602 */
#define QONCD		4916  /* added for Mark, para 422 */
#define QOFFCD		4917  /*  same */
#define GOTRGONCD  4918  /* same */
#define TRGACQUIRECD  4919  /*same */
#define QACQUIRECD  4920
#define GOTRGOFFCD  4921
#define DTMVBGNCD	  4922	/* added for Josh, para 702exp */
#define DTMVENDCD	  4923	/* added for Josh, para 702exp */
#define ACCEPTCAL     4924  /* added by JD, para 730 */
#define LASTCD    4925   		/* Added by MM for 65* series of paras */
#define BRFIXCD	4926
#define CNTRMNDCD 4927
#define PSEUDTRGCD 4928	/* added by Sharath for spm761 */
#define TRGC1CD	4930
#define TRGC2CD	4931
#define TRGC3CD	4932
#define SACACC 4933  /*  mn added -- saccade accepted, awaiting feedback */
#define BLOCKCD   4944  /* jig & mn added -- NEW BLOCK */

/*
 *	Event codes specific per laboratory.
 */
#define FPONCD			1010
#define FPONBLINKCD	1012
#define FPOFFBLINKCD	1014
#define FPDIMCD		1020
#define FPOFFCD		1025
#define REWCD			1030
#define REWOFFCD		1037
#define OMITREW		1038 /* added by Long Ding 2008 */
#define STIMCD			1100
#define STOFFCD		1101
#define STONBLINKCD	1120	/* stim blink */
#define STOFFBLINKCD	1122	/* stim blink off */
#define PHIONCD		1125	/* phi blink on */
#define PHISTARTCD	1127	/* phi sequence started */
#define PHIENDCD		1129	/* phi sequence ended */
#define BASECD			1500	/* Code that can be modified by an
				   action return.  Leave with 300 consecutive
				   unused numbers.  */

/*
 *	Paradigm init codes.
 */
#define EP_1		101		/* fix pt with stim on */
#define EP_2		102		/* fix pt with ramp stim */
#define EP_3		103		/* ramp fix pt with stationary stim */

#define E_D0		2000		/* ramp direction series */
#define E_D45		2001
#define E_D90		2002
#define E_D135		2003
#define E_D180		2004
#define E_D225		2005
#define E_D270		2006
#define E_D315		2007

#define E_VT5		2031
#define E_VT10		2032
#define E_VT15		2033
#define E_VT25		2034
#define E_VT40		2035

#define E_VT2		2040
#define E_VT4   	2041
#define E_VT8   	2042
#define E_VT16  	2043
#define E_VT32  	2044
#define E_VT64  	2045
#define E_VT128 	2046

#define E_VS5		2050		/* velocity stimulation series */
#define E_VS10		2051
#define E_VS15		2052
#define E_VS25		2053
#define E_VS50		2054
#define E_VS100	2055
#define E_VS200	2056
#define E_VS400 	2057

#define E_VS1		2070
#define E_VS2		2071
#define E_VS4   	2072
#define E_VS8   	2073
#define E_VS16  	2074
#define E_VS32  	2075
#define E_VS64  	2076
#define E_VS128 	2077

/*
 * Stabilization codes.  The code is determined by ORing the
 * base code, STBONCD or STBOFFCD with the stabilization flag
 * values, STB_H, STB_V, STB_B = 01, 02, 03.
 */
#define STBONCD	2048
#define STBOFFCD	2064

/*
 * Acuity answer codes
 *	ANSWERCD | 01 = yes, ANSWERCD = no
 */
#define ANSWERCD	2100

/*
 * when the mirror is flicked to blank the image, this code goes out
 */
#define BLANKCD	2150

/* local paradigm EVENT codes */
#define TARGONCD 	3000
#define TARGC1CD	3001
#define TARGC2CD	3002
#define TARGC3CD	3003
#define TARGC4CD	3004
#define TARGOFFCD 3005		/*added by Long, 1-18-2007 */
#define FDBKONCD 	3006		/*added by Long, 1-18-2007 */
#define FPCHG 		3007		/*added by Long, 2-27-2007 to indicate FP color change after task jump*/
#define TARG1OFF	3102		/* added by YL, 2011/03/23 */
#define TARG2OFF	3103		/* added by YL, 2011/03/23 */
#define COHCHG    3104		/* added by TD, 2013/11/18 */

/* the time of motion-direction change, added by TD on 2014-01-29 */
#define DIRCHG0 3105
#define DIRCHG1 3106
#define DIRCHG2 3107
#define DIRCHG3 3108
#define DIRCHG4 3109
#define DIRCHG5 3110
#define DIRCHG6 3111
#define DIRCHG7 3112
#define DIRCHG8 3113
#define DIRCHG9 3114


/* OLD CODES 
#define TARGON1CD	3100
#define TARGON2CD	3101
#define TARG1OFF	3102
#define TARG2OFF	3103
#define BRANCHCD	2300
#define ONSTABCD	2100
#define ONSLIPCD	2400
#define ONSTPCD	2500
#define ONRMPCD	2600
#define OFFSTABC	2101
#define DARK		2700
#define OKNSTART	2702
#define OKNSAT		2703
#define OKAN		2704
*/

/*
 * defines for rampflags
 */
#define RS_ENDLST		01
#define RS_CDONE		02
#define RS_ADONE		04
#define RS_MONITOR	010
#define RS_TDONE		020
#define RS_SAC			040
#define RS_STAB		0100
#define RS_OFF			0200
#define RS_SLIP		0400
#define RS_STP			01000
#define RS_RMP			02000
#define RS_RAMP		04000

/*
 * RASHBASS TABLE
 *		Contains fixation position, ramp length, polar
 *	coordinates for ramp, and step dimensions for generalized
 *	step-ramp tracking paradigm.
 */
typedef struct {
	int rs_xw, rs_yw;	/* eye window dimensions, in 10ths of deg */
	int rs_vel, rs_ang;	/* polar velocity, in deg/sec and deg */
	int rs_xoff, rs_yoff;	/* step dimensions */
	int rs_ecode;		/* event code for this stimulus */
	int rs_good;		/* number of good trials */
	int rs_count;		/* number of successful trials */
} RASHTBL ;

typedef struct {
	int rs_xw;		/* eye window dimensions, in 10ths of deg */
	int rs_len;		/* 1/2 total ramp length in 10ths of deg */
	int rs_vel, rs_ang;	/* polar velocity, in deg/sec and deg */
	int rs_xoff, rs_yoff;	/* step dimensions */
	int rs_ecode;		/* event code for this stimulus */
	int rs_good;		/* number of good trials */
	int rs_count;		/* number of successful trials */
} RAMPTBL ;

/*
 *Paradigm Ecodes
 */
#define EYINWD 	1006
#define SACMAD		1007
#define PURSCD 	1008
#define BCKDRK		1026
#define BCKLIT		1027
#define TGBLCD		1028
#define TGBLOF		1029
#define TARGON		1040
#define TARGOF		1041
#define STABON		1050
#define STABOF		1051
#define TTGBLC		1052
#define TSTABO		1053
#define TSTIMC		1054
#define TTRGON 	1055
#define TFPNBK 	1056
#define TVDIS		1057
#define TVOFF		1058
#define ELESTM		1059
#define TVACC		1060
#define BCKNOIS	1061
#define TELESTM	1062
#define ELEOFF 	1063

/*
 * E codes for D. Waitzman (labi) multiple paradigm routine
 */
/*
#define SACSPONT 		7000		/ start of spontaneous saccade /
#define PARA_NO_SAC 	7002		/ start of visual type paradigm /
#define PARA_DO_SAC 	7004		/ start saccade paradigm to mov field /
#define PARA_REC_SAC 7006		/ start of remembered saccade paradigm /
#define PARA_DARK_SAC 7008		/ fixation paradigm, collect 2 saccades in dark /
#define PARA_GAP_SAC 7010		/ start of expressed saccade paradigm /
#define STDIMCD  		7012		/ dimming the stimulus /
#define DK_SAC1 		7014		/ first saccade in dark post fixation /
#define DK_SAC2 		7016		/ second saccade in dark post fixation /
#define PARA_DBL_SAC 7018		/ start of double saccade paradigm /
#define TAGDIMCD 		7020		/ dimming of the target code (not a visual stimulus /
*/

/* 
 * new shadlen lab (UW) codes  are tacked on to 4900 series
 */
