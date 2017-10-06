#include <iostream>

#include "MainController.hpp"
#include "Data.hpp"
#include "Inputter.hpp"

void MainController::checkSDLError() {
	const char *error = SDL_GetError();
	if (*error != '\0') {
		std::cout<<"SDL Error:"<<error<<std::endl;
		SDL_ClearError();
	}
}

void MainController::renderText(char const* text, double x, double y, double sx, double sy) {

}

MainController::MainController() {
	data = new Data;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cout<<std::string(SDL_GetError())<<std::endl;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	constexpr Uint32 width = 1000;
	constexpr Uint32 height = 500;
	data->window = SDL_CreateWindow("Pull Wall", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!(data->window)) {
		std::cout<<"Unable to create a window!"<<std::endl;
	}
	checkSDLError();

	data->context = SDL_GL_CreateContext(data->window);
	checkSDLError();

	SDL_GL_SetSwapInterval(1);

	glViewport(0, 0, width, height);
}

MainController::~MainController() {
	delete data;
}

int MainController::run() {
	Inputter inputter{};
	inputter.controller = this;
	while (!(data->quit)) {
		inputter.update();
	}

	return 0;
}

void MainController::quit() {
	std::cout<<"Quitting..."<<std::endl;
	data->quit = true;
}

void MainController::moveLeft() {
	;
}

void MainController::moveDown() {
	;
}

void MainController::moveRight() {
	;
}

void MainController::moveUp() {
	;
}
