#include <iostream>
#include <vector>
#include <cstdio>
#include <cassert>
#include <iomanip>
#include <string>
#include <queue>
#include <deque>

using std::cout;
using std::vector;
using std::string;
using std::queue;
using std::deque;

const float GAMMA = 0.95f;
const float EPSILON = 0.1f;
const float ALPHA = 0.5f;
const int TD_N = 1;
const float GAMMA_TO_THE_N = pow(GAMMA, TD_N);

const int M = 500;


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
int selectAction(const State& state, float epsilon)
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
bool nextState(const State& state, int a, State& next_state, float& r)
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

float rewardSum(deque<float>& rewards)
{
	float reward_sum = 0;
	
	for (int i = 0; i < rewards.size(); i++)
	{
		reward_sum *= GAMMA;

		reward_sum += rewards.back();

		rewards.push_front(rewards.back());
		rewards.pop_back();
	}

	return reward_sum;
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

	queue<State> states({ state });
	queue<int> actions({ a });
	deque<float> rewards;


	bool running = true;

	while (running)
	{
		// get next state
		float r;
		running = nextState(state, a, next_state, r);

		next_a = selectAction(next_state, epsilon);

		states.push(next_state);
		actions.push(next_a);
		rewards.push_back(r);


		if (debug) // print
			cout << "[" << state.x << ", " << state.y << "] a:" << a << " r:" << r << " -> ";
		else if (rewards.size() == TD_N) // update state action pair
		{
			State updated_state = states.front(); states.pop();
			int action_taken = actions.front(); actions.pop();

			float next_q_prediction = expectedSarsaPrediction(next_state, epsilon);

			float reward_sum = rewardSum(rewards);

			float expected_r = reward_sum + GAMMA_TO_THE_N * next_q_prediction;
			policy[updated_state.x][updated_state.y][action_taken] += ALPHA * (expected_r - policy[updated_state.x][updated_state.y][action_taken]);

			rewards.pop_front();
			
		}



		// execute action
		state = next_state;
		a = next_a;
	}

	if (debug) cout << "[" << state.x << ", " << state.y << "] -> DONE.\n\n";


	while (rewards.size())
	{
		State updated_state = states.front();
		int action_taken = actions.front();


		float expected_r = rewardSum(rewards);
		policy[updated_state.x][updated_state.y][action_taken] += ALPHA * (expected_r - policy[updated_state.x][updated_state.y][action_taken]);


		states.pop();
		actions.pop();
		rewards.pop_front();

	}



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
