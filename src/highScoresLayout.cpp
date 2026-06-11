#include "highScoresLayout.h"
#include "game.h"
#include "globals.h"
#include "utilities.h"

static const int HIGH_SCORE_ROW_COUNT = 10;

static UILayout* HighScoresModeButtonEventHandler(SDL_Event *event, GameScreen **newScreen, void *userData)
{
	HighScoresLayout *layout = (HighScoresLayout*)userData;

	if (event->type == SDL_KEYUP)
	{
		switch (event->key.keysym.sym)
		{
			case SDLK_RETURN:
			case SDLK_SPACE:
			case SDLK_RIGHT:
				layout->ChangeMode(false);
				break;

			case SDLK_LEFT:
				layout->ChangeMode(true);
				break;
		}
	}

	return nullptr;
}

static UILayout* HighScoresClearButtonEventHandler(SDL_Event *event, GameScreen **newScreen, void *userData)
{
	HighScoresLayout *layout = (HighScoresLayout*)userData;

	if (event->type == SDL_KEYUP)
	{
		switch (event->key.keysym.sym)
		{
			case SDLK_RETURN:
			case SDLK_SPACE:
				layout->ClearCurrentMode();
				break;
		}
	}

	return nullptr;
}

static UILayout* HighScoresBackButtonEventHandler(SDL_Event *event, GameScreen **newScreen, void *userData)
{
	UILayout *newLayout = nullptr;

	if (event->type == SDL_KEYUP)
	{
		switch (event->key.keysym.sym)
		{
			case SDLK_RETURN:
			case SDLK_SPACE:
				Globals::menuLayout.Enter();
				newLayout = &Globals::menuLayout;
				break;
		}
	}

	return newLayout;
}

void HighScoresLayout::Enter()
{
	m_Mode = Globals::MODE;
	m_NeedsRefresh = true;

	m_UIButton[m_SelectedButton].Select(false);
	m_SelectedButton = HIGH_SCORES_UI_BUTTON_BACK;
	m_UIButton[m_SelectedButton].Select(true);
}

void HighScoresLayout::ChangeMode(bool prev)
{
	m_Mode = (GameMode)Utilities::ModuloSum(m_Mode, prev ? -1 : 1, GAME_MODE_TOTAL);
	m_NeedsRefresh = true;
}

void HighScoresLayout::ClearCurrentMode()
{
	Game::Instance().HighScores().Clear(m_Mode);
	m_NeedsRefresh = true;
}

UILayout* HighScoresLayout::HandleInput(SDL_Event *event, GameScreen **newScreen)
{
	UILayout *newLayout = nullptr;

	if (event->type == SDL_KEYUP)
	{
		switch (event->key.keysym.sym)
		{
			case SDLK_UP:
			case SDLK_DOWN:
				m_UIButton[m_SelectedButton].Select(false);

				m_SelectedButton = (HighScoresUIButton)Utilities::ModuloSum(m_SelectedButton,
					event->key.keysym.sym == SDLK_UP ? -1 : 1, HIGH_SCORES_UI_BUTTON_TOTAL);

				m_UIButton[m_SelectedButton].Select(true);
				break;

			case SDLK_ESCAPE:
				newLayout = &Globals::menuLayout;
				break;

			default:
				newLayout = m_UIButton[m_SelectedButton].HandleInput(event, newScreen, this);
				break;
		}
	}

	return newLayout;
}

GameEvent HighScoresLayout::Update(uint32_t elapsed)
{
	TTF_Font *font = Game::Instance().Resources().GetFont();

	if (m_NeedsRefresh)
	{
		RefreshRows(font);
		m_NeedsRefresh = false;
	}

	return GAME_EVENT_NOTHING_HAPPENS;
}

