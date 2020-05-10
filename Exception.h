#pragma once
#include <iostream>
using namespace std;

class Exception {
protected:
	int errorCode;
	Exception(int number) : errorCode(number) {}
public:
	virtual void ErrorText() {};
};

class BadInput : public Exception {
public:
	BadInput(int number) : Exception(number) {}
	int input(int min, int max) noexcept(false);
	string inputString();
	void ErrorText() override;
};
