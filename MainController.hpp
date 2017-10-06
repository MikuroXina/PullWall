
struct Data;

class MainController {
private:
	Data *data;

	void checkSDLError();
	void renderText(char const*, double, double, double, double);
public:
	MainController();
	~MainController();

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
