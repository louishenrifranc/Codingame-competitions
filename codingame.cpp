#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <cmath>
#include <cassert>
#include <exception>
#include <chrono>
using namespace std;
struct playerDeadException : public exception {
	virtual const char* what() const throw() {
		return "";
	}
};
struct wonGameException : public exception {
	virtual const char* what() const throw() {
		return "";
	}
};
enum MOVES {
	MOVE = 1 << 1,
	SHOOT = 1 << 2
};


class Timer {
public:
	std::chrono::high_resolution_clock::time_point start;
	float limitCoefficient = 0.44;
	void reset() {
		start = std::chrono::high_resolution_clock::now();
	}
	bool stop() const
	{
		auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
		if (time > limitCoefficient * 100)
			return false;
		return true;
	}
};

struct Point {
	Point(const int& a, const int& b) : X(a), Y(b) {};
	Point() : X(-1), Y(-1) { };
	bool inMap() const
	{
		if (X < 0 || Y < 0 || X > 16001 || Y > 9001)
			return false;
		return true;
	}
	Point& operator=(const Point& rhs) {
		if (&rhs == this) {
			return *this;
		}
		X = rhs.X;
		Y = rhs.Y;
		return *this;
	}

	Point& operator+=(const Point& rhs) {
		X += rhs.X;
		Y += rhs.Y;
		return *this;
	}

	Point&  operator/=(const int& N) {
		X /= N;
		Y /= N;
		return *this;
	}


	void rotate(const Point& p, double theta = 3.14159)
	{
		X -= p.X;
		Y -= p.Y;
		X = p.X + X * cos(theta) - Y * sin(theta);
		Y = p.Y + X * sin(theta) + Y * cos(theta);
	}



	void moveWithinRange(const Point& point, const int& moveRange = 1000) {
		const int dist = distance(point);
		double newX = X, newY = Y;
		//! Point is inside the move limit range
		if (dist < moveRange) {
			newX = point.X;
			newY = point.Y;
		}
		else {
			//! Point is outside of move limit range
			double w = (double)(moveRange) / dist;
			newX += (point.X - newX) * w;
			newY += (point.Y - newY) * w;
		}
		roundTo(newX, point.X);
		roundTo(newY, point.Y);
		//! Normalize the point
		normalizeWithinRange();
		X = newX;
		Y = newY;
	}

	static void roundTo(double& point, const int& destination) {
		if (destination < point) {
			point = floor(point);
		}
		else {
			point = ceil(point);
		}
	}

	inline int distance2(const Point& rhs) const
	{
		return (X - rhs.X)*(X - rhs.X) + (Y - rhs.Y)*(Y - rhs.Y);
	}

	inline int distance(const Point& rhs) const {
		return sqrt(distance2(rhs));
	}

	int numberSteps(const Point* p, int sizeStep) const
	{
		int res = (int)(sqrt(distance2(*p)) - 1760) / sizeStep;
		if (res < 0)
			res = 0;
		if (res > 0)
			res++;
		return res;
	}
	void normalizeWithinRange()
	{
		if (X < 0) X = 0;
		if (X >= 16000) X = 16000;
		if (Y < 0) Y = 0;
		if (Y > 9001) Y = 9000;
	}


	int X, Y;
};


struct Move : Point {
	int m_move;
	int m_target; // For a SHOOT, the ID, for a MOVE the distance
};

struct Ennemy : Point {
	int m_life;
	int m_dataTarget;
};

struct Data : Point {
	Data() : Point(-1, -1), catched(false) {};
	Data& operator=(const Point& rhs) {
		if (&rhs == this)
			return *this;
		catched = false;
		X = rhs.X;
		Y = rhs.Y;
		return *this;
	}

	bool catched;
};

struct State {
	State() = default;
	Point m_player;
	vector<Ennemy> m_ennemies;
	vector<Data> m_datas;
	int m_turnNumber;
	int m_numberOfShotFired;
	int m_numberOfInitialLife;
};


//! Update the state for the player
void update(State& state, const int& X, const int& Y) {
	state.m_player.X = X;
	state.m_player.Y = Y;
}

//! Update the state for data
void update(State& state, const int& ID, const int& X, const int& Y) {
	assert(ID < state.m_ennemies.size());
	state.m_datas[ID] = Point(X, Y);
}


//! Update the state for ennemi
void update(State& state, const int& ID, const int& X, const int& Y, const int& life) {
	assert(ID < state.m_ennemies.size());
	Ennemy& ennemi = state.m_ennemies[ID];
	ennemi.X = X;
	ennemi.Y = Y;
	ennemi.m_life = life;
}




void play(State& state, const Move& move) {
	if (move.m_move == MOVES::MOVE) {
		state.m_player.moveWithinRange(move, move.m_target);
	}
	else {
		Ennemy& ennemi = state.m_ennemies[move.m_target];
		ennemi.m_life -= (125000.0) / state.m_player.distance(ennemi);
		if (ennemi.m_life < 0) {
			ennemi.m_life = 0;
		}
		++state.m_numberOfShotFired;
	}
}

void findNewTarget(const State& state, Ennemy& ennemi) {
	long int minDist = std::numeric_limits<long int>::max();

	for (int ID(0); ID < state.m_datas.size(); ID++) {
		const auto& data = state.m_datas[ID];
		if (!data.catched) {
			long int distance2Data = ennemi.distance2(data);
			if (minDist > distance2Data) {
				minDist = distance2Data;
				ennemi.m_dataTarget = ID;
			}
		}
	}

}


