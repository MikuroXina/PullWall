#include <SDL.h>

class Inputter {
private:
	SDL_Event event;
public:
	class MainController *controller;
	
	void update();
};
