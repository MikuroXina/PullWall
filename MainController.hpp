#include <string>

#include "SoundManager.hpp"

struct Data;

class MainController {
private:
	Data *data;
	SoundManager sound{};

	void checkSDLError();
public:
	MainController();
	~MainController();

	void init();

	int run();

	// Move events
	void moveLeft();
	void moveDown();
	void moveRight();
	void moveUp();

	void moveLock();
	void moveUnlock();

	// Grab events
	void grabWall();
	void releaseWall();

	// Other events
	void quit();
	void clear();
	void updateDisplay();

	Data const* getData() const { return data; }
};
