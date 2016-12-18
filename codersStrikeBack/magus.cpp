#include <cmath>
#include <string>
#include <ctime>
#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <chrono>
#include <memory>
using namespace std;
#define PI 3.14159265359
int nombreCheckPoint = 0;


class GameParameter {
public:
	static const int rayonCheckpoint = 600;
	static const int rayonCheckpoint_2 = 600 * 600;
};
class Point {
public:
	Point(const float& a, const float& b) : x(a), y(b) {};
	Point() {};
	~Point() {};
	bool operator==(const Point& rhs) { if (this == &rhs) return true; if (rhs.x != x) return false; if (rhs.y != y) return false; return true; };
	float distance2(const Point& a) const { return (this->x - a.x)*(this->x - a.x) + (this->y - a.y)*(this->y - a.y); }
	float distance(const Point& a) const { return sqrt(this->distance2(a)); }

	// Return the point closest to this from the droite defining by the two points f1,f2
	Point closest(const Point& f1, const Point& f2)
	{
		float a1 = (f1.y - f2.y) / (f1.x - f2.x);
		float b1 = f1.y - a1*f1.x; // line from a to b : y = coeff *x + oao
		float a2 = -1 / a1; // y perpendicular y = -1/a1 x + b2 with y passing by (x,y)
		float b2 = this->y - a2*this->x;

		Point res;
		res.x = (b2 - b1) / (a1 - a2);
		res.y = a1*res.x + b1;
		return res;
	}
	float x;
	float y;
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

class Unit : public Point {
public:

	int id;
	float r;
	float vx;
	float vy;
};

class CheckPoint : public Unit {};

class Pod : public Unit {
public:
	float angle;
	int nextCheckPointID;
	int checked;
	int timeout;
	// std::shared_ptr<Pod> partner;
	bool shield;

	// Init a pod with default value
	void init() {
		vx = 0;
		vy = 0;
		shield = false;
		id = 0;
		r = 600;
		angle = 0;
		nextCheckPointID = 0;
		checked = 0;
	}

	//// Verified
	// Retourne l'angle qu'il faudrait avoir
	// O  : EST 
	// 90 : SUD
	float getAngle(const Point& p)
	{
		float d = sqrt(this->distance2(p));
		float dx = (p.x - this->x) / d;
		float a = acos(dx) * 180.0 / PI;
		if ((p.y - this->y) < 0)
			a = 360.0 - a;
		return a;
	}

	//// Verified
	// Calcule la difference de d'angle la meilleure (dans quel sens tourn�)
	// return : valeur positive-> tourner dans le sens horloge
	//			valeur negative-> tourner dans le sens anti horaire
	float diffAngle(const Point& p)
	{
		float a = this->getAngle(p);
		float right = this->angle <= a ? a - this->angle : 360.0 - this->angle + a;
		float left = this->angle >= a ? this->angle - a : this->angle + 360.0 - a;
		if (right < left)
			return right;
		else
			return -left;
	}

	// Verified
	// Faire tourner un point d'un nouveau angle
	void rotate(const Point& p)
	{
		float a = diffAngle(p);
		if (a > 18.0)
			a = 18.0;
		else if (a < 18.0)
			a = -18.0;
		angle += a;

		if (angle >= 360.0)
			angle = angle - 360.0;
		else if (angle < 0.0)
			angle = 360.0 + angle;
	}

	// Verified
	void rotate(const int& theta) {
		angle += theta;
		if (angle >= 360.0)
			angle = angle - 360.0;
		else if (angle < 0.0)
			angle = 360.0 + angle;
	}

	void boost(int thrust)
	{
		if (shield)
			return;

		float ra = angle* PI / 180.0;
		vx += cos(ra)*thrust;
		vy += sin(ra)*thrust;
	}

	void move(float t)
	{
		this->x += this->vx*t;
		this->y += this->vy*t;
	}

	void end()
	{
		this->x = static_cast<int>(x);
		this->y = static_cast<int>(y);

		this->vy = static_cast<int>(vy * 0.85);
		this->vx = static_cast<int>(vx * 0.85);

		this->timeout -= 1;
	}

	void play(const Point& p, const int& thrust)
	{
		rotate(p);
		boost(thrust);
		move(1.0);
		end();
	}

