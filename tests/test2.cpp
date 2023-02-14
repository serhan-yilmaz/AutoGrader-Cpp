#include "MyClass.h"
#include <iostream>
using namespace std;
int main(){
	cout << "[Running] Test 2..." << endl;
	cout << "---------------------"<< endl;
	MyClass mc;
	mc.setValue(3);
	int a =  mc.getValue() == 3;
	cout << "---------------------"<< endl;
	cout << "[Done] Test 2." << endl;
	return a;
}
