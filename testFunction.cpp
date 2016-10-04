/*

void testMove() {
	Point init(0, 0);
	Point end(16000, 9000);
	init.moveWithinRange(end, init.distance(end) / 2);
	assert(init.X == 8000 && init.Y == 4500);
	init.moveWithinRange(Point(0, 0), init.distance(end) * sqrt(2));
	assert(init.X == 0 && init.Y == 0);
}


void testUpdate() {
	State state;
	state.m_datas.resize(2);
	update(state, 1000, 2000);
	assert(state.m_player.X == 1000 && state.m_player.Y == 2000);
	state.m_ennemies.resize(3);
	update(state, 1, 1000, 1000, 100);
	Ennemy& ennemi = state.m_ennemies[1];
	assert(ennemi.m_life == 100 && ennemi.X == 1000 && ennemi.Y == 1000);

	update(state, 1, 10, 10, 50);
	assert(ennemi.m_life == 50);
}


void testFindTarget() {
	Data data, data1, data2, data3;
	data = Point(5, 5);
	data2 = Point(10, 10);
	data3 = Point(10, 0);
	data1 = Point(3, 7); data1.catched = true;
	Ennemy ennemi;
	ennemi.X = 3; ennemi.Y = 7;
	State state;
	state.m_datas = vector<Data>({ data,data1,data2,data3 });
	findNewTarget(state, ennemi);
	assert(ennemi.m_dataTarget == 0);
}



void testMove() {
	Data data, data1, data2, data3;
	data = Point(5, 5);
	data2 = Point(10, 10);
	data3 = Point(10, 0);
	data1 = Point(3, 7); data1.catched = true;
	Ennemy ennemi;
	ennemi.X = 3; ennemi.Y = 7; ennemi.m_life = 100;
	State state;
	state.m_datas = vector<Data>({ data,data1,data2,data3 });
	state.m_player = Point(5, 5);
	Move move; move.X = 4; move.Y = 4; move.m_move = MOVES::MOVE; move.m_target = 5;
	play(state, move);
	assert(state.m_player.X == 4 && state.m_player.Y == 4);

	move; move.X = 4; move.Y = 0; move.m_move = MOVES::MOVE; move.m_target = 1;
	play(state, move);
	assert(state.m_player.X == 4 && state.m_player.Y == 3);

}



void testFindTarget() {
	State state;
	state.m_player = Point(5, 5);
	Ennemy ennemi; ennemi.X = 4, ennemi.Y = 4, ennemi.m_life = 250000;
	state.m_ennemies = vector<Ennemy>({ ennemi });

	Move m; m.m_move = MOVES::SHOOT; m.m_target = 0;

	play(state, m);
	assert(ennemi.m_life == 125000);
}



Move dumbAI(const State& state) {
	int minAllDist = numeric_limits<int>::max();
	Point target;
	for (const auto& data : state.m_datas) {
		Point target;
		int ennemyDistMin = numeric_limits<int>::max();
		for (const auto& ennemi : state.m_ennemies) {
			int ennemyDist = data.distance2(ennemi);
			if (ennemyDistMin > ennemyDist) {
				ennemyDist = ennemyDistMin;
				p = ennemi;
			}
		}
		if (ennemyDistMin < minAllDist) {
			int distancetoData = state.m_player.distance(p);

		}
	}
}



void testMovingPlayer() {
	State state;

	state.m_player = Point(10000, 4000);

	Ennemy ennemi; ennemi.X = 9999, ennemi.Y = 3999, ennemi.m_life = 250;
	state.m_ennemies = vector<Ennemy>({ ennemi });

	Data data, data1, data2, data3;
	data.X = 4000; data.Y = 4000;
	state.m_datas = vector<Data>({ data });

	Timer time; time.reset();
	geneticAlgorithmTurn(time, state, vector<Genome>());


}
*/