#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

void town_special(BYTE *pBuff, int nCel)
{
#if 0
	int w;
	BYTE *end;
	BYTE width;
	BYTE *src, *dst;
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)pSpecialCels;
	src = &pSpecialCels[pFrameTable[nCel]];
	dst = pBuff;
	end = &src[pFrameTable[nCel + 1] - pFrameTable[nCel]];

	for(; src != end; dst -= BUFFER_WIDTH + 64) {
		for(w = 64; w;) {
			width = *src++;
			if(!(width & 0x80)) {
				w -= width;
					if(width & 1) {
						dst[0] = src[0];
						src++;
						dst++;
					}
					width >>= 1;
					if(width & 1) {
						dst[0] = src[0];
						dst[1] = src[1];
						src += 2;
						dst += 2;
					}
					width >>= 1;
					for(; width; width--) {
						dst[0] = src[0];
						dst[1] = src[1];
						dst[2] = src[2];
						dst[3] = src[3];
						src += 4;
						dst += 4;
					}
			} else {
				width = -(char)width;
				dst += width;
				w -= width;
			}
		}
	}
#endif
}

void town_draw_e_flag(BYTE *pBuff, int x, int y, int sx, int sy)
{
	int i;
	BYTE *dst;
	MICROS *pMap;

	dst = pBuff;
	pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];

	for (i = 0; i < 12; i += 2) {
		level_cel_block = pMap->mt[i];
		if (level_cel_block != 0) {
			drawLowerScreen(dst);
		}
		level_cel_block = pMap->mt[i + 1];
		if (level_cel_block != 0) {
			drawLowerScreen(dst + 32);
		}
		dst -= BUFFER_WIDTH * 32;
	}

	town_draw_town(pBuff, x, y, sx, sy, 0);
}

void town_draw_town(BYTE *pBuff, int x, int y, int sx, int sy, BOOL some_flag)
{
	int mi, px, py;
	char bv;

	if (dItem[x][y] != 0) {
		bv = dItem[x][y] - 1;
		px = sx - item[bv]._iAnimWidth2;
		if (bv == pcursitem) {
			CelDecodeClr(181, px, sy, item[bv]._iAnimData, item[bv]._iAnimFrame, item[bv]._iAnimWidth);
		}
		/// ASSERT: assert(item[bv]._iAnimData);
		CelDrawHdrOnly(px, sy, item[bv]._iAnimData, item[bv]._iAnimFrame, item[bv]._iAnimWidth);
	}
	if (dFlags[x][y] & BFLAG_MONSTLR) {
		mi = -(dMonster[x][y - 1] + 1);
		px = sx - towner[mi]._tAnimWidth2;
		if (mi == pcursmonst) {
			CelDecodeClr(166, px, sy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth);
		}
		/// ASSERT: assert(towner[mi]._tAnimData);
		CelDrawHdrOnly(px, sy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth);
	}
	if (dMonster[x][y] > 0) {
		mi = dMonster[x][y] - 1;
		px = sx - towner[mi]._tAnimWidth2;
		if (mi == pcursmonst) {
			CelDecodeClr(166, px, sy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth);
		}
		/// ASSERT: assert(towner[mi]._tAnimData);
		CelDrawHdrOnly(px, sy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth);
	}
	if (dFlags[x][y] & BFLAG_PLAYERLR) {
		bv = -(dPlayer[x][y - 1] + 1);
		px = sx + plr[bv]._pxoff - plr[bv]._pAnimWidth2;
		py = sy + plr[bv]._pyoff;
		if (bv == pcursplr) {
			Cl2DecodeFrm2(165, px, py, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth);
		}
		/// ASSERT: assert(plr[bv]._pAnimData);
		Cl2DecodeFrm1(px, py, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth);
		if (some_flag && plr[bv]._peflag) {
			town_draw_e_flag(pBuff - 64, x - 1, y + 1, sx - 64, sy);
		}
	}
	if (dFlags[x][y] & BFLAG_DEAD_PLAYER) {
		DrawDeadPlayer(x, y, sx, sy, 0);
	}
	if (dPlayer[x][y] > 0) {
		bv = dPlayer[x][y] - 1;
		px = sx + plr[bv]._pxoff - plr[bv]._pAnimWidth2;
		py = sy + plr[bv]._pyoff;
		if (bv == pcursplr) {
			Cl2DecodeFrm2(165, px, py, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth);
		}
		/// ASSERT: assert(plr[bv]._pAnimData);
		Cl2DecodeFrm1(px, py, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth);
		if (some_flag && plr[bv]._peflag) {
			town_draw_e_flag(pBuff - 64, x - 1, y + 1, sx - 64, sy);
		}
	}
	if (dFlags[x][y] & BFLAG_MISSILE) {
		DrawMissile(x, y, sx, sy, 0);
	}
	if (dArch[x][y] != 0) {
		town_special(pBuff, dArch[x][y]);
	}
}

