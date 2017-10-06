#include <iostream>

#include "Inputter.hpp"
#include "MainController.hpp"

void Inputter::update() {
	while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					std::cout<<"Pressed ESC key!"<<std::endl;
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
				}
			}
		}
}
