#include "Input.h"

namespace BR {

Input::Input() {}

void Input::pressKey(KeyButtons key) {
    keyEvents.push_back(KeyEvent(key, PRESSED));
    keyButtons[(size_t)key].push();
};

// rename to releaseKey
void Input::letgoKey(KeyButtons key) {
    keyEvents.push_back(KeyEvent(key, RELEASED));
    keyButtons[(size_t)key].release();
};

/*Current status of the key*/
bool Input::getButton(KeyButtons key) const {
    return keyButtons[(size_t)key].pressed;
}

/*************************MOUSE***********************************/

void Input::pressButton(MouseButtons key) { mouseButtons[(size_t)key].push(); };

// rename to releaseKey
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

void Input::reset() { keyEvents.clear(); }

void Input::moveMouse(float x, float y) {
    mouse_x += x * 0.1;
    mouse_y += y * 0.1;

    if (mouse_x < 0)
        mouse_x = 0;
    if (mouse_y < 0)
        mouse_y = 0;

    if (mouse_x >= 640)
        mouse_x = 640;
    if (mouse_y >= 400)
        mouse_y = 400;

    mouse_move_x = x;
    mouse_move_y = y;
}

void Input::getMouseCoords(unsigned int &x, unsigned int &y) const {
    x = std::min((unsigned int)mouse_x, (unsigned int)639);
    y = std::min((unsigned int)mouse_y, (unsigned int)399);
}

void Input::getMouseMove(short &x, short &y) const {
    x = (short)mouse_move_x;
    y = (short)mouse_move_y;
}

} // namespace BR