void town_draw(int x, int y, int sx, int sy, int chunks, int some_flag)
{
	int i, j;
	BYTE *dst;
	MICROS *pMap;

	/// ASSERT: assert(gpBuffer);

	if (some_flag) {
		if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
			level_cel_block = dPiece[x][y];
			if (level_cel_block != 0) {
				dst = &gpBuffer[sx + 32 + PitchTbl[sy]];
				pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];
				for (i = 1; i < 17; i += 2) {
					level_cel_block = pMap->mt[i];
					if (level_cel_block != 0) {
						drawLowerScreen(dst);
					}
					dst -= BUFFER_WIDTH * 32;
				}
				town_draw_town(&gpBuffer[sx + PitchTbl[sy]], x, y, sx, sy, 0);
			} else {
				world_draw_black_tile(&gpBuffer[sx + PitchTbl[sy]]);
			}
		} else {
			world_draw_black_tile(&gpBuffer[sx + PitchTbl[sy]]);
		}
		x++;
		y--;
		sx += 64;
	}

	for (j = 0; j < chunks - some_flag; j++) {
		if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
			level_cel_block = dPiece[x][y];
			if (level_cel_block != 0) {
				dst = &gpBuffer[sx + PitchTbl[sy]];
				pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];
				for (i = 0; i < 16; i += 2) {
					level_cel_block = pMap->mt[i];
					if (level_cel_block != 0) {
						drawLowerScreen(dst);
					}
					level_cel_block = pMap->mt[i + 1];
					if (level_cel_block != 0) {
						drawLowerScreen(dst + 32);
					}
					dst -= BUFFER_WIDTH * 32;
				}
				town_draw_town(&gpBuffer[sx + PitchTbl[sy]], x, y, sx, sy, 1);
			} else {
				world_draw_black_tile(&gpBuffer[sx + PitchTbl[sy]]);
			}
		} else {
			world_draw_black_tile(&gpBuffer[sx + PitchTbl[sy]]);
		}
		x++;
		y--;
		sx += 64;
	}

	if (some_flag) {
		if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
			level_cel_block = dPiece[x][y];
			if (level_cel_block != 0) {
				dst = &gpBuffer[sx + PitchTbl[sy]];
				pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];
				for (i = 0; i < 16; i += 2) {
					level_cel_block = pMap->mt[i];
					if (level_cel_block != 0) {
						drawLowerScreen(dst);
					}
					dst -= BUFFER_WIDTH * 32;
				}
				town_draw_town(&gpBuffer[sx + PitchTbl[sy]], x, y, sx, sy, 0);
			} else {
				world_draw_black_tile(&gpBuffer[sx + PitchTbl[sy]]);
			}
		} else {
			world_draw_black_tile(&gpBuffer[sx + PitchTbl[sy]]);
		}
	}
}

void T_DrawGame(int x, int y)
{
	int i, sx, sy, chunks, blocks;

	scr_pix_width = SCREEN_WIDTH;
	scr_pix_height = VIEWPORT_HEIGHT;
	dword_5C2FF8 = 10;
	dword_5C2FFC = 11;

	sx = ScrollInfo._sxoff + SCREEN_X;
	sy = ScrollInfo._syoff + 175;
	x -= 10;
	y--;
	chunks = ceil(SCREEN_WIDTH / 64);
	blocks = ceil(VIEWPORT_HEIGHT / 32) + 4;

	if (chrflag || questlog) {
		x += 2;
		y -= 2;
		sx += 288;
		chunks -= 4;
	}
	if (invflag || sbookflag) {
		x += 2;
		y -= 2;
		sx -= 32;
		chunks -= 4;
	}

	switch (ScrollInfo._sdir) {
	case SDIR_N:
		sy -= 32;
		x--;
		y--;
		blocks++;
		break;
	case SDIR_NE:
		sy -= 32;
		x--;
		y--;
		chunks++;
		blocks++;
		break;
	case SDIR_E:
		chunks++;
		break;
	case SDIR_SE:
		chunks++;
		blocks++;
		break;
	case SDIR_S:
		blocks++;
		break;
	case SDIR_SW:
		sx -= 64;
		x--;
		y++;
		chunks++;
		blocks++;
		break;
	case SDIR_W:
		sx -= 64;
		x--;
		y++;
		chunks++;
		break;
	case SDIR_NW:
		sx -= 64;
		sy -= 32;
		x -= 2;
		chunks++;
		blocks++;
		break;
	}

	/// ASSERT: assert(gpBuffer);
	gpBufEnd = &gpBuffer[PitchTbl[VIEWPORT_HEIGHT + SCREEN_Y]];
	for (i = 0; i < blocks; i++) {
		town_draw(x, y, sx, sy, chunks, 0);
		y++;
		sx -= 32;
		sy += 16;
		town_draw(x, y, sx, sy, chunks, 1);
		x++;
		sx += 32;
		sy += 16;
	}
}

