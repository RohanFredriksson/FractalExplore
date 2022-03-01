#include "window.hpp"

int main() {

	Window* window = Window::get();
	window->init();
	window->loop();
	window->exit();

	return 0;
}