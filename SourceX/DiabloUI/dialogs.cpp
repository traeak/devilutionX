#include "DiabloUI/dialogs.h"

#include "controls/menu_controls.h"
#include "all.h"
#include "dx.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/button.h"
#include "DiabloUI/fonts.h"
#include "DiabloUI/errorart.h"
#include "display.h"

namespace devilution {

namespace {

Art dialogArt;
bool fontWasLoaded;

bool dialogEnd;

void DialogActionOK()
{
	dialogEnd = true;
}

std::vector<UiItemBase *> vecNULL;
std::vector<UiItemBase *> vecOkDialog;

// clang-format off
#define BLANKCOLOR { 0, 0xFF, 0, 0 }
// clang-format on

void LoadFallbackPalette()
{
	// clang-format off
	static const SDL_Color fallback_palette[256] = {
		{ 0x00, 0x00, 0x00, 0 },
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR,
		{ 0xff, 0xfd, 0x9f, 0 },
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		{ 0xe8, 0xca, 0xca, 0 },
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		{ 0x05, 0x07, 0x0c, 0 },
		{ 0xff, 0xe3, 0xa4, 0 },
		{ 0xee, 0xd1, 0x8c, 0 },
		{ 0xdd, 0xc4, 0x7e, 0 },
		{ 0xcc, 0xb7, 0x75, 0 },
		{ 0xbc, 0xa8, 0x6c, 0 },
		{ 0xab, 0x9a, 0x63, 0 },
		{ 0x98, 0x8b, 0x5d, 0 },
		{ 0x87, 0x7e, 0x54, 0 },
		{ 0x78, 0x6f, 0x49, 0 },
		{ 0x69, 0x60, 0x3f, 0 },
		{ 0x5b, 0x51, 0x34, 0 },
		{ 0x48, 0x40, 0x27, 0 },
		{ 0x39, 0x31, 0x1d, 0 },
		{ 0x31, 0x28, 0x16, 0 },
		{ 0x1a, 0x14, 0x08, 0 },
		{ 0x14, 0x0b, 0x00, 0 },
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR,
		{ 0xff, 0xbd, 0xbd, 0 },
		{ 0xf4, 0x96, 0x96, 0 },
		{ 0xe8, 0x7d, 0x7d, 0 },
		{ 0xe0, 0x6c, 0x6c, 0 },
		{ 0xd8, 0x5b, 0x5b, 0 },
		{ 0xcf, 0x49, 0x49, 0 },
		{ 0xc7, 0x38, 0x38, 0 },
		{ 0xbf, 0x27, 0x27, 0 },
		{ 0xa9, 0x22, 0x22, 0 },
		{ 0x93, 0x1e, 0x1e, 0 },
		{ 0x7c, 0x19, 0x19, 0 },
		{ 0x66, 0x15, 0x15, 0 },
		{ 0x4f, 0x11, 0x11, 0 },
		{ 0x39, 0x0d, 0x0d, 0 },
		{ 0x23, 0x09, 0x09, 0 },
		{ 0x0c, 0x05, 0x05, 0 },
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR,
		{ 0xf3, 0xf3, 0xf3, 0 },
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR,
		{ 0xff, 0xff, 0x00, 0 },
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR,
	};
	// clang-format on
	ApplyGamma(logical_palette, fallback_palette, 256);
}

void Init(const char *text, const char *caption, bool error, bool renderBehind)
{
	if (caption == NULL) {
		SDL_Rect rect1 = { (Sint16)(PANEL_LEFT + 180), (Sint16)(UI_OFFSET_Y + 168), 280, 144 };
		vecOkDialog.push_back(new UiImage(&dialogArt, rect1));

		SDL_Rect rect2 = { (Sint16)(PANEL_LEFT + 200), (Sint16)(UI_OFFSET_Y + 211), 240, 80 };
		vecOkDialog.push_back(new UiText(text, rect2, UIS_CENTER));

		SDL_Rect rect3 = { (Sint16)(PANEL_LEFT + 265), (Sint16)(UI_OFFSET_Y + 265), SML_BUTTON_WIDTH, SML_BUTTON_HEIGHT };
		vecOkDialog.push_back(new UiButton(&SmlButton, "OK", &DialogActionOK, rect3, 0));
	} else {
		SDL_Rect rect1 = { (Sint16)(PANEL_LEFT + 127), (Sint16)(UI_OFFSET_Y + 100), 385, 280 };
		vecOkDialog.push_back(new UiImage(&dialogArt, rect1));

		SDL_Color color = { 255, 255, 0, 0 };
		SDL_Rect rect2 = { (Sint16)(PANEL_LEFT + 147), (Sint16)(UI_OFFSET_Y + 110), 345, 20 };
		vecOkDialog.push_back(new UiText(text, color, rect2, UIS_CENTER));

		SDL_Rect rect3 = { (Sint16)(PANEL_LEFT + 147), (Sint16)(UI_OFFSET_Y + 141), 345, 190 };
		vecOkDialog.push_back(new UiText(caption, rect3, UIS_CENTER));

		SDL_Rect rect4 = { (Sint16)(PANEL_LEFT + 264), (Sint16)(UI_OFFSET_Y + 335), SML_BUTTON_WIDTH, SML_BUTTON_HEIGHT };
		vecOkDialog.push_back(new UiButton(&SmlButton, "OK", &DialogActionOK, rect4, 0));
	}

	if (!renderBehind) {
		LoadBackgroundArt("ui_art\\black.pcx");
		if (ArtBackground.surface == NULL) {
			LoadFallbackPalette();
		}
	}
	SetFadeLevel(256);
	if (caption == NULL) {
		LoadMaskedArt(error ? "ui_art\\srpopup.pcx" : "ui_art\\spopup.pcx", &dialogArt);
	} else {
		if (error) {
			LoadArt(&dialogArt, popupData, 385, 280);
		} else {
			LoadMaskedArt("ui_art\\lpopup.pcx", &dialogArt);
		}
	}
	LoadSmlButtonArt();

	fontWasLoaded = font != NULL;
	if (!fontWasLoaded)
		LoadTtfFont();
}

void Deinit()
{
	dialogArt.Unload();
	UnloadSmlButtonArt();
	if (!fontWasLoaded)
		UnloadTtfFont();

	for (std::size_t i = 0; i < vecOkDialog.size(); i++) {
		UiItemBase *pUIItem = vecOkDialog[i];
		delete pUIItem;
	}
	vecOkDialog.clear();
}

void DialogLoop(std::vector<UiItemBase *> items, std::vector<UiItemBase *> renderBehind)
{
	SDL_Event event;
	dialogEnd = false;
	do {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				UiItemMouseEvents(&event, items);
				break;
			default:
				switch (GetMenuAction(event)) {
				case MenuAction_BACK:
				case MenuAction_SELECT:
					dialogEnd = true;
					break;
				default:
					break;
				}
				break;
			}
			UiHandleEvents(&event);
		}

