#pragma once
#include "colorScheme.h"
#include <SDL_ttf.h>

class ResourceManager
{
public:
	/**
	 * Load media and other resources.
	 */
	bool LoadResources();

	/**
	 * Unload recources.
	 */
	void UnloadResources();

	/**
	 * Returns font.
	 */
	TTF_Font* GetFont();
	TTF_Font* GetTitleFont();

	/**
	 *
	 */
	ColorScheme* FindColorScheme(char *name);

private:
	bool LoadFont();

	bool LoadColorSchemes();
	void AddColorScheme(ColorScheme *newScheme);
	bool IsUniqueColorSchemeName(char *name);
	void DestroyColorSchemes();

	TTF_Font *m_Font;
	TTF_Font *m_TitleFont;
	ColorScheme *m_ColorSchemes;
};