void HighScoresLayout::Render(SDL_Renderer *renderer)
{
	for (int i = 0; i < HIGH_SCORES_UI_LABEL_TOTAL; i++)
	{
		m_UILabel[i].Render(renderer);
	}
	for (int i = 0; i < HIGH_SCORES_UI_BUTTON_TOTAL; i++)
	{
		m_UIButton[i].Render(renderer);
	}
}

bool HighScoresLayout::CreateLayout(SDL_Renderer *renderer)
{
	TTF_Font *font = Game::Instance().Resources().GetFont();
	SDL_Color *textc = &Globals::COLOR_SCHEME->m_Text;
	SDL_Color *selectorc = &Globals::COLOR_SCHEME->m_ButtonSelector;

	if (!m_UILabel[HIGH_SCORES_UI_LABEL_TITLE].Create("最高分", font, textc, renderer,
		0.5f, 0.10f, true, 0.75f)
		|| !m_UILabel[HIGH_SCORES_UI_LABEL_MODE].Create("模式：训练", font, textc, renderer,
			0.5f, 0.20f, true, 0.45f)
		|| !m_UILabel[HIGH_SCORES_UI_LABEL_HINT].Create("排行榜按模式分别保存，每个模式保留前 10 名", font, textc, renderer,
			0.5f, 0.755f, true, 0.30f))
	{
		return false;
	}

	for (int i = 0; i < HIGH_SCORE_ROW_COUNT; i++)
	{
		if (!m_UILabel[HIGH_SCORES_UI_LABEL_ROW_0 + i].Create("-", font, textc, renderer,
			0.5f, 0.295f + (float)i * 0.045f, true, 0.32f))
		{
			return false;
		}
	}

	if (!m_UIButton[HIGH_SCORES_UI_BUTTON_MODE].Create("切换模式", font, textc, selectorc, renderer,
		0.32f, 0.87f, true, HighScoresModeButtonEventHandler, 0.42f)
		|| !m_UIButton[HIGH_SCORES_UI_BUTTON_CLEAR].Create("清空记录", font, textc, selectorc, renderer,
			0.50f, 0.87f, true, HighScoresClearButtonEventHandler, 0.42f)
		|| !m_UIButton[HIGH_SCORES_UI_BUTTON_BACK].Create("返回菜单", font, textc, selectorc, renderer,
			0.68f, 0.87f, true, HighScoresBackButtonEventHandler, 0.42f))
	{
		return false;
	}

	m_UIButton[m_SelectedButton].Select(true);
	m_NeedsRefresh = true;

	return true;
}

void HighScoresLayout::DestroyLayout()
{
}

void HighScoresLayout::RefreshRows(TTF_Font *font)
{
	HighScoreEntry entries[MAX_HIGH_SCORE_ENTRIES_PER_MODE];
	size_t count = Game::Instance().HighScores().CopyTopScores(m_Mode, entries, MAX_HIGH_SCORE_ENTRIES_PER_MODE);
	char text[128];

	sprintf_s(text, "模式：%s", Globals::GameModeDisplayName(m_Mode));
	m_UILabel[HIGH_SCORES_UI_LABEL_MODE].SetText(text, font);

	if (count == 0)
	{
		m_UILabel[HIGH_SCORES_UI_LABEL_ROW_0].SetText("暂无记录，来开一局吧", font);
		for (int i = 1; i < HIGH_SCORE_ROW_COUNT; i++)
		{
			m_UILabel[HIGH_SCORES_UI_LABEL_ROW_0 + i].SetText("-", font);
		}
		return;
	}

	for (int i = 0; i < HIGH_SCORE_ROW_COUNT; i++)
	{
		if (i < (int)count)
		{
			sprintf_s(text, "%02d. %-12s %03u 分  %s",
				i + 1, entries[i].m_Player, entries[i].m_Score, entries[i].m_Date);
			m_UILabel[HIGH_SCORES_UI_LABEL_ROW_0 + i].SetText(text, font);
		}
		else
		{
			m_UILabel[HIGH_SCORES_UI_LABEL_ROW_0 + i].SetText("-", font);
		}
	}
}
