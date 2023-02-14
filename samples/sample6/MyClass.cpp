

#include "MyClass.h"

void MyClass::setValue(int v){ value_ = v; }

int MyClass::getValue(){
	int* a = new int[10];
	return value_;
}