		if (renderBehind.size() == 0) {
			SDL_FillRect(DiabloUiSurface(), NULL, 0);
		} else {
			UiRenderItems(renderBehind);
		}
		UiRenderItems(items);
		DrawMouse();
		UiFadeIn();
	} while (!dialogEnd);
}

} // namespace

void UiOkDialog(const char *text, const char *caption, bool error, std::vector<UiItemBase *> renderBehind)
{
	static bool inDialog = false;

	if (!gbActive || inDialog) {
		if (SDL_ShowCursor(SDL_ENABLE) <= -1) {
			SDL_Log(SDL_GetError());
		}
#ifndef RUN_TESTS
		if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, text, caption, NULL) <= -1) {
			SDL_Log(SDL_GetError());
#else
		{
#endif
			SDL_Log(text);
			SDL_Log(caption);
		}
		return;
	}

	inDialog = true;
	Init(text, caption, error, renderBehind.size() > 0);
	DialogLoop(vecOkDialog, renderBehind);
	Deinit();
	inDialog = false;
}

void UiErrorOkDialog(const char *text, const char *caption, std::vector<UiItemBase *> renderBehind)
{
	UiOkDialog(text, caption, /*error=*/true, renderBehind);
}

void UiErrorOkDialog(const char *text, const char *caption, bool error)
{
	UiOkDialog(text, caption, error, vecNULL);
}

void UiErrorOkDialog(const char *text, std::vector<UiItemBase *> renderBehind)
{
	UiErrorOkDialog(text, NULL, renderBehind);
}

} // namespace devilution
