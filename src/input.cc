#include "input.h"

void Input::beginNewFrame() {
	pressed_keys.clear();
	released_keys.clear();
}

void Input::keyDownEvent(const SDL_Event& event) {
	pressed_keys[event.key.keysym.scancode] = true;
	held_keys[event.key.keysym.scancode] = true;
}

void Input::keyUpEvent(const SDL_Event& event) {
	released_keys[event.key.keysym.scancode] = true;
	held_keys[event.key.keysym.scancode] = false;
}

bool Input::wasKeyPressed(SDL_Scancode key) {
	return pressed_keys[key];
}

bool Input::wasKeyReleased(SDL_Scancode key) {
	return released_keys[key];
}

bool Input::isKeyHeld(SDL_Scancode key) {
	return held_keys[key];
}
