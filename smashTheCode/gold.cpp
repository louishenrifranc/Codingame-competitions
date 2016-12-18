#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <iterator>
#include <vector>
#include <algorithm>
#include <deque>
#include <map>
#include <set>
#include <ctime>
#include <chrono>
#include <array>
#include <fstream>

int DEBUG = 0;
int dirX[] = { -1,0,0,1 };
int dirY[] = { 0,-1,1,0 };
bool no_time = false;
int Tour = 0;
int nbreTME(0), nbreTMP(0);
int depth = 5;

int tailleMap;




using namespace std;
using namespace std::chrono;
using genome = vector<int>;
vector<char> mymap(72, '@'); // La map
vector<char> advmap(72, '@');
map<string, int> previousPlay;

deque<char> nextinput; // next bloc to add
int next_column; // next value
int next_perm;
map<string, pair<int, int> > scores;
high_resolution_clock::time_point start;

// ------------------------------------------- Tools --------------------------------------------------------------------------------

inline bool getDiffTime()
{
	double x = (Tour == 0 ? 500.0 : 100.0);
	auto fin = std::chrono::high_resolution_clock::now();
	if (std::chrono::duration_cast<std::chrono::milliseconds>(fin - start).count() > 0.92 * x)
	{
		no_time = true;
		return true;
	}
	return false;
}

void output()
{
	if (no_time)
		cerr << "no more time" << endl;
	for (int i = 11; i > -1; i--)
	{
		{
			for (int j(0); j < 6; j++)
				cerr << mymap.at(j + i * 6) << " ";
		}
		cerr << endl;
	}
}

inline int permForAColumn(const int& j)
{
	int x = rand() % 4;
	if (j == 0) {
		x = rand() % 3;
		if (x == 2) {
			x++;
		}
	}
	else if (j == 5)
		x = rand() % 3 + 1;
	return x;
}

int sizeMap(const vector<char>& map)
{
	int n(0);
	for_each(map.begin(), map.end(), [&n](const char& x) {
		if (x != '@')
			n++;
	});
	return n;
}

// ------------------------------------------ Modelisation --------------------------------------------------------

int raining(vector<char>& deque, const int &nbcouche)
{
	int nbfullColumn(0);
	for (int i(0); i < 6; i++)
	{
		int j(0); int k(nbcouche);
		while (j < 12 && k > 0)
		{
			if (deque[j] == '@') {
				deque[j] = 'A'; k--;
			}
			j++;
		}
		if (j == 12 && k != 0)
			nbfullColumn++;
	}
	return (nbfullColumn == 6 ? 0 : 1);
}

void updateNode(const vector<char>& deque, const char& couleur, const int& nextpos, vector<int>& foo)
{
	if (couleur == '@')
		return;
	for (int i(0); i < 4; i++)
	{
		int u = nextpos / 6;
		int v = nextpos - 6 * u;
		u += dirX[i];
		v += dirY[i];
		int newpos = u * 6 + v;

		if (u >= 0 && u <= 11 && v >= 0 && v <= 5)
		{
			char bf = deque[newpos];
			if ((couleur == bf || bf == 'A') && find(foo.begin(), foo.end(), newpos) == foo.end())
			{
				foo.push_back(newpos);
				if (bf != 'A')
					updateNode(deque, couleur, newpos, foo);
			}
		}
	}
}


void updatePosition(vector<char>& deque, const char& k, const int& pos, vector<int>& foo, const set<int>& foo2 = set<int>())
{
	foo.push_back(pos);

	for (int i(0); i < 3; i++)
	{
		int u = pos / 6;
		int v = pos - 6 * u;
		u += dirX[i];
		v += dirY[i];
		int nextpos = u * 6 + v;
		if (u >= 0 && u <= 11 && v >= 0 && v <= 5)
		{
			char bf = deque[nextpos];
			if (bf != '@')
			{
				if ((bf == k || bf == 'A') && find(foo.begin(), foo.end(), u * 6 + v) == foo.end() && find(foo2.begin(), foo2.end(), nextpos) == foo2.end())
				{
					foo.push_back(nextpos);
					if (bf != 'A')
						updateNode(deque, k, nextpos, foo);
				}
			}
		}
	}
	for_each(begin(foo), end(foo), [&deque, &k](const int& x)
	{
		if (deque[x] != 'A')
			deque[x] = k;
	});
}

void dropp(vector<char>& deque, int x)
{
	deque[x] = '@';
	while (x < 72)
	{
		deque[x] = (x + 6 > 71 ? '@' : deque[x + 6]);
		x += 6;
	}
}


