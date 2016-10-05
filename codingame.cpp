#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <cmath>
#include <cassert>
#include <exception>
#include <chrono>

#pragma GCC optimize "O3,omit-frame-pointer,inline"
using namespace std;

enum MOVES {
	MOVE = 1 << 1,
	SHOOT = 1 << 2
};

namespace FastMath {

	#define cast_uint32_t (uint32_t)
	static inline float
		fastlog2(float x)
	{
		union { float f; uint32_t i; } vx = { x };
		union { uint32_t i; float f; } mx = { (vx.i & 0x007FFFFF) | 0x3f000000 };
		float y = vx.i;
		y *= 1.1920928955078125e-7f;

		return y - 124.22551499f
			- 1.498030302f * mx.f
			- 1.72587999f / (0.3520887068f + mx.f);
	}


	static inline float
		fastpow2(float p)
	{
		float offset = (p < 0) ? 1.0f : 0.0f;
		float clipp = (p < -126) ? -126.0f : p;
		int w = clipp;
		float z = clipp - w + offset;
		union { uint32_t i; float f; } v = { cast_uint32_t((1 << 23) * (clipp + 121.2740575f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z)) };

		return v.f;
	}

	static inline float
		fasterpow2(float p)
	{
		float clipp = (p < -126) ? -126.0f : p;
		union { uint32_t i; float f; } v = { cast_uint32_t((1 << 23) * (clipp + 126.94269504f)) };
		return v.f;
	}

	static inline float
		fasterlog2(float x)
	{
		union { float f; uint32_t i; } vx = { x };
		float y = vx.i;
		y *= 1.1920928955078125e-7f;
		return y - 126.94269504f;
	}

	static inline float fastpow(float x,float p)
	{
		return fastpow2(p * fastlog2(x));
	}

	static inline float sqrt(float x)
	{
		unsigned int i = *(unsigned int*)&x;
		i += 127 << 23;
		i >>= 1;
		return *(float*)&i;
	}

}

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
		Point newPoint = Point(newX, newY);
		newPoint.normalizeWithinRange();
		X = newPoint.X;
		Y = newPoint.Y;
		if (X < 0 || Y < 0) {
			cerr << "Negative value" << X << " " << Y;
		}
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
		return FastMath::sqrt(distance2(rhs));
	}


	void normalizeWithinRange()
	{
		if (X < LIMIT_MIN_X) X = LIMIT_MIN_X;
		else if (X >= LIMIT_MAX_X) X = LIMIT_MAX_X;
		if (Y < LIMIT_MIN_Y) Y = LIMIT_MIN_Y;
		else if (Y >= LIMIT_MAX_Y) Y = LIMIT_MAX_Y;
	}

	template<typename T>
	static Point barycentre(const vector<T>& points) {
		Point centre(0, 0);
		for (const auto& point : points) {
			centre += point;
		}
		if (points.size() > 0) {
			centre /= points.size();
		}
		return centre;
	}

	template<typename T>
	static float angleDifferenceinDegree(const T& initPoint, const T& targetPoint) {
		float angle = (180.0 * atan2(targetPoint.Y - initPoint.Y, targetPoint.X - initPoint.X)) / 3.14159;
		if (angle < 0) {
			angle += 360.0;
		}
		return angle;
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
		ennemi.m_life -= (125000.0) / FastMath::fastpow(state.m_player.distance(ennemi), 1.2);
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

//! 1 for player death, who no more data
//! 0 if nothing has happened
//! 2 if player won
//!
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
	nextMove.normalizeWithinRange();
	// Add the move to the vector of moves

	moves.emplace_back(nextMove);

	return playRound(state, nextMove);
}



//! Correct function 
int movingPlayerRandomly(State& state, vector<Move>& moves, const float amplitude = 0.3) {
	int ID(0);
	while (rand() & 7 > 4) {
		float randAngle;
		int boolean = rand() & 7;
		if (boolean < 1) {
			randAngle = rand() % 360;//rand() % 360;
		}
		else if(boolean > 5){
			randAngle = Point::angleDifferenceinDegree(state.m_player, Point::barycentre(state.m_ennemies));
		}
		else {
			randAngle = Point::angleDifferenceinDegree(state.m_player, Point::barycentre(state.m_datas));
		}

		ID	= playRound(state, randAngle, moves);
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



int shootRandomEnnemy(State& state,vector<Move>& moves) {
	Move move; move.m_move = MOVES::SHOOT;
	if (rand() & 3 > 1) move.m_target = closestEnnemy(state);
	else {
		int ID = 0, compteur = 0;
		do {
			ID = rand() % state.m_ennemies.size();
			++compteur;
		} while (state.m_ennemies[ID].m_life == 0 && compteur < 20);
		if (compteur == 20) {
			return 1;
		}
		move.m_target = ID;
	}
	assert(move.m_target != -1);
	moves.emplace_back(move);
	return playRound(state,move);
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
		//bestMoves = dumbIA(state);
	}

	int numberofGeneration = 0;
	while (time.stop()) {
		++numberofGeneration;
		vector<Move> newMoves;
		State stateCopy = state;
		int initTurnNumber = stateCopy.m_turnNumber;
		int ID;
		int fitnessNewMoves = 1;
		int distanceToClosestEnemy = stateCopy.m_ennemies[closestEnnemy(stateCopy)].distance2(stateCopy.m_player);
		double amplitudeMove = 0.3;
		if (distanceToClosestEnemy < ENNEMY_KILL_RANGE2 * 2) {
			amplitudeMove = 6;
		}

		while (true) {
			int ID;
			if(rand() & 1) ID = shootRandomEnnemy (stateCopy, newMoves);
			else {
				ID = movingPlayerRandomly(stateCopy, newMoves, amplitudeMove);
			}
			if (rand() & 4 < 1) {
				fitnessNewMoves *= fitnessState(stateCopy, 0, initTurnNumber);
				break;

			}
			if (ID == 2) {
				fitnessNewMoves *= fitnessState(stateCopy, 1, initTurnNumber);
				break;
			}
			else if (ID == 1) {
				fitnessNewMoves *= fitnessState(stateCopy, -1, initTurnNumber);
				break ;
			}
			else if (ID == 0) {
				fitnessNewMoves *= FastMath::fastpow(0.7, stateCopy.m_turnNumber - initTurnNumber);
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





void outputNextMove(State& state, const Move& move) {
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
	playRound(state, move);
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
		outputNextMove(state, bestmove[0]);
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
