#include <iostream>
#include <vector>
#include <cassert>

using std::cout;
using std::vector;

//const int STATES = 5;
//const int ACTIONS = 2;
const float GAMMA = 1.0f;
const float HEADS = 0.4f;
const int MAX = 100;

vector<int> policy(MAX, 0);
vector<float> value(MAX + 1, 0);

int reward(int new_s)
{
	return 0;

	if (new_s == MAX)
		return 1;
}

void improvePolicyBasedOnValue()
{
	for (int s = 1; s < MAX; s++)
	{
		int best_action = -1;
		float best_action_value = -1;

		for (int a = 1; a <= std::min(s, MAX - s); a++)
		{
			assert(s + a <= MAX);
			assert(s - a >= 0);

			float action_value = 0;
			action_value += HEADS * (reward(s + a) + GAMMA * value[s + a]);
			action_value += (1 - HEADS) * (reward(s - a) + GAMMA * value[s - a]);
			if (action_value > best_action_value + 0.00001f)
			{
				best_action_value = action_value;
				best_action = a;
			}
		}
		//cout << "best action at state " << s << " is to bet " << best_action << '\n';
		policy[s] = best_action;
	}
}


void updateValueToMatchPolicy()
{
	vector<float> new_value(MAX + 1, 0);

	for (int s = 1; s < MAX; s++)
	{
		int chosen_action = policy[s];

		assert(s + chosen_action <= MAX);
		assert(s - chosen_action >= 0);
		
		new_value[s] = 0;
		new_value[s] += HEADS * (reward(s + chosen_action) + GAMMA * value[s + chosen_action]);
		new_value[s] += (1 - HEADS) * (reward(s - chosen_action) + GAMMA * value[s - chosen_action]);
	}
	for (int s = 1; s < MAX; s++)
		value[s] = new_value[s];

}

int main()
{
	value[MAX] = 1.0f;
	for (int i = 0; i < 10000; i++)
	{
		improvePolicyBasedOnValue();

		/*for (int i = 1; i < MAX; i++)
		{
			cout << policy[i] << '\n';
		}

		cout << '\n';*/
		updateValueToMatchPolicy();

		//for (int i = 1; i < MAX; i++)
		//{
		//	cout << value[i] << '\n';
		//}
		//cout << '\n';
		//
	}

	cout << "Final Value Function\n";
	for (int i = 1; i < MAX; i++)
	{
		cout << value[i] << '\n';
	}

	cout << "Final Policy\n";
	for (int i = 1; i < MAX; i++)
	{
		cout << policy[i] << '\n';
	}
	
}
