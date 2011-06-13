// GameTimer.cpp: implementation of the GameTimer class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <mmsystem.h>
#include "GameTimer.h"

#pragma comment(lib, "winmm") 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GameTimer::GameTimer()
{
	if(!QueryPerformanceFrequency((LARGE_INTEGER*)&frequency)){
		performance_timer = false;
		mm_timer_start = timeGetTime();
		resolution = 1.0f/1000.0f;
		frequency = 1000;
		lastTime = getTime();
	}
	else{
		QueryPerformanceCounter((LARGE_INTEGER*)&performance_timer_start);
		performance_timer = true;
		resolution = double(double(1)/double(frequency));
		lastTime = getTime();
	}


		
}

GameTimer::~GameTimer()
{

	

}

double GameTimer::getTime(){
	__int64 time;
	if(performance_timer){
		QueryPerformanceCounter((LARGE_INTEGER*)&time);
		return double(time-performance_timer_start) * resolution;
	}
	else
	{
		return double(timeGetTime()-mm_timer_start) * resolution;
	}

}
static bool first = true;
double GameTimer::getElapsedTimeMil(){
	if(first){
		first = false;
		lastTime = getTime();
		return 0;
	}
		
	double currentTime = getTime();
	double elapsedTime = currentTime-lastTime;
	lastTime = currentTime;
	return elapsedTime*1000;
}

double GameTimer::getElapsedTimeSec(){
	return getElapsedTimeMil()/1000.0;
}


