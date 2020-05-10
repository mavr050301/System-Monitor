#include <iostream>
#include <string>
#include "Exception.h"
using namespace std;

int BadInput::input(int min, int max)
{
	bool flag = true;
	int value;
	do
	{
		try
		{
			cin >> value;
			flag = true;
			if (!cin || cin.peek() != '\n')
			{
				flag = false;
				throw BadInput(1);
			}
			if (value<min || value>max)
			{
				flag = false;
				throw BadInput(3);
			}
		}
		catch (BadInput err)
		{
			rewind(stdin);
			err.ErrorText();
			if (err.errorCode == 3) {
				cout << "You must enter a value in the range(" << min << "," << max << ")." << endl;
			}
			cin.sync();
			cin.clear();
		}
		catch (...) {
			cout << "\nError\n";
			cin.sync();
			cin.clear();
		}
	} while (!flag);
	return value;
}
void BadInput::ErrorText()
{
	cout << "Input Error (code " << this->errorCode << "): ";
	switch (this->errorCode)
	{
	case 1:
	{
		cout << "Invalid character entered." << endl;
		cout << "Entering any characters except numbers is prohibited." << endl;
		break;
	}
	case 2:
	{
		cout << "Name length limit exceeded!" << endl;
		cout << "You can enter up to 10 characters" << endl;
		break;
	}
	case 3:
	{
		cout << "The number entry limit has been exceeded!" << endl;
		break;
	}
	case 4:
	{
		cout << "You have entered the wrong character set!Only letter input is available" << endl;
		break;
	}
	default:
		cout << "Repeat Entry" << endl;
		break;
	}
}
string BadInput::inputString() {
	string st;
	int i = 0, len = 0;
	bool flag = true;
	rewind(stdin);
	while (flag) {
		try {
			getline(cin, st);
			len = st.length();
			if (len == 0) {
				throw BadInput(4);
			}
			for (int i = 0; i < len; i++) {
				if (st[i] == ' ') {
					throw BadInput(4);
				}
				if ((st[i]<'a' || st[i]>'z') && (st[i]<'A' || st[i]>'Z') && (st[i]>'1' || st[i]<'9')) {
					throw BadInput(4);
				}
			}
			if (len >= 10) {
				throw BadInput(2);
			}
			flag = false;
		}
		catch (BadInput err) {
			rewind(stdin);
			err.ErrorText();
			cin.clear();
			cin.sync();
		}
		catch (...) {
			cout << "\nError\n";
			cin.sync();
			cin.clear();
		}
	}
	return st;
}
