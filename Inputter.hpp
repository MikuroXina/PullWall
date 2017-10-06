#include <SDL.h>

class Inputter {
private:
	SDL_Event event;
	class MainController *controller;
public:
	void update();
}
