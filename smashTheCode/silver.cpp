#define _CRT_SECURE_NO_WARNINGS 
#include <iostream>
#include <string>
#include <iterator>
#include <functional>
#include <vector>
#include <algorithm>
#include <deque>
#include <map>
#include <set>
#include <queue>
#include <ctime>
#include <chrono>
#include <fstream>
int DEBUG = 0;

int dirX[] = { -1,0,0,1 };
int dirY[] = { 0,-1,1,0 };
bool no_time = false;
int Tour = 0;

int depth = 3;



using namespace std;
using namespace std::chrono;
vector<char> mapp(72, '@'); // La map
vector<string> ch = { "hey","I","just","met","you","and","this","is","crazy","so",
"here","my","number","...","06","22","oh","wait","you","already","loose" };

deque<char> nextinput; // next bloc to add
int next_column; // next value
int next_perm;
map<string, pair<int, int> > scores;
high_resolution_clock::time_point start;

inline string to_string(vector<char> deque)
{
	string x(deque.begin(), deque.end());
	return x;
}

inline bool getDiffTime()
{
	auto fin = std::chrono::high_resolution_clock::now();
	if (std::chrono::duration_cast<std::chrono::milliseconds>(fin - start).count() > 0.97 * 100)
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
	for (int i = 11; i > -1; i--) {
		{
			for (int j(0); j < 6; j++)
				cerr << mapp.at(j + i * 6) << " ";
		}
		cerr << endl;
	}
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
			// int w = (bf - 'B') / 5;
			if ((couleur == bf || bf == 'A') && find(foo.begin(), foo.end(), newpos) == foo.end())
			{
				foo.push_back(newpos);
				if (bf != 'A')
					updateNode(deque, couleur, newpos, foo);
			}
		}
	}
}

//!
//! \brief Find all bloc of the same color in the neighborhood of pos
//!
//! \param : k : the color of the new element at pos
//! \param : pos : the pos of the element
//! \param : foo : a vector to keep track of position of all block of the same color
//! \return :int : return the score for the updatePosition
//!
void updatePosition(const vector<char>& deque, const char& k, const int& pos, vector<int>& foo, const set<int>& foo2 = set<int>())
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
			if (bf != '@') {
				if ((bf == k || bf == 'A') && find(foo.begin(), foo.end(), nextpos) == foo.end() && (foo2.size() == 0 || find(foo2.begin(), foo2.end(), nextpos) == foo2.end()))
				{
					foo.push_back(nextpos);
					if (bf != 'A')
						updateNode(deque, k, nextpos, foo);
				}
			}
		}
	}
}

void dropp(vector<char>& deque, int x)
{
	deque[x] = '@';
	while (x < 72) {
		deque[x] = (x + 6 > 71 ? '@' : deque[x + 6]);
		x += 6;
		if (x == '@')
			break;
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
		if (k == 11)
			return;
		pos2 = j + k * 6;
	}
	else {
		if (k > 10)
			return;
		pos2 = j + (k + 1) * 6;
	}
	a = nextinput[(perm == 3 ? 2 * i + 1 : 2 * i)];
	b = nextinput[(perm == 3 ? 2 * i : 2 * i + 1)];
}

void clearMap(vector<char> &deque, vector<int>& foo, int &score, int depth = 0)
{
	clear(deque, foo);
	vector<int> foo1;
	for (const auto &pos : foo)
	{
		if (find(foo1.begin(), foo1.end(), pos) == foo1.end()) {
			vector<int> foobuff;
			updateNode(deque, deque[pos], pos, foobuff);
			if (taille(foobuff, deque) > 3)
			{
				foo1.insert(foo1.end(), foobuff.begin(), foobuff.end());
			}
		}
	}
	int B = foo1.size();
	if (B) {
		score += (10 * B)*((depth == 0 ? 8 : depth * 8) + max(B - 4, 0));
		clearMap(deque, foo1, score, (depth == 0 ? 8 : depth * 8));
	}
}


void move(vector<char>& deque, const int& i, const int& pos1, const int& pos2, int& score, const int& a, const int& b)
{

	vector<int> foo;
	set<int> fooX; int B(0);

	if (a == b) // If there are both the same
	{
		deque[pos1] = a;
		deque[pos2] = b;
		updatePosition(deque, a, pos2, foo);
		if (taille(foo, deque) > 3) {
			fooX.insert(foo.begin(), foo.end());
			B += foo.size();
		}
	}
	else {
		deque[pos1] = a;
		updatePosition(deque, a, pos1, foo);
		if (taille(foo, deque) > 3) {
			fooX.insert(foo.begin(), foo.end());
			B += foo.size();
		}

		foo.clear();
		deque[pos2] = b;
		updatePosition(deque, b, pos2, foo, fooX);
		if (taille(foo, deque) > 3) {
			std::copy(foo.begin(), foo.end(), std::inserter(fooX, fooX.end()));
			B += foo.size();
		}
	}

	vector<int> res(fooX.begin(), fooX.end());

	if (taille(res, deque) > 3) {
		score = (10 * B)*((a == b ? 2 : 0) + max(B - 4, 0));
		clearMap(deque, res, score);
	}

}






vector<char> bestDeque;
using Cuple = pair<int, vector<pair<int, int>>>;
auto comp = [](const Cuple& a, const Cuple& b) { if (a.first > b.first) return true; else return false; };

