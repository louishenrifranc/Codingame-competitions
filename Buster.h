#pragma once
class Buster {
public:
	int id;
	int X, Y;
	int isCarryingGhost;
	int remainingStunDuration;
	int remainingStunCoolDown;
	int lastSeen;
	int ghostID;
	Buster(int id, int x, int y, bool isCarryingGhost, int remainingStunDuration, int remainStunCoolDown, int ghostID, int lastSeen)
		: id(id), X(x), Y(y), isCarryingGhost(isCarryingGhost), remainingStunDuration(remainingStunDuration), remainingStunCoolDown(remainStunCoolDown), lastSeen(lastSeen), ghostID(ghostID) {};

	int getId() const { return id; };

	bool hasStun() const { return remainingStunDuration == 0; };
	
	bool operator==(const Buster& rhs) const {
		if (this == &rhs)
			return true;
		if (rhs.id != id) return false;
		if (rhs.X != X) return false;
		if (rhs.Y != Y) return false;
		if (rhs.isCarryingGhost != isCarryingGhost) return false;
		if (rhs.remainingStunDuration != remainingStunDuration) return false;
		if (rhs.remainingStunCoolDown != remainingStunCoolDown) return false;
		if (lastSeen != rhs.lastSeen) return false;
		if (ghostID != rhs.ghostID) return false;
	}

};