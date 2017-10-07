#include <string>

struct Data;

class MainController {
private:
	Data *data;

	void checkSDLError();
public:
	MainController();
	~MainController();

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
};
