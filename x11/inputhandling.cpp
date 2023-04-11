#include "inputhandling.h"
#include "pccore.h"
#include "keystat.h"
#include "soundmng.h"

namespace BR {

//PC-9801 keyboard layout (more or less)
// STOP  COPY    F1  F2  F3  F4  F5  F6  F7  F8  F9  F10  (RU  RD)*
//
// ESC       1  2  3  4  5  6  7  8  9  0  -  ^  ¥  BS     INS  DEL    HOME HELP   -    /
//    TAB      Q  W  E  R  T  Y  U  I  O  P  @  [  ||||||| (       )*    7    8    9    *
// CTRL CAPS     A  S  D  F  G  H  J  K  L  ;  : ] |ENTER|     UP        4    5    6    +
//  --SHIFT--      Z  X  C  V  B  N  M  <  >  ? --  SHIFT   LT    RT     1    2    3    =
//        KANA  GRPH (**) ----------SPACE---------- XFER       DN        0    ,    .   RET
//
//
// * On some layouts Roll Up Down are replaced with vf1 vf2 vf3 vf4 vf5 and is instead
// placed under INS and DEL
// ** On some layouts there was also an NFER Key here.
// -- left of shift is actually the long '-'


class KeyMapping {
public:
	KeyButtons key;
	uint8_t res;
};

//NKEY_KANA			= 0x72,
//NKEY_GRPH			= 0x73,

// we are using keycodes here, not scancodes as we want to replicate
// the original keyboard (not completely possible)

static KeyMapping keyTable [] {
	{KeyButtons::KEY_ESC,          0x00},
	{KeyButtons::KEY_1,            0x01},
	{KeyButtons::KEY_2,            0x02},
	{KeyButtons::KEY_3,            0x03},
	{KeyButtons::KEY_4,            0x04},
	{KeyButtons::KEY_5,            0x05},
	{KeyButtons::KEY_6,            0x06},
	{KeyButtons::KEY_7,            0x07},
	{KeyButtons::KEY_8,            0x08},
	{KeyButtons::KEY_9,            0x09},
	{KeyButtons::KEY_0,            0x0a},
	{KeyButtons::KEY_ENTER,        0x1c},

	{KeyButtons::KEY_Q,            0x10},
	{KeyButtons::KEY_W,            0x11},
	{KeyButtons::KEY_E,            0x12},
	{KeyButtons::KEY_R,            0x13},
	{KeyButtons::KEY_T,            0x14},
	{KeyButtons::KEY_Y,            0x15},
	{KeyButtons::KEY_U,            0x16},
	{KeyButtons::KEY_I,            0x17},
	{KeyButtons::KEY_O,            0x18},
	{KeyButtons::KEY_P,            0x19},

	{KeyButtons::KEY_A,            0x1d},
	{KeyButtons::KEY_S,            0x1e},
	{KeyButtons::KEY_D,            0x1f},

	{KeyButtons::KEY_F,            0x20},
	{KeyButtons::KEY_G,            0x21},
	{KeyButtons::KEY_H,            0x22},
	{KeyButtons::KEY_J,            0x23},
	{KeyButtons::KEY_K,            0x24},
	{KeyButtons::KEY_L,            0x25},

	{KeyButtons::KEY_Z,            0x29},
	{KeyButtons::KEY_X,            0x2a},
	{KeyButtons::KEY_C,            0x2b},
	{KeyButtons::KEY_V,            0x2c},
	{KeyButtons::KEY_B,            0x2d},
	{KeyButtons::KEY_N,            0x2e},
	{KeyButtons::KEY_M,            0x2f},

	{KeyButtons::KEY_SPACE,	       0x34},
	{KeyButtons::KEY_RIGHT_ALT,    0x35},

	{KeyButtons::KEY_ARROW_UP,     0x3a},
	{KeyButtons::KEY_ARROW_LEFT,   0x3b},
	{KeyButtons::KEY_ARROW_RIGHT,  0x3c},
	{KeyButtons::KEY_ARROW_DOWN,   0x3d},

	{KeyButtons::KEY_NUMPAD_NUM,   0x3e},
	//HELP Key -- Apparently used to access the bios
	{KeyButtons::KEY_F11,          0x3f},
	{KeyButtons::KEY_NUMPAD_MINUS, 0x40},
	{KeyButtons::KEY_NUMPAD_DIV,   0x41},
	{KeyButtons::KEY_NUM_7,        0x42},
	{KeyButtons::KEY_NUM_8,        0x43},
	{KeyButtons::KEY_NUM_9,        0x44},
	{KeyButtons::KEY_NUMPAD_MULT,  0x45},
	{KeyButtons::KEY_NUM_4,        0x46},
	{KeyButtons::KEY_NUM_5,        0x47},
	{KeyButtons::KEY_NUM_6,        0x48},
	{KeyButtons::KEY_NUMPAD_PLUS,  0x49},
	{KeyButtons::KEY_NUM_1,        0x4a},
	{KeyButtons::KEY_NUM_2,        0x4b},
	{KeyButtons::KEY_NUM_3,        0x4c},
	{KeyButtons::KEY_NUMPAD_ENTER, 0x4d},
	{KeyButtons::KEY_NUM_0,        0x4e},
	{KeyButtons::KEY_NUMPAD_COMMA, 0x4f},

	{KeyButtons::KEY_F1,           0x62},
	{KeyButtons::KEY_F2,           0x63},
	{KeyButtons::KEY_F3,           0x64},
	{KeyButtons::KEY_F4,           0x65},
	{KeyButtons::KEY_F5,           0x66},
	{KeyButtons::KEY_F6,           0x67},
	{KeyButtons::KEY_F7,           0x68},
	{KeyButtons::KEY_F8,           0x69},
	{KeyButtons::KEY_F9,           0x6a},
	{KeyButtons::KEY_F10,          0x6b},

	{KeyButtons::KEY_LEFT_SHIFT,   0x70},
	{KeyButtons::KEY_LEFT_CTRL,    0x74},
};

//static void kbdTest(GLFWInput& input){
//	for(auto& keyEvent : input.keyEvents){
//		static int i = 1;
//
//		if(keyEvent.key == KeyButtons::KEY_ENTER && keyEvent.state == PRESSED){
//			keystat_keydown(i);
//		}
//		else if(keyEvent.key == KeyButtons::KEY_ENTER && keyEvent.state == RELEASED){
//			keystat_keyup(i);
//		}
//
//		i++;
//	}
//}

static void mapAndSendKey(KeyEvent& keyEvent){
	for(KeyMapping& mapping : keyTable){
		if(mapping.key == keyEvent.key){
			if(keyEvent.state== PRESSED){
				//printf("res %d\n", mapping.res);
				keystat_keydown(mapping.res);
			}
			else {
				//printf("res %d\n", mapping.res);
				keystat_keyup(mapping.res);
			}
			break;
		}
	}
}

template <class T1>
T1 next(const T1& t1){
	return (T1)((((int)t1) + 1)% (int)T1::END);
}

void handleInput(Input& input, ViewPortMode& viewPortMode, VisualScreen& visualScreen, Sfx::PulseSoundEngine& soundRef){
	if(input.getButton(KeyButtons::KEY_SUPER)){
		for(auto& keyEvent : input.keyEvents){
			if(keyEvent.key == KeyButtons::KEY_E && keyEvent.state == PRESSED){
				viewPortMode = next(viewPortMode);
			}

			if(keyEvent.key == KeyButtons::KEY_NUMPAD_DIV && keyEvent.state == PRESSED){
				soundRef.decreaseVol(0.05);
			}

			if(keyEvent.key == KeyButtons::KEY_NUMPAD_MULT && keyEvent.state == PRESSED){
				soundRef.increaseVol(0.05);
			}

			if(keyEvent.key == KeyButtons::KEY_M && keyEvent.state == PRESSED){
				soundRef.toggleMute();
			}

			if(keyEvent.key == KeyButtons::KEY_I && keyEvent.state == PRESSED){
				pccore_reset(&soundRef);
			}

			if(keyEvent.key == KeyButtons::KEY_K && keyEvent.state == PRESSED){
				visualScreen = next(visualScreen);
			}
		}
	}
	else {
		for(auto& keyEvent : input.keyEvents){
			mapAndSendKey(keyEvent);
			//kbdTest();
		}
	}
}
}
