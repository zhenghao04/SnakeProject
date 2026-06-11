#pragma once
#include "gameScreen.h"
#include <cstdint>
#include <vector>

const int MAX_HIGH_SCORE_PLAYER_SIZE = 32;
const int MAX_HIGH_SCORE_DATE_SIZE = 32;
const int MAX_HIGH_SCORE_ENTRIES_PER_MODE = 10;

struct HighScoreEntry
{
	GameMode m_Mode;
	uint32_t m_Score;
	char m_Player[MAX_HIGH_SCORE_PLAYER_SIZE];
	char m_Date[MAX_HIGH_SCORE_DATE_SIZE];
};

class HighScoreManager
{
public:
	HighScoreManager();

	bool Load();
	bool Save();

	int AddScore(GameMode mode, uint32_t score, const char *playerName);
	size_t CopyTopScores(GameMode mode, HighScoreEntry *out, size_t maxEntries);
	uint32_t BestScore(GameMode mode);
	void Clear(GameMode mode);

private:
	bool m_Loaded;
	std::vector<HighScoreEntry> m_Entries;

	void Trim();
};
