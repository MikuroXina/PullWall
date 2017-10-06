#include <OpenGL/gl.h>
#include <SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H

struct Data {
public:
	bool quit = false;
	int playerPosX = 9;
	int playerPosY = 0;
	bool walls[10][5]{};
	GLuint wallTexID=0;
	GLuint playerTexID=1;

	SDL_Window *window{};
	SDL_GLContext context = NULL;

	FT_Face face;
};
