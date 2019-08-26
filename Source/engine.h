//HEADER_GOES_HERE
#ifndef __ENGINE_H__
#define __ENGINE_H__

extern char gbPixelCol;  // automap pixel color 8-bit (palette entry)
extern BOOL gbRotateMap; // flip - if y < x
extern int orgseed;
extern int SeedCount;
extern BOOL gbNotInView; // valid - if x/y are in bounds

void CelDrawDatOnly(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth);
void CelDecodeOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);
void CelDecDatOnly(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth, bool hdr = false, bool lighting = false, bool transparency = false);
void CelDrawHdrOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);
void CelDecDatLightOnly(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, BYTE *tbl = NULL);
void CelDecDatLightTrans(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth);
void CelDecodeHdrLightOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);
#define CelDecodeHdrLightTrans(pBuff, pCelBuff, nCel, nWidth) CelDecDatOnly(pBuff, pCelBuff, nCel, nWidth, true, true, true)
void CelDrawHdrLightRed(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, char light);
void Cel2DecDatOnly(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, bool clipped = true);
void Cel2DrawHdrOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, bool hdr = true);
void Cel2DecodeHdrOnly(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth, bool hdr = false, bool lighting = false, bool transparency = false);
void Cel2DecDatLightOnly(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth);
void Cel2DecDatLightTrans(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth);
void Cel2DecodeHdrLight(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);
#define Cel2DecodeLightTrans(pBuff, pCelBuff, nCel, nWidth) Cel2DecodeHdrOnly(pBuff, pCelBuff, nCel, nWidth, true, true, true)
void Cel2DrawHdrLightRed(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, char light);
void CelDecodeRect(BYTE *pBuff, int hgt, int wdt, BYTE *pCelBuff, int nCel, int nWidth);
void CelDecodeClr(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);
void CelDrawHdrClrHL(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);
void ENG_set_pixel(int sx, int sy, BYTE col);
void engine_draw_pixel(int sx, int sy);
void DrawLine(int x0, int y0, int x1, int y1, BYTE col);
int GetDirection(int x1, int y1, int x2, int y2);
void SetRndSeed(int s);
int GetRndSeed();
int random(BYTE idx, int v);
void engine_debug_trap(BOOL show_cursor);
BYTE *DiabloAllocPtr(DWORD dwBytes);
void mem_free_dbg(void *p);
BYTE *LoadFileInMem(char *pszName, DWORD *pdwFileLen);
DWORD LoadFileWithMem(const char *pszName, void *p);
void Cl2ApplyTrans(BYTE *p, BYTE *ttbl, int nCel);
void Cl2DecodeFrm1(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, bool lighting = false);
void Cl2DecDatFrm1(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth);
void Cl2DecodeFrm2(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);
void Cl2DecDatFrm2(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, char col);
void Cl2DecodeFrm3(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, char light);
void Cl2DecDatLightTbl1(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, BYTE *pTable);
#define Cl2DecodeLightTbl(sx, sy, pCelBuff, nCel, nWidth) Cl2DecodeFrm1(sx, sy, pCelBuff, nCel, nWidth, true)
void Cl2DecodeFrm4(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, bool hdr = false, bool lighting = false);
void Cl2DecDatFrm4(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth);
void Cl2DecodeClrHL(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);
void Cl2DecDatClrHL(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, char col);
void Cl2DecodeFrm5(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, char light);
void Cl2DecDatLightTbl2(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, BYTE *pTable);
#define Cl2DecodeFrm6(sx, sy, pCelBuff, nCel, nWidth) Cl2DecodeFrm4(sx, sy, pCelBuff, nCel, nWidth, true, true)
void PlayInGameMovie(char *pszMovie);

/* rdata */

extern const int RndInc;
extern const int RndMult;

#endif /* __ENGINE_H__ */
