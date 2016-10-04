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

namespace GameParameters {
	const auto ENNEMY_KILL_RANGE = 2000;
	const auto ENNEMY_KILL_RANGE2 = ENNEMY_KILL_RANGE*ENNEMY_KILL_RANGE;

	const auto PLAYER_MOVE_MAX_RANGE = 1000;
	const auto PLAYER_MOVE_MAX_RANGE2 = PLAYER_MOVE_MAX_RANGE*PLAYER_MOVE_MAX_RANGE;

	const auto ENNEMY_MOVE_MAX_RANGE = 500;
	const auto ENNEMY_MOVE_MAX_RANGE2 = ENNEMY_MOVE_MAX_RANGE*ENNEMY_MOVE_MAX_RANGE;

	const auto ENNEMY_CATCH_RANGE = 500;
	const auto ENNEMY_CATCH_RANGE2 = ENNEMY_MOVE_MAX_RANGE2;

	const auto LIMIT_MIN_X = 0;
	const auto LIMIT_MAX_X = 16000;
	const auto LIMIT_MIN_Y = 0;
	const auto LIMIT_MAX_Y = 9000;
};
using namespace GameParameters;

class Timer {
public:
	std::chrono::high_resolution_clock::time_point start;
	float limitCoefficient = 0.80;
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
		if (X < LIMIT_MIN_X || Y < LIMIT_MIN_Y || X > LIMIT_MIN_X + 1 || Y > LIMIT_MAX_Y + 1)
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

	//! Round the point to be outside of the range
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
		if (X < LIMIT_MIN_X) X = LIMIT_MIN_X;
		else if (X >= LIMIT_MAX_X + 1) X = LIMIT_MAX_X;
		if (Y < LIMIT_MIN_Y) Y = LIMIT_MIN_Y;
		else if (Y > LIMIT_MAX_Y + 1) Y = LIMIT_MAX_Y;
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
	// assert(ID < state.m_ennemies.size());
	state.m_datas[ID] = Point(X, Y);
}


//! Update the state for ennemi
void update(State& state, const int& ID, const int& X, const int& Y, const int& life) {
	// assert(ID < state.m_ennemies.size());
	Ennemy& ennemi = state.m_ennemies[ID];
	ennemi.X = X;
	ennemi.Y = Y;
	ennemi.m_life = life;
}



//! Correct function verified
void play(State& state, const Move& move) {
	if (move.m_move == MOVES::MOVE) {
		state.m_player.moveWithinRange(move, move.m_target);
	}
	else {
		Ennemy& ennemi = state.m_ennemies[move.m_target];
		ennemi.m_life -= (125000.0) / pow(state.m_player.distance(ennemi), 1.2);
		if (ennemi.m_life < 0) {
			ennemi.m_life = 0;
		}
		++state.m_numberOfShotFired;
	}
}


//! Correct function verified
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


int playRound(State& state, const Move& move) {

	//! 1. Move the ennemies
	for (auto& ennemi : state.m_ennemies) {
		ennemi.moveWithinRange(state.m_datas[ennemi.m_dataTarget], ENNEMY_MOVE_MAX_RANGE);
	}

	//! 2. Move the player
	if (move.m_move == MOVES::MOVE) {
		play(state, move);
	}

	//! 3. Check if player is dead
	for (const auto& ennemi : state.m_ennemies) {
		if (ennemi.m_life > 0) {
			if (ennemi.distance2(state.m_player) < ENNEMY_KILL_RANGE2) {
				return 1;
			}
		}
	}

	//! 4. Shoot the ennemi
	if (move.m_move == MOVES::SHOOT) {
		play(state, move);
	}

	//! 5. Check if I won the game
	bool stillOnePlayerAlive = false;


	//! 6. Check if data has been catched
	for (auto& ennemi : state.m_ennemies) {
		if (ennemi.m_life > 0) {
			stillOnePlayerAlive = true;
			if (ennemi.distance2(state.m_datas[ennemi.m_dataTarget]) < ENNEMY_CATCH_RANGE2) {
				state.m_datas[ennemi.m_dataTarget].catched = true;
				findNewTarget(state, ennemi);
			}
		}
	}

	if (stillOnePlayerAlive == false) {
		return 2;
	}

	//! 7. Update the turn number
	++state.m_turnNumber;

	//! 8. Check if any data is still available
	bool stillSomeFreeData = false;
	for (const auto& data : state.m_datas) {
		if (data.catched == false) {
			stillSomeFreeData = true;
			break;
		}
	}

	if (stillSomeFreeData == false) {
		return 1;
	}

	return 0;
}