void T_DrawZoom(int x, int y)
{
	int i, sx, sy, chunks, blocks;
	int wdt, nSrcOff, nDstOff;

	scr_pix_width = SCREEN_WIDTH / 2 + 64;
	scr_pix_height = VIEWPORT_HEIGHT / 2 + 16;
	dword_5C2FF8 = 6;
	dword_5C2FFC = 6;

	sx = ScrollInfo._sxoff + 64;
	sy = ScrollInfo._syoff + 143;
	x -= 6;
	y--;
	chunks = 6;
	blocks = 0;

	switch (ScrollInfo._sdir) {
	case SDIR_NONE:
		break;
	case SDIR_N:
		sy -= 32;
		x--;
		y--;
		blocks++;
		break;
	case SDIR_NE:
		sy -= 32;
		x--;
		y--;
		chunks++;
		blocks++;
		break;
	case SDIR_E:
		chunks++;
		break;
	case SDIR_SE:
		chunks++;
		blocks++;
		break;
	case SDIR_S:
		blocks++;
		break;
	case SDIR_SW:
		sx -= 64;
		x--;
		y++;
		chunks++;
		blocks++;
		break;
	case SDIR_W:
		sx -= 64;
		x--;
		y++;
		chunks++;
		break;
	case SDIR_NW:
		sx -= 64;
		sy -= 32;
		x -= 2;
		chunks++;
		blocks++;
		break;
	}

	/// ASSERT: assert(gpBuffer);
	gpBufEnd = &gpBuffer[PitchTbl[143]];
	for (i = 0; i < 7; i++) {
		town_draw(x, y, sx, sy, chunks, 0);
		y++;
		sx -= 32;
		sy += 16;
		town_draw(x, y, sx, sy, chunks, 1);
		x++;
		sx += 32;
		sy += 16;
	}
	/// ASSERT: assert(gpBuffer);
	gpBufEnd = &gpBuffer[PitchTbl[320]];
	for (i = 0; i < blocks; i++) {
		town_draw(x, y, sx, sy, chunks, 0);
		y++;
		sx -= 32;
		sy += 16;
		town_draw(x, y, sx, sy, chunks, 1);
		x++;
		sx += 32;
		sy += 16;
	}
	for (i = 0; i < 7; i++) {
		town_draw(x, y, sx, sy, chunks, 0);
		y++;
		sx -= 32;
		sy += 16;
		town_draw(x, y, sx, sy, chunks, 1);
		x++;
		sx += 32;
		sy += 16;
	}

	if (chrflag || questlog) {
		nSrcOff = SCREENXY(112, 159);
		nDstOff = SCREENXY(320, 350);
		wdt = SCREEN_WIDTH / 2 - 320 / 2;
	} else if (invflag || sbookflag) {
		nSrcOff = SCREENXY(112, 159);
		nDstOff = SCREENXY(0, 350);
		wdt = SCREEN_WIDTH / 2 - 320 / 2;
	} else {
		nSrcOff = SCREENXY(32, 159);
		nDstOff = SCREENXY(0, 350);
		wdt = SCREEN_WIDTH / 2;
	}

	/// ASSERT: assert(gpBuffer);

	int hgt;
	BYTE *src, *dst1, *dst2;

	src = &gpBuffer[nSrcOff];
	dst1 = &gpBuffer[nDstOff];
	dst2 = &gpBuffer[nDstOff + BUFFER_WIDTH];

	for (hgt = 176; hgt != 0; hgt--, src -= BUFFER_WIDTH + wdt, dst1 -= 2 * (BUFFER_WIDTH + wdt), dst2 -= 2 * (BUFFER_WIDTH + wdt)) {
		for (i = wdt; i != 0; i--) {
			*dst1++ = *src;
			*dst1++ = *src;
			*dst2++ = *src;
			*dst2++ = *src;
			src++;
		}
	}
}

