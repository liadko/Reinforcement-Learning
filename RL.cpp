#include <iostream>
#include <vector>
#include <cstdio>
#include <cassert>
#include <iomanip>
#include <string>
#include <windows.h> // For clipboard functions

using std::cout;
using std::vector;
using std::string;

//const int STATES = 5;
//const int ACTIONS = 2;
//const float GAMMA = 1.0f;
const float EPSILON = 0.03f;
const float ALPHA = 1.0f / 5000;

const int M = 100'000'000;

// HIT, ACE, PLAYER SUM, DEALER SUM
float policy[2][2][21 + 1][11 + 1];

int actions[30], has_ace[30], player_sums[30], dealer_sums[30];
int episode_length = 0;

int randomCard()
{
	int card = rand() % 13 + 1;

	if (card == 1)
		return 11;

	if (card < 10)
		return card;

	return 10;
}


void copyToClipboard(const std::string& s) {
	OpenClipboard(0);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
	if (!hg) {
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg), s.c_str(), s.size() + 1);
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
	GlobalFree(hg);
}

struct State
{
	int player = 0, player_ace = 0;
	int dealer = 0, dealer_ace = 0;


	void playerDrawCard()
	{
		int random_card = randomCard();
		if (random_card == 11)
			player_ace++;


		player += random_card;

		if (player > 21 && player_ace)
		{
			player_ace--;
			player -= 10;
		}


	}
	void dealerDrawCard()
	{
		int random_card = randomCard();
		if (random_card == 11)
			dealer_ace++;


		dealer += random_card;

		if (dealer > 21 && dealer_ace)
		{
			dealer_ace--;
			dealer -= 10;
		}


	}
	void dealerDrawAll()
	{

		while (dealer < 17)
		{
			int random_card = randomCard();
			if (random_card == 11)
				dealer_ace++;


			dealer += random_card;

			if (dealer > 21 && dealer_ace)
			{
				dealer_ace--;
				dealer -= 10;
			}
		}


	}
};

int policySaysHit(int player_sum, int dealer_sum, int ace, float epsilon)
{
	if ((float)rand() / RAND_MAX < epsilon)
		return rand() % 2;


	return policy[1][ace][player_sum][dealer_sum] > policy[0][ace][player_sum][dealer_sum];
}

void changePolicyBasedOnEpisode(int result)
{
	int a, ace, p, d;
	for (int t = 0; t < episode_length; t++)
	{
		a = actions[t];  ace = has_ace[t]; p = player_sums[t]; d = dealer_sums[t];


		policy[a][ace][p][d] += ALPHA * (result - policy[a][ace][p][d]);

	}
}

float playGame(float epsilon)
{
	State state;

	state.dealerDrawCard();

	state.playerDrawCard();
	state.playerDrawCard();

	while (state.player < 12)
		state.playerDrawCard();

	episode_length = 0; // start recording episode

	while (state.player <= 21)
	{
		bool hit = policySaysHit(state.player, state.dealer, state.player_ace, epsilon);

		has_ace[episode_length] = state.player_ace;
		player_sums[episode_length] = state.player;
		dealer_sums[episode_length] = state.dealer;
		actions[episode_length] = hit;
		episode_length++;

		if (hit)
			state.playerDrawCard();

		else // stand
			break;

	}

	// BUST
	if (state.player > 21)
		return -1;

	// BLACKJACK
	if (state.player == 21)
		return 1;


	state.dealerDrawAll();


	if (state.dealer > 21)
		return 1;
	if (state.player < state.dealer)
		return -1;

	return 0;

}

int main()
{
	srand(time(NULL));


	for (int a = 0; a <= 1; a++)
		for (int ace = 0; ace <= 1; ace++)
			for (int p = 12; p <= 21; p++)
				for (int d = 2; d <= 11; d++)
				{
					policy[a][ace][p][d] = 0;
				}




	for (int i = 0; i < M; i++)
	{
		int game = playGame(EPSILON);
		//cout << game << " ";

		/*for(int t = 0; t < episode_length; t++)
			cout << "t" << t << "-> ACE:" << has_ace[t]  << " P:" << player_sums[t] << " D:" << dealer_sums[t] << "  ";
		cout << "RESULT: " << game << '\n';*/


		changePolicyBasedOnEpisode(game);
	}

	string s = "";
	for (int d = 1; d <= 10; d++)
	{
		for (int p = 12; p <= 21; p++)
		{
			float no_ace_decision = policy[1][0][p][d] - policy[0][0][p][d];

			if (d == 1)
				no_ace_decision = policy[1][0][p][11] - policy[0][0][p][11];
			
			s += std::to_string(no_ace_decision);
			
			if (p < 21)
				s += "\t";

			if (p == 21)
				s += "\t\t\t";

		}
		for (int p = 12; p <= 21; p++)
		{
			float has_ace_decision = policy[1][1][p][d] - policy[0][1][p][d];

			if (d == 1)
				has_ace_decision = policy[1][1][p][11] - policy[0][1][p][11];

			
			s += std::to_string(has_ace_decision);

			if (p < 21)
				s += "\t";


		}
		s += '\n';
	}

	cout << s;
	copyToClipboard(s);

	int TEST_N = 10000;
	int wins = 0, losses = 0, draws = 0;
	for (int i = 0; i < TEST_N; i++)
	{
		int game = playGame(0);

		if (game == 1)
			wins++;
		else if (game == -1)
			losses++;
		else
			draws++;

	}
	cout << "\n\nW/L/D " << (float)wins / (TEST_N) * 100 << "/" << (float)losses / (TEST_N) * 100 << "/" << (float)draws / (TEST_N) * 100;

	std::cin;
}
