#pragma once
#include "highScoreManager.h"
#include "uiLayout.h"
#include "uiWidget.h"

enum HighScoresUILabel
{
	HIGH_SCORES_UI_LABEL_TITLE = 0,
	HIGH_SCORES_UI_LABEL_MODE,
	HIGH_SCORES_UI_LABEL_ROW_0,
	HIGH_SCORES_UI_LABEL_ROW_1,
	HIGH_SCORES_UI_LABEL_ROW_2,
	HIGH_SCORES_UI_LABEL_ROW_3,
	HIGH_SCORES_UI_LABEL_ROW_4,
	HIGH_SCORES_UI_LABEL_ROW_5,
	HIGH_SCORES_UI_LABEL_ROW_6,
	HIGH_SCORES_UI_LABEL_ROW_7,
	HIGH_SCORES_UI_LABEL_ROW_8,
	HIGH_SCORES_UI_LABEL_ROW_9,
	HIGH_SCORES_UI_LABEL_HINT,
	HIGH_SCORES_UI_LABEL_TOTAL
};

enum HighScoresUIButton
{
	HIGH_SCORES_UI_BUTTON_MODE = 0,
	HIGH_SCORES_UI_BUTTON_CLEAR,
	HIGH_SCORES_UI_BUTTON_BACK,
	HIGH_SCORES_UI_BUTTON_TOTAL
};

class HighScoresLayout : public UILayout
{
public:
	HighScoresLayout() :
		m_SelectedButton(HIGH_SCORES_UI_BUTTON_BACK),
		m_Mode(GAME_MODE_TRAINING),
		m_NeedsRefresh(true)
	{};

	void Enter();
	void ChangeMode(bool prev);
	void ClearCurrentMode();

	UILayout* HandleInput(SDL_Event *event, GameScreen **newScreen);
	GameEvent Update(uint32_t elapsed);
	void Render(SDL_Renderer *renderer);

	bool CreateLayout(SDL_Renderer *renderer);
	void DestroyLayout();

private:
	UILabel m_UILabel[HIGH_SCORES_UI_LABEL_TOTAL];
	UIButton m_UIButton[HIGH_SCORES_UI_BUTTON_TOTAL];

	HighScoresUIButton m_SelectedButton;
	GameMode m_Mode;
	bool m_NeedsRefresh;

	void RefreshRows(TTF_Font *font);
};