void T_DrawView(int StartX, int StartY)
{
	light_table_index = 0;
	cel_transparency_active = 0;
	if (zoomflag)
		T_DrawGame(StartX, StartY);
	else
		T_DrawZoom(StartX, StartY);
	if (automapflag)
		DrawAutomap();
	if (stextflag && !qtextflag)
		DrawSText();
	if (invflag) {
		DrawInv();
	} else if (sbookflag) {
		DrawSpellBook();
	}
	DrawDurIcon();
	if (chrflag) {
		DrawChr();
	} else if (questlog) {
		DrawQuestLog();
	} else if (plr[myplr]._pStatPts && !spselflag) {
		DrawLevelUpIcon();
	}
	if (uitemflag)
		DrawUniqueInfo();
	if (qtextflag)
		DrawQText();
	if (spselflag)
		DrawSpellList();
	if (dropGoldFlag)
		DrawGoldSplit(dropGoldValue);
	if (helpflag)
		DrawHelp();
	if (msgflag)
		DrawDiabloMsg();
	if (PauseMode && !deathflag)
		gmenu_draw_pause();
	DrawPlrMsg();
	gmenu_draw();
	doom_draw();
	DrawInfoBox();
	DrawLifeFlask();
	DrawManaFlask();
}

void SetTownMicros()
{
	int i, x, y, lv;
	WORD *pPiece;
	MICROS *pMap;

	for (y = 0; y < MAXDUNY; y++) {
		for (x = 0; x < MAXDUNX; x++) {
			lv = dPiece[x][y];
			pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];
			if (lv != 0) {
				lv--;
				pPiece = (WORD *)&pLevelPieces[32 * lv];
				for (i = 0; i < 16; i++) {
					pMap->mt[i] = pPiece[(i & 1) + 14 - (i & 0xE)];
				}
			} else {
				for (i = 0; i < 16; i++) {
					pMap->mt[i] = 0;
				}
			}
		}
	}

	if (zoomflag) {
		scr_pix_width = SCREEN_WIDTH;
		scr_pix_height = VIEWPORT_HEIGHT;
		dword_5C2FF8 = 10;
		dword_5C2FFC = 11;
	} else {
		scr_pix_width = SCREEN_WIDTH / 2 + 64;
		scr_pix_height = VIEWPORT_HEIGHT / 2 + 48;
		dword_5C2FF8 = 6;
		dword_5C2FFC = 7;
	}
}

void T_FillSector(unsigned char *P3Tiles, unsigned char *pSector, int xi, int yi, int w, int h)
{
	int i, j, xx, yy;
	long v1, v2, v3, v4, ii;

	ii = 4;
	yy = yi;
	for (j = 0; j < h; j++) {
		xx = xi;
		for (i = 0; i < w; i++) {
			WORD *Map;

			Map = (WORD *)&pSector[ii];
			if (*Map) {
				v1 = *((WORD *)&P3Tiles[(*Map - 1) * 8]) + 1;
				v2 = *((WORD *)&P3Tiles[(*Map - 1) * 8] + 1) + 1;
				v3 = *((WORD *)&P3Tiles[(*Map - 1) * 8] + 2) + 1;
				v4 = *((WORD *)&P3Tiles[(*Map - 1) * 8] + 3) + 1;
			} else {
				v1 = 0;
				v2 = 0;
				v3 = 0;
				v4 = 0;
			}
			dPiece[xx][yy] = v1;
			dPiece[xx + 1][yy] = v2;
			dPiece[xx][yy + 1] = v3;
			dPiece[xx + 1][yy + 1] = v4;
			xx += 2;
			ii += 2;
		}
		yy += 2;
	}
}

void T_FillTile(unsigned char *P3Tiles, int xx, int yy, int t)
{
	long v1, v2, v3, v4;

	v1 = *((WORD *)&P3Tiles[(t - 1) * 8]) + 1;
	v2 = *((WORD *)&P3Tiles[(t - 1) * 8] + 1) + 1;
	v3 = *((WORD *)&P3Tiles[(t - 1) * 8] + 2) + 1;
	v4 = *((WORD *)&P3Tiles[(t - 1) * 8] + 3) + 1;

	dPiece[xx][yy] = v1;
	dPiece[xx + 1][yy] = v2;
	dPiece[xx][yy + 1] = v3;
	dPiece[xx + 1][yy + 1] = v4;
}