//! Correct function verified
int playRound(State& state, float& thetaDirection, vector<Move>& moves) {
	thetaDirection *= 3.14159 / 180;
	int doubleDistance = rand() & 1;
	int distance = PLAYER_MOVE_MAX_RANGE / (doubleDistance + 1);

	Move nextMove;
	nextMove.m_move = MOVES::MOVE;
	nextMove.m_target = distance;
	nextMove.X = state.m_player.X + cos(thetaDirection) * distance;
	nextMove.Y = state.m_player.Y + sin(thetaDirection) * distance;

	// Add the move to the vector of moves
	moves.emplace_back(nextMove);


	return playRound(state, nextMove);
}



//! Correct function verified ? the rand is oke I guess
int movingPlayerRandomly(State& state, vector<Move>& moves) {
	int ID(0);
	while ((float)(rand()) / (float)(RAND_MAX) > 0.3) {
		float randAngle = rand() % 360;
		ID = playRound(state, randAngle, moves);
	}
	return ID;
}


int distance2AfterMovingEnnemy(const State& state, const int& IdPlayer) {
	Point ennemyAfterMoving = state.m_ennemies[IdPlayer];

	ennemyAfterMoving.moveWithinRange(state.m_datas[state.m_ennemies[IdPlayer].m_dataTarget], ENNEMY_MOVE_MAX_RANGE);
	return ennemyAfterMoving.distance2(state.m_player);
}

// Return ID of the closest ennemy
int closestEnnemy(const State& state) {
	int maxDist = std::numeric_limits<int>::max();
	int ID(-1);
	for (auto iterator = state.m_ennemies.begin(); iterator != state.m_ennemies.end(); iterator++) {
		if (iterator->m_life > 0) {
			int distanceToEnnemy = distance2AfterMovingEnnemy(state, iterator - state.m_ennemies.begin());

			if (distanceToEnnemy < maxDist && distanceToEnnemy > ENNEMY_CATCH_RANGE2) { // (2000 - 1000)^2
				maxDist = distanceToEnnemy;
				ID = iterator - state.m_ennemies.begin();
			}
		}
	}
	return ID;
}

int killEnemy(State& state, const int& IdTarget, vector<Move>& moves) {
	if (state.m_ennemies[IdTarget].m_life <= 0) {
		return 0;
	}
	State oldState = state;
	Move nextMove;
	{
		Point ennemyAfterMoving = state.m_ennemies[IdTarget];
		ennemyAfterMoving.moveWithinRange(state.m_datas[state.m_ennemies[IdTarget].m_dataTarget], ENNEMY_MOVE_MAX_RANGE);
		nextMove.X = ennemyAfterMoving.X;
		nextMove.Y = ennemyAfterMoving.Y;
		nextMove.m_move = MOVES::MOVE;
		nextMove.m_target = 1000;
		if (state.m_ennemies[IdTarget].m_life < (125000.0) / pow(state.m_player.distance(ennemyAfterMoving), 1.2)) {
			goto finishHim;
		}
		int ID = playRound(state, nextMove);
		if (ID == 2) {
			cerr << "Should not say zero" << endl;
			return 2;
		}
		else if (ID == 1) {
			goto finishHim;
		}
		moves.emplace_back(nextMove);
		return killEnemy(state, IdTarget, moves);
	}
finishHim:
	{
	state = oldState;
	nextMove.m_move = MOVES::SHOOT;
	nextMove.m_target = IdTarget;
	int ID1 = playRound(state, nextMove);
	if (ID1 == 1) {
		return 1;
	}
	else {
		moves.emplace_back(nextMove);
		if (ID1 == 2) {
			return 2;
		}
	}
	return killEnemy(state, IdTarget, moves);
	}
}

int numberOfDataLeft(const State& state) {
	int numberofDataLeft = 0;
	for (const auto& data : state.m_datas) {
		if (data.catched == true) {
			++numberofDataLeft;
		}
	}
	return numberofDataLeft;
}

int numberOfEnemyKilled(const State& state) {
	int numberofEnemyKilled = 0;
	for (const auto& ennemi : state.m_ennemies) {
		if (ennemi.m_life < 1) {
			++numberofEnemyKilled;
		}
	}
	return numberofEnemyKilled;
}


