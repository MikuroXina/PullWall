#include <iostream>
#include <random>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <SDL_ttf.h>
#include <SDL_image.h>

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

MainController::MainController() {
	data = new Data;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cout<<std::string(SDL_GetError())<<std::endl;
	}

	TTF_Init();
	IMG_Init(IMG_INIT_PNG);

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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glViewport(0, 0, width, height);

	data->font = TTF_OpenFont("PixelMplus12-Regular.ttf", 24);
	if (data->font == nullptr) {
		std::cout << "Unable to open the font: " << TTF_GetError() << std::endl;
		throw -1;
	}

	data->renderer = SDL_CreateRenderer(data->window, -1, SDL_RENDERER_ACCELERATED);

	data->wallTex = IMG_Load("WallH.png");
	data->playerTex = IMG_Load("Player.png");

	// Init randomize
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> dice(0, 1);
	for (auto &column : data->walls) {
		for (auto &line : column) {
			line = (dice(mt) == 0 ? true : false);
		}
	}
}

MainController::~MainController() {
	SDL_FreeSurface(data->wallTex);
	SDL_FreeSurface(data->playerTex);
	SDL_DestroyRenderer(data->renderer);

	SDL_GL_DeleteContext(data->context);
	SDL_DestroyWindow(data->window);

	TTF_CloseFont(data->font);
	data->font = nullptr;

	TTF_Quit();
	SDL_Quit();

	delete data;
}

int MainController::run() {
	Inputter inputter{};
	inputter.controller = this;
	updateDisplay();
	while (!(data->quit)) {
		inputter.update();
	}

	return 0;
}

void MainController::quit() {
	std::cout<<"Quitting..."<<std::endl;
	data->quit = true;
}

void MainController::clear() {
	// Render sprite

	SDL_Delay(3000);
	quit();
}

void MainController::updateDisplay() {
	glClearColor(0.0, 0.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	std::cout << "Rendering score..." << std::endl;

	SDL_SetRenderDrawColor(data->renderer, 0, 0, 0, 255);
	SDL_RenderClear(data->renderer);

	// Draw walls
	{
		SDL_Rect wallRect;

		SDL_Texture *wallTexBuf = SDL_CreateTextureFromSurface(data->renderer, data->wallTex);
		for (int i=0; i<10; i+=1) {
			if (i%2 == 0) {
				for (int j=0; j<9; j+=1) { // Vertical walls
					if (data->walls[i][j]) {
						wallRect.x = j * 100 + 50;
						wallRect.y = i * 50 + 40;
						wallRect.w = 80;
						wallRect.h = 10;
						SDL_RenderCopyEx(data->renderer, wallTexBuf, NULL, &wallRect, 90, NULL, SDL_FLIP_NONE);
					}
				}
			} else {
				for (int j=0; j<10; j+=1) { // Horizontal walls
					if (data->walls[i][j]) {
						wallRect.x = j * 100;
						wallRect.y = i * 50 + 40;
						wallRect.w = 80;
						wallRect.h = 10;
						SDL_RenderCopy(data->renderer, wallTexBuf, NULL, &wallRect);
					}
				}
			}
		}
		SDL_DestroyTexture(wallTexBuf);
	}

	// Draw player
	{
		SDL_Rect playerRect;
		playerRect.x = data->playerPosX * 100 + 10;
		playerRect.y = data->playerPosY * 100 + 10;
		playerRect.w = 90;
		playerRect.h = 90;

		SDL_Texture *playerTexBuf = SDL_CreateTextureFromSurface(data->renderer, data->playerTex);
		switch (data->playerDir) {
		case Direction::Up:
			SDL_RenderCopyEx(data->renderer, playerTexBuf, NULL, &playerRect, 0, NULL, SDL_FLIP_NONE);
			break;
		case Direction::Right:

			break;
		case Direction::Down:

			break;
		case Direction::Left:

			break;
		}
	}

	// Draw socre
	SDL_Color white = {0xff, 0xff, 0xff};
	char const *text = ("Step: " + std::to_string(data->elapsedSteps) + " , Moved: " + std::to_string(data->wallMovedTimes)).c_str();
	SDL_Surface *stepImage = TTF_RenderText_Solid(data->font, text, white);
	SDL_Texture *stepTex = SDL_CreateTextureFromSurface(data->renderer, stepImage);

	SDL_Rect stepViewRect;
	stepViewRect.x = 0;
	stepViewRect.y = 0;
	stepViewRect.w = 100;
	stepViewRect.h = 100;

	TTF_SizeText(data->font, text, &(stepViewRect.w), &(stepViewRect.h));

	SDL_RenderCopy(data->renderer, stepTex, NULL, &stepViewRect);

	SDL_DestroyTexture(stepTex);
	SDL_FreeSurface(stepImage);

	SDL_GL_SwapWindow(data->window);

	SDL_Delay(1);
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

void MainController::grabWall() {
	;
}

void MainController::releaseWall() {
	;
}