void T_Pass3()
{
	int xx, yy, x;
	unsigned char *P3Tiles, *pSector;

	for (yy = 0; yy < MAXDUNY; yy += 2) {
		for (xx = 0; xx < MAXDUNX; xx += 2) {
			dPiece[xx][yy] = 0;
			dPiece[xx + 1][yy] = 0;
			dPiece[xx][yy + 1] = 0;
			dPiece[xx + 1][yy + 1] = 0;
		}
	}

	P3Tiles = LoadFileInMem("Levels\\TownData\\Town.TIL", 0);
	pSector = LoadFileInMem("Levels\\TownData\\Sector1s.DUN", 0);
	T_FillSector(P3Tiles, pSector, 46, 46, 25, 25);
	mem_free_dbg(pSector);
	pSector = LoadFileInMem("Levels\\TownData\\Sector2s.DUN", 0);
	T_FillSector(P3Tiles, pSector, 46, 0, 25, 23);
	mem_free_dbg(pSector);
	pSector = LoadFileInMem("Levels\\TownData\\Sector3s.DUN", 0);
	T_FillSector(P3Tiles, pSector, 0, 46, 23, 25);
	mem_free_dbg(pSector);
	pSector = LoadFileInMem("Levels\\TownData\\Sector4s.DUN", 0);
	T_FillSector(P3Tiles, pSector, 0, 0, 23, 23);
	mem_free_dbg(pSector);

	if (gbMaxPlayers == 1) {
		if (!(plr[myplr].pTownWarps & 1)) {
			T_FillTile(P3Tiles, 48, 20, 320);
		}
		if (!(plr[myplr].pTownWarps & 2)) {
			T_FillTile(P3Tiles, 16, 68, 332);
			T_FillTile(P3Tiles, 16, 70, 331);
		}
		if (!(plr[myplr].pTownWarps & 4)) {
			for (x = 36; x < 46; x++) {
				T_FillTile(P3Tiles, x, 78, random(0, 4) + 1);
			}
		}
	}

	if (quests[13]._qactive != 3 && quests[13]._qactive) {
		T_FillTile(P3Tiles, 60, 70, 342);
	} else {
		T_FillTile(P3Tiles, 60, 70, 71);
	}

	mem_free_dbg(P3Tiles);
}

void CreateTown(int entry)
{
	int x, y;

	dminx = 10;
	dminy = 10;
	dmaxx = 84;
	dmaxy = 84;

	if (entry == 0) {
		ViewX = 75;
		ViewY = 68;
	} else if (entry == 1) {
		ViewX = 25;
		ViewY = 31;
	} else if (entry == 7) {
		if (TWarpFrom == 5) {
			ViewX = 49;
			ViewY = 22;
		}
		if (TWarpFrom == 9) {
			ViewX = 18;
			ViewY = 69;
		}
		if (TWarpFrom == 13) {
			ViewX = 41;
			ViewY = 81;
		}
	}

	T_Pass3();
	memset(dLight, 0, sizeof(dLight));
	memset(dFlags, 0, sizeof(dFlags));
	memset(dPlayer, 0, sizeof(dPlayer));
	memset(dMonster, 0, sizeof(dMonster));
	memset(dObject, 0, sizeof(dObject));
	memset(dItem, 0, sizeof(dItem));
	memset(dArch, 0, sizeof(dArch));

	for (y = 0; y < MAXDUNY; y++) {
		for (x = 0; x < MAXDUNX; x++) {
			if (dPiece[x][y] == 360) {
				dArch[x][y] = 1;
			} else if (dPiece[x][y] == 358) {
				dArch[x][y] = 2;
			} else if (dPiece[x][y] == 129) {
				dArch[x][y] = 6;
			} else if (dPiece[x][y] == 130) {
				dArch[x][y] = 7;
			} else if (dPiece[x][y] == 128) {
				dArch[x][y] = 8;
			} else if (dPiece[x][y] == 117) {
				dArch[x][y] = 9;
			} else if (dPiece[x][y] == 157) {
				dArch[x][y] = 10;
			} else if (dPiece[x][y] == 158) {
				dArch[x][y] = 11;
			} else if (dPiece[x][y] == 156) {
				dArch[x][y] = 12;
			} else if (dPiece[x][y] == 162) {
				dArch[x][y] = 13;
			} else if (dPiece[x][y] == 160) {
				dArch[x][y] = 14;
			} else if (dPiece[x][y] == 214) {
				dArch[x][y] = 15;
			} else if (dPiece[x][y] == 212) {
				dArch[x][y] = 16;
			} else if (dPiece[x][y] == 217) {
				dArch[x][y] = 17;
			} else if (dPiece[x][y] == 216) {
				dArch[x][y] = 18;
			}
		}
	}

	SetTownMicros();
}

DEVILUTION_END_NAMESPACE
