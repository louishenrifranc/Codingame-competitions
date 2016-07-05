#pragma once

#pragma once
class Ghost {
public:
	int id;
	int X, Y;
	int stamina;
	int bustCnt;
	Ghost(int id, int x, int y, int stamina, int bustCnt) : id(id), X(x), Y(y), stamina(stamina), bustCnt(bustCnt) {};
	bool operator==(const Ghost& rhs) const {
		if (this == &rhs)
			return true;
		if (rhs.id != id) return false;
		if (rhs.X != X) return false;
		if (rhs.Y != Y) return false;
		if (rhs.stamina != stamina) return false;
		if (bustCnt != rhs.bustCnt) return false;
	}

};