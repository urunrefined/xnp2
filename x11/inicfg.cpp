#include "inicfg.h"
#include "commng.h"

//This is currently shadowing the global np2oscfg/np2cfg,
//but this is fine, as the globals are set to be removed
IniCfg::IniCfg(NP2OSCFG& np2oscfg, tagNP2Config& np2cfg)
{
	INITBL iniitems[] = {
		{"fontfile", INITYPE_STR,	np2cfg.fontfile,	MAX_PATH},
		{"hdrvroot", INIRO_STR,		np2cfg.hdrvroot,	MAX_PATH},
		{"hdrv_acc", INIRO_UINT8,	&np2cfg.hdrvacc,	0},

		{"pc_model", INITYPE_STR,	np2cfg.model,		sizeof(np2cfg.model)},

		{"clk_base", INITYPE_UINT32,	&np2cfg.baseclock,	0},
		{"clk_mult", INITYPE_UINT32,	&np2cfg.multiple,	0},

		{"DIPswtch", INITYPE_ARGH8,	np2cfg.dipsw,		3},
		{"MEMswtch", INITYPE_ARGH8,	np2cfg.memsw,		8},
		{"ExMemory", INIMAX_UINT8,	&np2cfg.EXTMEM,		13},
		{"ITF_WORK", INIRO_BOOL,	&np2cfg.ITF_WORK,	0},

		{"HDD1FILE", INITYPE_STR,	np2cfg.sasihdd[0],	MAX_PATH},
		{"HDD2FILE", INITYPE_STR,	np2cfg.sasihdd[1],	MAX_PATH},

		{"FDD1FILE", INITYPE_STR,	np2cfg.fdd[0],	MAX_PATH},
		{"FDD2FILE", INITYPE_STR,	np2cfg.fdd[1],	MAX_PATH},
		{"FDD3FILE", INITYPE_STR,	np2cfg.fdd[2],	MAX_PATH},
		{"FDD4FILE", INITYPE_STR,	np2cfg.fdd[3],	MAX_PATH},

		{"SampleHz", INITYPE_UINT32,	&np2cfg.samplingrate,	0},
		{"Latencys", INITYPE_UINT16,	&np2cfg.delayms,	0},
		{"SNDboard", INITYPE_HEX8,	&np2cfg.SOUND_SW,	0},
		{"BEEP_vol", INIAND_UINT8,	&np2cfg.BEEP_VOL,	3},
		{"xspeaker", INIRO_BOOL,	&np2cfg.snd_x,		0},

		{"SND14vol", INITYPE_ARGH8,	np2cfg.vol14,		6},
	//	{"opt14BRD", INITYPE_ARGH8,	np2cfg.snd14opt,	3},
		{"opt26BRD", INITYPE_HEX8,	&np2cfg.snd26opt,	0},
		{"opt86BRD", INITYPE_HEX8,	&np2cfg.snd86opt,	0},
		{"optSPBRD", INITYPE_HEX8,	&np2cfg.spbopt,		0},
		{"optSPBVR", INITYPE_HEX8,	&np2cfg.spb_vrc,	0},
		{"optSPBVL", INIMAX_UINT8,	&np2cfg.spb_vrl,	24},
		{"optSPB_X", INITYPE_BOOL,	&np2cfg.spb_x,		0},
		{"optMPU98", INITYPE_HEX8,	&np2cfg.mpuopt,		0},

		{"volume_F", INIMAX_UINT8,	&np2cfg.vol_fm,		128},
		{"volume_S", INIMAX_UINT8,	&np2cfg.vol_ssg,	128},
		{"volume_A", INIMAX_UINT8,	&np2cfg.vol_adpcm,	128},
		{"volume_P", INIMAX_UINT8,	&np2cfg.vol_pcm,	128},
		{"volume_R", INIMAX_UINT8,	&np2cfg.vol_rhythm,	128},

		{"Seek_Snd", INITYPE_BOOL,	&np2cfg.MOTOR,		0},

		{"btnRAPID", INITYPE_BOOL,	&np2cfg.BTN_RAPID,	0},
		{"btn_MODE", INITYPE_BOOL,	&np2cfg.BTN_MODE,	0},
		{"MS_RAPID", INITYPE_BOOL,	&np2cfg.MOUSERAPID,	0},

		{"VRAMwait", INITYPE_ARGH8,	np2cfg.wait,		6},
		{"DspClock", INIAND_UINT8,	&np2oscfg.DISPCLK,	3},
		{"DispSync", INITYPE_BOOL,	&np2cfg.DISPSYNC,	0},
		{"Real_Pal", INITYPE_BOOL,	&np2cfg.RASTER,		0},
		{"RPal_tim", INIMAX_UINT8,	&np2cfg.realpal,	64},
		{"uPD72020", INITYPE_BOOL,	&np2cfg.uPD72020,	0},
		{"GRCG_EGC", INIAND_UINT8,	&np2cfg.grcg,		3},
		{"color16b", INITYPE_BOOL,	&np2cfg.color16,	0},
		{"LCD_MODE", INIAND_UINT8,	&np2cfg.LCD_MODE,	0x03},
		{"BG_COLOR", INIROAND_HEX32,	&np2cfg.BG_COLOR,	0xffffff},
		{"FG_COLOR", INIROAND_HEX32,	&np2cfg.FG_COLOR,	0xffffff},

		{"pc9861_e", INITYPE_BOOL,	&np2cfg.pc9861enable,	0},
		{"pc9861_s", INITYPE_ARGH8,	np2cfg.pc9861sw,	3},
		{"pc9861_j", INITYPE_ARGH8,	np2cfg.pc9861jmp,	6},

		{"calendar", INITYPE_BOOL,	&np2cfg.calendar,	0},
		{"USE144FD", INITYPE_BOOL,	&np2cfg.usefd144,	0},
		{"FDDRIVE1", INIRO_BITMAP,	&np2cfg.fddequip,	0},
		{"FDDRIVE2", INIRO_BITMAP,	&np2cfg.fddequip,	1},
		{"FDDRIVE3", INIRO_BITMAP,	&np2cfg.fddequip,	2},
		{"FDDRIVE4", INIRO_BITMAP,	&np2cfg.fddequip,	3},

		{"keyboard", INIRO_KB,		&np2oscfg.KEYBOARD,	0},
		{"F12_COPY", INITYPE_UINT8,	&np2oscfg.F12KEY,	0},
		{"Joystick", INITYPE_BOOL,	&np2oscfg.JOYPAD1,	0},
		{"Joy1_dev", INITYPE_STR,	&np2oscfg.JOYDEV[0],	MAX_PATH},

		{"confirm_", INITYPE_BOOL,	&np2oscfg.confirm,	0},

		{"mpu98map", INITYPE_STR,	np2oscfg.mpu.mout,	MAX_PATH},
		{"mpu98min", INITYPE_STR,	np2oscfg.mpu.min,	MAX_PATH},
		{"mpu98mdl", INITYPE_STR,	np2oscfg.mpu.mdl,	64},
		{"mpu98def", INITYPE_STR,	np2oscfg.mpu.def,	MAX_PATH},

		{"com1port", INIMAX_UINT8,	&np2oscfg.com[0].port,	COMPORT_MIDI},
		{"com1para", INITYPE_UINT8,	&np2oscfg.com[0].param,	0},
		{"com1_bps", INITYPE_UINT32,	&np2oscfg.com[0].speed,	0},
		{"com1mmap", INITYPE_STR,	np2oscfg.com[0].mout,	MAX_PATH},
		{"com1mmdl", INITYPE_STR,	np2oscfg.com[0].mdl,	64},
		{"com1mdef", INITYPE_STR,	np2oscfg.com[0].def,	MAX_PATH},

		{"com2port", INIMAX_UINT8,	&np2oscfg.com[1].port,	COMPORT_MIDI},
		{"com2para", INITYPE_UINT8,	&np2oscfg.com[1].param,	0},
		{"com2_bps", INITYPE_UINT32,	&np2oscfg.com[1].speed,	0},
		{"com2mmap", INITYPE_STR,	np2oscfg.com[1].mout,	MAX_PATH},
		{"com2mmdl", INITYPE_STR,	np2oscfg.com[1].mdl,	64},
		{"com2mdef", INITYPE_STR,	np2oscfg.com[1].def,	MAX_PATH},

		{"com3port", INIMAX_UINT8,	&np2oscfg.com[2].port,	COMPORT_MIDI},
		{"com3para", INITYPE_UINT8,	&np2oscfg.com[2].param,	0},
		{"com3_bps", INITYPE_UINT32,	&np2oscfg.com[2].speed,	0},
		{"com3mmap", INITYPE_STR,	np2oscfg.com[2].mout,	MAX_PATH},
		{"com3mmdl", INITYPE_STR,	np2oscfg.com[2].mdl,	64},
		{"com3mdef", INITYPE_STR,	np2oscfg.com[2].def,	MAX_PATH},

	#if defined(SUPPORT_STATSAVE)
		{"STATSAVE", INIRO_BOOL,	&np2oscfg.statsave,	0},
	#endif

		{"jast_snd", INITYPE_BOOL,	&np2oscfg.jastsnd,	0},

		{"MIDIOUTd", INITYPE_STR,	&np2oscfg.MIDIDEV[0],	MAX_PATH},
		{"MIDIIN_d", INITYPE_STR,	&np2oscfg.MIDIDEV[1],	MAX_PATH},
		{"MIDIWAIT", INITYPE_UINT32,	&np2oscfg.MIDIWAIT,	0},
	};

	config.assign(iniitems, iniitems + sizeof(iniitems) / sizeof(iniitems[0]));

}
