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


// TD LEARNING
const float GAMMA = 1.0f;
const float EPSILON = 0.0f;
const float ALPHA = 0.025f;
const int TD_N = 10;
const float GAMMA_TO_THE_N = pow(GAMMA, TD_N);
const int M = 1185;

// RBF
const float RBF_SCALER = 0.003f;

const int ACTION_COUNT = 3;
const int FEATURES_W = 35; // X and W are feature vectors
const int FEATURE_COUNT = FEATURES_W * FEATURES_W;

// MOUNTAIN CAR
std::pair<float, float> pos_bounds = { -1.2f, 0.5f };
std::pair<float, float> vel_bounds = { -0.07f, 0.07f };

float W[ACTION_COUNT][FEATURE_COUNT];


struct State
{
	float x, v;
};

float RBF(const State& state, float x, float v)
{
	float dx = state.x - x;
	float dv = state.v - v;

	dx /= (pos_bounds.second - pos_bounds.first) * RBF_SCALER;
	dv /= (vel_bounds.second - vel_bounds.first) * RBF_SCALER;

	float sqr_dist = dx * dx + dv * dv;
	return expf(-sqr_dist);
}

vector<float> getFeatures(const State& state)
{
	vector<float> X(FEATURE_COUNT);

	float dx = (pos_bounds.second - pos_bounds.first) / (FEATURES_W - 1);
	float dv = (vel_bounds.second - vel_bounds.first) / (FEATURES_W - 1);

	float feature_sum = 0;

	float current_v = vel_bounds.first;
	int i = 0;
	for (int v = 0; v < FEATURES_W; v++)
	{
		float current_x = pos_bounds.first;
		for (int x = 0; x < FEATURES_W; x++)
		{
			float rbf = RBF(state, current_x, current_v);
			feature_sum += rbf;


			X[i] = rbf;
			i++;

			current_x += dx;
		}

		current_v += dv;

	}

	for (i = 0; i < FEATURE_COUNT; i++)
		X[i] /= feature_sum;


	return X;

}

float qFunction(const vector<float>& feature_vector, int a)
{
	// get feature vector
	assert(a >= -1 && a <= 1);

	if (a == 0) return -10000; // disregard action 0

	float action_value = 0;
	for (int i = 0; i < FEATURE_COUNT; i++)
		action_value += W[a + 1][i] * feature_vector[i]; // WEIGHT TIMES FEATURE

	return action_value;
}

// max over q function, with epsilon considerashe
int selectAction(const vector<float>& feature_vector, float epsilon)
{
	int best_action = -1;
	float best_action_value = qFunction(feature_vector, -1);
	for (int i = 0; i <= 1; i++)
	{
		float action_value = qFunction(feature_vector, i);
		if (action_value > best_action_value)
		{
			best_action_value = action_value;
			best_action = i;
		}
	}
	return best_action;
}

// returns true if the state is not terminal
bool nextState(const State& state, int a, State& next_state, float& r)
{

	next_state.v = state.v + a / 1000.0f - cos(3.0f * state.x) / 400.0f;
	next_state.v = std::max(vel_bounds.first, std::min(vel_bounds.second, next_state.v));

	next_state.x = state.x + next_state.v;

	if (next_state.x >= 0.5f)
	{
		r = 0;
		return false;
	}
	if (next_state.x <= -1.2f)
	{
		next_state.x = -1.2f;
		next_state.v = 0;
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

State getInitialState()
{
	// x = [-0.6, -0.4] , v = 0
	return { -0.6f + 0.2f * ((float)rand() / RAND_MAX) , 0 };
}

int playGame(float epsilon, bool print = false)
{
	State state = getInitialState(); vector<float> feature_vector = getFeatures(state);
	int a = selectAction(feature_vector, epsilon);
	State next_state; int next_a; vector<float> next_feature_vector;

	queue<vector<float>> features({ feature_vector });
	queue<int> actions({ a });
	deque<float> rewards;


	bool running = true;
	int t;
	for (t = 0; running; t++)
	{
		// get next state, reward, and termination
		float r;
		running = nextState(state, a, next_state, r);

		// extract next feature vector
		next_feature_vector = getFeatures(next_state);

		// select next action based on features
		next_a = selectAction(next_feature_vector, epsilon);

		actions.push(next_a);
		features.push(next_feature_vector);
		rewards.push_back(r);


		if (print) // print
			cout << a << ", " << state.x  << "\n";

		// update state action pair
		if (rewards.size() == TD_N) 
		{
			vector<float>& feature_vector_to_update = features.front();
			int action_taken = actions.front();


			float reward_sum = rewardSum(rewards);

			float expected_r = reward_sum + GAMMA_TO_THE_N * qFunction(next_feature_vector, next_a);
			for (int i = 0; i < FEATURE_COUNT; i++)
			{
				W[action_taken + 1][i] += ALPHA * (expected_r - W[action_taken + 1][i] * feature_vector_to_update[i]) * feature_vector_to_update[i];
			}

			features.pop();
			actions.pop();
			rewards.pop_front();

		}



		// execute action
		state = next_state;
		a = next_a;
	}

	if (print) cout << state.x << "-> DONE.\n\n";

	while (rewards.size())
	{
		vector<float>& feature_vector_to_update = features.front();
		int action_taken = actions.front();

		float reward_sum = rewardSum(rewards);

		float expected_r = reward_sum;
		for (int i = 0; i < FEATURE_COUNT; i++)
		{
			W[action_taken + 1][i] += ALPHA * (expected_r - W[action_taken + 1][i] * feature_vector_to_update[i]) * feature_vector_to_update[i];
		}

		features.pop();
		actions.pop();
		rewards.pop_front();

	}


	return t;
}

int main()
{
	//srand(time(NULL));

	// init policy
	for (int a = 0; a < ACTION_COUNT; a++)
		for (int f = 0; f < FEATURE_COUNT; f++)
			W[a][f] = 0;


	for (int i = 0; i < M; i++)
	{
		int steps = playGame(EPSILON);

		cout << steps;
		if (steps < 150)
			cout << " BINGO";
		if (steps < 100)
			cout << " HOLLLLLLLLYYYYYYYYYYYY SHITTTTTTTTTT";
		cout << '\n';

		//cout << "Run Number " << i + 1 << " took " << steps << " steps.\n";
	}


	// run without 
	cout << playGame(0.0f, true) << " STEPS";

}
