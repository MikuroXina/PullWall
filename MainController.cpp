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

	// Setup font
	data->font = TTF_OpenFont("PixelMplus12-Regular.ttf", 24);
	if (data->font == nullptr) {
		std::cout << "Unable to open the font: " << TTF_GetError() << std::endl;
		throw -1;
	}

	data->renderer = SDL_CreateRenderer(data->window, -1, SDL_RENDERER_ACCELERATED);

	// Setup texture
	data->wallTex = IMG_Load("WallH.png");
	data->playerTex = IMG_Load("Player.png");

	// Setup sounds
	sound.registerSound("Step1.wav"); // Id:0
	sound.registerSound("Step2.wav"); // Id:1
	sound.registerSound("Clear.wav"); // Id:2
	sound.registerSound("Grab.wav"); // Id:3
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

void MainController::init() {
	data->quit = false;
	data->clear = false;
	data->playerPosX = 9;
	data->playerPosY = 4;
	data->isGrabWall = false;
	data->isSneaking = false;
	data->playerDir = Direction::Up;
	data->elapsedSteps = 0;
	data->wallMovedTimes = 0;
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

int MainController::run() {
	Inputter inputter{};
	inputter.controller = this;
	updateDisplay();
	while (!(data->quit)) {
		inputter.update();
	}

	return data->clear ? 0 : 1;
}

void MainController::quit() {
	std::cout<<"Quitting..."<<std::endl;
	data->quit = true;
}

void MainController::clear() {
	int score = data->elapsedSteps * 10 - data->wallMovedTimes * 30;

	// Render sprite
	SDL_Color yellow = {0xff, 0xff, 0x00};
	SDL_Color red = {0xff, 0x00, 0x00};
	SDL_Color green = {0x00, 0xff, 0x00};
	char const *text1 = "CLEAR!";
	char const *text2 = ("SCORE : " + std::to_string(score)).c_str();
	SDL_Surface *spriteImage1 = TTF_RenderText_Solid(data->font, text1, yellow);
	SDL_Texture *spriteTex1 = SDL_CreateTextureFromSurface(data->renderer, spriteImage1);
	SDL_Surface *spriteImage2 = TTF_RenderText_Solid(data->font, text2, (score < 0 ? red : green));
	SDL_Texture *spriteTex2 = SDL_CreateTextureFromSurface(data->renderer, spriteImage2);

	SDL_Rect spriteViewRect1;
	spriteViewRect1.x = 400;
	spriteViewRect1.y = 200;
	spriteViewRect1.w = 5000;
	spriteViewRect1.h = 5000;

	SDL_Rect spriteViewRect2;
	spriteViewRect2.x = 400;
	spriteViewRect2.y = 250;
	spriteViewRect2.w = 5000;
	spriteViewRect2.h = 5000;

	TTF_SizeText(data->font, text1, &(spriteViewRect1.w), &(spriteViewRect1.h));
	TTF_SizeText(data->font, text2, &(spriteViewRect2.w), &(spriteViewRect2.h));

	SDL_RenderCopy(data->renderer, spriteTex1, NULL, &spriteViewRect1);
	SDL_RenderCopy(data->renderer, spriteTex2, NULL, &spriteViewRect2);

	SDL_DestroyTexture(spriteTex1);
	SDL_FreeSurface(spriteImage1);
	SDL_DestroyTexture(spriteTex2);
	SDL_FreeSurface(spriteImage2);

	SDL_GL_SwapWindow(data->window);

	sound.playSound(2);

	SDL_Delay(3000);
	data->clear = true;
	quit();
}

void MainController::updateDisplay() {
	SDL_SetRenderDrawColor(data->renderer, 0, 0, 0, 255);
	SDL_RenderClear(data->renderer);

	// Draw walls
	{
		SDL_Rect wallRect;

		SDL_Texture *wallTexBuf = SDL_CreateTextureFromSurface(data->renderer, data->wallTex);
		for (int i=0; i<9; i+=1) {
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
		if (data->isSneaking) {
			SDL_SetRenderDrawColor(data->renderer, 192, 0, 0, 255);
			SDL_Point points[] = {
				{data->playerPosX * 100 + 10, data->playerPosY * 100 + 10},
				{data->playerPosX * 100 + 80, data->playerPosY * 100 + 10},
				{data->playerPosX * 100 + 80, data->playerPosY * 100 + 80},
				{data->playerPosX * 100 + 10, data->playerPosY * 100 + 80},
				{data->playerPosX * 100 + 10, data->playerPosY * 100 + 10}
			};
			SDL_RenderDrawLines(data->renderer, points, 5);
		}

		if (data->isGrabWall) {
			SDL_SetRenderDrawColor(data->renderer, 192, 255, 0, 255);
			SDL_Point points[] = {
				{data->playerPosX * 100 + 20, data->playerPosY * 100 + 20},
				{data->playerPosX * 100 + 70, data->playerPosY * 100 + 20},
				{data->playerPosX * 100 + 70, data->playerPosY * 100 + 70},
				{data->playerPosX * 100 + 20, data->playerPosY * 100 + 70},
				{data->playerPosX * 100 + 20, data->playerPosY * 100 + 20}
			};
			SDL_RenderDrawLines(data->renderer, points, 5);
		}

		SDL_Rect playerRect;
		playerRect.x = data->playerPosX * 100 + 10;
		playerRect.y = data->playerPosY * 100 + 10;
		playerRect.w = 70;
		playerRect.h = 70;

		SDL_Texture *playerTexBuf = SDL_CreateTextureFromSurface(data->renderer, data->playerTex);
		switch (data->playerDir) {
		case Direction::Up:
			SDL_RenderCopyEx(data->renderer, playerTexBuf, NULL, &playerRect, 0, NULL, SDL_FLIP_NONE);
			break;
		case Direction::Right:
			SDL_RenderCopyEx(data->renderer, playerTexBuf, NULL, &playerRect, 90, NULL, SDL_FLIP_NONE);
			break;
		case Direction::Down:
			SDL_RenderCopyEx(data->renderer, playerTexBuf, NULL, &playerRect, 180, NULL, SDL_FLIP_NONE);
			break;
		case Direction::Left:
			SDL_RenderCopyEx(data->renderer, playerTexBuf, NULL, &playerRect, 270, NULL, SDL_FLIP_NONE);
			break;
		}

		SDL_DestroyTexture(playerTexBuf);

		if (data->playerPosX == 0 && data->playerPosY == 0) {
			clear();
		}
	}

	// Draw socre
	SDL_Color green = {0x00, 0xff, 0x00};
	char const *text = ("Step: " + std::to_string(data->elapsedSteps) + " , Moved: " + std::to_string(data->wallMovedTimes)).c_str();
	SDL_Surface *stepImage = TTF_RenderText_Solid(data->font, text, green);
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
	if (!(data->isSneaking) && data->playerPosX > 0) {
		if (!(data->walls[(data->playerPosY * 2)][(data->playerPosX - 1)])) {
			data->playerPosX -= 1;
			data->elapsedSteps += 1;
			if (data->playerDir == Direction::Left) {
				data->wallMovedTimes += 1;
			}
			sound.playSound(0);
		} else {
			sound.playSound(1);
		}
	}
	if (data->isGrabWall && data->playerDir != Direction::Left) {
		data->wallMovedTimes += 1;
	}
	data->playerDir = Direction::Left;
	updateDisplay();
}

void MainController::moveDown() {
	if (!(data->isSneaking) && data->playerPosY < 4) {
		if (!(data->walls[(data->playerPosY * 2 + 1)][data->playerPosX])) {
			data->playerPosY += 1;
			data->elapsedSteps += 1;
			if (data->playerDir == Direction::Down) {
				data->wallMovedTimes += 1;
			}
			sound.playSound(0);
		} else {
			sound.playSound(1);
		}
	}
	if (data->isGrabWall && data->playerDir != Direction::Down) {
		data->wallMovedTimes += 1;
	}
	data->playerDir = Direction::Down;
	updateDisplay();
}

void MainController::moveRight() {
	if (!(data->isSneaking) && data->playerPosX < 9) {
		if (!(data->walls[(data->playerPosY * 2)][data->playerPosX])) {
			data->playerPosX += 1;
			data->elapsedSteps += 1;
			if (data->playerDir == Direction::Right) {
				data->wallMovedTimes += 1;
			}
			sound.playSound(0);
		} else {
			sound.playSound(1);
		}
	}
	if (data->isGrabWall && data->playerDir != Direction::Right) {
		data->wallMovedTimes += 1;
	}
	data->playerDir = Direction::Right;
	updateDisplay();
}

void MainController::moveUp() {
	if (!(data->isSneaking) && data->playerPosY > 0) {
		if (!(data->walls[(data->playerPosY * 2 - 1)][data->playerPosX])) {
			data->playerPosY -= 1;
			data->elapsedSteps += 1;
			if (data->playerDir == Direction::Up) {
				data->wallMovedTimes += 1;
			}
			sound.playSound(0);
		} else {
			sound.playSound(1);
		}
	}
	if (data->isGrabWall && data->playerDir != Direction::Up) {
		data->wallMovedTimes += 1;
	}
	data->playerDir = Direction::Up;
	updateDisplay();
}

void MainController::moveLock() {
	data->isSneaking = true;
	updateDisplay();
}

void MainController::moveUnlock() {
	data->isSneaking = false;
	updateDisplay();
}

void MainController::grabWall() {
	if (!(data->isGrabWall)) {
		switch (data->playerDir) {
		case Direction::Left:
			data->walls[(data->playerPosY * 2)][(data->playerPosX - 1)] = false;
			break;
		case Direction::Down:
			data->walls[(data->playerPosY * 2 + 1)][data->playerPosX] = false;
			break;
		case Direction::Right:
			data->walls[(data->playerPosY * 2)][data->playerPosX] = false;
			break;
		case Direction::Up:
			data->walls[(data->playerPosY * 2 - 1)][data->playerPosX] = false;
			break;
		}

		data->isGrabWall = true;
		sound.playSound(3);
		updateDisplay();
	}
}

void MainController::releaseWall() {
	if (data->isGrabWall) {
		switch (data->playerDir) {
		case Direction::Left:
			if (!(data->walls[(data->playerPosY * 2)][(data->playerPosX - 1)])) {
				data->walls[(data->playerPosY * 2)][(data->playerPosX - 1)] = true;
				data->isGrabWall = false;
			}
			break;
		case Direction::Down:
			if (!(data->walls[(data->playerPosY * 2 + 1)][data->playerPosX])) {
				data->walls[(data->playerPosY * 2 + 1)][data->playerPosX] = true;
				data->isGrabWall = false;
			}
			break;
		case Direction::Right:
			if (!(data->walls[(data->playerPosY * 2)][data->playerPosX])) {
				data->walls[(data->playerPosY * 2)][data->playerPosX] = true;
				data->isGrabWall = false;
			}
			data->walls[(data->playerPosY * 2)][data->playerPosX] = true;
			break;
		case Direction::Up:
			if (!(data->walls[(data->playerPosY * 2 - 1)][data->playerPosX])) {
				data->walls[(data->playerPosY * 2 - 1)][data->playerPosX] = true;
				data->isGrabWall = false;
			}
			data->walls[(data->playerPosY * 2 - 1)][data->playerPosX] = true;
			break;
		}

		sound.playSound(3);
		updateDisplay();
	}
}