	void applyMove(const int& theta, const int& thrust, const vector<CheckPoint>& allCheckPoint) {
		rotate(theta);
		boost(thrust);
		endMove();
		if (checkCollisionWithCheckPoint(allCheckPoint[nextCheckPointID])) {
			nextCheckPointID += 1;
			if (nextCheckPointID == allCheckPoint.size()) {
				nextCheckPointID = 0;
			}
			checked++;
		}
	}

	bool checkCollisionWithCheckPoint(const CheckPoint& nextCheckPoint) {
		if (distance2(nextCheckPoint) < GameParameter::rayonCheckpoint_2) {
			return true;
		}
		return false;
	}


	void endMove() {
		move(1.0);
		end();
	}
};



class Move {
public:
	float angle;
	int thrust;
	void mutate(float amplitude,const int nombreTour) {
		float ramin = angle - 36.0 * amplitude;
		float ramax = angle + 36.0 * amplitude;
		if (ramin < -18.0) {
			ramin = -18.0;
		}
		if (ramax > 18.0) {
			ramax = 18.0;
		}
		angle = rand() % (int)(ramax - ramin + 1) + ramin;


		int pmin = thrust - 100 * amplitude;
		int pmax = thrust + 100 * amplitude;
		if (pmin < 0) {
			pmin = 0;
		}
		if (pmax > 100) {
			pmax = 100;
		}
		thrust = rand() % (pmax - pmin + 1) + pmin;
	}
	void init() {
		angle = rand() % 36 - 18;
		thrust = rand() % 100;
	}
	int hashCode() const
	{
		int result = 0;
		result = 31 * angle;
		result = 31 * result + thrust;
		return result;
	}
};

class Genotype {
public:
	deque<Move> moves;

	int hashCode() const
	{
		int hah(0);
		for (const Move& move : moves)
		{
			hah += move.hashCode();
		}
		return hah;
	}

	void mutate(const int& size,const int& nombreTour)
	{
		double amplitude = 5000.0 / (double)(nombreTour);
		if (amplitude > 1) {
			amplitude = 1;
		}

		for (int i(0); i < size; i++)
		{
			moves[i].mutate(amplitude,nombreTour+i);
		}
	}
	void drop() const {
		// divided by 2 because it is more lisible
		for (int i(0); i < moves.size(); i++) {
			cerr << moves[i].angle << " " << moves[i].thrust << " | ";
		}
	}
	void shift() {
		moves.pop_front();
		Move newMove; newMove.init();
		moves.push_back(newMove);
	}

	// Apply the move for the pod with all the checkpoints
	float score(Pod pod, const vector<CheckPoint>& allCheckPoints) const {

		float score = 0.0;
		int previousCheckPointId = pod.nextCheckPointID;

		for (int i(0); i < moves.size(); i++) {
			pod.applyMove(moves[i].angle, moves[i].thrust, allCheckPoints);
		}

		if (previousCheckPointId != pod.nextCheckPointID) {
			score += 10000 * pod.checked;
		}

		score -= 100*std::abs(pod.diffAngle(allCheckPoints[pod.nextCheckPointID]));
		score -= std::abs(pod.distance(allCheckPoints[pod.nextCheckPointID]));

		return score;
	};
};


class HC {

public:
	void init(int size, Genotype& bestNextMove) {
		for (int i(0); i < size; i++) {
			Move move; move.init();
			bestNextMove.moves.push_back(move);
		}
	}

	void newTurn(const Pod& myPod, Genotype& bestNextMove, vector<CheckPoint> allCheckPoint, const int& nombreTour, const Timer& timer) {
		int sizeGen = bestNextMove.moves.size();
		bestNextMove.shift();
		map<int, int> allmovesCalculated;

		Genotype bestGenotypeRound = bestNextMove;

		float bestScore = bestNextMove.score(myPod, allCheckPoint);
		cerr << "Initial best score " << bestScore << endl;
		bestNextMove.drop();
		cerr << endl;
		int nombreIteration = 0;
		while (timer.stop()) {
			float scoreIter;
			bestGenotypeRound.mutate(sizeGen,nombreTour);

			if (allmovesCalculated.count(bestGenotypeRound.hashCode()))
				continue;
			if ((scoreIter = bestGenotypeRound.score(myPod, allCheckPoint)) > bestScore) {
				bestScore = scoreIter;
				bestNextMove = bestGenotypeRound;
			}
			else {
				bestGenotypeRound = bestNextMove;
			}
			nombreIteration++;
			allmovesCalculated[bestGenotypeRound.hashCode()] = 1;
		}

		cerr << "Nombre iterations " << nombreIteration << endl;
		cerr << "Best score " << bestScore << endl;
		bestNextMove.drop();
		cerr << endl;
	}

