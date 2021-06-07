#include "Input.h"

namespace BR
{

Input::Input() :  mouse_x(0), mouse_y(0) {}

void Input::pressKey(KeyButtons key){
	keyEvents.push_back(KeyEvent(key, PRESSED));
	keyButtons[(size_t)key].push();
};

//rename to releaseKey
void Input::letgoKey(KeyButtons key) {
	keyEvents.push_back(KeyEvent(key, RELEASED));
	keyButtons[(size_t)key].release();
};

/*Current status of the key*/
bool Input::getButton(KeyButtons key) const {
	return keyButtons[(size_t)key].pressed;
}

/*************************MOUSE***********************************/

void Input::pressButton(MouseButtons key){
	mouseButtons[(size_t)key].push();
};

//rename to releaseKey
void Input::letgoButton(MouseButtons key) {
	mouseButtons[(size_t)key].release();
};

/*One-time key-press (gets reset each frame)*/
bool Input::getTriggeredButton(MouseButtons key) const {
	return mouseButtons[(size_t)key].triggered;
};

/*Current status of the key*/
bool Input::getButton(MouseButtons key) const {
	return mouseButtons[(size_t)key].pressed;
};

void Input::reset(){
	keyEvents.clear();
}

void Input::updateMouse(unsigned int x, unsigned int y){
	mouse_x = x;
	mouse_y = y;
}

}
