#include "global.h"
#include "kbdmng.h"
#include "commng.h"

NP2CFG	np2cfg = {
	0,                                       // UINT8 uPD72020;
	1,                                       // UINT8 DISPSYNC;
	0,                                       // UINT8 RASTER;
	32,                                      // UINT8 realpal;
	0,                                       // UINT8 LCD_MODE;
	0,                                       // UINT8 KEY_MODE;
	0x40,                                    // UINT8 XSHIFT;
	0,                                       // UINT8 BTN_RAPID;
	0,                                       // UINT8 BTN_MODE;

	{0x3e, 0x73, 0x7b},                      // UINT8 dipsw[3];
	0,                                       // UINT8 MOUSERAPID

	0,                                       // UINT8 calendar;
	0,                                       // UINT8 usefd144;

	{1, 1, 6, 1, 8, 1},                      // UINT8 wait[6];

	OEMTEXT("VX"),                           // OEMCHAR model[8];
	PCBASECLOCK25,                           // UINT  baseclock;
	PCBASEMULTIPLE,                          // UINT  multiple;
	{0x48, 0x05, 0x04, 0x00, 0x01, 0x00, 0x00, 0x6e}, //UINT8	memsw[8];
	1,                                       //UINT8  ITF_WORK;
	1,                                       //UINT8  EXTMEM;
	2,                                       //UINT8  grcg;
	1,                                       //UINT8  color16;
	0x000000,                                //UINT32 BG_COLOR;
	0xffffff,                                //UINT32 FG_COLOR;
	44100,                                   //UINT32 samplingrate;
	250,                                     //UINT16 delayms;
	4,                                       //UINT8  SOUND_SW;
	0,                                       //UINT8  snd_x;
	{0, 0, 0},                               //UINT8  snd14opt[3];


	0xd1,                                    //UINT8  snd26opt;
	0x7f,                                    //UINT8  snd86opt;
	0xd1,                                    //UINT8  spbopt;
	0,                                       //UINT8  spb_vrc;
	0,                                       //UINT8  spb_vrl;
	1,                                       //UINT8  spb_x;
	3,										 //UINT8  BEEP_VOL;
	{0x0c, 0x0c, 0x08, 0x06, 0x03, 0x0c},    //UINT8  vol14[6];

	64,                                      //UINT8  vol_fm;
	64,                                      //UINT8  vol_ssg;
	64,                                      //UINT8  vol_adpcm;
	64,                                      //UINT8  vol_pcm;
	64,                                      //UINT8  vol_rhythm;

	1,                                       //UINT8  mpuenable;
	0x82,                                    //UINT8  mpuopt;
	0,                                       //UINT8  pc9861enable;

	{0x17, 0x04, 0x1f},                      //UINT8  pc9861sw[3];
	{0x0c, 0x0c, 0x02, 0x10, 0x3f, 0x3f},    //UINT8  pc9861jmp[6];

	3,                                       //UINT8  fddequip;
	1,                                       //UINT8  MOTOR;
	80,                                      //UINT8  PROTECTMEM;
	0,                                       //UINT8  hdrvacc;

	{OEMTEXT(""), OEMTEXT("")},              //OEMCHAR sasihdd[2][MAX_PATH];
	{OEMTEXT(""), OEMTEXT(""), OEMTEXT(""), OEMTEXT("")}, //OEMCHAR	fdd[4][MAX_PATH];
#if defined(SUPPORT_SCSI)
	{OEMTEXT(""), OEMTEXT(""), OEMTEXT(""), OEMTEXT("")}, //OEMCHAR	scsihdd[4][MAX_PATH];
#endif
	OEMTEXT(""),                             //OEMCHAR fontfile[MAX_PATH];
	OEMTEXT("")                              //OEMCHAR hdrvroot[MAX_PATH];
};


NP2OSCFG np2oscfg = {
	0,					/* DISPCLK */

	KEY_KEY106,			/* KEYBOARD */
	0,					/* F12KEY */
	0,					/* JOYPAD1 */
	0,					/* JOYPAD2 */
	{ "", "" },					/* JOYDEV */

	{ "", "", ""},	/* mpu */
	{
		{ "", "", ""}, /* com1 */
		{ "", "", ""}, /* com2 */
		{ "", "", ""}, /* com3 */
	},

	0,					/* confirm */

	0,					/* statsave */
	0,					/* hostdrv_write */
	0,					/* jastsnd */

	{ "", "" },			/* MIDIDEV */
	0,					/* MIDIWAIT */
};
