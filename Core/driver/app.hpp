#pragma once

#include "PID.hpp"
#include "CANFD.hpp"

class Application
{
public:
	Application() : pid1(PID(true, 1)), pid2(PID(true, 1)), pid3(PID(true, 1)),  pid4(PID(true, 1)) {};
	int Initilized();
	int loop();
	//inline CANFD getCAN() { return canfd; }
private:
	PID pid1;
	PID pid2;
	PID pid3;
	PID pid4;
	//CANFD canfd;
};

extern Application app;
