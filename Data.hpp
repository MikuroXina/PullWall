#include <OpenGL/gl.h>
#include <SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <SDL_ttf.h>

enum struct Direction {
	Up=0,
	Right,
	Down,
	Left,
};

struct Data {
public:
	bool quit = false;
	int playerPosX = 9;
	int playerPosY = 4;
	bool walls[10][10]{};
	bool isGrabWall = false;
	bool isSneaking = false;
	Direction playerDir = Direction::Up;
	unsigned int elapsedSteps = 0;
	unsigned int wallMovedTimes = 0;

	SDL_Window *window{};
	SDL_GLContext context = NULL;
	SDL_Renderer *renderer = nullptr;
	SDL_Surface *wallTex = nullptr;
	SDL_Surface *playerTex = nullptr;

	TTF_Font *font;
};
