#pragma once
#include "Point.h"

class CheckPoint {
private:
	const static int NEVER = -2;
	Point p;
	int lastSeen;
public:
	CheckPoint(Point p, int lastSeen) : p(p), lastSeen(lastSeen){}
};
