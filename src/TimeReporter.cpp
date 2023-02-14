#include "TimeReporter.h"

#include <iostream>

TimeReporter::TimeReporter()
{
	reset();
}

void TimeReporter::stop()
{
	end = std::chrono::high_resolution_clock::now();
}

void TimeReporter::reset()
{
	start = std::chrono::high_resolution_clock::now();
	end = std::chrono::high_resolution_clock::now();
}

double TimeReporter::getMS()
{
	return getUS() * 1e-3;
}

double TimeReporter::getUS()
{
	auto elapsed = end - start;
	long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	return microseconds;
}

double TimeReporter::get()
{
	return getUS() * 1e-6;
}

double TimeReporter::report()
{
	stop();
	return get();
}

double TimeReporter::reportMS()
{
	stop();
	return getMS();
}

double TimeReporter::reportUS()
{
	stop();
	return getUS();
}

void TimeReporter::print()
{
	std::cout << "Time passed : " << get() << " s" << std::endl;
}

void TimeReporter::printMS()
{
	std::cout << "Time passed : " << getMS() << " ms" << std::endl;
}

void TimeReporter::printUS()
{
	std::cout << "Time passed : " << getUS() << " us" << std::endl;
}
