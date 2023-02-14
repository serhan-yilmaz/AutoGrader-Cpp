#include <stdlib.h>
#include "MyClass.h"

void MyClass::setValue(int v){
	while( v == 3 ){
		value_ = v;
	}
}

int MyClass::getValue(){
	return value_;
}


