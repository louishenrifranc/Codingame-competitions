#include <iostream>
#include "Utils.h"
using namespace std;


int main()
{
	Point p(0, 0);
	cerr << Utils::runawaYPoint(0,9000,0,0, 800).X << " " << Utils::runawaYPoint(0, 9000, 0, 0, 800).Y << endl;
	
	system("pause");
	return 0;
}