void clear(vector<char>& deque, vector<int> &foo)
{
	sort(foo.begin(), foo.end(), [](const int& a, const int& b)
	{
		auto aa = a / 6;
		auto bb = b / 6;
		if (aa > bb)
			return true;
		else if (aa == bb && a - 6 * aa > b - 6 * bb)
			return true;
		else
			return false;
	});

	for_each(foo.begin(), foo.end(),
		[&deque](const int& x)
	{
		dropp(deque, x);
	});
}

inline int taille(const vector<int>& foo, const vector<char>& deque)
{
	int n(0);
	for_each(foo.begin(), foo.end(), [&n, &deque](const int& x)
	{
		if (deque[x] != 'A')
			n++;
	});
	return n;
}


void transform(const vector<char>& deque, const int &i, int j, const int& perm, int& pos1, int& pos2, char& a, char& b)
{
	int k(0);
	while (k < 12 && deque[j + k * 6] != '@')
		k++;
	pos1 = j + k * 6;
	if (k > 11)
		return;
	if (!(perm & 1))
	{
		j = (perm == 2 ? j - 1 : j + 1);
		if (j < 0 || j > 5)
			return;
		k = 0;
		while (k < 12 && deque[j + k * 6] != '@')
			k++;
		if (k > 11)
			return;
		pos2 = j + k * 6;
	}
	else
	{
		if (k > 10)
			return;
		pos2 = j + (k + 1) * 6;
	}
	a = nextinput[(perm == 3 ? 2 * i + 1 : 2 * i)];
	b = nextinput[(perm == 3 ? 2 * i : 2 * i + 1)];
}

void clearMap(vector<char> &deque, vector<int>& foo, int &score, int & fitness, const int& i, int depth = 8)
{
	clear(deque, foo);
	vector<int> foo1;
	vector<char> couleur;
	for (const auto &pos : foo)
	{
		if (find(foo1.begin(), foo1.end(), pos) == foo1.end())
		{
			vector<int> foobuff;
			updateNode(deque, deque[pos], pos, foobuff);
			if (taille(foobuff, deque) > 3)
			{
				foo1.insert(foo1.end(), foobuff.begin(), foobuff.end());
				if (find(couleur.begin(), couleur.end(), deque[pos]) == couleur.end())
				{
					couleur.push_back(deque[pos]);
				}
			}
		}
	}
	int B = taille(foo1, deque);
	if (B > 3)
	{
		int X = (B - 4);
		int V = foo1.size();
		if (B >= 11) X = 8;
		score += (10 * depth)*((B + X + couleur.size())) + (V * 4) + (std::max(0, ((tailleMap / 7) - 3)*(V - B)));
		//	fitness += (Tour + depth - i)*(10 * foo1.size())*(depth * 3 + X + couleur.size());
		clearMap(deque, foo1, score, fitness, i, depth * 2);
	}
}


void move(vector<char>& deque, const int& i, const int& pos1, const int& pos2, int& score, const int& a, const int& b, int& fitness)
{

	vector<int> foo;
	set<int> fooX;
	int B(0);
	int CP(0);
	if (a == b) // If there are both the same
	{
		deque[pos1] = a;
		deque[pos2] = b;
		updatePosition(deque, a, pos2, foo);
		if (taille(foo, deque) > 3)
		{
			fooX.insert(foo.begin(), foo.end());
			B += foo.size();
		}
		CP = 0;
	}
	else
	{
		deque[pos1] = a;
		updatePosition(deque, a, pos1, foo);
		if (taille(foo, deque) > 3)
		{
			fooX.insert(foo.begin(), foo.end());
			B += taille(foo, deque); CP++;
		}

		foo.clear();
		deque[pos2] = b;
		updatePosition(deque, b, pos2, foo, fooX);
		if (taille(foo, deque) > 3)
		{
			std::copy(foo.begin(), foo.end(), std::inserter(fooX, fooX.end()));
			B += taille(foo, deque); CP++;
		}
	}

	vector<int> res(fooX.begin(), fooX.end());

	if (taille(res, deque) == 0)
	{
		score = 0;
	}
	else if (taille(res, deque) > 3)
	{
		int X = (B - 4);
		int V = res.size();
		if (B >= 11) X = 8;
		score = (10 * B)*(CP + X) + (4 * V) + (std::max(0, ((tailleMap / 7) - 3)*(V - B)));
		// fitness = (Tour + depth - i)*(10 * res.size())*(CP + X);
		clearMap(deque, res, score, fitness, i);
	}

}




