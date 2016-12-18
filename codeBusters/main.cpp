#include <iostream>
#include <string>
#include <typeinfo>
#include <vector>
#include <chrono>
#include <algorithm>
#include <ctype.h>
#include <deque>
#define PI 3.14159265
using namespace std;

#ifdef DEBUG
#define PRINT(X) cerr << X;
#else
#define PRINT(X)
#endif
std::chrono::high_resolution_clock::time_point start;

struct Ghost; struct Buster; struct Map;

struct Point {
	Point(const int& a, const int& b) : X(a), Y(b) {};
	int X = -1, Y = -1;
	bool inMap() const
	{
		if (X < 0 || Y < 0 || X > 16001 || Y > 9001)
			return false;
		return true;
	}
	Point& operator+=(const Point& p)
	{
		X += p.X;
		Y += p.Y;
		return *this;
	}
	Point& operator/=(const int& N)
	{
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
	Point move(const Point& f1, const int& dist) const
	{
		double B = f1.distance2(*this);
		if (B == 0)
			return *this;
		Point x3(this->X, f1.Y);

		int A = x3.distance2(f1);
		int C = x3.distance2(*this);

		double dX = A / B * dist * dist;
		dX = (int)sqrt(dX);
		if (f1.X < this->X)
			dX = -dX;

		double dY = C / B * dist * dist;
		dY = (int)sqrt(dY);
		if (f1.Y < this->Y)
			dY = -dY;

		return Point(X + dX, Y + dY);
	}
	inline int distance2(const Point& V) const
	{
		return (X - V.X)*(X - V.X) + (Y - V.Y)*(Y - V.Y);
	}
	Point* closestPoint(vector<Point* > points)
	{
		Point *p_ptr = points[0];
		for (Point* p : points)
			if (distance2(*p_ptr) > distance2(*p))
				p_ptr = p;
		return p_ptr;
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
	void normalize()
	{
		if (X < 0) X = 0;
		if (X >= 16000) X = 16000;
		if (Y < 0) Y = 0;
		if (Y > 9001) Y = 9000;
	}

};
struct Thorus : Point {
	Thorus(const int& a, const int& b) : Point(a, b) {};
	int short_r = RAYON_CAPTURE_MIN_2;
	int long_r = RAYON_CAPTURE_MAX_2;

	// Check if a point is in a thorus
	bool inRange(const Point& p) const
	{
		int dist = this->distance2(p);
		if (dist <= long_r && dist >= short_r)
			return true;
		return false;
	}
};

struct Base : Point {
	Base() :Point(-1, -1) {};
};


//--------------------------------------------------------------------------------------
const int MOVE = 1;
const int BUST = 2;
const int STUN = 4;
const int RELEASE = 3;

const int FOG_SIZE = 2200;
const int GHOST_STEP = 400;
const int BUSTER_STEP = 800;
const int RAYON_DEPOT = 1600;
const int RAYON_CAPTURE_MAX = 1760;
const int RAYON_CAPTURE_MIN = 900;
const int FOG_SIZE_2 = FOG_SIZE*FOG_SIZE;
const int RAYON_DEPOT_2 = RAYON_DEPOT * RAYON_DEPOT;
const int RAYON_CAPTURE_MAX_2 = RAYON_CAPTURE_MAX*RAYON_CAPTURE_MAX;
const int RAYON_CAPTURE_MIN_2 = RAYON_CAPTURE_MIN*RAYON_CAPTURE_MIN;

static int nombreTour = 0;
const int VOL = 1;
const int ANNULATION = 0;
const int NOT_BUSY = 2;
const int BUSY = 4;
const int PATROLING = 8;
const int FOUND_ENNEMY = 16;
const int GOING_HOME = 32;

const int NO_HELP_REQUIRED = 0;
const int HELP = 1;

const int NORMAL_BUSTER = 0;
const int PATROL_BUSTER = 1;
const int RESUER_BUSTER = 2;
const int HIDER_BUSTER = 3;
const int GROUP_BUSTER = 4;
const int ESCORT_BUSTER = 5;
const int ZONE_DECOUVERTE = 0;
const int ZONE_PRINCIPALE = 0;
vector<Point> base_0 = { Point(13200,7199),Point(12823,7905),Point(12777,8056),Point(12823,7905),Point(13200,7199),Point(13768,6636),Point(14434,6193),Point(14905,5823),Point(14434,6193),Point(13768,6636) };
vector<Point> base_1 = { Point(2622,2015),Point(2008,2528),Point(1331,2954),Point(2008,2528),Point(2622,2015),Point(3074,1355),Point(3245,869),Point(3074,1355) };
int nbBuster, ghostCount;
Base my_base, ennemyBase;
int team_ID;


vector<Point> POSITIONS_TO_EXPLORE = {
	Point{ 3111, 1555 },
	Point{ 6222, 1800 },
	Point{ 9333, 1555 },
	Point{ 12444, 1555 },
	Point{ 14445, 1555 },
	Point{ 14445, 4666 },
	Point{ 10888, 7745 },
	Point{ 7777, 7745 } ,
	Point{ 4666, 7745 } ,
	Point{ 1555, 7745 } ,
	Point{ 1555, 6222 }
};

vector<Point> Zones{
	Point{3000,1000},Point{4000,1000},Point{ 5000,1000 },Point{ 6000,1000 },Point{ 7000,1000 },Point{ 8000,1000 },Point{ 9000,1000 },
	Point{ 10000,1000 },Point{ 11000,1000 },Point{ 12000,1000 },Point{ 13000,1000 },Point{ 14000,1000 },Point{ 15000,1000 },
	Point{ 3000,2000 },Point{ 4000,2000 },Point{ 5000,2000 },Point{ 6000,2000 },Point{ 7000,2000 },Point{ 8000,2000 },Point{ 9000,2000 },
	Point{ 10000,2000 },Point{ 12000,2000 },Point{ 12000,2000 },Point{ 13000,2000 },Point{ 14000,2000 },Point{ 15000,2000 },
	Point{15000,3000},Point{ 15000,4000 },Point{ 15000,5000 },

	Point{1000,8000},Point{ 2000,8000 },Point{ 3000,8000 },Point{ 4000,8000 },Point{ 5000,8000 },Point{ 6000,8000 },Point{ 7000,8000 },
	Point{ 8000,8000 },Point{ 9000,8000 },Point{ 10000,8000 },Point{ 11000,8000 },Point{ 12000,8000 },
	Point{1000,7000},Point{1000,6000},Point{1000,5000}
};

vector<int> lastSeenZone(Zones.size(), 0);
//--------------------------------------------------------------------------------------




struct Turn {
	vector<Ghost> ghosts;
	vector<Buster*> myBusters;
	vector<Buster> otherBusters;
	int getScore() const {}
	void analyse();
	void collectiveBehaviour(const int& deb, const int& fin, vector<int>& idActionner);
	int numberofStunesInmyArea = 0;
	int numberofTimeStuck = 0;
	int numberofStunesInEnnemyArea = 0;
	int numberofTimeIseeXEnnemyInMyArea = 0;
	void getPreviousState(const int&);
	int numberofMyBusterOnGhost(const Ghost* f);
	vector<Ghost*> targetedGhost;
	void clearTargetedGhost();
	int lastSeenEnnemy = 100;
	int nombreGhostSafe = 0;
	int nombreGhostCapture() const;
	pair<int, int> numberofSpecificBuster(const int& IDPlayer, const int& codeBuster);
};
Turn mainTurn;



// Message & Action
struct Message {
	Message(const int& m, const Point& p, const int& e) : dest(p), message(m), emetteurID(e) {};
	Point dest;
	int message;
	int emetteurID;
};

// Ghost structure
struct Ghost : Thorus {
	Ghost(const int& a, const int& b) : Thorus(a, b) {};
	bool fantomeVoleAEnnemi = false;
	int IDGhostRamene = -1;
	int delayvisibilite = 100;
	int lastX, lastY;
	int busterOnHim = 0;
	int lastSeen = 100;
	int  ID = -1;
	bool catched = false;
	bool dead = false;
	int resistance = -1;
	int dontattackhim = 0;

	void updateTurn()
	{
		dontattackhim = max(0, dontattackhim - 1);
		if (X == -1) delayvisibilite = max(delayvisibilite - 1, -1);
	}

	bool canAttack() const
	{
		return (dontattackhim == 0);
	}

	friend ostream& operator<<(ostream& os, const Ghost& f)
	{
		os << "Ghost n^ " << f.ID << " : posX = " << f.X << ", posY = " << f.Y << endl;
		os << "\t\t" << "lastX = " << f.lastX << ", lastY = " << f.lastY << ", lastSeen = " << f.lastSeen << endl;
		os << "\t\t" << "dead = " << (f.dead ? "oui" : "non") << ", pris = " << (f.catched ? "oui" : "non") << endl;
		os << "\t\t" << "nombrebusteronHim" << f.busterOnHim << " " << "resistance " << f.resistance << endl;
		os << "\t\t" << "dontattackhim" << f.dontattackhim << " " << "delayvisibillite " << f.delayvisibilite << endl;
		return os;
	}

	Point flee() const;
};

// All Buster types
struct Buster : Point {
	Buster() : Point(-1, -1), m(NOT_BUSY, my_base, ID), nextPointToExplore(-1, -1) { direction = ID; };
	//----------------------------------------------
	int lastSeen = 100;
	int ID = -1;
	// eviter de stunner un ennemi qui catch
	bool entraindecapturer = false;
	// nextmove affiche
	int nextAction = MOVE;

	int ghostCaptured = -1; // -1 if not captured anything
	// ghost targeted
	int ghostTargeted = -1;
	// delay stunned
	int delayStunned = 0; // 20 compteur for attack
	int delayRecharging = 0; //
	int busterStunned = -1;
	string message = "";
	int direction;
	Message m;
	int maximumResistanceAttack_ = 40;
	Point nextPointToExplore;
	int pathindex = 0;
	int numberofTimeStun = 0;
	bool movedone = false;
	vector<int> allStates;
	bool busy = false;
	deque<pair<int, Point> > nextmoves;
	int lastX;
	int lastY;
	int moveHiden = -1;
	//----------------------------------------------
	// Bool if for the case where it is legals to have to the ghost on his position. Correct non bullshit
	virtual Ghost* closestAliveGhostCatchable() const
	{
		Ghost* f_ptr = nullptr;
		Point last(lastX, lastY);
		for (Ghost& f : mainTurn.ghosts)
		{
			if (f.dead == false && f.X != -1 && f.resistance <= maximumResistanceAttack_ && (f.inRange(last)) && f.canAttack())
			{
				if (f_ptr == nullptr)
					f_ptr = &f;
				if (f_ptr->resistance > f.resistance)
					f_ptr = &f;
			}
		}
		return f_ptr;
	}

	// Retourne le nombre de fantomes dans la zone
	int numberPointClose(const vector<Buster>& ennemis, const int taille)
	{
		int res(0);
		Point last(lastX, lastY);
		for (const Buster& b : ennemis)
		{
			if (b.lastSeen == 1 && last.distance2(b) <= taille)
				res++;
		}
		return res;
	}

	// Retourne le fantome le plus pres qui est attrapable
	virtual Ghost* closestAliveGhostInRange() const
	{
		Ghost* f_ptr = nullptr;
		for (Ghost& f : mainTurn.ghosts)
		{
			if (f.dead == false && f.X != -1 && f.resistance <= maximumResistanceAttack_ && distance2(f) <= FOG_SIZE_2 && f.canAttack())
			{
				if (f_ptr == nullptr)
					f_ptr = &f;
				if (distance2(*f_ptr) > distance2(f))
					f_ptr = &f;
			}
		}
		return f_ptr;
	}

	// Retourne le meilleur fantome avec un compromis entre la derniere fois qu'on l'a vu, sa resistance et la distance par rapport a ce fantome
	// 
	virtual Ghost* bestAliveGhost() const
	{
		Point last(lastX, lastY);
		Ghost* f_ptr = nullptr;
		for (Ghost& f : mainTurn.ghosts)
		{
			if (f.dead == false && f.X != -1 && f.resistance <= maximumResistanceAttack_ && f.canAttack())
			{
				if (f_ptr == nullptr)
					f_ptr = &f;
				else if (f.resistance < f_ptr->resistance && last.distance2(f) < last.distance2(*f_ptr))
					f_ptr = &f;
				else if (f.resistance + last.numberSteps(&f, BUSTER_STEP) < f_ptr->resistance
					&& (f.resistance - last.numberSteps(&f, BUSTER_STEP)*f.busterOnHim > 0))
					f_ptr = &f;
			}
		}
		for (Ghost& f : mainTurn.ghosts)
		{
			if (f.dead == false && f.resistance <= maximumResistanceAttack_ && f.canAttack() && f.lastSeen < 18) {
				Point lastf(f.lastX, f.lastY);
				if (f.resistance + last.numberSteps(&lastf, BUSTER_STEP) <= (f_ptr == nullptr ? (f.lastSeen > 10 ? 15 : 40) : f_ptr->resistance)
					&& (f.resistance - f.lastSeen*f.busterOnHim - last.numberSteps(&lastf, BUSTER_STEP)*f.busterOnHim > 0))
					f_ptr = &f;
			}
		}
		return f_ptr;
	}

	// Retourne un buster qui est dans le rayon de stun
	Buster* closestBusterInRange(int dist) const
	{
		Point last(lastX, lastY);
		Buster* b_ptr = nullptr;
		for (Buster& b : mainTurn.otherBusters) {
			if (b.delayStunned == 0 && b.X != -1 && b.lastSeen == 0 && last.distance2(b) <= dist) {
				if (b_ptr == nullptr)
					b_ptr = &b;
				else if (b.ghostCaptured != -1) // pas interessant qu'on on rentre a la maison
					b_ptr = &b;
			}
		}
		return b_ptr;
	}

	// se deplacer vers
	void moveTo(const Point& p) { X = p.X; Y = p.Y; nextAction = MOVE; }

	// recherche la meilleure position ou aller pour le ghost
	Point pointNearGhost(const Thorus& f) const
	{
		Point p1(f);
		if (lastX == f.X && lastY == f.Y)
		{
			for (int theta = -360; theta < 100; theta += 1)
			{
				double x = lastX + BUSTER_STEP*cos(theta*PI / 180.0);
				double y = lastY + BUSTER_STEP*sin(theta*PI / 180.0);
				Point p2 = Point(x, y);
				if (p2.inMap()) {
					p1 = p2;
				}
			}
		}
		else for (int theta = -360; theta < 100; theta += 1)
		{
			double x = lastX + BUSTER_STEP*cos(theta*PI / 180.0);
			double y = lastY + BUSTER_STEP*sin(theta*PI / 180.0);
			Point p2 = Point(x, y);
			if (f.inRange(p2) && p2.inMap()) {
				p1 = p2;
			}
		}
		return p1;
	}

	// Find the point closest to destination that is farer from b
	Point pointToAvoidEnnemy(const Point* destination) const
	{
		Point barycentreEnnemi(0, 0); Point last(lastX, lastY);
		int sum = 0;
		for (Buster& b : mainTurn.otherBusters) {
			if (b.delayStunned == 0 && b.X != -1 && b.lastSeen == 0 && b.delayRecharging == 0 && last.distance2(b) <= FOG_SIZE_2) {
				Point nextEnnemyPosition(b.move(last, min(BUSTER_STEP, distance2(b))));
				barycentreEnnemi += nextEnnemyPosition;
				sum++;
			}
		}
		if (sum == 0)
			return *destination;
		barycentreEnnemi /= sum;
		for (int theta = -360; theta <= 0; theta += 1)
		{
			double x = X + BUSTER_STEP*cos(theta*PI / 180.0);
			double y = Y + BUSTER_STEP*sin(theta*PI / 180.0);
			Point p1 = Point(x, y);
			if (p1.distance2(barycentreEnnemi) > RAYON_CAPTURE_MAX_2 && p1.inMap()) {
				if (last.X == lastX && last.Y == lastY)
					last = p1;
				else if (p1.distance2(*destination) < last.distance2(*destination))
					last = p1;
			}
		}
		if (last.X == lastX && last.Y == lastY) {
			last = *destination;
		}
		return last;
	}

	// Find a strategic point to go where
	bool goSomewhere();


	// Catch a fantom (metier function)
	void bustGhost(Ghost* f)
	{
		nextAction = BUST;
		ghostTargeted = f->ID;

		if (f->resistance <= 0)
		{
			f->catched = true;
			ghostCaptured = f->ID;
			for (Buster& b : mainTurn.otherBusters)
			{
				if (b.delayStunned > 0 && distance2(b) <= RAYON_CAPTURE_MAX_2)
				{
					f->fantomeVoleAEnnemi = true;
				}
			}
			f->IDGhostRamene = ID;
		}
		f->dontattackhim = 0;
		f->resistance--;
	}

	// Update turn (metier function)
	void newTurnUpdate()
	{
		delayStunned = max(0, delayStunned - 1);
		delayRecharging = max(0, delayRecharging - 1);
		lastSeen++;
		if (nombreTour < 15) {
			maximumResistanceAttack_ = 15;
		}
		else
			maximumResistanceAttack_ = 40;
	}

	// Release a fantom (metier function)
	void releaseGhost(bool inBase = false)
	{
		if (ghostCaptured == -1)
			return;

		mainTurn.ghosts[ghostCaptured].catched = false;

		if (inBase) {
			mainTurn.ghosts[ghostCaptured].dead = true;
			nextAction = RELEASE;
			mainTurn.nombreGhostSafe++;
		}
		ghostCaptured = -1;
		ghostTargeted = -1;
	}

	// Bring ghost home
	virtual bool bringGhostHome()
	{
		// If I don't have a ghost captured 
		if (ghostCaptured == -1 || (ghostCaptured != -1 && mainTurn.ghosts[ghostCaptured].lastSeen == 0)) {
			ghostCaptured = -1;
			return false;
		}

		// If I am near base
		Point previous(lastX, lastY);
		if (previous.distance2(my_base) < RAYON_DEPOT_2) {
			releaseGhost(true);
		}
		else {
			Buster *b = closestBusterInRange(FOG_SIZE_2);
			if (b == nullptr) {
				// If we are near the end
				if (nombreTour >= 175 && mainTurn.numberofStunesInmyArea >= 1)
				{
					if (previous.distance2(Point(16000, 0)) < previous.distance2(Point(0, 9000)))
						moveTo(Point(16000, 0));
					else
						moveTo(Point(0, 9000));
				}
				// If it is a stolen ghost, try to avoid the straight line to the base
				else if (mainTurn.ghosts[ghostCaptured].fantomeVoleAEnnemi == true)
				{
					if (my_base.X == 0)
					{
						if (lastX > 12000)
							moveTo(Point(12000, 0));
						else
							moveTo(my_base);
					}
					else if (my_base.X == 16000)
					{
						if (lastX < 4000)
							moveTo(Point(4000, 9000));
						else
							moveTo(my_base);
					}
				}
				else moveTo(my_base);
			}
			else {
				moveTo(pointToAvoidEnnemy(&my_base));
			}
		}
		m = Message(GOING_HOME, my_base, ID);
		return true;
	}

	// Catch ghost in range (if there is a better ghost not in range, go to it)
	virtual bool catchGhostIfInRange()
	{
		Ghost *f = closestAliveGhostCatchable();
		if (f != nullptr) {
			if (nombreTour > 50 && bestAliveGhost() != f && f->resistance > 3) {
				Ghost* best = bestAliveGhost();
				cerr << "switch proie " << f->ID << " to " << best->ID << endl;
				if (best->lastSeen == 0) {
					Point flee = best->flee();
					moveTo(pointNearGhost(Thorus(flee.X, flee.Y)));
				}
				else {
					Point lastSeenPoint = Point(best->lastX, best->lastY);
					moveTo(lastSeenPoint);
				}
				return true;
			}
			// Si ya pas meilleure, je l'attrape
			bustGhost(f);
			cerr << "Catch a Ghost ID :" << f->ID << endl;
			return true;
		}
		return false;
	}

	// At the end, will follow an ennemy
	bool followEnnemy()
	{
		Buster* b_ptr = nullptr;
		Point last(lastX, lastY);
		for (Buster& b : mainTurn.otherBusters) {
			if (b.delayStunned == 0 && b.X != -1 && b.lastSeen == 0 && b.ghostCaptured != -1 && last.distance2(b) <= RAYON_CAPTURE_MAX_2) {
				if (delayRecharging <= b.delayRecharging)
					b_ptr = &b;
			}
		}
		if (b_ptr != nullptr)
		{
			moveTo(*b_ptr);
			return true;
		}
		return false;
	}

	// Move to a ghost in range
	bool moveGhostInRange()
	{
		Ghost *f = closestAliveGhostInRange();
		if (f != nullptr) {
			Point flee = f->flee();
			Point p = pointNearGhost(Thorus(flee.X, flee.Y));
			cerr << "Move to ghost in range :" << f->ID << endl;
			moveTo(p);
			return true;
		}
		return false;
	}

	// Move to the best catchable ghost
	virtual bool moveToGhost()
	{
		Ghost *f = bestAliveGhost();
		if (f != nullptr)
		{
			if (f->lastSeen == 0) {
				Point flee = bestAliveGhost()->flee();
				moveTo(pointNearGhost(Thorus(flee.X, flee.Y)));
			}
			else {
				Point lastSeenPoint = Point(f->lastX, f->lastY);
				moveTo(lastSeenPoint);
			}
			return true;
		}
		return false;
	}

	// Stun a ennemy 
	virtual bool stunEnnemy()
	{
		if (delayRecharging > 0) {
			cerr << "recharching";
			return false;
		}
		Buster*	b = closestBusterInRange(RAYON_CAPTURE_MAX_2);
		if (b == nullptr)
			return false;
		if ((b->ghostCaptured == -1 && ghostTargeted == -1) && (b->numberofTimeStun < 1)) {
			cerr << "avoid fight with buster" << b->ID << endl;
			return false;
		}
		if (b->entraindecapturer)
		{
			cerr << "try to stun ennemy" << b->ID << " but is catching" << endl;
			Ghost* g_ptr = nullptr;
			for (Ghost& g : mainTurn.ghosts)
			{
				if (g.resistance > 10 && g.lastSeen == 0 && g.dead == false && g.catched == false && g.distance2(*b) <= FOG_SIZE_2) {
					if (g_ptr == nullptr)
						g_ptr = &g;
					else if (g.distance2(*b) < g_ptr->distance2(*b))
						g_ptr = &g;
				}
			}
			if (g_ptr != nullptr)
			{
				cerr << "it is better to not stun him" << endl;
				return false;
			}
			cerr << "I will stun him anyway" << endl;
		}
		if (b->ghostCaptured != -1 && b->distance2(ennemyBase) <= 1760 * 1760)
			return false;


		nextAction = STUN;
		cerr << "Stun Ennemy " << b->ID << endl;
		b->delayStunned = 10;
		delayRecharging = 20;

		if (ghostCaptured != -1)
			releaseGhost();

		busterStunned = b->ID + (team_ID == 1 ? 0 : nbBuster);
		return true;
	}

	// Stunned ?
	bool stunned()
	{
		if (delayStunned > 0) {
			Ghost *g = closestAliveGhostInRange();
			if (g != nullptr) {
				mainTurn.targetedGhost.push_back(g);
			}
			nextAction = MOVE;
			return true;
		}
		return false;
	}

	// Find next move
	virtual void nextMove()
	{
		cerr << ID << " is a Buster" << endl;

		if (stunned())			   goto END1; // si il est assom�
		if (stunEnnemy())		   goto END1; // si je peux callaisse un bougnoul
		if (bringGhostHome())      goto END1; // si je peux ramener a la maison
		if (catchGhostIfInRange()) goto END; // si je peux attraper fantome
		if (moveGhostInRange())	   goto END;
		if (moveToGhost())		   goto END; // si je peux me deplacer vers fantomes
											 //	if (movetoSeenGhost())	   goto END;
		if (goSomewhere())         goto END; // sinon go explorer chemin pr�d�fini
	END1:
		// Even if a move has been set in the analysis function, the three first functions override this move !!
		if(nextmoves.size()) cerr << "annulation dun move important " << endl;
		nextmoves.clear();
	END:
		setNextAction();
	}

	// Update message
	void setNextAction() {
		if (nextmoves.size() == 0)
		{
			switch (nextAction)
			{
			case MOVE:
				nextAction = MOVE;
				message = "MOVE " + to_string(X) + " " + to_string(Y);
				break;
			case BUST:
				nextAction = BUST;
				message = "BUST " + to_string(ghostTargeted);
				break;
			case RELEASE:
				nextAction = RELEASE;
				message = "RELEASE";
				break;
			case STUN:
				nextAction = STUN;
				message = "STUN " + to_string(busterStunned);
				break;
			default:
				break;
			}
		}
		else {
			auto& x = nextmoves[0];
			switch (x.first)
			{
			case MOVE:
				nextAction = MOVE;
				message = "MOVE " + to_string(x.second.X) + " " + to_string(x.second.Y);
				break;
			case BUST:
				nextAction = BUST;
				message = "BUST " + to_string(ghostTargeted);
				break;
			case RELEASE:
				nextAction = RELEASE;
				message = "RELEASE";
				break;
			case STUN:
				nextAction = STUN;
				message = "STUN " + to_string(busterStunned);
				break;
			default:
				break;
			}
		}
	}

	// Deep copy (usefull for cast)
	virtual void deepCopy(const Buster& b)
	{
		moveHiden = b.moveHiden;
		direction = b.direction;
		X = b.X;
		Y = b.Y;
		lastX = b.lastX;
		lastY = b.lastY;
		ID = b.ID;
		nextAction = b.nextAction;
		ghostCaptured = b.ghostCaptured; // -1 if not captured anything
		ghostTargeted = b.ghostTargeted;
		delayStunned = b.delayStunned; // 20 compteur for attack
		delayRecharging = b.delayRecharging; //
		busterStunned = b.busterStunned;
		message = b.message;
		maximumResistanceAttack_ = b.maximumResistanceAttack_;
		m.dest = b.m.dest;
		m.message = b.m.message;
		lastSeen = b.lastSeen;
		nextPointToExplore.X = b.nextPointToExplore.X;
		nextPointToExplore.Y = b.nextPointToExplore.Y;
		allStates = b.allStates;
		busy = b.busy;
		nextmoves = b.nextmoves;
	}

	// Operator <<
	friend ostream& operator<<(ostream& os, const Buster& f)
	{
		os << "Buster n^ " << f.ID << " : posX = " << f.X << ", posY = " << f.Y << ", ghostTargeted " << f.ghostTargeted << endl;
		os << "\t\t" << "ghostCaptured = " << f.ghostCaptured << ", nextAction = " << f.nextAction << endl;

		return os;
	}
};


// The patroler stays near the ennemy base, and helped normal buster when he is near
struct Patroler : Buster {
	virtual void nextMove()
	{
		cerr << ID << " is a Patroler " << (busy ? "busy" : "not busy") << endl;
		if (stunned())			   goto ENDS1; // si il est assom�
		if (stunEnnemy())		   goto ENDS1; // si je peux callaisse un bougnoul
		if (bringGhostHome())      goto ENDS1; // si je peux ramener a la maison
		if (catchGhostIfInRange()) goto ENDS; // si je peux attraper fantome
		if (moveGhostInRange())	   goto ENDS;
		if (goSomewhere())         goto ENDS; // sinon go explorer chemin pr�d�fini
	ENDS1:
		if (nextmoves.size())cerr << "annulation dun move important " << endl;
		nextmoves.clear();
	ENDS:
		setNextAction();
	}

	bool catchGhostIfInRange()
	{
		Ghost *f = closestAliveGhostCatchable();
		if (f != nullptr) {
			if (nombreTour > 50 && bestAliveGhost() != f && f->resistance > 2) {
				Ghost* best = bestAliveGhost();
				cerr << "switch proie " << f->ID << " to " << best->ID << endl;
				if (best->lastSeen == 0 && numberSteps(best, BUSTER_STEP) < 7) {
					Point flee = best->flee();
					moveTo(pointNearGhost(Thorus(flee.X, flee.Y)));
					return true;
				}
				else {
					Point lastSeenPoint = Point(best->lastX, best->lastY);
					if (numberSteps(&lastSeenPoint, BUSTER_STEP) < 7) {
						moveTo(lastSeenPoint);
						return true;
					}
				}
			}
			bustGhost(f);
			cerr << "Catch a Ghost ID :" << f->ID << endl;
			return true;
		}
		if (f == nullptr)
		{
			Ghost* f1 = bestAliveGhost();
			if (f1 == nullptr)
				return false;
			else
			{
				cerr << "patroler mais va quand meme aider" << endl;
				if (f1->lastSeen == 0 && numberSteps(f1, BUSTER_STEP) < 8) {
					Point flee = f1->flee();
					moveTo(pointNearGhost(Thorus(flee.X, flee.Y)));
					return true;
				}
				else {
					Point lastSeenPoint = Point(f1->lastX, f1->lastY);
					if (numberSteps(&lastSeenPoint, BUSTER_STEP) < 8) {
						moveTo(lastSeenPoint);
						return true;
					}
				}
			}
		}
		return false;
	}

	virtual bool stunEnnemy()
	{
		cerr << "try stun ennemy" << endl;
		if (delayRecharging > 0) {
			cerr << "recharging" << endl;
			return false;
		}


		Buster* b = closestBusterInRange(RAYON_CAPTURE_MAX_2);

		if (b == nullptr) {
			cerr << "No buster near" << endl;
			return false;
		}
		if ((b->ghostCaptured == -1 && ghostTargeted == -1)) {
			cerr << "we don't have ghosts so avoid the fight with " << b->ID << endl;
			return false;
		}
		if (b->entraindecapturer)
		{
			cerr << "try to stun ennemy" << b->ID << " but is catching" << endl;
			Ghost* g_ptr = nullptr;
			for (Ghost& g : mainTurn.ghosts)
			{
				if (g.resistance > 10 && g.lastSeen == 0 && g.dead == false && g.catched == false && g.distance2(*b) <= FOG_SIZE_2) {
					if (g_ptr == nullptr)
						g_ptr = &g;
					else if (g.distance2(*b) < g_ptr->distance2(*b))
						g_ptr = &g;
				}
			}
			if (g_ptr != nullptr)
			{
				cerr << "it is better to not stun him" << endl;
				return false;
			}
			cerr << "I will stun him anyway" << endl;
		}



		cerr << "Stun Ennemy " << b->ID << endl;
		nextAction = STUN;
		b->delayStunned = 10;
		delayRecharging = 20;

		if (ghostCaptured != -1)
			releaseGhost();

		busterStunned = b->ID + (team_ID == 1 ? 0 : nbBuster);
		return true;
	}

	virtual bool goSomewhere()
	{
		cerr << "patroling " << endl;
		switch (my_base.X)
		{
		case 0: {
			auto res = mainTurn.numberofSpecificBuster(ID, PATROL_BUSTER);
			switch (res.first)
			{
			case 0:
				if (nombreTour & 1) moveTo(Point(14585.8, 7585.79));
				else moveTo(Point(14260.5, 7260.52));
				break;
			case 1:
				if (nombreTour & 1) moveTo(Point(14585.8, 7585.79));
				else moveTo(Point(14260.5, 7260.52));
				break;
			case 2:
				if (nombreTour & 1)
					if (res.second == 0) moveTo(Point(13242.5, 8513.79));
					else			    moveTo(Point(14802.9, 5711.08));
				else
					if (res.second == 0) moveTo(Point(12553.2, 8392.23));
					else				moveTo(Point(15042.3, 6368.86));
					break;
			case 3:
				if (nombreTour & 1)
				{
					if (res.second == 0)	     moveTo(Point(14145.6, 8250.79));
					else if (res.second == 1)	 moveTo(Point(14561.3, 7610.68));
					else						 moveTo(Point(15186.5, 7172.91));
				}
				else {
					if (res.second == 0)          moveTo(Point(13403.9, 7951.1));
					else if (res.second == 1)	 moveTo(Point(13985.8, 7054.96));
					else                         moveTo(Point(14861.1, 6442.07));
				}
			}
		}
				break;
		case 16000: {
			auto res = mainTurn.numberofSpecificBuster(ID, PATROL_BUSTER);
			switch (res.first)
			{
			case 0:
				if (nombreTour & 1) moveTo(Point(1414.21, 1414.21));
				else moveTo(Point(1739.48, 1739.48));
				break;
			case 1:
				if (nombreTour & 1) moveTo(Point(1414.21, 1414.21));
				else moveTo(Point(1739.48, 1739.48));
				break;
			case 2:
				if (nombreTour & 1)
					if (res.second == 0) moveTo(Point(3382.89, 1231.27));
					else			    moveTo(Point(957.656, 2631.14));
				else
					if (res.second == 0) moveTo(Point(2631.14, 957.656));
					else				moveTo(Point(1231.27, 3382.89));
					break;
			case 3:
				if (nombreTour & 1)
				{

					if (res.second == 0)			 moveTo(Point(1854.37, 749.213));
					else if (res.second == 1)	 moveTo(Point(1438.68, 1389.32));
					else						 moveTo(Point(813.473, 1827.09));
				}
				else {
					if (res.second == 0)          moveTo(Point(2596.11, 1048.9));
					else if (res.second == 1)	 moveTo(Point(2014.15, 1945.04));
					else                         moveTo(Point(1138.86, 2557.93));
				}
			}}
					break;
		}
		return true;
	}
};


// The rescuer try to rescue a ghost
struct Rescuer : Buster {
	Rescuer() : Buster() {};
	virtual void nextMove()
	{
		cerr << ID << " is a Rescuer" << endl;
		if (stunned()) {
			nextmoves.clear();  goto ENDR1;
		}
		if (stunEnnemy()) {
			nextmoves.clear();  goto ENDR1;
		}
		if (bringGhostHome()) {
			nextmoves.clear();  goto ENDR1;
		}
		if (catchGhostIfInRange()) {
			nextmoves.clear();  goto ENDR1;
		}
		if (closestAliveGhostInRange() != nullptr && closestAliveGhostInRange()->resistance <= 3) {
			Point flee = closestAliveGhostInRange()->flee();
			Point p = pointNearGhost(Thorus(flee.X, flee.Y));
			cerr << "Move to ghost in range :" << closestAliveGhostInRange()->ID << endl;
			moveTo(p);
			nextmoves.clear(); goto ENDR1;
		}
		if (busy) { nextAction = nextmoves[0].first; X = nextmoves[0].second.X; Y = nextmoves[0].second.Y; goto ENDR; };
	ENDR1 :
		if (nextmoves.size())cerr << "annulation dun move important" << endl;
		nextmoves.clear();
	ENDR:
		setNextAction();
	}
};

// At the end : hide
struct ConcealBuster : Buster {
	ConcealBuster() : Buster() {  }
	virtual void nextMove()
	{
		cerr << ID << " is a ConcealBuster" << endl;
		if (stunned())			   goto ENDC1; // si il est assom�
		if (stunEnnemy())		   goto ENDC1; // si je peux callaisse un bougnoul
		if (bringGhostHome())      goto ENDC1; // si je peux ramener a la maison
		if (catchGhostIfInRange()) goto ENDC1; // si je peux attraper fantome
		if (moveGhostInRange())	   goto ENDC;
		if (followEnnemy())		   goto ENDC;
		if (moveToGhost())		   goto ENDC;
		//	if (movetoSeenGhost())     goto ENDC;
		if (goSomewhere())         goto ENDC; // sinon go explorer chemin pr�d�fini
	ENDC1:
		if (nextmoves.size()) cerr << "annulation dun move important" << endl;
		nextmoves.clear();
		ENDC:
		setNextAction();
	}

	virtual bool bringGhostHome()
	{
		if (ghostCaptured == -1 || (ghostCaptured != -1 && mainTurn.ghosts[ghostCaptured].lastSeen == 0)) {
			ghostCaptured = -1;
			return false;
		}
		Point previous(lastX, lastY);
		Buster *b = closestBusterInRange(FOG_SIZE_2);
		Point pa = Point(16000, 0);
		Point pb = Point(0, 9000);
		if (b == nullptr) {
			if (previous.distance2(my_base) <= 3000 * 3000) {
				moveTo(my_base);
			}
			if (previous.distance2(pa) < previous.distance2(pb))
				moveTo(pa);
			else
				moveTo(pb);
			if (previous.distance2(my_base) < RAYON_DEPOT_2) {
				releaseGhost(true);
			}
		}
		else {
			if (previous.distance2(pa) < previous.distance2(pb)) {
				moveTo(pointToAvoidEnnemy(&pa));
			}
			else
				moveTo(pointToAvoidEnnemy(&pb));
		}
		cerr << "Hide" << endl;
		return true;
	}
};


// 
bool Buster::goSomewhere() {
	if (nombreTour < 100) {
		if (nextPointToExplore.X == -1)
		{
			direction = ID;
			pathindex = rand() % POSITIONS_TO_EXPLORE.size();
			nextPointToExplore = POSITIONS_TO_EXPLORE[pathindex];
		}
		if (lastX == nextPointToExplore.X && lastY == nextPointToExplore.Y)
		{
			if (direction & 1) pathindex++;
			else			   pathindex--;
			if (pathindex == -1)
				pathindex = POSITIONS_TO_EXPLORE.size() - 1;
			else if (pathindex == POSITIONS_TO_EXPLORE.size())
				pathindex = 0;
			nextPointToExplore = POSITIONS_TO_EXPLORE[pathindex];
		}
		cerr << "Move somewhere at " << nextPointToExplore.X << " " << nextPointToExplore.Y << endl;
		moveTo(nextPointToExplore);
	}
	else
	{
		cerr << "Time to explore more" << endl;
		int max = 0; int best;
		for (int i(0); i < Zones.size(); i++)
		{
			if (lastSeenZone[i] > max)
				max = lastSeenZone[i], best = i;
			else if (max != 0 && lastSeenZone[i] == max)
			{
				Point last(lastX, lastY);
				if (last.distance2(Zones[i]) < last.distance2(Zones[best]))
				{
					best = i;
				}
			}
		}
		moveTo(Zones[best]);
	}
	return true;
}

// Get the previous dynamic type of a buster
void Turn::getPreviousState(const int& ID)
{
	if (myBusters[ID]->allStates.size() == 1)
		return;
	myBusters[ID]->allStates.pop_back();
	while (myBusters[ID]->allStates.back() == RESUER_BUSTER)
		myBusters[ID]->allStates.pop_back();
	int previousSTate = myBusters[ID]->allStates.back();

	switch (previousSTate)
	{
	case HIDER_BUSTER: {
		ConcealBuster *ch = new ConcealBuster;
		ch->deepCopy(*myBusters[ID]);
		myBusters[ID] = ch;
	}
					   break;
	case PATROL_BUSTER: {
		cerr << ID << "go back to patrol buster" << endl;
		Patroler *ch2 = new Patroler;
		ch2->deepCopy(*myBusters[ID]);
		myBusters[ID] = ch2;
	}
						break;
	case NORMAL_BUSTER: {
		cerr << ID << "go back to normal buster" << endl;
		Buster *ch3 = new Buster;
		ch3->deepCopy(*myBusters[ID]);
		myBusters[ID] = ch3;
	}
						break;
	}
}

// Return the number of specific busters near ennemy base (used to adapt movement of the patrolers)
pair<int, int> Turn::numberofSpecificBuster(const int& ID, const int& numero) {
	int res(0); int res1;
	for (const Buster* b : myBusters)
	{
		if (ID == b->ID)
			res1 = res;
		if (b->allStates.back() == numero && b->distance2(ennemyBase) <= 4000 * 4000 && b->delayStunned == 0)
			res++;
	}
	return pair<int, int>(res, res1);
}

// The ghost flee away of the position of the nearer ghost
Point Ghost::flee() const
{
	if (busterOnHim || resistance <= 0) {
		return *this;
	}

	Buster* lepluspres = nullptr;
	for_each(mainTurn.myBusters.begin(), mainTurn.myBusters.end(), [this, &lepluspres](Buster* b) {
		Point p1(b->lastX, b->lastY);
		if (p1.distance2(*this) <= FOG_SIZE_2) {
			if (lepluspres == nullptr)
				lepluspres = b;
			else if (lepluspres->distance2(*this) > p1.distance2(*this))
				lepluspres = b;
		}

	});
	for_each(mainTurn.otherBusters.begin(), mainTurn.otherBusters.end(), [this, &lepluspres](Buster& b) {
		Point p1(b.X, b.Y);
		if (b.lastSeen == 0 && p1.distance2(*this) <= FOG_SIZE_2) {
			if (lepluspres == nullptr)
				lepluspres = &b;
			else if (lepluspres->distance2(*this) > p1.distance2(*this))
				lepluspres = &b;
		}
	});
	if (lepluspres == nullptr)
		return *this;
	lepluspres->rotate(*this);
	Point p = this->move(*lepluspres, GHOST_STEP);
	p.normalize();
	cerr << ID << " will move" << endl;
	return p;
}

// Update the zones when a buster passed near
void updateLastSeenZone()
{

	for (int i(0); i<Zones.size(); i++) {
		lastSeenZone[i]++;
		for (Buster* b : mainTurn.myBusters)
		{
			if (b->distance2(Zones[i]) <= 1000*1000)
			{
				lastSeenZone[i] = 0;
			}
		}
	}
}

// Main class, read input, init, play
struct CodeBuster {

	void init()
	{
		cin >> nbBuster; cin.ignore();
		cin >> ghostCount; cin.ignore();
		cin >> team_ID; cin.ignore();
#ifdef DEBUG1
		cerr << nbBuster << endl << ghostCount << endl << team_ID << endl;
#endif
		if (team_ID == 0)
			my_base.X = 0,
			my_base.Y = 0,
			ennemyBase.X = 16000,
			ennemyBase.Y = 9000;
		else
			my_base.X = 16000,
			my_base.Y = 9000,
			ennemyBase.X = 0,
			ennemyBase.Y = 0;
		// My bots
		mainTurn.ghosts = vector<Ghost>();
		mainTurn.myBusters = vector<Buster*>();
		mainTurn.otherBusters = vector<Buster>();

		for (int i(0); i < nbBuster; i++)
		{
			Buster *b;
			if (nbBuster >= 2 && i > 0) {
				b = new Patroler;
				b->allStates.push_back(PATROL_BUSTER);
			}
			else {
				b = new Buster;
				b->allStates.push_back(NORMAL_BUSTER);
			}
			mainTurn.myBusters.emplace_back(b);
			mainTurn.myBusters[i]->ID = i;
		}
		// Ennemies
		mainTurn.otherBusters.resize(nbBuster, Buster());
		for (int i(0); i < nbBuster; i++)
			mainTurn.otherBusters[i].ID = i;

		// Fantoms
		mainTurn.ghosts.resize(ghostCount, Ghost(-1, -1));
	}

	void readInput()
	{
		start = std::chrono::high_resolution_clock::now();
		nombreTour++;

		int entities; // the number of busters and ghosts visible to you
		cin >> entities; cin.ignore();
#ifdef DEBUG1
		cerr << entities << endl;
#endif
		for_each(mainTurn.ghosts.begin(), mainTurn.ghosts.end(), [](Ghost& f) {
			f.lastSeen++;
		});
		for_each(mainTurn.otherBusters.begin(), mainTurn.otherBusters.end(), [](Buster& b) {
			b.newTurnUpdate();
		});
		for_each(mainTurn.myBusters.begin(), mainTurn.myBusters.end(), [](Buster* b) {
			b->newTurnUpdate();
		});
		vector<vector<int> > turn;
		bool seenEnnemy = false;
		for (int i = 0; i < entities; i++) {

			turn.push_back(vector<int>());
			int entityId; // buster id or ghost id
			int x;
			int y; // position of this buster / ghost
			int entityType; // the team id if it is a buster, -1 if it is a ghost.
			int state; // For busters: 0=idle, 1=carrying a ghost.
			int value; // For busters: Ghost id being carried. For ghosts: number of busters attempting to trap this ghost.
			cin >> entityId >> x >> y >> entityType >> state >> value; cin.ignore();
			turn[i].push_back(entityId);
			turn[i].push_back(x);
			turn[i].push_back(y);
			turn[i].push_back(entityType);
			turn[i].push_back(state);
			turn[i].push_back(value);
#ifdef DEBUG1
			cerr << entityId << " " << x << " " << y << " " << entityType << " " << state << " " << value << endl;
#endif

		}
		std::sort(turn.begin(), turn.end(), [](const vector<int>& a, const vector<int>& b) {
			if (a[3] == team_ID && b[3] != team_ID)
				return true;
			else if (a[3] != team_ID && b[3] == team_ID)
				return false;
			else if (a[3] == team_ID && b[3] == team_ID)
				return true;

			if (a[3] == -1 && b[3] != -1)
				return false;
			else if (a[3] != -1 && b[3] == -1)
				return true;
			else if (a[3] == b[3])
				return true;
		});
		for (const vector<int>& v : turn) {
			int entityId = v[0]; // buster id or ghost id
			int x = v[1];
			int y = v[2]; // position of this buster / ghost
			int entityType = v[3]; // the team id if it is a buster, -1 if it is a ghost.
			int state = v[4]; // For busters: 0=idle, 1=carrying a ghost.
			int value = v[5]; // For busters: Ghost id being carried. For ghosts: number of busters attempting to trap this ghost.

			if (entityType == 1)
				entityId -= nbBuster;

			// If it is a buster
			if (entityType == team_ID) {
				cerr << "MB" << entityId << " S: " << mainTurn.myBusters[entityId]->allStates.back() << " DR: " << mainTurn.myBusters[entityId]->delayRecharging << endl;
				Buster* buster = mainTurn.myBusters[entityId];
				buster->lastX = x;
				buster->lastY = y;
				int previousdelayStunned = buster->delayStunned;
				buster->ID = entityId;
				switch (state)
				{
				case 0:
					buster->ghostCaptured = -1;
					buster->delayStunned = 0; buster->busterStunned = -1;
					break;
				case 1:
					buster->ghostCaptured = value; mainTurn.ghosts[value].catched = true;
					buster->delayStunned = 0; buster->busterStunned = -1;
					break;
				case 2:
					if (previousdelayStunned == 0 && buster->distance2(my_base) <= 5000 * 5000)
					{
						mainTurn.numberofStunesInmyArea++;
					}
					if (previousdelayStunned == 0 && buster->distance2(ennemyBase) <= 5000 * 5000)
					{
						mainTurn.numberofStunesInEnnemyArea++;
					}
					if (previousdelayStunned == 0)
					{
						Point last(x, y);
						int numberofEnnemis = buster->numberPointClose(mainTurn.otherBusters, RAYON_CAPTURE_MAX_2);
						if (numberofEnnemis == 1)
						{
							Buster* ee = nullptr;
							for (Buster& e : mainTurn.otherBusters)
							{
								if (last.distance2(e) <= RAYON_CAPTURE_MAX_2 && e.lastSeen == 1) {
									if (ee == nullptr) ee = &e;
									else if (ee->distance2(last) > e.distance2(last)) ee = &e;
								}
							}
							if (ee != nullptr) {
								// j'enregistre qu'il ma stun !
								ee->delayRecharging = 20;
								ee->numberofTimeStun++;
							}
						}
					}
					buster->ghostCaptured = -1;
					buster->delayStunned = value; buster->busterStunned = -1;
					break;
				}
				buster->X = x; buster->Y = y;
				buster->lastSeen = 0;
				buster->busy = false;
				buster->movedone = false;
				if (buster->nextmoves.size() != 0) buster->nextmoves.pop_front();
			}
			// Else if it is a ghost
			else if (entityType == -1)
			{
				cerr << "G" << entityId << " " << value << endl;
				Ghost& ghost = mainTurn.ghosts[entityId];
				ghost.X = ghost.lastX = x;
				ghost.Y = ghost.lastY = y;
				ghost.busterOnHim = value;
				ghost.lastSeen = 0;
				ghost.ID = entityId;
				ghost.catched = false;
				ghost.resistance = state;
				ghost.delayvisibilite = 0;
			}
			// Else if it is an ennemy
			else if (entityType == (team_ID == 1 ? 0 : 1)) {
				seenEnnemy = true;
				cerr << "B" << entityId << endl;
				Buster* buster = &mainTurn.otherBusters[entityId];
				buster->lastSeen = 0;
				buster->ID = entityId;
				buster->lastX = x; buster->lastY = y;
				buster->X = x; buster->Y = y;
				buster->busterStunned = -1;
				switch (state)
				{
				case 0:
					buster->ghostCaptured = -1;
					buster->delayStunned = 0;
					buster->entraindecapturer = false;
					break;
				case 1:
					buster->ghostCaptured = value;
					cerr << "Update position for ghost " << value << endl;
					mainTurn.ghosts[value].lastX = x;
					mainTurn.ghosts[value].lastY = y;
					mainTurn.ghosts[value].resistance = 0;
					mainTurn.ghosts[value].catched = false;
					buster->delayStunned = 0;
					buster->entraindecapturer = false;
					break;
				case 2:
					buster->ghostCaptured = -1;
					buster->delayStunned = value;
					buster->entraindecapturer = false;
				case 3:
					buster->entraindecapturer = true;
				}
			}

		}
		if (seenEnnemy == true)
			mainTurn.lastSeenEnnemy = 0;
		else
			mainTurn.lastSeenEnnemy++;
		for_each(mainTurn.ghosts.begin(), mainTurn.ghosts.end(), [](Ghost& f) {
			{
				if (f.lastSeen != 0)
					f.X = f.Y = -1;
				f.updateTurn();
			}});
		for_each(mainTurn.otherBusters.begin(), mainTurn.otherBusters.end(), [](Buster& f) {
			if (f.lastSeen != 0)
				f.X = f.Y = -1;
		});

	}

	// A reflechir
	vector<int> orderPlayers()
	{
		vector<int> res;
		for (int i(0); i < nbBuster; i++)
		{
			const Buster* m = mainTurn.myBusters[i];
			if (m->nextAction == BUST) {
				res.emplace_back(i);
			}
		}
		for (int i(0); i < nbBuster; i++)
		{
			if (find(res.begin(), res.end(), i) == res.end())
			{
				res.emplace_back(i);
			}
		}
		for (int i(0); i < res.size(); i++)
			return res;
	}
	void plays()
	{
		updateLastSeenZone();
		if(nombreTour > 35)
			mainTurn.analyse();
		mainTurn.clearTargetedGhost();
		vector<int> order = orderPlayers();
		for (const int& id : order) {
			if (mainTurn.myBusters[id]->movedone == false) mainTurn.myBusters[id]->nextMove();
		}
		for (int i(0); i < nbBuster; i++) {
			Buster* b = mainTurn.myBusters[i];
			cout << b->message << endl;
		}
	}
	int nombreTour = 0;
};

// Number of buster that are targgetting a ghost
int Turn::numberofMyBusterOnGhost(const Ghost* f)
{
	int res(0);
	for (Buster* b : myBusters)
	{
		if (b->nextAction == BUST && b->ghostTargeted == f->ID)
			res++;
	}
	return res;
}

// Core function (analysis of the map)
void Turn::analyse()
{
	// Analyser les moves deja construit
	int nombrefantomevolemort(0);
	for (Ghost& g : ghosts)
	{
		if (g.dead == true && g.fantomeVoleAEnnemi == true && myBusters[g.IDGhostRamene]->allStates[0] == PATROL_BUSTER) {
			nombrefantomevolemort++;
		}
	}
	int mybuster = 0;
	int ennebuster = 0;
	for (Buster* b : myBusters)
	{
		if (b->distance2(ennemyBase) <= 3000 * 3000) {
			mybuster++;
		}
	}
	for (Buster& eb : otherBusters)
	{
		if (!(eb.lastSeen > 0 || eb.distance2(ennemyBase) >= 3600 * 3600)) 
			ennebuster++;
	}
	if (ennebuster > mybuster)
	{
		numberofTimeIseeXEnnemyInMyArea++;
	}
	if (nombreTour > 120 && numberofTimeIseeXEnnemyInMyArea > 10 && nbBuster == 2)
	{
		cerr << "too much aggression, stop patrol" << endl;
		for (Buster* b : myBusters)
		{
			if (b->allStates.back() == PATROL_BUSTER)
			{
				Buster* r1 = new Buster;
				r1->deepCopy(*b);
				r1->nextmoves.clear();
				myBusters[b->ID] = r1;
				myBusters[b->ID]->allStates.clear();
				myBusters[b->ID]->allStates.push_back(NORMAL_BUSTER);
			}
		}
	}
	// 


	//cerr << "Number of time " << numberofTimeIseeXEnnemyInMyArea << endl;
	//cerr << "Nombre de fantomes voles morts par un patroleur" << nombrefantomevolemort << endl;
	cerr << "Stune in my area" << numberofStunesInmyArea << endl;
	//cerr << "Stune in his area" << numberofStunesInEnnemyArea << endl;
	//cerr << "Last seen ennemy " << mainTurn.lastSeenEnnemy << " turns" << endl;
	//cerr << "Number of ghost safe " << mainTurn.nombreGhostSafe  << " and captured " << mainTurn.nombreGhostCapture() << endl;


	// Si il ramene un bot
	for (Buster* b : myBusters)
	{
		if (b->ghostCaptured != -1 && mainTurn.ghosts[b->ghostCaptured].lastSeen != 0) {
			cerr << b->ID << " is busy" << endl;
			b->busy = true;
		}
	}
	for (Buster* b : myBusters)
	{
		if (nombreGhostCapture() > ghostCount / 2) {
			for (Buster* b1 : myBusters)
			{
				if (b1->ID != b->ID && b1->busy == false && b->ghostCaptured != -1 && mainTurn.ghosts[b->ghostCaptured].lastSeen != 0)
				{
					cerr << "Protection" << endl;
					b1->nextmoves.push_back(pair<int, Point>(MOVE, *b));
					b1->busy = true;
				}
			}
		}
	}

	// Gagner des combatsb
	for (Buster* b : myBusters)
	{
		if (b->nextAction == BUST && b->busy == false)
		{
			Ghost& g = ghosts[b->ghostTargeted];
			int numberofBusteronGhost = numberofMyBusterOnGhost(&g);
			cerr << "number of buster on ghost " << g.ID << "with " << g.busterOnHim << "busters on him is " << numberofBusteronGhost << endl;
			if (numberofBusteronGhost < ceil(g.busterOnHim / 2.0)) {
				numberofTimeStuck = 0;
				vector<Buster* > busterCanHelp;
				int mindelay = 1000;
				for (Buster* busterSaver : myBusters) {
					if (busterSaver->ID != b->ID && !busterSaver->busy) {
						int numberStep = busterSaver->numberSteps(&g, BUSTER_STEP);
						if (g.resistance - numberStep*g.busterOnHim >= 0 && busterSaver->delayRecharging >= numberStep)
						{
							busterCanHelp.push_back(busterSaver);
							mindelay = min(mindelay, busterSaver->numberSteps(&g, BUSTER_STEP));
						}
					}
				}
				if (ceil(g.busterOnHim/2.0) > numberofBusteronGhost && g.resistance - mindelay*g.busterOnHim <= 0) {
					cerr << "Catching " << b->ghostTargeted << " is stupid " << endl;
					g.dontattackhim = g.resistance / (g.busterOnHim - numberofBusteronGhost);
				}
				else {
					for (Buster* busterSaver : busterCanHelp) {
						cerr << "Buster" << busterSaver->ID << " become a rescuer" << endl;
						Rescuer *r = new Rescuer;
						r->deepCopy(*busterSaver);
						r->nextmoves.clear();
						r->nextmoves.push_back(pair<int, Point>(MOVE, r->pointNearGhost(g)));
						myBusters[busterSaver->ID] = r;
						myBusters[busterSaver->ID]->busy = true;
						myBusters[busterSaver->ID]->allStates.push_back(RESUER_BUSTER);
					}
				}
			}
			else if (numberofBusteronGhost == ceil(g.busterOnHim / 2.0) && g.resistance <= 0)
			{
				cerr << "stuck " << numberofTimeStuck << endl;

				numberofTimeStuck++;
				if (numberofTimeStuck > 40)
				{
					b->nextmoves.push_back(pair<int, Point>(MOVE, g));
					b->busy = true;
				}
			}
		}
		else {
			numberofTimeStuck = 0;
		}
	}

	// Venir en aide a unbuster rentrant
	for (Buster* b : myBusters)
	{
		if (numberofStunesInmyArea >= 2 && nombreTour < 150)
		{
			if (b->busy == false)
			{
				bool canhelp = false;
				for (Buster* b1 : myBusters)
				{
					if (b1->ID != b->ID && b1->ghostCaptured != -1)
					{
						cerr << "t" << endl;
						if (b->distance2(my_base) <= b1->distance2(my_base) + 5000)
						{
							canhelp = true;
							cerr << b->ID << " gonna help " << b1->ID << endl;
							b->nextmoves.push_back(pair<int, Point>(MOVE, b1->move(my_base, 2*BUSTER_STEP)));
							b->busy = true;
						}
					}
				}
				if (canhelp == true)
					break;
			}
		}
	}
	
	// Si je me fais voler un fantome
	for (Buster* b : myBusters)
	{
		if (b->busy == false && b->delayStunned == 0)
		{
			if (targetedGhost.size() == 0)
				break;
			else {
				for (Ghost* g : targetedGhost)
				{
					cerr << "need to rescue " << g->ID << endl;
					if (g->lastSeen == 0)
					{
						cerr << "still seen" << endl;
						Rescuer *r = new Rescuer;
						r->deepCopy(*b);
						r->nextmoves.clear();
						r->nextmoves.push_back(pair<int, Point>(MOVE, r->pointNearGhost(*g)));
						myBusters[b->ID] = r;
						myBusters[b->ID]->busy = true;
						myBusters[b->ID]->allStates.push_back(RESUER_BUSTER);
						cerr << b->ID << "will rescue him" << endl;
					}
					else {
						cerr << "cant seen" << endl;
						Point dernierVision(g->lastX, g->lastY);
						int numberStepForEnnemi = dernierVision.numberSteps(&ennemyBase, BUSTER_STEP);
						int numberofStepForBuster = b->numberSteps(&ennemyBase, BUSTER_STEP);
						Point go = dernierVision.move(ennemyBase, g->lastSeen*(BUSTER_STEP));
						if (numberofStepForBuster <= numberStepForEnnemi)
						{
							cerr << b->ID << "will go to save him at << " << go.X << " " << go.Y << endl;
							if (ennemyBase.X == 16000)
							{
								Rescuer *r = new Rescuer;
								r->deepCopy(*b);
								r->nextmoves.clear();

								if (go.X > 13000 && go.Y > 6000) {
									cerr << "nop go to ennemy base" << endl;
									auto res = mainTurn.numberofSpecificBuster(b->ID, PATROL_BUSTER);
									switch (res.first)
									{
									case 0:
										if (nombreTour & 1) go = (Point(14585.8, 7585.79));
										else go = (Point(14260.5, 7260.52));
										break;
									case 1:
										if (nombreTour & 1) go = (Point(14585.8, 7585.79));
										else go = (Point(14260.5, 7260.52));
										break;
									case 2:
										if (nombreTour & 1)
											if (res.second == 0) go = (Point(13242.5, 8513.79));
											else			    go = (Point(14802.9, 5711.08));
										else
											if (res.second == 0) go = (Point(12553.2, 8392.23));
											else				go = (Point(15042.3, 6368.86));
											break;
									case 3:
										if (nombreTour & 1)
										{
											if (res.second == 0)	     go = (Point(14145.6, 8250.79));
											else if (res.second == 1)	 go = (Point(14561.3, 7610.68));
											else						 go = (Point(15186.5, 7172.91));
										}
										else {
											if (res.second == 0)          go = (Point(13403.9, 7951.1));
											else if (res.second == 1)	 go = (Point(13985.8, 7054.96));
											else                         go = (Point(14861.1, 6442.07));
										}
									}
									r->nextmoves.push_back(pair<int, Point>(MOVE, go));
									myBusters[b->ID] = r;
									myBusters[b->ID]->busy = true;
									myBusters[b->ID]->allStates.push_back(RESUER_BUSTER);
								}
							}
							else if (ennemyBase.X == 0)
							{
								Rescuer *r = new Rescuer;
								r->deepCopy(*b);
								r->nextmoves.clear();

								if (go.X < 3000 && go.Y < 3000) {
									cerr << "nop go to ennemy base" << endl;
									auto res = mainTurn.numberofSpecificBuster(b->ID, PATROL_BUSTER);
									switch (res.first)
									{
									case 0:
										if (nombreTour & 1) go = (Point(1414.21, 1414.21));
										else go = (Point(1739.48, 1739.48));
										break;
									case 1:
										if (nombreTour & 1) go = (Point(1414.21, 1414.21));
										else go = (Point(1739.48, 1739.48));
										break;
									case 2:
										if (nombreTour & 1)
											if (res.second == 0) go = (Point(3382.89, 1231.27));
											else			    go = (Point(957.656, 2631.14));
										else
											if (res.second == 0) go = (Point(2631.14, 957.656));
											else				go = (Point(1231.27, 3382.89));
											break;
									case 3:
										if (nombreTour & 1)
										{

											if (res.second == 0)			 go = (Point(1854.37, 749.213));
											else if (res.second == 1)	 go = (Point(1438.68, 1389.32));
											else						 go = (Point(813.473, 1827.09));
										}
										else {
											if (res.second == 0)          go = (Point(2596.11, 1048.9));
											else if (res.second == 1)	 go = (Point(2014.15, 1945.04));
											else                         go = (Point(1138.86, 2557.93));
										}
									}
									r->nextmoves.push_back(pair<int, Point>(MOVE, go));
									myBusters[b->ID] = r;
									myBusters[b->ID]->busy = true;
									myBusters[b->ID]->allStates.push_back(RESUER_BUSTER);
								}
							}
							else {
								cerr << b->ID << " is to far from ennemy base" << endl;
							}
						}
					}
				}
			}
		}
	}
	// bringhisthome busty cancel
	for (Buster* b : myBusters)
	{
		if (b->ghostCaptured != -1){
			b->busy = false;
			b->movedone = false;
			b->nextmoves.clear();
		}
	}

	// get previous state
	for (Buster* b : myBusters)
	{
		if (b->busy == false)
		{
			getPreviousState(b->ID);
		}
	}
	for (Buster* b : myBusters)
	{
		if (b->allStates.back() == PATROL_BUSTER && b->delayRecharging > 2)
		{
			cerr << b->ID << " become a normal buster because he can't stun" << endl;
			Buster* r1 = new Buster;
			r1->deepCopy(*b);
			r1->nextmoves.clear();
			myBusters[b->ID] = r1;
			if(myBusters[b->ID]->allStates.back() != NORMAL_BUSTER)
				myBusters[b->ID]->allStates.push_back(NORMAL_BUSTER);
			myBusters[b->ID]->allStates.push_back(NORMAL_BUSTER);
		}
	}


	// hide
	for (Buster* b : myBusters)
	{
		if (b->busy == false)
		{
			if (((nombreTour > 150) && 
				(b->ID != 1 || (b->ID == 1 && b->ghostCaptured != -1) || (numberofTimeIseeXEnnemyInMyArea > 10) || lastSeenEnnemy > 20)) ||
				(nombreTour > 170 && (b->ID != 1 || (b->ID == 1 && b->ghostCaptured != -1)))
				)
			{
				cerr << "Going into hiding time (analysis) " << endl;
				ConcealBuster *r = new ConcealBuster;
				r->deepCopy(*b);
				r->nextmoves.clear();
				myBusters[b->ID] = r;
				myBusters[b->ID]->busy = true;
				myBusters[b->ID]->allStates.push_back(HIDER_BUSTER);
			}
		}
	}

	// switch roles
	for (Buster* b : myBusters)
	{
		if (b->busy == false)
		{
			for (Buster* b1 : myBusters)
			{
				if (b1->ID != b->ID && b1->busy == false)
				{
					if (b->allStates.back() == PATROL_BUSTER && b1->allStates.back() == NORMAL_BUSTER)
					{
						if (b->distance2(ennemyBase) > b1->distance2(ennemyBase))
						{
							cerr << "switch between " << b->ID << " and " << b1->ID << endl;
							Patroler *r = new Patroler;
							r->deepCopy(*b1);
							r->nextmoves.clear();

							Buster* r1 = new Buster;
							r1->deepCopy(*b);
							r1->nextmoves.clear();

							myBusters[b->ID] = r1;
							myBusters[b->ID]->allStates.push_back(NORMAL_BUSTER);

							myBusters[b1->ID] = r;
							myBusters[b1->ID]->allStates.push_back(PATROL_BUSTER);

						}
					}
				}
			}
		}
	}
	cerr << "Fin analyse" << endl << endl;
}

// Return the number of ghost captured
int Turn::nombreGhostCapture() const {
	int res(nombreGhostSafe);
	for (Buster* b : myBusters)
	{
		if (b->ghostCaptured != -1)
			res++;
	}
	return res;
}

// Maintain and update the list of last seen ghosts
void Turn::clearTargetedGhost()
{
	int i = 0;
	while (i < targetedGhost.size())
	{
		if (targetedGhost[i]->lastSeen == 0) {
			cerr << "delete ghost " << targetedGhost[i]->ID << endl;
			targetedGhost.erase(targetedGhost.begin() + i);
		}
		else
		{
			Ghost* g = targetedGhost[i];
			Point p(g->lastX, g->lastY);
			cerr << g->ID << ", lastSeenX and Y :" << g->lastX << " " << g->lastY << endl;
			if (g->lastSeen > p.numberSteps(&ennemyBase, BUSTER_STEP) + 7) {
				cerr << "delete long unseen ghost " << endl;
				targetedGhost.erase(targetedGhost.begin() + i);
			}
			else {
				bool captured = false;
				for (Buster* b : myBusters)
				{
					if (b->ghostCaptured == g->ID)
					{
						cerr << "I got it back ";
						targetedGhost.erase(targetedGhost.begin() + i);
						captured = true;
						break;
					}
				}
				if (!captured)
				{
					for (Buster& b : otherBusters)
					{
						if (b.ghostCaptured == g->ID && b.distance2(ennemyBase) <= RAYON_CAPTURE_MAX_2)
						{
							cerr << "Ennemy has dropped it in his base" << endl;;
							targetedGhost.erase(targetedGhost.begin() + i);
							captured = true;
							break;
						}
					}
				}
				if (!captured) {
					cerr << "don't delete ghost now because he need " << p.numberSteps(&ennemyBase, BUSTER_STEP);
					cerr << " to come back and we haven't seen him since" << targetedGhost[i]->lastSeen << endl;
					i++;
				}
			}
		}
	}
}

// Main function
int main()
{
	srand(time(NULL));
	CodeBuster cd;
	cd.init();
	while (true)
	{
		nombreTour++;
		cd.readInput();
		cd.plays();
		cerr << "Tour " << nombreTour << endl;
	}
}
