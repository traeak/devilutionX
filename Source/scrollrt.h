//HEADER_GOES_HERE
#ifndef __SCROLLRT_H__
#define __SCROLLRT_H__

extern int light_table_index;
extern int PitchTbl[BUFFER_HEIGHT];
extern BYTE *gpBufEnd;
extern DWORD level_cel_block;
extern char arch_draw_type;
extern DDSURFACEDESC DDS_desc;
extern int cel_transparency_active;
extern int level_piece_id;
extern void (*DrawPlrProc)(int, int, int, int, int, BYTE *, int, int, int, int);
extern int draw_monster_num;

void ClearCursor();
void DrawMissile(int x, int y, int sx, int sy, BOOL pre);
void DrawDeadPlayer(int x, int y, int sx, int sy);
void DrawPlayer(int pnum, int x, int y, int px, int py, BYTE *pCelBuff, int nCel, int nWidth);
void DrawUi(int StartX, int StartY);
void DrawGame(int x, int y);
void scrollrt_draw(int x, int y, int sx, int sy, int chunks);
void scrollrt_draw_dungeon(BYTE *pBuff, int sx, int sy, int dx, int dy);
void DrawMonster(int x, int y, int mx, int my, int m);
void DrawObject(int x, int y, int ox, int oy, BOOL pre);
void ClearScreenBuffer();
#ifdef _DEBUG
void ScrollView();
void EnableFrameCount();
#endif
void scrollrt_draw_game_screen(BOOL draw_cursor);
void scrollrt_draw_cursor_back_buffer();
void scrollrt_draw_cursor_item();
void DrawMain(int dwHgt);
#ifdef _DEBUG
void DrawFPS();
#endif
void DoBlitScreen(DWORD dwX, DWORD dwY, DWORD dwWdt, DWORD dwHgt);
void DrawAndBlit();

/* rdata */

/* data */

/* used in 1.00 debug */
extern char *szMonModeAssert[18];
extern char *szPlrModeAssert[12];

#endif /* __SCROLLRT_H__ */
