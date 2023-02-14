#pragma once

#include <chrono>

class TimeReporter {
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	std::chrono::time_point<std::chrono::high_resolution_clock> end;

public:
	TimeReporter();

	void stop();
	void reset();
	double getMS();
	double getUS();
	double get();
	double report();
	double reportMS();
	double reportUS();
	void print();
	void printMS();
	void printUS();
};