int play1(vector<char>& deque = mapp, const int& i = 0, int score = 0)
{
	if (i == depth) {
		return score;
	}
	if (no_time || getDiffTime())
		return score;

	int min = -1000, localscore(0), bestcolumn = 0, bestpermut = 0;
	vector<int> v = { 0,1,2,3,4,5 };
	random_shuffle(v.begin(), v.end());
	for (auto j : v) {
		for (int perm(0); perm < 4; perm++) {
			int pos1(-1), pos2(-1); char a, b;
			transform(deque, i, j, perm, pos1, pos2, a, b);

			vector<char> previousDeque = deque;
			int localscore = 0;

			if (pos1 != -1 && pos2 != -1)
			{
				try {
					move(deque, i, pos1, pos2, localscore, a, b);
				}
				catch (std::exception e)
				{
					cerr << e.what() << endl;
				}

				int					res = play1(deque, i + 1, (score + localscore));

				if (res > min)
				{
					min = res;
					bestcolumn = j;
					bestpermut = perm;
					if (i == 0)
						bestDeque = deque;
				}
			}
			deque = previousDeque;
		}
	}
	return min;
}

int tryagain(vector<char> deque, priority_queue< Cuple, vector<Cuple>, decltype(comp) > bestscores)
{
	int incr(0);
	int score;
	int scorefinal = -10000;
	int res;
	try {
		while (!bestscores.empty()) {
			if (no_time || getDiffTime())
				break;
			auto x = bestscores.top(); bestscores.pop();

			for (int i(0); i < x.second.size(); i++)
			{
				int pos1(-1), pos2(-1); char a, b;
				transform(deque, i, x.second.at(i).first, x.second.at(i).second, pos1, pos2, a, b);
				move(deque, i, pos1, pos2, score, a, b);
			}

			int s = play1(deque);

			deque = mapp;

			if (s > scorefinal)
			{
				res = incr;
			}
			incr++;
		}
	}
	catch (std::exception e)
	{
		cerr << e.what() << endl;
	};

	return res;
}




int play(priority_queue< Cuple, vector<Cuple>, decltype(comp) >& bestscores, vector<pair<int, int> >& previousMove, vector<char>& deque = mapp, const int& i = 0, int score = 0)
{
	if (i == depth) {
		auto elmt = Cuple(score, previousMove);
		bestscores.push(elmt);
		if (bestscores.size() > 5)
			bestscores.pop();
		return score;
	}
	if (no_time || getDiffTime())
		return score;
	int min = -1000, localscore(0), bestcolumn = 0, bestpermut = 0;
	{
		vector<int> v = { 0,1,2,3,4,5 };
		random_shuffle(v.begin(), v.end());
		for (auto j : v) {
			for (int perm(0); perm < 4; perm++) {

				int pos1(-1), pos2(-1); char a, b;
				transform(deque, i, j, perm, pos1, pos2, a, b);

				vector<char> previousDeque = deque;
				int localscore = 0;

				if (pos1 != -1 && pos2 != -1)
				{
					move(deque, i, pos1, pos2, localscore, a, b);
					previousMove.push_back(make_pair(j, perm));

					int res = play(bestscores, previousMove, deque, i + 1, (score + localscore));

					if (previousMove.size() > 0)
						previousMove.pop_back();

					if (res > min)
					{
						min = res;
						bestcolumn = j;
						bestpermut = perm;
						if (i == 0)
							bestDeque = deque;
					}
				}
				deque = previousDeque;
			}
		}
	}
	if (i == 0)
	{
		int res = tryagain(deque, bestscores);
		cerr << "best genome :" << res << endl;
		next_column = bestcolumn; mapp = bestDeque; next_perm = bestpermut;
	}
	return min;
}



void readInput(const int& k)
{
	no_time = false;
	for (int i(0); i < 8; i++)
	{
		char color1, color2;
		cin >> color1 >> color2;
		color1 += 17;
		color2 += 17;

		if (!k) {
			nextinput.push_back(color1);
			nextinput.push_back(color2);
		}
		else if (i == 7) {
			nextinput.pop_front(); nextinput.pop_front();
			nextinput.push_back(color1); nextinput.push_back(color2);
		}
	}
	for (int i = 11; i > -1; i--) {
		string row;
		cin >> row;
		for (int j(0); j < row.size(); j++)
		{
			if (row.at(j) == '0') {
				mapp[i * 6 + j] = 'A';
			}

		}
	}
	for (int i = 0; i < 12; i++) {
		string row; // One line of the map ('.' = empty, '0' = skull block, '1' to '5' = colored block)
		cin >> row; cin.ignore();
	}
	start = high_resolution_clock::now();
}



inline void printinfo(priority_queue< Cuple, vector<Cuple>, decltype(comp) >& bestscores)
{
	cerr << "time : " << std::chrono::duration_cast<std::chrono::milliseconds>(high_resolution_clock::now() - start).count() << endl;
	cerr << "best move :" << endl;
	while (!bestscores.empty())
	{
		auto x = bestscores.top(); bestscores.pop();
		cerr << x.first << " : ";
		for (const auto& y : x.second)
			cerr << " " << y.first << " " << y.second << " | ";
		cerr << endl;
	}
}

int main()
{
	srand(time(NULL));
	priority_queue< Cuple, vector<Cuple>, decltype(comp) >	bestscores(comp);
	// generateGame();
	int u = 40;
	//std::freopen("test", "r", stdin);	

	while (1) {
		readInput(Tour);
		vector<pair<int, int> > c; priority_queue< Cuple, vector<Cuple>, decltype(comp) >	bestscores(comp);
		play(bestscores, c);
		output();

		printinfo(bestscores);
		cout << next_column << " " << next_perm << " "; // "x": the column in which to drop your blocks
	// 	cout << ch[Tour % ch.size()];
		cout << endl;
		Tour++;
	}
}