	void output(const Pod& myPod,const Move& bestNextMove, const vector<CheckPoint>& allCheckpoint) const {
		float a = bestNextMove.angle + myPod.angle;
		if (a >= 360.0) {
			a = a - 360.0;
		}
		else if (a < 0.0) {
			a += 360.0;
		}
		a = a* PI / 180.0;
		int px = myPod.x + cos(a) * 10000.0;
		int py = myPod.y + sin(a) * 10000.0;
		
		cout << px << " " << py << " " << bestNextMove.thrust << endl;
	}
};

class Updator {
public:
	// Updator::updatePodPosition(myPodA, x, y, vx, vy, angle, nextCheckPointId);
	static void updatePodPosition(Pod& previousBuster, const int& newpositionX, const int& newpositionY, const int& newspeedX, const int& newSpeedY, const int& nextAngle, const int& nextCheckPointID)
	{
		previousBuster.vx = newspeedX;
		previousBuster.vy = newSpeedY;
		previousBuster.x = newpositionX;
		previousBuster.y = newpositionY;
		if (previousBuster.nextCheckPointID != nextCheckPointID) {
			previousBuster.checked++;
		}
		previousBuster.nextCheckPointID = nextCheckPointID;
		previousBuster.angle = nextAngle;
	}

	static void updateTurn(int& nombreTour) {
		nombreTour++;
	}
};

class Connector {
	Pod myPodA;
	Pod myPodB;
	Genotype bestNextMoveA;
	Genotype bestNextMoveB;
	vector<CheckPoint> allCheckpoints;
	Timer timer;
	int nombreTour = 0;
	const int depthSearch = 8;
public:
	void init()
	{
		// Init pod
		myPodA.init();
		myPodB.init();
		nombreCheckPoint = 0;
		int checkPointCount, laps;
		// Init checkPoint
		cin >> laps >> checkPointCount;
		cerr << checkPointCount << endl;
		for (int i(0); i < checkPointCount; i++) {
			int checkpointX, checkPointY;
			cin >> checkpointX >> checkPointY;
			cerr << checkpointX << " " << checkPointY << endl;
			CheckPoint ch;
			ch.x = checkpointX;
			ch.y = checkPointY;
			allCheckpoints.push_back(ch);
		}
	};

	void play()
	{
		HC hillClimbing;
		// Init gens
		hillClimbing.init(depthSearch, bestNextMoveA);
		hillClimbing.init(depthSearch, bestNextMoveB);
		// Read input
		while (true) {
			for (int i(0); i < 2; i++) {
				int x, y, vx, vy, angle, nextCheckPointId;
				cin >> x >> y >> vx >> vy >> angle >> nextCheckPointId;
				//cerr << " " << x << " " << y << " " << vx << " " << vy << " " << angle << " " << nextCheckPointId << endl;

				if (i & 1) {
					Updator::updatePodPosition(myPodA, x, y, vx, vy, angle, nextCheckPointId);
				}
				else {
					Updator::updatePodPosition(myPodB, x, y, vx, vy, angle, nextCheckPointId);
				}
			}
			for (int i(0); i < 2; i++) {
				int x, y, vx, vy, angle, nextCheckPointId;
				cin >> x >> y >> vx >> vy >> angle >> nextCheckPointId;
				//cerr << " " << x << " " << y << " " << vx << " " << vy << " " << angle << " " << nextCheckPointId << endl;

			}
			Updator::updateTurn(nombreTour);

			timer.reset();
			hillClimbing.newTurn(myPodA, bestNextMoveA, allCheckpoints, nombreTour, timer);
			timer.reset();
			hillClimbing.newTurn(myPodB, bestNextMoveB, allCheckpoints, nombreTour, timer);

			hillClimbing.output(myPodA, bestNextMoveA.moves[0], allCheckpoints);
			hillClimbing.output(myPodB, bestNextMoveB.moves[0], allCheckpoints);
		}
	}

};




int main() {
	srand(time(NULL));
	Connector connector;
	connector.init();
	connector.play();
}