int fitnessState(const State& state, const int& victory, const int& initTurnNumber) {
	if (victory == 1) {
		int numberofDataLeft = numberOfDataLeft(state);
		return numberofDataLeft
			* 3
			* max(0, state.m_numberOfInitialLife - 3 * state.m_numberOfShotFired)
			+ 100 * numberofDataLeft
			+ 10 * state.m_ennemies.size();
	}
	else if (victory == -1) {
		return -1;
	}
	else if (victory == 0) {
		int numberofDataLeft = numberOfDataLeft(state);
		int numberofEnemyKilled = numberOfEnemyKilled(state);
		return numberofDataLeft
			* 3
			* max(0.0, (state.m_numberOfInitialLife / (numberofEnemyKilled + 0.01)) - 3 * state.m_numberOfShotFired)
			+ 100 * numberofDataLeft
			+ 10 * numberofEnemyKilled;
	}
}

void geneticAlgorithmTurn(Timer& time, const State& state, vector<Move>& bestMoves, int& previousBestFitness) {
	if (bestMoves.empty() == false) {
		bestMoves.erase(bestMoves.begin());
	}
	time.reset();
	int bestFitness = previousBestFitness;

	int noKillAction = true;
	for (const auto& move : bestMoves) {
		if (move.m_move == MOVES::SHOOT) {
			noKillAction = false;
		}
	}
	// Just moving so don't care
	if (noKillAction == true) {
		bestFitness = -1;
	}

	int numberofGeneration = 0;
	while (time.stop()) {
		++numberofGeneration;
		vector<Move> newMoves;
		State stateCopy = state;
		int initTurnNumber = stateCopy.m_turnNumber;
		int ID;
		int fitnessNewMoves = 1;

		ID = movingPlayerRandomly(stateCopy, newMoves);
		if (ID == 2) {
			fitnessNewMoves *= 20 * fitnessState(state, 1, initTurnNumber);
		}
		int ID1;

		while (ID == 0) {

			int IDClosestEnnemy = closestEnnemy(stateCopy);
			if (IDClosestEnnemy == -1) {
				cerr << " Should never been here";
				break;
			}
			
			if (rand() & 3 >= 1) {
				ID1 = killEnemy(stateCopy, IDClosestEnnemy, newMoves);
				
			} else {
				fitnessNewMoves *= fitnessState(stateCopy, 0, initTurnNumber);
				break;
			}
			if (ID1 == 2) {
				fitnessNewMoves *= fitnessState(stateCopy, 1, initTurnNumber);
				break;
			}
			else if (ID1 == 1) {
				fitnessNewMoves *= fitnessState(stateCopy, -1, initTurnNumber);
				break;
			} else if(ID1 == 0) {
				fitnessNewMoves *= pow(0.7,state.m_turnNumber - initTurnNumber);
			}
		}

		if (fitnessNewMoves > bestFitness) {
			bestFitness = fitnessNewMoves;
			bestMoves = newMoves;
		}
	}

	previousBestFitness = bestFitness;
	cerr << "Generation number " << numberofGeneration << endl;
}

void outputNextMove(const Move& move) {
	if (true /* check corectness output, and output simpleIA */) {
		if (move.m_move == MOVES::MOVE) {
			cout << "MOVE " << move.X << " " << move.Y << endl;
		}
		else {
			cout << "SHOOT " << move.m_target << endl;
		}
	}
	else {

	}

}



int main()
{
	Timer time;
	State state;
	state.m_turnNumber = 0;
	state.m_numberOfInitialLife = 0;
	state.m_numberOfShotFired = 0;

	vector<Move> bestmove;
	int previousBestFitness = -1;
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

			if (state.m_turnNumber == 1) {
				state.m_numberOfInitialLife += enemyLife;
			}
		}

		geneticAlgorithmTurn(time, state, bestmove, previousBestFitness);
		outputNextMove(bestmove[0]);
		for (const Move& move : bestmove) {
			if (move.m_move == MOVES::MOVE) {
				cerr << "Moving to " << move.X << " " << move.Y << endl;
			}
			else {
				cerr << "Shooting at " << move.m_target << endl;
			}
		}
		cerr << "Fitness " << previousBestFitness << endl;
	}
}
