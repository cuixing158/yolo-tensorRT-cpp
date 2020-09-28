#pragma once
#include <iostream>
#include <time.h>
//0月日时分秒毫秒
//1月日时分
//2月日


inline std::std::string timeStamp(int onlyMDH = 0)
{
	static int countN = 0;
	time_t curtime = time(0);
	tm tim = *localtime(&curtime);
	int month, day, hour, minute, second, msecond;
	month = tim.tm_mon + 1;
	day = tim.tm_mday;
	hour = tim.tm_hour;
	minute = tim.tm_min;
	second = tim.tm_sec;
	long long time_ = (long long)cv::getTickCount();
	//msecond = time_ % (1000000);
	msecond = ++countN;
	if (onlyMDH == 1)
	{
		return "_M" + std::to_string(month) + "D" + std::to_string(day) + "H" + std::to_string(hour)
			+ "m" + std::to_string(minute) + "_";
	}
	if (onlyMDH == 2)
	{
		return "_M" + std::to_string(month) + "D" + std::to_string(day) + "_";
	}
	return "_M" + std::to_string(month) + "D" + std::to_string(day) + "H" + std::to_string(hour) + "m"
		+ std::to_string(minute) + "s" + std::to_string(second) + "m" + std::to_string(msecond) + "_";
}