vector<char> bestDeque;

// ------------------------------------------ Reading Input ----------------------

void readInput(const int& k)
{
	no_time = false;
	for (int i(0); i < 8; i++)
	{
		char color1, color2;
		cin >> color1 >> color2;
		color1 += 17;
		color2 += 17;

		if (!k)
		{
			nextinput.push_back(color1);
			nextinput.push_back(color2);
		}
		else if (i == 7)
		{
			nextinput.pop_front();
			nextinput.pop_front();
			nextinput.push_back(color1);
			nextinput.push_back(color2);
		}
	}
	for (int i = 11; i > -1; i--)
	{
		string row;
		cin >> row;
		for (int j(0); j < row.size(); j++)
		{
			mymap[i * 6 + j] = row[j] + 17;
			if (row.at(j) == '0')
			{
				mymap[i * 6 + j] = 'A';
			}
			if (row.at(j) == '.')
				mymap[i * 6 + j] = '@';
		}
	}
	for (int i = 11; i > -1; i--)
	{
		string row; // One line of the map ('.' = empty, '0' = skull block, '1' to '5' = colored block)
		cin >> row;
		for (int j(0); j < row.size(); j++)
		{
			advmap[i * 6 + j] = row[j] + 17;
			if (row.at(j) == '0')
			{

				advmap[i * 6 + j] = 'A';
			}
			if (row.at(j) == '.')
				advmap[i * 6 + j] = '@';
		}
	}
	start = high_resolution_clock::now();
	tailleMap = sizeMap(mymap);

	//output();
}


// ------------------------------------------------ Hill Climbing -------------------------------------------------------------


void randomize(genome &A)
{
	for (int i(0); i < depth; i++)
	{
		A[2 * i] = rand() % 6;
		A[2 * i + 1] = permForAColumn(A[2 * i]);
	}
}


void swift(genome& gen)
{
	gen.erase(gen.begin()); gen.erase(gen.begin());

	int j = rand() % 6;
	int perm = permForAColumn(j);

	gen.push_back(j); gen.push_back(perm);
}




// -1 : my defeat
// -2 : bot defeat
int simulate(const genome& genomeP/*, const genome& genomeE*/, int& finalScoreP/*int& finalScoreE*/, int &fitnessP,/* int &fitnessE,
																												   */ int& nbTeteMortP,/* int& nbTeteMortE*/ int depthG = depth)
{

	for (int i(0); i < depthG; i++)
	{
		int scoreE(0), scoreP(0), fitE(0), fitP(0);
		int pos1(-1), pos2(-1);
		char a, b;

		// My turn
		transform(mymap, i, genomeP.at(2 * i), genomeP.at(2 * i + 1), pos1, pos2, a, b);
		if (pos2 == -1 || pos1 == -1) {
			return -3;
		}
		// if (i == depthG / 2 && !getDiffTime())
		// 	return -3;
		move(mymap, i, pos1, pos2, scoreP, a, b, fitnessP);

		//	pos1 = -1; pos2 = -1;
		// Adv turn
		//	transform(advmap, i, genomeE.at(2 * i), genomeE.at(2 * i + 1), pos1, pos2, a, b);
		//
		//	if (pos2 == -1 || pos1 == -1) {
		//		return -4;
		//	}

		//	move(advmap, i, pos1, pos2, scoreE, a, b, fitnessE);

		//	finalScoreE += scoreE; 
		finalScoreP += (depth + 2 - i)*scoreP;
		// fitnessE += (depth - i)*fitE;
		fitnessP += (depth - i)*fitP;

		// scoreE /= 70; 
		// scoreP /= 70;

		// if (scoreP + nbTeteMortP >= 6) {
		// 
		// 	// int res = raining(advmap, (scoreP + nbTeteMortP) / 6);
		// 	// if (res == 0)
		// 	// 	return -2;
		// }

		// nbTeteMortP = (scoreP + nbTeteMortP) - 6 * ((scoreP + nbTeteMortP) / 6);
		// if (scoreE + nbTeteMortE >= 6)
		// {
		// 	int res = raining(mymap, (scoreE + nbTeteMortE) / 6);
		// 	if (res == 0)
		// 		return -1;
		// }
		// nbTeteMortE = (scoreE + nbTeteMortE) - 6 * ((scoreE + nbTeteMortE) / 6);
	}
	if (depth == 1)
	{
		cerr << "Player score : " << finalScoreP << endl;
		//	cerr << "Ennemy score : " << finalScoreE << endl;
	}
	return 1; // Everything is all right !!
}


