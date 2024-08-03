#include <iostream>
#include <vector>
#include <cstdio>
#include <cassert>
#include <iomanip>
#include <string>

using std::cout;
using std::vector;
using std::string;

const float GAMMA = 0.95f;
const float EPSILON = 0.1f;
const float ALPHA = 0.5f;


const int M = 10000;


#define W 12
#define H 4

float policy[W][H][4];

int dx[4] = { 0, 1, 0, -1 };
int dy[4] = { -1, 0, 1, 0 };


// returns direction
int queryPolicy(int x, int y, float epsilon)
{
	if ((static_cast<float>(rand()) / RAND_MAX) < epsilon)
		return rand() % 4;

	int best_a = 0;
	for (int a = 1; a < 4; a++)
		if (policy[x][y][a] > policy[x][y][best_a])
			best_a = a;

	return best_a;
}

// returns if game still running
bool nextState(int x, int y, int a, int& next_x, int& next_y, int& r)
{
	next_x = x + dx[a];
	next_y = y + dy[a];

	if (next_x < 0) next_x = 0;
	else if (next_x >= W) next_x = W - 1;
	if (next_y < 0) next_y = 0;
	else if (next_y >= H) next_y = H - 1;


	if (next_y == H - 1)
	{
		if (next_x == W - 1)
		{
			r = 0;
			return false;
		}

		else if (next_x > 0)
		{
			r = -100;
			return false;
		}

	}

	r = -1;
	return true;
}

// consider the actual next_action.
float sarsaPrediction(int next_x, int next_y, int next_a)
{
	return policy[next_x][next_y][next_a];
}

// consider the best possible next_action.
float qLearningPrediction(int next_x, int next_y)
{
	return policy[next_x][next_y][queryPolicy(next_x, next_y, 0)];
}

float expectedSarsaPrediction(int next_x, int next_y, float epsilon)
{
	float* probs = new float[4];
	for (int a = 0; a < 4; a++)
		probs[a] = epsilon / 4.0f;

	int best_action = queryPolicy(next_x, next_y, 0);
	probs[best_action] += 1 - epsilon;


	float weighted_average = 0;
	for (int a = 0; a < 4; a++)
		weighted_average += policy[next_x][next_y][a] * probs[a];
	
	
	return weighted_average;
}

void playGame(float epsilon, bool debug = false)
{
	int x = 0, y = 3, a = queryPolicy(x, y, epsilon);
	int next_x, next_y, next_a;
	int r;


	bool running = true;

	while (running)
	{
		// get next state
		running = nextState(x, y, a, next_x, next_y, r);

		next_a = queryPolicy(next_x, next_y, epsilon);


		if (debug) // print
			cout << "[" << x << ", " << y << "] a:" << a << " r:" << r << " -> ";
		else // update state action pair
		{
			//float next_q_prediction = sarsaPrediction(next_x, next_y, next_a);
			//float next_q_prediction = qLearningPrediction(next_x, next_y);
			float next_q_prediction = expectedSarsaPrediction(next_x, next_y, epsilon);

			float expected_r = r + GAMMA * next_q_prediction;
			policy[x][y][a] += ALPHA * (expected_r - policy[x][y][a]);
		}
		
		



		// execute action
		x = next_x; y = next_y;
		a = next_a;
	}

	if (debug) cout << "[" << x << ", " << y << "] -> DONE.\n\n";



}

int main()
{
	srand(time(NULL));

	// init policy
	for (int x = 0; x < W; x++)
		for (int y = 0; y < H; y++)
			for (int a = 0; a < 4; a++)
			{
				policy[x][y][a] = 0;
			}



	for (int i = 0; i < M; i++)
	{
		playGame(EPSILON, 0);
	}

	for (int i = 0; i < 4; i++)
	{
		cout << "Action " << i << ". " << policy[0][3][i] << '\n';

	}

	playGame(0, true);
}
