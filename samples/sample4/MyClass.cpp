#include <stdlib.h>
#include "MyClass.h"

void MyClass::setValue(int v){ }

int MyClass::getValue(){
	int* a;
	delete a;
	abort();
	return 2;
}


