#pragma once
#include "Timer.h"

class TestTimer
{
public:
	TestTimer()
	{
		timeBeginPeriod(1);
		timer::stopwatch::Test();
		timer::alarmclock::Test();
		timeEndPeriod(1);
	}
};