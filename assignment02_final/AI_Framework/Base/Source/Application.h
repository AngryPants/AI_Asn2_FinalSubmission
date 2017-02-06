#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"

struct GLFWwindow;

class Application
{
public:
	static Application& GetInstance()
	{
		static Application app;
		return app;
	}
	void Init();
	void Run();
	void Exit();

	void UpdateInput();
	void PostInputUpdate();
	
	static void MouseButtonCallbacks(GLFWwindow* window, int button, int action, int mods);

	int GetWindowHeight() const;
	int GetWindowWidth() const;
	float GetWindowAspectRatio() const;

private:
	Application();
	~Application();

	static bool IsKeyPressed(unsigned short key);

	enum SUB_UPDATES {
		SU_WINDOW_SIZE,
		NUM_SUB_UPDATES,
	};
	
	double elapsedTime;
	double accumulatedTime[NUM_SUB_UPDATES];
	void RunSubUpdates();

	int m_window_width = 1200;
	int m_window_height = 900;

	//Declare a window object
	StopWatch m_timer;
};

#endif