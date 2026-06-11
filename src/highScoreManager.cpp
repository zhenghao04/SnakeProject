#include "highScoreManager.h"
#include "utilities.h"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <string>

static const char *HIGH_SCORE_FOLDER_PATH = "user";
static const char *HIGH_SCORE_FILE_PATH = "user\\highscores.tsv";

static bool CompareHighScores(const HighScoreEntry &a, const HighScoreEntry &b)
{
	if (a.m_Mode != b.m_Mode)
	{
		return a.m_Mode < b.m_Mode;
	}
	if (a.m_Score != b.m_Score)
	{
		return a.m_Score > b.m_Score;
	}
	return strcmp(a.m_Date, b.m_Date) > 0;
}

static void CopyText(char *dst, size_t size, const char *src)
{
	strncpy_s(dst, size, src ? src : "", _TRUNCATE);
}

static void CopyText(char *dst, size_t size, const std::string &src)
{
	strncpy_s(dst, size, src.c_str(), _TRUNCATE);
}

static void FillDate(char *dst, size_t size)
{
	time_t t = time(nullptr);
	struct tm localTime;

	localtime_s(&localTime, &t);
	strftime(dst, size, "%Y-%m-%d %H:%M", &localTime);
}

HighScoreManager::HighScoreManager() :
	m_Loaded(false)
{
}

bool HighScoreManager::Load()
{
	m_Entries.clear();
	m_Loaded = true;

	if (!Utilities::FileExists(HIGH_SCORE_FILE_PATH))
	{
		return true;
	}

	std::ifstream file(HIGH_SCORE_FILE_PATH);
	if (!file.is_open())
	{
		LOG_ERR("Failed opening high score file: %s", HIGH_SCORE_FILE_PATH);
		return false;
	}

	std::string line;
	while (std::getline(file, line))
	{
		size_t first = line.find('\t');
		size_t second = line.find('\t', first == std::string::npos ? first : first + 1);
		size_t third = line.find('\t', second == std::string::npos ? second : second + 1);

		if (first == std::string::npos || second == std::string::npos || third == std::string::npos)
		{
			continue;
		}

		int mode = atoi(line.substr(0, first).c_str());
		if (mode < 0 || mode >= GAME_MODE_TOTAL)
		{
			continue;
		}

		HighScoreEntry entry;
		entry.m_Mode = (GameMode)mode;
		entry.m_Score = (uint32_t)strtoul(line.substr(first + 1, second - first - 1).c_str(), nullptr, 10);
		CopyText(entry.m_Date, sizeof(entry.m_Date), line.substr(second + 1, third - second - 1));
		CopyText(entry.m_Player, sizeof(entry.m_Player), line.substr(third + 1));

		m_Entries.push_back(entry);
	}

	Trim();
	return true;
}

bool HighScoreManager::Save()
{
	if (!Utilities::FileExists(HIGH_SCORE_FOLDER_PATH))
	{
		if (!Utilities::CreateNewDirectory(HIGH_SCORE_FOLDER_PATH))
		{
			LOG_ERR("Failed creating high score folder: %s", HIGH_SCORE_FOLDER_PATH);
			return false;
		}
	}

	std::ofstream file(HIGH_SCORE_FILE_PATH, std::ios::trunc);
	if (!file.is_open())
	{
		LOG_ERR("Failed writing high score file: %s", HIGH_SCORE_FILE_PATH);
		return false;
	}

	Trim();
	for (size_t i = 0; i < m_Entries.size(); i++)
	{
		file << (int)m_Entries[i].m_Mode << '\t'
			<< m_Entries[i].m_Score << '\t'
			<< m_Entries[i].m_Date << '\t'
			<< m_Entries[i].m_Player << '\n';
	}

	return true;
}

int HighScoreManager::AddScore(GameMode mode, uint32_t score, const char *playerName)
{
	if (score == 0)
	{
		return 0;
	}

	if (!m_Loaded)
	{
		Load();
	}

	HighScoreEntry entry;
	entry.m_Mode = mode;
	entry.m_Score = score;
	CopyText(entry.m_Player, sizeof(entry.m_Player), playerName && strlen(playerName) ? playerName : "玩家");
	FillDate(entry.m_Date, sizeof(entry.m_Date));

	m_Entries.push_back(entry);
	Trim();
	Save();

	int rank = 0;
	for (size_t i = 0; i < m_Entries.size(); i++)
	{
		if (m_Entries[i].m_Mode != mode)
		{
			continue;
		}

		rank++;
		if (m_Entries[i].m_Score == entry.m_Score
			&& !strcmp(m_Entries[i].m_Date, entry.m_Date)
			&& !strcmp(m_Entries[i].m_Player, entry.m_Player))
		{
			return rank;
		}
	}

	return 0;
}

size_t HighScoreManager::CopyTopScores(GameMode mode, HighScoreEntry *out, size_t maxEntries)
{
	if (!m_Loaded)
	{
		Load();
	}

	Trim();

	size_t copied = 0;
	for (size_t i = 0; i < m_Entries.size() && copied < maxEntries; i++)
	{
		if (m_Entries[i].m_Mode == mode)
		{
			out[copied++] = m_Entries[i];
		}
	}

	return copied;
}

uint32_t HighScoreManager::BestScore(GameMode mode)
{
	HighScoreEntry entries[MAX_HIGH_SCORE_ENTRIES_PER_MODE];
	size_t count = CopyTopScores(mode, entries, MAX_HIGH_SCORE_ENTRIES_PER_MODE);

	return count ? entries[0].m_Score : 0;
}

void HighScoreManager::Clear(GameMode mode)
{
	if (!m_Loaded)
	{
		Load();
	}

	m_Entries.erase(std::remove_if(m_Entries.begin(), m_Entries.end(),
		[mode](const HighScoreEntry &entry) { return entry.m_Mode == mode; }),
		m_Entries.end());
	Save();
}

void HighScoreManager::Trim()
{
	std::sort(m_Entries.begin(), m_Entries.end(), CompareHighScores);

	std::vector<HighScoreEntry> trimmed;
	int modeCounts[GAME_MODE_TOTAL] = { 0 };

	for (size_t i = 0; i < m_Entries.size(); i++)
	{
		int mode = (int)m_Entries[i].m_Mode;
		if (mode < 0 || mode >= GAME_MODE_TOTAL)
		{
			continue;
		}
		if (modeCounts[mode] >= MAX_HIGH_SCORE_ENTRIES_PER_MODE)
		{
			continue;
		}

		trimmed.push_back(m_Entries[i]);
		modeCounts[mode]++;
	}

	m_Entries.swap(trimmed);
}
