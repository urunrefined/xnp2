#include "inputhandling.h"
#include "keystat.h"
#include "pccore.h"
#include "soundmng.h"

#include <fcntl.h>
#include <sys/stat.h>

namespace BR {

// clang-format off

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

// clang-format on

struct N98Keymapping {
    PC98Key key;
    const char *keyname;
};

struct CodeKeymapping {
    KeyButtons key;
    const char *keyname;
};

// clang-format off

N98Keymapping n98Keymapping [] {
    {NKEY_ESC			,"ESC"},
	{NKEY_1				,"1"},
	{NKEY_2				,"2"},
	{NKEY_3				,"3"},
	{NKEY_4				,"4"},
	{NKEY_5				,"5"},
	{NKEY_6				,"6"},
	{NKEY_7				,"7"},

	{NKEY_8				,"8"},
	{NKEY_9				,"9"},
	{NKEY_0				,"0"},
	{NKEY_MINUS			,"MINUS"},
	{NKEY_CIRCUMFLEX    ,"CIRCUMFLEX"},
	{NKEY_YEN			,"YEN"},
	{NKEY_BACKSPACE		,"BACKSPACE"},
	{NKEY_TAB			,"TAB"},

	{NKEY_Q				,"Q"},
	{NKEY_W				,"W"},
	{NKEY_E				,"E"},
	{NKEY_R				,"R"},
	{NKEY_T				,"T"},
	{NKEY_Y				,"Y"},
	{NKEY_U				,"U"},
	{NKEY_I				,"I"},

	{NKEY_O				,"O"},
	{NKEY_P				,"P"},
	{NKEY_ATMARK		,"AT"},
	{NKEY_LEFTSBRACKET	,"["},
	{NKEY_RETURN		,"RETURN"},
	{NKEY_A				,"A"},
	{NKEY_S				,"S"},
	{NKEY_D				,"D"},

	{NKEY_F				,"F"},
	{NKEY_G				,"G"},
	{NKEY_H				,"H"},
	{NKEY_J				,"J"},
	{NKEY_K				,"K"},
	{NKEY_L				,"L"},
	{NKEY_SEMICOLON		,";"},
	{NKEY_COLON			,":"},

	{NKEY_RIGHTSBRACKET	,"]"},
	{NKEY_Z				,"Z"},
	{NKEY_X				,"X"},
	{NKEY_C				,"C"},
	{NKEY_V				,"V"},
	{NKEY_B				,"B"},
	{NKEY_N				,"N"},
	{NKEY_M				,"M"},

	{NKEY_COMMA			,","},
	{NKEY_DOT			,"."},
	{NKEY_SLASH			,"/"},
	{NKEY_UNDERSCORE	,"_"},
	{NKEY_SPACE			,"SPACE"},
	{NKEY_XFER			,"XFER"},
	{NKEY_ROLLUP		,"ROLLUP"},
	{NKEY_ROLLDOWN		,"ROLLDOWN"},

	{NKEY_INS			,"INS"},
	{NKEY_DEL			,"DEL"},
	{NKEY_UP			,"UP"},
	{NKEY_LEFT			,"LEFT"},
	{NKEY_RIGHT			,"RIGHT"},
	{NKEY_DOWN			,"DOWN"},
	{NKEY_HOMECLR		,"HOMECLR"},
	{NKEY_HELP			,"HELP"},

	{NKEY_KP_MINUS		,"KPMINUS"},
	{NKEY_KP_SLASH		,"KPSLASH"},
	{NKEY_KP_7			,"KP7"},
	{NKEY_KP_8			,"KP8"},
	{NKEY_KP_9			,"KP9"},
	{NKEY_KP_ASTERISK	,"KASTERISK"},
	{NKEY_KP_4			,"KP4"},
	{NKEY_KP_5			,"KP5"},

	{NKEY_KP_6			,"KP6"},
	{NKEY_KP_PLUS		,"KPPLUS"},
	{NKEY_KP_1			,"KP1"},
	{NKEY_KP_2			,"KP2"},
	{NKEY_KP_3			,"KP3"},
	{NKEY_KP_EQUAL		,"KP="},
	{NKEY_KP_0			,"KP0"},
	{NKEY_KP_COMMA		,"KP,"},

	{NKEY_KP_DOT		,"KPDOT"},
	{NKEY_NFER			,"NFER"},
	{NKEY_VF1			,"VF1"},
	{NKEY_VF2			,"VF2"},
	{NKEY_VF3			,"VF3"},
	{NKEY_VF4			,"VF4"},
	{NKEY_VF5			,"VF5"},

	{NKEY_STOP			,"STOP"},
	{NKEY_COPY			,"COPY"},
	{NKEY_F1			,"F1"},
	{NKEY_F2			,"F2"},
	{NKEY_F3			,"F3"},
	{NKEY_F4			,"F4"},
	{NKEY_F5			,"F5"},
	{NKEY_F6			,"F6"},

	{NKEY_F7			,"F7"},
	{NKEY_F8			,"F8"},
	{NKEY_F9			,"F9"},
	{NKEY_F10			,"F10"},

	{NKEY_SHIFT			,"SHIFT"},
	{NKEY_CAPS			,"CAPS"},
	{NKEY_KANA			,"KANA"},
	{NKEY_GRPH			,"GRPH"},
	{NKEY_CTRL			,"CTRL"}
};

PC98Key getPC98KeyOfString(const std::string& str){
    for(const auto& pc98 : n98Keymapping ){
        if(str == pc98.keyname) return pc98.key;
    }
    
    return NKEY_NONE;
}

CodeKeymapping internalKeymapping [] {
    {KeyButtons::KEY_ESC			,"ESC"},
	{KeyButtons::KEY_1				,"1"},
	{KeyButtons::KEY_2				,"2"},
	{KeyButtons::KEY_3				,"3"},
	{KeyButtons::KEY_4				,"4"},
	{KeyButtons::KEY_5				,"5"},
	{KeyButtons::KEY_6				,"6"},
	{KeyButtons::KEY_7				,"7"},
	{KeyButtons::KEY_8				,"8"},
	{KeyButtons::KEY_9				,"9"},
	{KeyButtons::KEY_0				,"0"},
    
	{KeyButtons::KEY_MINUS			,"MINUS"},
	//{NKEY_CIRCUMFLEX    ,"CIRCUMFLEX"},
	//{NKEY_YEN			,"YEN"},
	{KeyButtons::KEY_BACKSPACE		,"BACKSPACE"},
	{KeyButtons::KEY_TAB			,"TAB"},

	{KeyButtons::KEY_Q				,"Q"},
	{KeyButtons::KEY_W				,"W"},
	{KeyButtons::KEY_E				,"E"},
	{KeyButtons::KEY_R				,"R"},
	{KeyButtons::KEY_T				,"T"},
	{KeyButtons::KEY_Y				,"Y"},
	{KeyButtons::KEY_U				,"U"},
	{KeyButtons::KEY_I				,"I"},

	{KeyButtons::KEY_O				,"O"},
	{KeyButtons::KEY_P				,"P"},
	//{KeyButtons::KEY_ATMARK		,"AT"},
	//{KeyButtons::KEY_LEFTSBRACKET	,"["},
	{KeyButtons::KEY_ENTER		    ,"RETURN"},
	{KeyButtons::KEY_A				,"A"},
	{KeyButtons::KEY_S				,"S"},
	{KeyButtons::KEY_D				,"D"},

	{KeyButtons::KEY_F				,"F"},
	{KeyButtons::KEY_G				,"G"},
	{KeyButtons::KEY_H				,"H"},
	{KeyButtons::KEY_J				,"J"},
	{KeyButtons::KEY_K				,"K"},
	{KeyButtons::KEY_L				,"L"},
	//{KeyButtons::KEY_SEMICOLON		,";"},
	//{KeyButtons::KEY_COLON			,":"},

	//{NKEY_RIGHTSBRACKET	,"]"},
	{KeyButtons::KEY_Z				,"Z"},
	{KeyButtons::KEY_X				,"X"},
	{KeyButtons::KEY_C				,"C"},
	{KeyButtons::KEY_V				,"V"},
	{KeyButtons::KEY_B				,"B"},
	{KeyButtons::KEY_N				,"N"},
	{KeyButtons::KEY_M				,"M"},
    
	{KeyButtons::KEY_COMMA				,","},
	{KeyButtons::KEY_PERIOD				,"."},
	//{NKEY_SLASH			,"/"},
	//{NKEY_UNDERSCORE	,"_"},
	{KeyButtons::KEY_SPACE			,"SPACE"},
	//{NKEY_XFER			,"XFER"},
	//{NKEY_ROLLUP		    ,"ROLLUP"},
	//{NKEY_ROLLDOWN		,"ROLLDOWN"},

	{KeyButtons::KEY_INSERT			,"INS"},
	{KeyButtons::KEY_DELETE			,"DEL"},
	{KeyButtons::KEY_ARROW_UP       ,"UP"},
	{KeyButtons::KEY_ARROW_LEFT		,"LEFT"},
	{KeyButtons::KEY_ARROW_RIGHT	,"RIGHT"},
	{KeyButtons::KEY_ARROW_DOWN     ,"DOWN"},
    
	//{NKEY_HOMECLR		,"HOMECLR"},
	//{NKEY_HELP			,"HELP"},

	{KeyButtons::KEY_NUMPAD_MINUS		,"KPMINUS"},
	{KeyButtons::KEY_NUMPAD_DIV		,"KPSLASH"},
	{KeyButtons::KEY_NUM_7			,"KP7"},
	{KeyButtons::KEY_NUM_8			,"KP8"},
	{KeyButtons::KEY_NUM_9			,"KP9"},
	{KeyButtons::KEY_NUMPAD_MULT	,"KASTERISK"},
	{KeyButtons::KEY_NUM_4			,"KP4"},
	{KeyButtons::KEY_NUM_5			,"KP5"},
	{KeyButtons::KEY_NUM_6			,"KP6"},
	{KeyButtons::KEY_NUMPAD_PLUS	,"KPPLUS"},
	{KeyButtons::KEY_NUM_1			,"KP1"},
	{KeyButtons::KEY_NUM_2			,"KP2"},
	{KeyButtons::KEY_NUM_3			,"KP3"},
	//{NKEY_KP_EQUAL		,"KP="},
	{KeyButtons::KEY_NUM_0			,"KP0"},
	//{NKEY_KP_COMMA		,"KP,"},
	{KeyButtons::KEY_NUMPAD_COMMA	,"KPDOT"},
	{KeyButtons::KEY_NUMPAD_ENTER	,"ENTER"},
	{KeyButtons::KEY_NUMPAD_NUM	    ,"KPNUM"},
    
	{KeyButtons::KEY_RIGHT_ALT	    ,"RIGHTALT"},
    
	//{NKEY_NFER			,"NFER"},
	//{NKEY_VF1			,"VF1"},
	//{NKEY_VF2			,"VF2"},
	//{NKEY_VF3			,"VF3"},
	//{NKEY_VF4			,"VF4"},
	//{NKEY_VF5			,"VF5"},

	//{NKEY_STOP			,"STOP"},
	//{NKEY_COPY			,"COPY"},
	{KeyButtons::KEY_F1			,"F1"},
	{KeyButtons::KEY_F2			,"F2"},
	{KeyButtons::KEY_F3			,"F3"},
	{KeyButtons::KEY_F4			,"F4"},
	{KeyButtons::KEY_F5			,"F5"},
	{KeyButtons::KEY_F6			,"F6"},
	{KeyButtons::KEY_F7			,"F7"},
	{KeyButtons::KEY_F8			,"F8"},
	{KeyButtons::KEY_F9			,"F9"},
	{KeyButtons::KEY_F10		,"F10"},
	{KeyButtons::KEY_F11		,"F11"},
	{KeyButtons::KEY_F12		,"F12"},
    
	{KeyButtons::KEY_SLASH		,"SLASH"},

	{KeyButtons::KEY_LEFT_SHIFT			,"SHIFT"},
	//{NKEY_CAPS			,"CAPS"},
	//{NKEY_KANA			,"KANA"},
	//{NKEY_GRPH			,"GRPH"},
	{KeyButtons::KEY_LEFT_CTRL			,"CTRL"}
};


KeyButtons getInternalKeyOfString(const std::string& str){
    for(const auto& key : internalKeymapping ){
        if(str == key.keyname) return key.key;
    }
    
    return KeyButtons::KEY_NONE;
}

// clang-format on

// we are using keycodes here, not scancodes as we want to replicate
// the original keyboard (not completely possible)

// clang-format off

static KeyMappingNew keyTable[]{
    {KeyButtons::KEY_ESC, NKEY_ESC},
    {KeyButtons::KEY_1, NKEY_1},
    {KeyButtons::KEY_2, NKEY_2},
    {KeyButtons::KEY_3, NKEY_3},
    {KeyButtons::KEY_4, NKEY_4},
    {KeyButtons::KEY_5, NKEY_5},
    {KeyButtons::KEY_6, NKEY_6},
    {KeyButtons::KEY_7, NKEY_7},
    {KeyButtons::KEY_8, NKEY_8},
    {KeyButtons::KEY_9, NKEY_9},
    {KeyButtons::KEY_0, NKEY_0},
    {KeyButtons::KEY_MINUS, NKEY_MINUS},
    {KeyButtons::KEY_ENTER, NKEY_RETURN},

    {KeyButtons::KEY_Q, NKEY_Q},
    {KeyButtons::KEY_W, NKEY_W},
    {KeyButtons::KEY_E, NKEY_E},
    {KeyButtons::KEY_R, NKEY_R},
    {KeyButtons::KEY_T, NKEY_T},
    {KeyButtons::KEY_Y, NKEY_Y},
    {KeyButtons::KEY_U, NKEY_U},
    {KeyButtons::KEY_I, NKEY_I},
    {KeyButtons::KEY_O, NKEY_O},
    {KeyButtons::KEY_P, NKEY_P},

    {KeyButtons::KEY_A, NKEY_A},
    {KeyButtons::KEY_S, NKEY_S},
    {KeyButtons::KEY_D, NKEY_D},

    {KeyButtons::KEY_F, NKEY_F},
    {KeyButtons::KEY_G, NKEY_G},
    {KeyButtons::KEY_H, NKEY_H},
    {KeyButtons::KEY_J, NKEY_J},
    {KeyButtons::KEY_K, NKEY_K},
    {KeyButtons::KEY_L, NKEY_L},

    {KeyButtons::KEY_Z, NKEY_Z},
    {KeyButtons::KEY_X, NKEY_X},
    {KeyButtons::KEY_C, NKEY_C},
    {KeyButtons::KEY_V, NKEY_V},
    {KeyButtons::KEY_B, NKEY_B},
    {KeyButtons::KEY_N, NKEY_N},
    {KeyButtons::KEY_M, NKEY_M},

    {KeyButtons::KEY_SPACE, NKEY_SPACE},
    {KeyButtons::KEY_RIGHT_ALT, NKEY_XFER},

    {KeyButtons::KEY_ARROW_UP, NKEY_UP},
    {KeyButtons::KEY_ARROW_LEFT, NKEY_LEFT},
    {KeyButtons::KEY_ARROW_RIGHT, NKEY_RIGHT},
    {KeyButtons::KEY_ARROW_DOWN, NKEY_DOWN},

    {KeyButtons::KEY_NUMPAD_NUM, NKEY_HOMECLR},
    // HELP Key -- Apparently used to access the bios
    {KeyButtons::KEY_F11, NKEY_HELP},
    {KeyButtons::KEY_NUMPAD_MINUS, NKEY_KP_MINUS},
    {KeyButtons::KEY_NUMPAD_DIV, NKEY_KP_SLASH},
    {KeyButtons::KEY_NUM_7, NKEY_KP_7},
    {KeyButtons::KEY_NUM_8, NKEY_KP_8},
    {KeyButtons::KEY_NUM_9, NKEY_KP_9},
    {KeyButtons::KEY_NUMPAD_MULT, NKEY_KP_ASTERISK},
    {KeyButtons::KEY_NUM_4, NKEY_KP_4},
    {KeyButtons::KEY_NUM_5, NKEY_KP_5},
    {KeyButtons::KEY_NUM_6, NKEY_KP_6},
    {KeyButtons::KEY_NUMPAD_PLUS, NKEY_KP_PLUS},
    {KeyButtons::KEY_NUM_1, NKEY_KP_1},
    {KeyButtons::KEY_NUM_2, NKEY_KP_2},
    {KeyButtons::KEY_NUM_3, NKEY_KP_3},
    {KeyButtons::KEY_NUMPAD_ENTER, NKEY_RETURN},
    {KeyButtons::KEY_NUM_0, NKEY_KP_0},
    {KeyButtons::KEY_NUMPAD_COMMA, NKEY_KP_COMMA},
    
    {KeyButtons::KEY_COMMA, NKEY_COMMA},
    {KeyButtons::KEY_PERIOD, NKEY_DOT},

    {KeyButtons::KEY_F1, NKEY_F1},
    {KeyButtons::KEY_F2, NKEY_F2},
    {KeyButtons::KEY_F3, NKEY_F3},
    {KeyButtons::KEY_F4, NKEY_F4},
    {KeyButtons::KEY_F5, NKEY_F5},
    {KeyButtons::KEY_F6, NKEY_F6},
    {KeyButtons::KEY_F7, NKEY_F7},
    {KeyButtons::KEY_F8, NKEY_F8},
    {KeyButtons::KEY_F9, NKEY_F9},
    {KeyButtons::KEY_F10, NKEY_F10},

    {KeyButtons::KEY_LEFT_SHIFT, NKEY_SHIFT},
    {KeyButtons::KEY_LEFT_CTRL, NKEY_CTRL},
    {KeyButtons::KEY_BACKSPACE, NKEY_BACKSPACE},
    {KeyButtons::KEY_SLASH, NKEY_SLASH},
};

// clang-format on

// static void kbdTest(GLFWInput& input){
//	for(auto& keyEvent : input.keyEvents){
//		static int i = 1;
//
//		if(keyEvent.key == KeyButtons::KEY_ENTER && keyEvent.state ==
// PRESSED){ 			keystat_keydown(i);
//		}
//		else if(keyEvent.key == KeyButtons::KEY_ENTER && keyEvent.state
//== RELEASED){ 			keystat_keyup(i);
//		}
//
//		i++;
//	}
// }

static void mapAndSendKey(KeyEvent &keyEvent,
                          const std::vector<KeyMappingNew> &newKeyTable) {
    for (const KeyMappingNew &mapping : newKeyTable) {
        if (mapping.key == keyEvent.key) {
            if (keyEvent.state == PRESSED) {
                // printf("res %d\n", mapping.res);
                keystat_keydown(mapping.pc98Key);
            } else {
                // printf("res %d\n", mapping.res);
                keystat_keyup(mapping.pc98Key);
            }
            break;
        }
    }
}

template <class T1> T1 next(const T1 &t1) {
    return (T1)((((int)t1) + 1) % (int)T1::END);
}

void InputMapper::handleInput(Input &input, ViewPortMode &viewPortMode,
                              VisualScreen &visualScreen,
                              DoubleLines &doubleLine,
                              Sfx::PulseSoundEngine &soundRef) {

    if (input.getButton(KeyButtons::KEY_SUPER)) {
        for (auto &keyEvent : input.keyEvents) {
            if (keyEvent.key == KeyButtons::KEY_E &&
                keyEvent.state == PRESSED) {
                viewPortMode = next(viewPortMode);
            }

            if (keyEvent.key == KeyButtons::KEY_H &&
                keyEvent.state == PRESSED) {
                doubleLine = next(doubleLine);
            }

            if (keyEvent.key == KeyButtons::KEY_NUMPAD_DIV &&
                keyEvent.state == PRESSED) {
                soundRef.decreaseVol(0.05);
            }

            if (keyEvent.key == KeyButtons::KEY_NUMPAD_MULT &&
                keyEvent.state == PRESSED) {
                soundRef.increaseVol(0.05);
            }

            if (keyEvent.key == KeyButtons::KEY_M &&
                keyEvent.state == PRESSED) {
                soundRef.toggleMute();
            }

            if (keyEvent.key == KeyButtons::KEY_I &&
                keyEvent.state == PRESSED) {
                pccore_reset(&soundRef);
            }

            if (keyEvent.key == KeyButtons::KEY_K &&
                keyEvent.state == PRESSED) {
                visualScreen = next(visualScreen);
            }
        }
    } else {
        for (auto &keyEvent : input.keyEvents) {
            mapAndSendKey(keyEvent, mapping);
            // kbdTest();
        }
    }
}

static std::vector<std::string> convertBuffer(std::vector<uint8_t> &buf) {
    std::vector<std::string> ret;

    uint8_t *line = buf.data();

    for (size_t i = 0; i < buf.size(); i++) {
        if (buf[i] == '\n') {
            buf[i] = '\0';
            ret.push_back((const char *)line);
            line = buf.data() + i + 1;
        }
    }

    if (buf.back() != '\0') {
        buf.push_back('\0');
        ret.push_back(std::string((const char *)line));
    }

    return ret;
}

static std::vector<std::string> split(const std::string &str, const char term) {
    std::vector<std::string> result;
    size_t begin = 0;

    while (begin < str.size()) {
        size_t end = str.find(term, begin);

        if (end == std::string::npos) {
            end = str.size();
        }

        if (end - begin) {
            result.push_back(str.substr(begin, end - begin));
        }

        begin = end + 1;
    }

    return result;
}

std::vector<KeyMappingNew> readKeyMap(const char *filename) {
    int fd = open(filename, O_RDONLY);

    struct stat buf;

    if (fd == -1) {
        throw "Could not open keymap";
    }

    if (fstat(fd, &buf) != 0) {
        throw "Could not stat keymap";
    };

    std::vector<uint8_t> buffer(buf.st_size);

    ssize_t ret = read(fd, (char *)buffer.data(), buffer.size());

    close(fd);

    if (ret == -1) {
        throw "Reading data from keymap failed";
    }

    if ((size_t)ret != buffer.size()) {
        throw "Data read and size of keymap does not match";
    }

    std::vector<std::string> lines = convertBuffer(buffer);

    std::vector<KeyMappingNew> mapping;

    for (const auto &line : lines) {
        const auto &strs = split(line, ' ');
        if (strs.size() == 2) {
            const std::string &left = strs[0];
            const std::string &right = strs[1];

            KeyButtons internalKey = getInternalKeyOfString(left);
            PC98Key pc98Key = getPC98KeyOfString(right);

            if (internalKey == KeyButtons::KEY_NONE) {
                printf("Ignore internal keystring %s\n", left.c_str());
                continue;
            }

            if (pc98Key == NKEY_NONE) {
                printf("Ignore pc98 keystring %s\n", right.c_str());
                continue;
            }

            mapping.push_back({internalKey, pc98Key});
        }
    }

    return mapping;
}

std::vector<KeyMappingNew> getDefaultKeyMap() {
    std::vector<KeyMappingNew> mapping;

    for (const auto &keyEntry : keyTable) {
        mapping.push_back({keyEntry.key, keyEntry.pc98Key});
    }

    return mapping;
}

void saveKeyMap(const char *filename,
                const std::vector<KeyMappingNew> &mapping) {
    if (access(filename, F_OK) == 0) {
        printf("Keymap %s already exists\n", filename);
        return;
    }

    printf("Create new keymap %s with default keys\n", filename);

    int fd = open(filename, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        throw "Cannot open keymap for writing";
    }

    for (const auto &elem : mapping) {
        const char *left = 0;
        const char *right = 0;

        for (const auto &key : internalKeymapping) {
            if (key.key == elem.key) {
                left = key.keyname;
                break;
            }
        }

        for (const auto &key : n98Keymapping) {
            if (key.key == elem.pc98Key) {
                right = key.keyname;
                break;
            }
        }

        if (!left) {
            printf("No internal mapping for %d\n", (int)elem.key);
            continue;
        }

        if (!right) {
            printf("No pc98key mapping for %d\n", elem.pc98Key);
            continue;
        }

        write(fd, left, strlen(left));
        write(fd, " ", 1);
        write(fd, right, strlen(right));
        write(fd, "\n", 1);
    }

    close(fd);
};

} // namespace BR
