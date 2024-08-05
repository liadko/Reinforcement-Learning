#include <iostream>
#include <vector>
#include <cstdio>
#include <cassert>
#include <iomanip>
#include <string>
#include <queue>

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

struct State
{
	int x, y;
};

// returns direction to move in, based on the policy and epsilon
int selectAction(const State& state , float epsilon)
{
	if ((static_cast<float>(rand()) / RAND_MAX) < epsilon)
		return rand() % 4;

	int best_a = 0;
	for (int a = 1; a < 4; a++)
		if (policy[state.x][state.y][a] > policy[state.x][state.y][best_a])
			best_a = a;

	return best_a;
}

// returns if game still running
bool nextState(const State& state, int a, State& next_state, int& r)
{
	next_state = { state.x + dx[a], state.y + dy[a] };
	
	// Bounds
	if (next_state.x < 0) next_state.x = 0;
	else if (next_state.x >= W) next_state.x = W - 1;
	if (next_state.y < 0) next_state.y = 0;
	else if (next_state.y >= H) next_state.y = H - 1;


	if (next_state.y == H - 1)
	{
		if (next_state.x == W - 1)
		{
			r = 0;
			return false;
		}

		else if (next_state.x > 0)
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
float qLearningPrediction(const State& next_state)
{
	return policy[next_state.x][next_state.y][selectAction(next_state, 0)];
}

// weigh the prediction based on the next action probabilities.
float expectedSarsaPrediction(const State& next_state, float epsilon)
{
	float probs[4];
	for (int a = 0; a < 4; a++)
		probs[a] = epsilon / 4.0f;

	int best_action = selectAction(next_state, 0);
	probs[best_action] += 1 - epsilon;


	float weighted_average = 0;
	for (int a = 0; a < 4; a++)
		weighted_average += policy[next_state.x][next_state.y][a] * probs[a];
	
	
	return weighted_average;
}

void playGame(float epsilon, bool debug = false)
{
	State state = { 0, 3 }; int a = selectAction(state, epsilon);
	State next_state; int next_a;
	
	
	int r;


	bool running = true;

	while (running)
	{
		// get next state
		running = nextState(state, a, next_state, r);

		next_a = selectAction(next_state, epsilon);


		if (debug) // print
			cout << "[" << state.x << ", " << state.y << "] a:" << a << " r:" << r << " -> ";
		else // update state action pair
		{
			//float next_q_prediction = sarsaPrediction(next_state, next_a);
			//float next_q_prediction = qLearningPrediction(next_state);
			float next_q_prediction = expectedSarsaPrediction(next_state, epsilon);

			float expected_r = r + GAMMA * next_q_prediction;
			policy[state.x][state.y][a] += ALPHA * (expected_r - policy[state.x][state.y][a]);
		}
		
		



		// execute action
		state = next_state;
		a = next_a;
	}

	if (debug) cout << "[" << state.x << ", " << state.y << "] -> DONE.\n\n";



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
		playGame(EPSILON);
	}


	playGame(0, true);
}