void playRound(State& state, const Move& move) throw() {

	//! 1. Move the ennemies
	for (auto& ennemi : state.m_ennemies) {
		ennemi.moveWithinRange(state.m_datas[ennemi.m_dataTarget], 500);
	}

	//! 2. Move the player
	if (move.m_move == MOVES::MOVE) {
		play(state, move);
	}

	//! 3. Check if player is dead
	bool stillOnePlayerAlive = false;
	for (const auto& ennemi : state.m_ennemies) {
		if (ennemi.m_life > 0) {
			stillOnePlayerAlive = true;
			if (ennemi.distance2(state.m_player) < 250000) {
				throw playerDeadException();
			}
		}
	}

	//! 4. Check if I won the game
	if (stillOnePlayerAlive == false) {
		throw wonGameException();
	}

	//! 5. Check if data has been catched
	for (auto& ennemi : state.m_ennemies) {
		if (ennemi.distance2(state.m_datas[ennemi.m_dataTarget]) < 250000) {
			state.m_datas[ennemi.m_dataTarget].catched = true;
			findNewTarget(state, ennemi);
		}
	}
	++state.m_turnNumber;
}



void playRound(State& state, float& thetaDirection) throw() {
	thetaDirection *= 3.14159 / 180;
	int doubleDistance = rand() & 1;
	int distance = 1000 / (doubleDistance + 1);

	Move nextMove;
	nextMove.m_move = MOVES::MOVE;
	nextMove.m_target = distance;
	nextMove.X = state.m_player.X + cos(thetaDirection) * distance;
	nextMove.Y = state.m_player.Y + sin(thetaDirection) * distance;
	playRound(state, nextMove);
}




bool movingPlayerRandomly(State& state) {
	while ((float)(rand()) / (float)(RAND_MAX) > 0.3) {
		float randAngle = rand() & 255 + rand() & 127;
		try {
			playRound(state, randAngle);
		}
		catch (exception e) {
			return false;
		}
	}
	return true;
}

// Return ID of the closest ennemy
int closestEnnemy(const State& state) {
	int maxDist = std::numeric_limits<int>::max();
	int ID(0);
	for (auto iterator = state.m_ennemies.begin(); iterator != state.m_ennemies.end();iterator++){
		if (iterator->m_life > 0) {
			int distanceEnnemi = iterator->distance2(state.m_player);
			if (distanceEnnemi < maxDist) {
				maxDist = distanceEnnemi;
				ID = iterator - state.m_ennemies.begin();
			}
		}
	}
	return ID;
}

void killEnemy(State& state, const int& IdTarget) throw() {
	if (state.m_ennemies[IdTarget].m_life <= 0) {
		return;
	}
	State oldState = state;
	Move nextMove;
	try {
		nextMove.X = state.m_ennemies[IdTarget].X;
		nextMove.Y = state.m_ennemies[IdTarget].Y;
		nextMove.m_move = MOVES::MOVE;
		nextMove.m_target = 1000;
		playRound(state, nextMove);
		killEnemy(state, IdTarget);
	} catch(playerDeadException e) {
		state = oldState;
		nextMove.m_move = MOVES::SHOOT;
		nextMove.m_target = IdTarget;
		playRound(state, nextMove);
		killEnemy(state, IdTarget);
	} catch(wonGameException e1) {
		return;
	}
}

int fitnessState(const State& state, const int& victory,const int& initTurnNumber) {
	if (victory == 1) {
		return INT_MAX * pow(0.9, state.m_turnNumber - initTurnNumber);
	} else if(victory == -1) {
		return 0;
	} else if(victory == 0) {
		
	}
}

void geneticAlgorithmTurn(Timer& time, const State& state) {
	while (time.stop() || true) {
		
		State stateCopy = state;
		int initTurnNumber = stateCopy.m_turnNumber;
		bool hasBeenAbleToMove = movingPlayerRandomly(stateCopy);

		while(hasBeenAbleToMove) {	
			try {
				if(rand() & 3 > 0) killEnemy(stateCopy, closestEnnemy(state));
				else {
					fitnessState(stateCopy, 0,initTurnNumber);
				}
			}
			catch (playerDeadException e) {
				// It means that we can't kill the ennemy before get killed
				fitnessState(stateCopy, -1, initTurnNumber);
			} catch(wonGameException e1) {
				fitnessState(stateCopy, 1, initTurnNumber);
			}
		}
	}
}




int main()
{
	State state;
	state.m_turnNumber = 0;
	state.m_numberOfShotFired = 0;
	// game loop
	while (1) {
		++state.m_turnNumber;
		int x;
		int y;
		cin >> x >> y; cin.ignore();
		update(state, x, y);


		int dataCount;
		cin >> dataCount; cin.ignore();
		if (state.m_datas.size() == 0) {
			state.m_datas.resize(dataCount);
		}
		for (int i = 0; i < dataCount; i++) {
			int dataId;
			int dataX;
			int dataY;
			cin >> dataId >> dataX >> dataY; cin.ignore();
			update(state, dataId, dataX, dataY);
		}


		int enemyCount;
		cin >> enemyCount; cin.ignore();
		if (state.m_ennemies.size() == 0) {
			state.m_ennemies.resize(enemyCount);
		}
		for (int i = 0; i < enemyCount; i++) {
			int enemyId;
			int enemyX;
			int enemyY;
			int enemyLife;
			cin >> enemyId >> enemyX >> enemyY >> enemyLife; cin.ignore();
			update(state, enemyId, enemyX, enemyY, enemyLife);
			Ennemy ennemi; 
			ennemi.X = enemyX;
			ennemi.Y = enemyY;
			findNewTarget(state, ennemi);

			if (state.m_turnNumber == 0) {
				state.m_numberOfInitialLife += enemyLife;
			}
		}

		cout << "MOVE 8000 4500" << endl; // MOVE x y or SHOOT id
	}
}
