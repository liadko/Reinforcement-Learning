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

const float GAMMA = 1.0f;
const float EPSILON = 0.0f;
const float ALPHA = 4e-5f;
const int TD_N = 2000;
const float GAMMA_TO_THE_N = pow(GAMMA, TD_N);

const int M = 500000;

const int STATES = 1000;
const int FEATURES = 10; // X and W are feature vectors


float W[FEATURES];


struct State
{
	int s;
};

float valueFunction(const State& state)
{
	int index = state.s / 100;
	return W[index]; // one hot encoding basically means just take the W at that index
}

int selectAction(const State& state, float epsilon)
{
	int mult = 1;
	if (rand() % 2 == 0)
		mult = -1;

	return mult * ((rand() % 100) + 1);
}

// returns if game still running
bool nextState(const State& state, int a, State& next_state, float& r)
{
	next_state = { state.s + a };

	if (next_state.s >= STATES - 1)
	{
		r = 1;
		return false;
	}
	if (next_state.s <= 0)
	{
		r = -1;
		return false;
	}

	r = 0;
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


void playGame(float epsilon, bool debug = false)
{
	State state = { 500 }; int a = selectAction(state, epsilon);
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
			cout << "[" << state.s << "] a:" << a << " r:" << r << " -> ";
		else if (rewards.size() == TD_N) // update state action pair
		{
			State updated_state = states.front(); states.pop();
			int action_taken = actions.front(); actions.pop();


			float reward_sum = rewardSum(rewards);
			int bucket_index = updated_state.s / 100;

			float expected_r = reward_sum + GAMMA_TO_THE_N * valueFunction(next_state);
			W[bucket_index] += ALPHA * (expected_r - W[bucket_index]);

			rewards.pop_front();

		}



		// execute action
		state = next_state;
		a = next_a;
	}

	if (debug) cout << "[" << state.s << "] -> DONE.\n\n";


	while (rewards.size())
	{
		State updated_state = states.front();
		int action_taken = actions.front();

		int bucket_index = updated_state.s / 100;

		float expected_r = rewardSum(rewards);
		W[bucket_index] += ALPHA * (expected_r - W[bucket_index]);


		states.pop();
		actions.pop();
		rewards.pop_front();

	}



}

int main()
{
	srand(time(NULL));

	// init policy
	for (int i = 0; i < FEATURES; i++)
		W[i] = 0;

	for (int i = 0; i < M; i++)
	{
		playGame(EPSILON);
	}

	playGame(0, true);

	for (int i = 0; i < 1000; i+=10)
		cout << valueFunction({ i }) << '\n';
}
