#include <iostream>

#include "Inputter.hpp"
#include "MainController.hpp"

void Inputter::update() {
	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				controller->quit();
				break;
			case SDLK_v:
				controller->grabWall();
				break;
			case SDLK_b:
				controller->releaseWall();
				break;
			case SDLK_DOWN:
				controller->moveDown();
				break;
			case SDLK_UP:
				controller->moveUp();
				break;
			case SDLK_LEFT:
				controller->moveLeft();
				break;
			case SDLK_RIGHT:
				controller->moveRight();
				break;
			case SDLK_SPACE:
				controller->moveLock();
				break;
			}
		}

		if (event.type == SDL_KEYUP) {
			switch (event.key.keysym.sym) {
			case SDLK_SPACE:
				controller->moveUnlock();
				break;
			}
		}
	}
}
