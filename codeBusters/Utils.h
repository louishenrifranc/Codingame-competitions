#pragma once
#include <vector>
#include <cmath>
#include "Buster.h"
#include "Point.h"
#include "Ghost.h"
#include "MoveType.h"
#include "Move.h"
#include "GameParameter.h"
class Utils {
public:
	// Different distance functions
	static double dist(const Buster& buster, const Point& p) {
		return dist(buster.X, buster.Y, p.X, p.Y);
	}

	static double dist(const Buster& buster, const Ghost& p) {
		return dist(buster.X, buster.Y, p.X, p.Y);
	}

	static double dist(int X1, int Y1, int X2, int Y2) {
		return sqrt(sqr(X1 - X2) + sqr(Y1 - Y2));
	}

	static double dist(const Buster& a, const Buster& b) {
		return dist(a.X, a.Y, b.X, b.Y);
	}

	static double dist(const Point& a, const Point& b) {
		return dist(a.X, a.Y, b.X, b.Y);
	}

	static double dist(const Point& p, const Ghost&g) {
		return dist(p.X, p.Y, g.X, g.Y);
	}

	static double dist(const Point& p, const Buster& b) {
		return dist(p.X, p.Y, b.X, b.Y);
	}

	static long sqr(int X) {
		return X * X;
	}


	// Aller de (fromX,fromY) à (toX,toY) 
	// mais la valeur de destination est arrondi a l'entier le plus près de totX,totY
	static Point moveToWithAllowedRange(int fromX, int fromY, int toX, int toY, int minRange) {
		double dist = Utils::dist(fromX, fromY, toX, toY);
		if (dist < minRange) {
			return Point(fromX, fromY);
		}
		double needDist = dist - minRange;
		double w = needDist / dist;
		double dX = (toX - fromX) * w;
		double dY = (toY - fromY) * w;
		int rX = roundTo(fromX + dX, toX);
		//noinspection SuspiciousNameCombination
		int rY = roundTo(fromY + dY, toY);
		return Point(rX, rY);
	}

	// Aller de (fromX,fromY) à (toX,toY) 
	// mais la valeur de destination est arrondi a l'entier le plus loin de totX,totY
	static Point moveToBeOutsideRange(int fromX, int fromY, int toX, int toY, int minRange) {
		double dist = Utils::dist(fromX, fromY, toX, toY);
		if (dist < minRange) {
			return Point(fromX, fromY);
		}
		double needDist = dist - minRange;
		double w = needDist / dist;
		double dX = (toX - fromX) * w;
		double dY = (toY - fromY) * w;
		int rX = roundAwaY(fromX + dX, toX);
		//noinspection SuspiciousNameCombination
		int rY = roundAwaY(fromY + dY, toY);
		return Point(rX, rY);
	}

	// Arrondi de X au plus loin de totX
	static int roundAwaY(double X, int toX) {
		if (toX < X) {
			return (int)ceil(X);
		}
		else {
			return (int)floor(X);
		}
	}

	// Arrondi de X au prus près de toX
	static int roundTo(double X, int toX) {
		if (toX > X) {
			return (int)ceil(X);
		}
		else {
			return (int)floor(X);
		}
	}

	// Aller dans le sens opposé a (scarVX,scarVY) en partant de (X,Y) avec un deplacement d'une distance de moveRange
	static Point runawaYPoint(int scarYX, int scarYY, int X, int Y, int moveRange) {
		if (X == scarYX && Y == scarYY) {
			return Point(X, Y);
		}
		double alpha = atan2(Y - scarYY, X - scarYX);
		double newX = X + moveRange * cos(alpha);
		double newY = Y + moveRange * sin(alpha);
		return Point(newX, newY);
	}

	// ** Pour CodeBuster**
	static Point getNewPosition(const Buster& buster, const Move& move) {
		int fromX = buster.X;
		int fromY = buster.Y;
		if (move.type != MoveType::MOVE) {
			return Point(fromX, fromY);
		}
		return getNewPosition(fromX, fromY, move.X, move.Y, GameParameters::MOVE_RANGE);
	}

	// Pour aller de (fromX,fromY) de manière linéaire
	// jusqu'à (toX,toY) en se déplacant de moveRange maximum
	static Point getNewPosition(int fromX, int fromY, int toX, int toY, int moveRange) {
		int newX;
		int newY;
		double dist = Utils::dist(fromX, fromY, toX, toY);
		if (dist <= moveRange) {
			newX = toX;
			newY = toY;
		}
		else {
			double w = moveRange / dist;
			double dX = (toX - fromX) * w;
			double dY = (toY - fromY) * w;
			newX = (int)round(fromX + dX);
			newY = (int)round(fromY + dY);
		}
		if (newX < 0) {
			newX = 0;
		}
		if (newY < 0) {
			newY = 0;
		}
		if (newX >= GameParameters::H) {
			newX = GameParameters::H - 1;
		}
		if (newY >= GameParameters::W) {
			newY = GameParameters::W - 1;
		}
		return Point(newX, newY);
	}

	// ** Pour CodeBuster**
	static Point getEnemYBase(Point mYBase) {
		if (mYBase.X == 0) {
			return Point(GameParameters::H - 1, GameParameters::W - 1);
		}
		else {
			return Point(0, 0);
		}
	}

	// Changement pour un pointeur
	static Buster* getWithId(std::vector<Buster> busters, int id) {
		for (Buster enemY : busters) {
			if (enemY.getId() == id) {
				return &enemY;
			}
		}
		return nullptr;
	}

	// ** Pour CodeBuster : position pour retourner a la base **
	static Point getPositionAfterMovingToBase(Buster courier, Point mYBase) {
		return getPositionAfterMovingToBase(courier.X, courier.Y, mYBase);
	}

	// ** Pour CodeBuster : position pour retourner a la base **
	static Point getPositionAfterMovingToBase(int courierX, int courierY, Point mYBase) {
		Point p = moveToWithAllowedRange(courierX, courierY, mYBase.X, mYBase.Y, GameParameters::RELEASE_RANGE);
		p = getNewPosition(courierX, courierY, p.X, p.Y, GameParameters::MOVE_RANGE);
		return p;
	}

	// Si l'on a un move particulier à effectuer, l'affecte au buster suivant ses coordonnées
	static Move simplifY(Buster buster, Move move) {
		if (move.type != MoveType::MOVE) {
			return move;
		}
		return Move::move(getNewPosition(buster, move));
	}

	// 
	static bool inBustRange(double dist) {
		return dist >= GameParameters::MIN_BUST_RANGE && dist <= GameParameters::MAX_BUST_RANGE;
	}

	static double dist(Ghost g, Point p) {
		return dist(g.X, g.Y, p.X, p.Y);
	}
};
