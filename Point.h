#pragma once

class Point{
public:
	int X, Y;
	Point(const int& X, const int& Y) : X(X), Y(Y) {};

	bool operator==(const Point& rhs) {
		if (this == &rhs)
			return true;
		return (rhs.X == X && rhs.Y == Y);
	}

};