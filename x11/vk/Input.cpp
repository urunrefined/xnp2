#include "Input.h"

namespace BR
{

Input::Input() :  mouse_x(0), mouse_y(0), rel_mouse_x(0.0f), rel_mouse_y(0.0f) {}

void Input::pressKey(KeyButtons key){
	keyButtons[(size_t)key].push();
};

//rename to releaseKey
void Input::letgoKey(KeyButtons key) {
	keyButtons[(size_t)key].release();
};

/*One-time key-press (gets reset each frame)*/
bool Input::getTriggeredKey(KeyButtons key) const {
	return keyButtons[(size_t)key].triggered;
};

/*Current status of the key*/
bool Input::getKey(KeyButtons key) const {
	return keyButtons[(size_t)key].pressed;
};

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

void Input::resetFrame(){
	rel_mouse_x = 0;
	rel_mouse_y = 0;
	
	for(HIDInput<KeyButtons>& key : keyButtons){
		key.resetTriggered();
	}
	
	for(HIDInput<MouseButtons>& button : mouseButtons){
		button.resetTriggered();
	}
}

void Input::reset(){
	mouse_x = 0;
	mouse_y = 0;
	
	rel_mouse_x = 0;
	rel_mouse_y = 0;
	
	for(HIDInput<KeyButtons>& key : keyButtons){
		key.resetTriggered();
		key.release();
	}
	
	for(HIDInput<MouseButtons>& button : mouseButtons){
		button.resetTriggered();
		button.release();
	}
	
}

void Input::updateMouse(unsigned int x, unsigned int y){
	int rel_mouse_x_a = (int) x - (int) mouse_x;
	int rel_mouse_y_a = (int) y - (int) mouse_y;
	
	rel_mouse_y_a = -rel_mouse_y_a;
	
	mouse_x = x;
	mouse_y = y;
	
	rel_mouse_x += rel_mouse_x_a;
	rel_mouse_y += rel_mouse_y_a;

}

int Input::getMouseRelX(){ return rel_mouse_x;}
int Input::getMouseRelY(){ return rel_mouse_y;}

bool Input::mouseHasMoved(){
	return rel_mouse_x || rel_mouse_y;
}

}
