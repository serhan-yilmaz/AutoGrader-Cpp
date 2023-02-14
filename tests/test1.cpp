#include "MyClass.h"
#include <iostream>
using namespace std;
int main(){
	cout << "[Running] Test 1..." << endl;
	cout << "---------------------"<< endl;
	MyClass mc;
	mc.setValue(2);
	int a = mc.getValue() == 2;
	cout << "---------------------"<< endl;
	cout << "[Done] Test 1." << endl;
	return a;
}
