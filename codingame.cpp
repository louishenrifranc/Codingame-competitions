#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <cmath>
#include <cassert>

using namespace std;

enum MOVES {
    MOVE = 1 << 1,
    SHOOT = 1 << 2
};

struct Point {
	Point(const int& a, const int& b) : X(a), Y(b) {};
	Point() : X(-1) ,Y(-1) { };
	bool inMap() const
	{
		if (X < 0 || Y < 0 || X > 16001 || Y > 9001)
			return false;
		return true;
	}
	Point& operator=(const Point& rhs) {
		if(&rhs == this) {
			return *this;
		} 
		X = rhs.X;
		Y = rhs.Y;
		return *this;
	}

	void rotate(const Point& p, double theta = 3.14159)
	{
		X -= p.X;
		Y -= p.Y;
		X = p.X + X * cos(theta) - Y * sin(theta);
		Y = p.Y + X * sin(theta) + Y * cos(theta);
	}
	
	void move(const Point& f1, const int& dist) 
	{
		double B = f1.distance2(*this);
		if (B == 0)
			return;
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

		X += dX;
		Y += dY; // Point(X + dX, Y + dY);
	}
	
	void moveWithinRange(const Point& point, const int& moveRange = 1000) {
		const int dist = distance(point);
		//! Point is inside the move limit range
		if(dist < moveRange) {
			X = point.X;
			Y = point.Y;
		} else {
		//! Point is outside of move limit range
			double w = moveRange / dist;
			X += (point.X - X) * w;
			Y += (point.Y - Y) * w;
		}
		
		//! Normalize the point
		normalizeWithinRange();
	}
	
	inline int distance2(const Point& rhs) const
	{
		return (X - rhs.X)*(X - rhs.X) + (Y - rhs.Y)*(Y - rhs.Y);
	}
	
	inline int distance(const Point& rhs) const  {
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
	
	int X,Y;
};


struct Move : Point{
    int8_t m_move;
    int8_t m_target;
};

struct Ennemy : Point{
    int8_t m_life;
    int8_t m_dataTarget;
};

struct Data : Point {
    Data& operator=(const Point& rhs) {
    	if(&rhs == this)
    		return *this;
    	catched = false;
    	X = rhs.X;
    	Y = rhs.Y;
    	return *this;
    }
    
    bool catched = false;
};

struct State {
    State() = default;
    Point m_player;
    vector<Ennemy> m_ennemies;
    vector<Data> m_datas;
};

//! Update the state for the player
void update(State& state,const int& X,const int& Y) {
    state.m_player.X = X;
    state.m_player.Y = Y;
}

//! Update the state for data
void update(State& state, const int& ID,const int& X,const int& Y) {
    assert(ID < state.m_ennemies.size());
    state.m_datas[ID] = Point(X,Y);
}


//! Update the state for ennemi
void update(State& state, const int& ID,const int& X,const int& Y,const int& life) {
    assert(ID < state.m_ennemies.size());
    Ennemy& ennemi = state.m_ennemies[ID];
    ennemi.X = X;
    ennemi.Y = Y;
    ennemi.m_life = life;
}


void play(State& state, const Move& move) {
    if(move.m_move == MOVES::MOVE) {
        state.m_player.moveWithinRange(move);
    } else {
        Ennemy& ennemi = state.m_ennemies[move.m_target];
        ennemi.m_life -= (125000.0) / state.m_player.distance(ennemi);
        if(ennemi.m_life < 0) {
        	ennemi.m_life = 0;
        }
    }
}

void findNewTarget(const State& state,Ennemy& ennemi) {
	long int minDist = std::numeric_limits<long int>::max();
	
	for(int ID(0); ID < state.m_datas.size(); ID++) {
		const auto& data = state.m_datas[ID]
		if(!data.catched) {
			long int distance2Data = ennemi.distance2(data);
			if(minDist < distance2Data) {
				minDist = distance2Data;
				ennemi.m_dataTarget = ID; 
			}
		}
	}

}

void playRound(State& state, const Move& move) {
	
	//! 1. Move the ennemies
	for(auto& ennemi : state.m_ennemies) {
		ennemi.move(state.m_datas[ennemi.m_dataTarget], 500);
	}
	
	//! 2. Move the player
	if(move.m_move == MOVES::MOVE) {
		play(state, move);
	}
	
	//! 3. Check if player is dead
	for(const auto& ennemi : state.m_ennemies) {
		if(ennemi.m_life > 0) {
			if(ennemi.distance2(state.m_player) > 250000) {
					
				// throw ennemyCatchYou();
			}
		}
	}

	//! 4. Check if data has been catched
	for(auto& ennemi: state.m_ennemies) {
		if(ennemi.distance2(state.m_datas[ennemi.m_dataTarget] ) < 250000) {
			state.m_datas[ennemi.m_dataTarget].catched = true;
			findNewTarget(state,ennemi);
		}
	}
}




int main()
{
    State state;
    // game loop
    while (1) {
        int x;
        int y;
        cin >> x >> y; cin.ignore();
        update(state,x,y);
        
        
        int dataCount;
        cin >> dataCount; cin.ignore();
        if(state.m_datas.size() == 0) {
            state.m_datas.resize(dataCount);
        }
        for (int i = 0; i < dataCount; i++) {
            int dataId;
            int dataX;
            int dataY;
            cin >> dataId >> dataX >> dataY; cin.ignore();
            update(state,dataId,dataX,dataY);
        }
        
        
        int enemyCount;
        cin >> enemyCount; cin.ignore();
        if(state.m_ennemies.size() == 0) {
            state.m_ennemies.resize(enemyCount);
        }
        for (int i = 0; i < enemyCount; i++) {
            int enemyId;
            int enemyX;
            int enemyY;
            int enemyLife;
            cerr << enemyLife << " ";
            cin >> enemyId >> enemyX >> enemyY >> enemyLife; cin.ignore();
            update(state,enemyId,enemyX,enemyY,enemyLife);
        }

        cout << "MOVE 8000 4500" << endl; // MOVE x y or SHOOT id
    }
}
