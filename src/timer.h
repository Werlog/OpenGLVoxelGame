class Timer
{
public:

	Timer();

	double deltaTime;

	void tick();
private:
	double lastTime;
};