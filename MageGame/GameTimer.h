///Game Timer
//Easy class to use for simple game timing, uses the windows performance timer if available
//just call getTime() anytime you want the time, easy right?
//uses doubles, but I've never had a problem with that...would be trivial to switch it to a long or something
#pragma once
class GameTimer  
{
public:
	GameTimer();
	virtual ~GameTimer();
	double getTime();
	double getElapsedTimeMil();
	double getElapsedTimeSec();

private:
	__int64			frequency;
	double			resolution;
	unsigned long	mm_timer_start;
	bool			performance_timer;
	__int64			performance_timer_start;
	double lastTime;
};
