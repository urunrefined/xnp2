#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <algorithm>

namespace BR {

enum class KeyButtons {
	KEY_LEFT_SHIFT = 0, KEY_LEFT_CTRL, KEY_LEFT_ALT, KEY_TAB,
	
	KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F,
	KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L,
	KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R,
	KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X,
	KEY_Y, KEY_Z,
	
	KEY_1, KEY_2, KEY_3, KEY_4, KEY_5,
	KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,
	
	KEY_NUM_1, KEY_NUM_2, KEY_NUM_3, KEY_NUM_4, KEY_NUM_5,
	KEY_NUM_6, KEY_NUM_7, KEY_NUM_8, KEY_NUM_9, KEY_NUM_0,
	
	KEY_BACKSPACE, KEY_DELETE, KEY_ENTER, KEY_SPACE, KEY_ESC, KEY_HOME, KEY_END, KEY_INSERT,
	KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
	
	KEY_NUMPAD_PLUS, KEY_NUMPAD_MINUS,
	KEY_ARROW_LEFT, KEY_ARROW_UP, KEY_ARROW_RIGHT, KEY_ARROW_DOWN,
	KEY_EQUAL, KEY_MINUS, KEY_PERIOD, KEY_SUPER,

	KEY_SIZE,
};

enum class MouseButtons {
	BUTTON_LEFT = 0, BUTTON_RIGHT, BUTTON_MIDDLE, SCROLL_UP, SCROLL_DOWN, BUTTON_SIZE
};

enum HIDAction {
	RELEASED = 0,
	PRESSED
};

class KeyEvent {
public:
	KeyButtons key;
	HIDAction state;

	KeyEvent(KeyButtons key_,
	HIDAction state_) : key(key_), state(state_)
	{}
};

template <class T1>
struct HIDInput{
	bool pressed;
	bool triggered;

	HIDInput() : pressed(false), triggered(false){
	}

	void push(){
		pressed = true;
		triggered = true;
	}

	void release(){
		pressed = false;
		triggered = false;
	}

	void resetTriggered(){
		triggered = false;
	}
};

//TODO: Split into input and devices (Keyboard / Mouse / Ctrl etc)

class Input {

private:	
	Input(const Input& input) = delete;
	std::array <HIDInput<MouseButtons>, (size_t)MouseButtons::BUTTON_SIZE> mouseButtons;
	
public:
	std::vector<KeyEvent> keyEvents;

	unsigned int mouse_x;
	unsigned int mouse_y;

	/*Relative*/
	int rel_mouse_x;
	int rel_mouse_y;

	Input();
	
	void pressKey(KeyButtons key);
	
	//rename to releaseKey
	void letgoKey(KeyButtons key);
	
	/*************************MOUSE***********************************/

	void pressButton(MouseButtons key);
	
	//rename to releaseKey
	void letgoButton(MouseButtons key);

	/*One-time key-press (gets reset each frame)*/
	bool getTriggeredButton(MouseButtons key) const;
	
	/*Current status of the key*/
	bool getButton(MouseButtons key) const;
	
	void resetFrame();
	void reset();
	void updateMouse(unsigned int x, unsigned int y);
	
	int getMouseRelX();
	int getMouseRelY();
	
	bool mouseHasMoved();
};

}

#endif // INPUT_H
