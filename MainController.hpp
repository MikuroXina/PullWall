
class Data;

class MainController {
private:
	Data *data;

	void renderText(char const*, double, double, double, double);
public:
	MainController();

	int run();

	// Move events
	void moveLeft();
	void moveDown();
	void moveRight();
	void moveUp();

	// Other events
	void clear();
	void updateDisplay();
};
