#include <OpenGL/gl.h>
#include <SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <SDL_ttf.h>

struct Data {
public:
	bool quit = false;
	int playerPosX = 9;
	int playerPosY = 0;
	bool walls[10][5]{};
	GLuint wallTexID=0;
	GLuint playerTexID=1;

	unsigned int elapsedSteps = 0;
	unsigned int wallMovedTimes = 0;

	SDL_Window *window{};
	SDL_GLContext context = NULL;

	TTF_Font *font;
};
