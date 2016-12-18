#define _CRT_SECURE_NO_WARNINGS 
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <deque>
#include <map>
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
	if (std::chrono::duration_cast<std::chrono::milliseconds>(fin - start).count() > 0.90 * 100)
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
int updatePosition(vector<char>& deque, const char& k, const int& pos, vector<int>& foo)
{
	foo.push_back(pos);

	for (int i(0); i < 3; i++)
	{
		int u = pos / 6;
		int v = pos - 6 * u;
		u += dirX[i];
		v += dirY[i];
		if (u >= 0 && u <= 11 && v >= 0 && v <= 5)
		{
			char bf = deque[u * 6 + v];
			if (bf != '@') {
				// int w = (bf - 'B') / 5;
				if ((bf == k || bf == 'A') && find(foo.begin(), foo.end(), u * 6 + v) == foo.end())
				{
					foo.push_back(u * 6 + v);
					if (bf != 'A')
						updateNode(deque, k, u * 6 + v, foo);
				}
			}
		}
	}
	int score = foo.size();
	for_each(begin(foo), end(foo), [&deque, &k](const int& x) {
		if (deque[x] != 'A')
			deque[x] = k;
	});
	return score;
}

void dropp(vector<char>& deque, int x)
{
	deque[x] = '@';
	while (x < 72) {
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

//!
//! \brief
//!
//! \param : j 
//! \param : i 
//! \return :int
//!
bool ajouterPiece(vector<char>& deque, int j, const int& perm, const int& i, int &code_suppression, int &nombrepiecesupprime, vector<int>& foo)
{
	int pos1, pos2;
	int k(0);
	while (k < 12 && deque[j + k * 6] != '@')
		k++;
	pos1 = j + k * 6;
	if (k > 11)
		return false; // Impossible to add
	if (!(perm & 1))
	{
		j = (perm == 2 ? j - 1 : j + 1);
		if (j < 0 || j > 5)
			return false;
		k = 0;
		while (k < 12 && deque[j + k * 6] != '@')
			k++;
		if (k == 11)
			return false;
		pos2 = j + k * 6;
	}
	else {
		if (k > 10)
			return false;
		pos2 = j + (k + 1) * 6;
	}


	char a, b;
	a = deque[pos1] = nextinput[(perm == 3 ? 2 * i + 1 : 2 * i)];

	int r = updatePosition(deque, a, pos1, foo);
	b = nextinput[(perm == 3 ? 2 * i : 2 * i + 1)];
	bool joue = false;
	vector<int> foo2;
	vector<int> foo3;
	if (taille(foo, deque) >= 3)
	{
		if (a == b) {
			joue = true;
			foo.push_back(pos2);
		}
		if (taille(foo, deque) > 3) {
			code_suppression += 1;
			foo2 = foo;
			// clear(deque, foo);
			nombrepiecesupprime = foo.size();
		}
	}
	if (!joue) {
		deque[pos2] = b;
		foo.clear();
		int s = updatePosition(deque, b, pos2, foo);
		if (taille(foo, deque) > 3) {
			code_suppression += 1;
			foo3 = foo;
			nombrepiecesupprime += foo.size();
		}
	}
	if (pos1 > pos2)
	{
		clear(deque, foo2);
		clear(deque, foo3);
	}
	else {
		clear(deque, foo3);
		clear(deque, foo2);
	}
	// Return the first block added
	return true;
}


void updateMap(const int &tour, vector<char>& deque, int &score, const int& code, const int& nombrepiecesupprime)
{
	int b = nombrepiecesupprime;
	int cp = 8 * (code - 1);
	int cb = code == 1 ? 0 : 2;
	int combo = 0;

	vector<char> couleur = { nextinput[2 * tour],nextinput[2 * tour + 1] };
	// approximation
	vector<int> visite(72, false);
	int i(0);
	while (i < 72)
	{
		vector<int> foo;
		if (deque[i] != 'A' && deque[i] != '@' && visite[i] == false)
		{
			// int bf = (deque[i] - 'B') / 5;
			// char k = deque[i] - 5 * bf;
			updateNode(deque, deque[i], i, foo);
			if (taille(foo, deque) > 3)
			{
				clear(deque, foo);
				b += nombrepiecesupprime; cp *= 2;
				if (find(couleur.begin(), couleur.end(), deque[i]) == couleur.end())
					couleur.push_back(deque[i]);
				i--;
			}
			else
			{
				for_each(foo.begin(), foo.end(), [&visite, &foo](const int& x)
				{
					visite[x] = true;
				});
			}
		}
		i++;
	}
	int buf = cp;
	while (buf >= 32)
	{
		combo++;
		buf >>= 1;
	}
	score = (10 * b)*(cp + couleur.size() + combo);
	int boule = score / 70;
	if (boule > 3) // OP SHIT Faut le detruire ya pas le time
		score *= 20;
}

vector<char> bestDeque;

//!
//! \brief
//!
//! \param : deque 
//! \param : i 
//! \param : score 
//! \return :int
//!
int play(vector<char>& deque = mapp, const int& i = 0, int score = 0)
{
	if (i == depth) {
		return score;
	}
	if (no_time || getDiffTime())
		return score;

	int min = -1000, localscore, bestcolumn = 0, bestpermut = 0;
	vector<int> v = { 0,1,2,3,4,5 };
	random_shuffle(v.begin(), v.end());
	for(auto j : v){
		for (int perm(0); perm < 4; perm++) {


			int nbrepieceSupprime = 0;
			int localscore = 0;
			vector<char> previousDeque = deque;
			vector<int> foo;
			int code = 0;
			if (ajouterPiece(deque, j, perm, i, code, nbrepieceSupprime, foo))
			{

				if (nbrepieceSupprime > 3)
				{

					updateMap(i, deque, localscore, code, nbrepieceSupprime);
					// update map en incrementing score
				}

				int res = play(deque, i + 1, (depth + 2 - i)*(score + localscore));

				if (res > min /*|| (res == min && rand() & 1)*/)
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
	if (i == 0)
	{
		next_column = bestcolumn;
		mapp = bestDeque;
		next_perm = bestpermut;
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

	//output();
}

void generateGame()
{
	ofstream flux("test");
	vector<pair<int, int> > vecteur;
	for (int i(0); i < 200; i++)
	{
		int n = rand() % 5 + 1;
		int n2 = rand() % 5 + 1;
		vecteur.push_back(make_pair(n, n2));
	}
	for (int i(0); i < 200 - 8; i++)
	{
		for (int j(i); j < i + 8; j++)
		{
			flux << vecteur.at(j).first << " " << vecteur.at(j).second << " ";
		}
		flux << endl;
	}
	flux.close();
}



int main()
{
	srand(time(NULL));
	// generateGame();
	int u = 40;
	//std::freopen("test", "r", stdin);	
	// game loop

	while (1) {
		readInput(Tour);
		play();
		output();
		Tour++;
		cout << next_column << " " << next_perm << endl; // "x": the column in which to drop your blocks
	}
}

