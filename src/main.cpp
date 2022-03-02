#include "window.hpp"

int main() {

	Window* window = Window::get();
	window->run();

	return 0;
}