int maxScore = 1;
void mutate(genome& a, int score)
{

	for (int i(0); i < depth; i++)
	{
		a[2 * i] = rand() % 6;
		a[2 * i + 1] = permForAColumn(a[2 * i]);
	}
}

void hillClimbing(genome genomeP, genome genomeE, genome& bestP, genome& bestE, map<string, int> saveGenome = map <string, int>())
{
	swift(genomeP); swift(genomeE);
	int finalScoreP(0), finalScoreE(0), bestScoreP(0), bestScoreE(0);
	int TP(nbreTMP), TE(nbreTME);
	int fitnessP(0), fitnessE(0), fitnessPBuff(0), fitnessEBuff(0);

	vector<char> mymapbuf = mymap;
	// vector<char> advmapbuf = advmap;

	int code = simulate(genomeP, /*genomeE*/ bestScoreP/*, bestScoreE*/, fitnessP, /*fitnessE*/ TP/*, TE*/);

	do
	{
		mymap = mymapbuf;
		bestScoreP = 0;
		randomize(genomeP);
		code = simulate(genomeP, /*genomeE*/ bestScoreP/*, bestScoreE*/, fitnessP, /*fitnessE*/ TP/*, TE*/);
	} while (code != 1 && !getDiffTime());
	if (no_time)
	{
		mymap = mymapbuf;
		
		output();
		for (int i(0); i < 6; i++)
			for (int j(0); j < 4; j++)
			{
				int pos11(-1), pos22(-1); char aa, bb;
				transform(mymap, 0, i, j, pos11, pos22, aa, bb);
				if (pos11 != -1 && pos22 != -1) {
					bestP[0] = i; bestP[1] = j;
					return;
				}
			}
	}
	mymap = mymapbuf;
	//advmap = advmapbuf;
	genome X;
	int T(0); // number of round

	while (!getDiffTime())
	{
		finalScoreP = 0;
		T++;
		// mutate(genomeE, bestScoreE); 
		mutate(genomeP, bestScoreP);
		string a, b;

		for (int i(0); i < 2 * depth; i++)
			a += to_string(genomeP.at(i));

		if (!(saveGenome.count(a) /*|| previousPlay.count(b)*/))
		{
			saveGenome[a] = 1;
			// previousPlay[b] = 1;
			int bufnbreTMP(nbreTMP); // useless
			int bufnbreTME(nbreTME); // useless

			// mymapbuf = mymap;
			// advmapbuf = advmap;

			int code = simulate(genomeP/*, genomeE*/, finalScoreP/*, finalScoreE*/, fitnessPBuff/*, fitnessEBuff*/, bufnbreTMP/*, bufnbreTME*/);

			mymap = mymapbuf;
			// advmap = advmapbuf;

			if (code == 1 && finalScoreP >= bestScoreP)
			{
				bestP = genomeP;
				// fitnessP = fitnessPBuff;
				bestScoreP = finalScoreP;
			}

			//			if (code != -2 && code != -3 && fitnessEBuff >= fitnessE)
			//			{
			//				fitnessE = fitnessEBuff;
			//				bestE = genomeE;
			//		}
		}
	}
	cerr << "Best genome :" << endl;
	for (int i(0); i < depth; i++)
	{
		cerr << "" << bestP[2 * i] << " " << bestP[2 * i + 1] << " | ";
	}
	// cerr << "Fitness : " << fitnessP << " " << endl;
	cerr << "Score : " << bestScoreP << endl;
	cerr << "Number of tour tries:" << T << endl;
}


void play()
{
	genome A(depth * 2), B(depth * 2), C(depth * 2), D(depth * 2);
	int a(0), b(0); // useless
	randomize(A); randomize(B);
	while (1)
	{
		// previousPlay.clear();
		readInput(Tour);
		hillClimbing(A, B, C, D);
		Tour++;
		cerr << "Time : " << std::chrono::duration_cast<std::chrono::milliseconds>(high_resolution_clock::now() - start).count() << endl;
		// cerr << "Dead head upon players : " << nbreTMP << " " << nbreTME << endl;

		cout << C[0] << " " << C[1] << endl; // "x": the column in which to drop your blocks

											 //	simulate(C/*, D*/, a/*, b*/, nbreTMP/*, nbreTME*/, a/*, b*/, 1); // a is for buffer values
		A = C; B = D;
		// output();
	}
}


int main()
{
	srand(time(NULL));
	int u = 40;
	// std::freopen("test", "r", stdin);
	play();

	return 1;
}

