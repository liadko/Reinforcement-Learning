#include <iostream>
#include <vector>


using std::cout;
using std::vector;

const int STATES = 5;
const int ACTIONS = 2;
const float GAMMA = 0.95f;

int main()
{
	vector<float> policy = { 0.4f, 0.6f };
	vector<float> value = { 19.2f, 16.5f, -1.0f, 18.1f, 16.2f };

	// 3 possible rewards
	vector<vector<float>> prob(STATES, vector<float>(3));

	prob[0][0] = 0.34f;
	prob[0][1] = 0.05f;
	prob[0][2] = 0.17f;

	prob[1][0] = 0.17f;
	prob[1][1] = 0.23f;
	prob[1][2] = 0.04f;

	prob[3][0] = 0.12f;
	prob[3][1] = 0.22f;
	prob[3][2] = 0.20f;

	prob[4][0] = 0.09f;
	prob[4][1] = 0.32f;
	prob[4][2] = 0.05f;

	float right_action_value = 0;
	for (int s = 3; s < STATES; s++)
	{
		for (int r = 0; r < 3; r++)
		{
			right_action_value += prob[s][r] * (r + GAMMA * value[s]);
		}
	}

	cout <<  "Expected Value For Right Action: " << right_action_value << '\n';

	float left_action_value = 0;
	for (int s = 0; s < 2; s++)
	{
		for (int r = 0; r < 3; r++)
		{
			left_action_value += prob[s][r] * (r + GAMMA * value[s]);
		}
	}

	cout << "Expected Value For Left Action: " << left_action_value << '\n';

	cout << "Value Of 2: " << 0.4f * left_action_value + 0.6f * right_action_value << '\n';
	/*for (int a = 0; a < ACTIONS; a++)
	{
		expected_2 += policy[a] * 
	}*/

}
