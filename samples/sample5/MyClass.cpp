#include <chrono>
#include <thread>
#include "MyClass.h"

void MyClass::setValue(int v){ }

int MyClass::getValue(){
	std::this_thread::sleep_for(std::chrono::milliseconds(100000));
	return 2;
}


