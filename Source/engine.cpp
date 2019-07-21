#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

char gbPixelCol; // automap pixel color 8-bit (palette entry)
int gbRotateMap; // BOOLEAN flip - if y < x
int orgseed;     // weak
int sgnWidth;
int sglGameSeed; // weak
#ifdef __cplusplus
static CCritSect sgMemCrit;
#endif
int SeedCount;   // weak
int gbNotInView; // BOOLEAN valid - if x/y are in bounds

const int rand_increment = 1;
const int rand_multiplier = 0x015A4E35;

void CelDrawDatOnly(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;

	/// ASSERT: assert(pDecodeTo != NULL);
	if (!pDecodeTo)
		return;
	/// ASSERT: assert(pRLEBytes != NULL);
	if (!pRLEBytes)
		return;

	int i;
	BYTE width;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	w = nWidth;

	for (; src != &pRLEBytes[nDataSize]; dst -= BUFFER_WIDTH + w) {
		for (i = w; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				if (width & 1) {
					dst[0] = src[0];
					src++;
					dst++;
				}
				width >>= 1;
				if (width & 1) {
					dst[0] = src[0];
					dst[1] = src[1];
					src += 2;
					dst += 2;
				}
				width >>= 1;
				for (; width; width--) {
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
				i -= width;
			}
		}
	}
}

void CelDecodeOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;

	CelDrawDatOnly(
	    &gpBuffer[sx + PitchTbl[sy]],
	    &pCelBuff[pFrameTable[nCel]],
	    pFrameTable[nCel + 1] - pFrameTable[nCel],
	    nWidth);
}

void CelDecDatOnly(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth)
{
	DWORD *pFrameTable;

	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(pBuff != NULL);
	if (!pBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;

	CelDrawDatOnly(
	    pBuff,
	    &pCelBuff[pFrameTable[nCel]],
	    pFrameTable[nCel + 1] - pFrameTable[nCel],
	    nWidth);
}

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void CelDrawHdrOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataSize, nDataCap;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];
	if (CelCap == 8)
		nDataCap = 0;
	else
		nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize -= nDataStart;

	CelDrawDatOnly(
	    &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]],
	    pRLEBytes + nDataStart,
	    nDataSize,
	    nWidth);
}

void CelDecDatLightOnly(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;
	BYTE *tbl;

	/// ASSERT: assert(pDecodeTo != NULL);
	if (!pDecodeTo)
		return;
	/// ASSERT: assert(pRLEBytes != NULL);
	if (!pRLEBytes)
		return;

	int i;
	BYTE width;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	tbl = &pLightTbl[light_table_index * 256];
	w = nWidth;

	for (; src != &pRLEBytes[nDataSize]; dst -= BUFFER_WIDTH + w) {
		for (i = w; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				if (width & 1) {
					dst[0] = tbl[src[0]];
					src++;
					dst++;
				}
				width >>= 1;
				if (width & 1) {
					dst[0] = tbl[src[0]];
					dst[1] = tbl[src[1]];
					src += 2;
					dst += 2;
				}
				width >>= 1;
				for (; width; width--) {
					dst[0] = tbl[src[0]];
					dst[1] = tbl[src[1]];
					dst[2] = tbl[src[2]];
					dst[3] = tbl[src[3]];
					src += 4;
					dst += 4;
				}
			} else {
				width = -(char)width;
				dst += width;
				i -= width;
			}
		}
	}
}
// 69BEF8: using guessed type int light_table_index;

void CelDecDatLightTrans(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;
	BYTE *tbl;

	/// ASSERT: assert(pDecodeTo != NULL);
	if (!pDecodeTo)
		return;
	/// ASSERT: assert(pRLEBytes != NULL);
	if (!pRLEBytes)
		return;

	int i;
	BYTE width;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	tbl = &pLightTbl[light_table_index * 256];
	w = nWidth;

	for(; src != &pRLEBytes[nDataSize]; dst -= BUFFER_WIDTH + w) {
		for(i = w; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				if(width & 1) {
					dst[0] = pTransTbl[tbl[src[0]]][dst[0]];
					src++;
					dst++;
				}
				width >>= 1;
				if(width & 1) {
					dst[0] = pTransTbl[tbl[src[0]]][dst[0]];
					dst[1] = pTransTbl[tbl[src[1]]][dst[1]];
					src += 2;
					dst += 2;
				}
				width >>= 1;
				for(; width; width--) {
					dst[0] = pTransTbl[tbl[src[0]]][dst[0]];
					dst[1] = pTransTbl[tbl[src[1]]][dst[1]];
					dst[2] = pTransTbl[tbl[src[2]]][dst[2]];
					dst[3] = pTransTbl[tbl[src[3]]][dst[3]];
					src += 4;
					dst += 4;
				}
			} else {
				width = -(char)width;
				dst += width;
				i -= width;
			}
		}
	}
}
// 69BEF8: using guessed type int light_table_index;

void CelDecodeLightOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	int nDataSize;
	BYTE *pDecodeTo, *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	pDecodeTo = &gpBuffer[sx + PitchTbl[sy]];

	if (light_table_index)
		CelDecDatLightOnly(pDecodeTo, pRLEBytes, nDataSize, nWidth);
	else
		CelDrawDatOnly(pDecodeTo, pRLEBytes, nDataSize, nWidth);
}
// 69BEF8: using guessed type int light_table_index;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void CelDecodeHdrLightOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataSize, nDataCap;
	BYTE *pRLEBytes, *pDecodeTo;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];
	if (CelCap == 8)
		nDataCap = 0;
	else
		nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize -= nDataStart;

	pRLEBytes += nDataStart;
	pDecodeTo = &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]];

	if (light_table_index)
		CelDecDatLightOnly(pDecodeTo, pRLEBytes, nDataSize, nWidth);
	else
		CelDrawDatOnly(pDecodeTo, pRLEBytes, nDataSize, nWidth);
}
// 69BEF8: using guessed type int light_table_index;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void CelDecodeHdrLightTrans(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataSize, nDataCap;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(pBuff != NULL);
	if (!pBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];
	if (CelCap == 8)
		nDataCap = 0;
	else
		nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize -= nDataStart;

	pRLEBytes += nDataStart;

	if (cel_transparency_active)
		CelDecDatLightTrans(pBuff, pRLEBytes, nDataSize, nWidth);
	else if (light_table_index)
		CelDecDatLightOnly(pBuff, pRLEBytes, nDataSize, nWidth);
	else
		CelDrawDatOnly(pBuff, pRLEBytes, nDataSize, nWidth);
}
// 69BEF8: using guessed type int light_table_index;
// 69CF94: using guessed type int cel_transparency_active;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void CelDrawHdrLightRed(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap, char light)
{
	int nDataStart, nDataSize, nDataCap, w, idx;
	BYTE *pRLEBytes, *dst, *tbl;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];
	if (CelCap == 8)
		nDataCap = 0;
	else
		nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize -= nDataStart;

	pRLEBytes += nDataStart;
	dst = &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]];

	idx = light4flag ? 1024 : 4096;
	if (light == 2)
		idx += 256;
	if (light >= 4)
		idx += (light - 1) << 8;

	BYTE width;
	BYTE *end;

	tbl = &pLightTbl[idx];
	end = &pRLEBytes[nDataSize];

	for (; pRLEBytes != end; dst -= BUFFER_WIDTH + nWidth) {
		for (w = nWidth; w;) {
			width = *pRLEBytes++;
			if (!(width & 0x80)) {
				w -= width;
				while (width) {
					*dst = tbl[*pRLEBytes];
					pRLEBytes++;
					dst++;
					width--;
				}
			} else {
				width = -(char)width;
				dst += width;
				w -= width;
			}
		}
	}
}
// 525728: using guessed type int light4flag;

void Cel2DecDatOnly(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;

	/// ASSERT: assert(pDecodeTo != NULL);
	if (!pDecodeTo)
		return;
	/// ASSERT: assert(pRLEBytes != NULL);
	if (!pRLEBytes)
		return;
	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;

	int i;
	BYTE width;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	w = nWidth;

	for (; src != &pRLEBytes[nDataSize]; dst -= BUFFER_WIDTH + w) {
		for (i = w; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				if (dst < gpBufEnd) {
					if (width & 1) {
						dst[0] = src[0];
						src++;
						dst++;
					}
					width >>= 1;
					if (width & 1) {
						dst[0] = src[0];
						dst[1] = src[1];
						src += 2;
						dst += 2;
					}
					width >>= 1;
					for (; width; width--) {
						dst[0] = src[0];
						dst[1] = src[1];
						dst[2] = src[2];
						dst[3] = src[3];
						src += 4;
						dst += 4;
					}
				} else {
					src += width;
					dst += width;
				}
			} else {
				width = -(char)width;
				dst += width;
				i -= width;
			}
		}
	}
}
// 69CF0C: using guessed type int gpBufEnd;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cel2DrawHdrOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataSize, nDataCap;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];
	if (CelCap == 8)
		nDataCap = 0;
	else
		nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize -= nDataStart;

	Cel2DecDatOnly(
	    &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]],
	    pRLEBytes + nDataStart,
	    nDataSize,
	    nWidth);
}

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cel2DecodeHdrOnly(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataSize, nDataCap;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(pBuff != NULL);
	if (!pBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];
	nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (CelCap == 8)
		nDataCap = 0;

	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize -= nDataStart;

	Cel2DecDatOnly(pBuff, pRLEBytes + nDataStart, nDataSize, nWidth);
}

void Cel2DecDatLightOnly(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;
	BYTE *tbl;

	/// ASSERT: assert(pDecodeTo != NULL);
	if (!pDecodeTo)
		return;
	/// ASSERT: assert(pRLEBytes != NULL);
	if (!pRLEBytes)
		return;
	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;

	int i;
	BYTE width;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	tbl = &pLightTbl[light_table_index * 256];
	w = nWidth;

	for (; src != &pRLEBytes[nDataSize]; dst -= BUFFER_WIDTH + w) {
		for (i = w; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				if (dst < gpBufEnd) {
					if (width & 1) {
						dst[0] = tbl[src[0]];
						src++;
						dst++;
					}
					width >>= 1;
					if (width & 1) {
						dst[0] = tbl[src[0]];
						dst[1] = tbl[src[1]];
						src += 2;
						dst += 2;
					}
					width >>= 1;
					for (; width; width--) {
						dst[0] = tbl[src[0]];
						dst[1] = tbl[src[1]];
						dst[2] = tbl[src[2]];
						dst[3] = tbl[src[3]];
						src += 4;
						dst += 4;
					}
				} else {
					src += width;
					dst += width;
				}
			} else {
				width = -(char)width;
				dst += width;
				i -= width;
			}
		}
	}
}
// 69BEF8: using guessed type int light_table_index;
// 69CF0C: using guessed type int gpBufEnd;

void Cel2DecDatLightTrans(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;
	BYTE *tbl;

	/// ASSERT: assert(pDecodeTo != NULL);
	if (!pDecodeTo)
		return;
	/// ASSERT: assert(pRLEBytes != NULL);
	if (!pRLEBytes)
		return;
	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;

	int i;
	BYTE width;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	tbl = &pLightTbl[light_table_index * 256];
	w = nWidth;

	for(; src != &pRLEBytes[nDataSize]; dst -= BUFFER_WIDTH + w) {
		for(i = w; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				if(dst < gpBufEnd) {
					if(width & 1) {
						dst[0] = pTransTbl[tbl[src[0]]][dst[0]];
						src++;
						dst++;
					}
					width >>= 1;
					if(width & 1) {
						dst[0] = pTransTbl[tbl[src[0]]][dst[0]];
						dst[1] = pTransTbl[tbl[src[1]]][dst[1]];
						src += 2;
						dst += 2;
					}
					width >>= 1;
					for(; width; width--) {
						dst[0] = pTransTbl[tbl[src[0]]][dst[0]];
						dst[1] = pTransTbl[tbl[src[1]]][dst[1]];
						dst[2] = pTransTbl[tbl[src[2]]][dst[2]];
						dst[3] = pTransTbl[tbl[src[3]]][dst[3]];
						src += 4;
						dst += 4;
					}
				} else {
					src += width;
					dst += width;
				}
			} else {
				width = -(char)width;
				dst += width;
				i -= width;
			}
		}
	}
}
// 69BEF8: using guessed type int light_table_index;
// 69CF0C: using guessed type int gpBufEnd;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cel2DecodeHdrLight(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataSize, nDataCap;
	BYTE *pRLEBytes, *pDecodeTo;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];
	nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (CelCap == 8)
		nDataCap = 0;

	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize -= nDataStart;

	pRLEBytes += nDataStart;
	pDecodeTo = &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]];

	if (light_table_index)
		Cel2DecDatLightOnly(pDecodeTo, pRLEBytes, nDataSize, nWidth);
	else
		Cel2DecDatOnly(pDecodeTo, pRLEBytes, nDataSize, nWidth);
}
// 69BEF8: using guessed type int light_table_index;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cel2DecodeLightTrans(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataSize, nDataCap;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];
	nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (CelCap == 8)
		nDataCap = 0;

	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize -= nDataStart;

	pRLEBytes += nDataStart;

	if (cel_transparency_active)
		Cel2DecDatLightTrans(pBuff, pRLEBytes, nDataSize, nWidth);
	else if (light_table_index)
		Cel2DecDatLightOnly(pBuff, pRLEBytes, nDataSize, nWidth);
	else
		Cel2DecDatOnly(pBuff, pRLEBytes, nDataSize, nWidth);
}
// 69BEF8: using guessed type int light_table_index;
// 69CF94: using guessed type int cel_transparency_active;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cel2DrawHdrLightRed(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap, char light)
{
	int nDataStart, nDataSize, nDataCap, w, idx;
	BYTE *pRLEBytes, *dst, *tbl;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];
	if (CelCap == 8)
		nDataCap = 0;
	else
		nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize -= nDataStart;

	pRLEBytes += nDataStart;
	dst = &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]];

	idx = light4flag ? 1024 : 4096;
	if (light == 2)
		idx += 256;
	if (light >= 4)
		idx += (light - 1) << 8;

	tbl = &pLightTbl[idx];

	BYTE width;
	BYTE *end;

	end = &pRLEBytes[nDataSize];

	for (; pRLEBytes != end; dst -= BUFFER_WIDTH + nWidth) {
		for (w = nWidth; w;) {
			width = *pRLEBytes++;
			if (!(width & 0x80)) {
				w -= width;
				if (dst < gpBufEnd) {
					while (width) {
						*dst = tbl[*pRLEBytes];
						pRLEBytes++;
						dst++;
						width--;
					}
				} else {
					pRLEBytes += width;
					dst += width;
				}
			} else {
				width = -(char)width;
				dst += width;
				w -= width;
			}
		}
	}
}
// 525728: using guessed type int light4flag;
// 69CF0C: using guessed type int gpBufEnd;

void CelDecodeRect(BYTE *pBuff, int CelSkip, int hgt, int wdt, BYTE *pCelBuff, int nCel, int nWidth)
{
	BYTE *pRLEBytes, *dst, *end;

	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(pBuff != NULL);
	if (!pBuff)
		return;

	int i;
	BYTE width;
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)&pCelBuff[4 * nCel];
	pRLEBytes = &pCelBuff[pFrameTable[0]];
	end = &pRLEBytes[pFrameTable[1] - pFrameTable[0]];
	dst = &pBuff[hgt * wdt + CelSkip];

	for (; pRLEBytes != end; dst -= wdt + nWidth) {
		for (i = nWidth; i;) {
			width = *pRLEBytes++;
			if (!(width & 0x80)) {
				i -= width;
				if (width & 1) {
					dst[0] = pRLEBytes[0];
					pRLEBytes++;
					dst++;
				}
				width >>= 1;
				if (width & 1) {
					dst[0] = pRLEBytes[0];
					dst[1] = pRLEBytes[1];
					pRLEBytes += 2;
					dst += 2;
				}
				width >>= 1;
				while (width) {
					dst[0] = pRLEBytes[0];
					dst[1] = pRLEBytes[1];
					dst[2] = pRLEBytes[2];
					dst[3] = pRLEBytes[3];
					pRLEBytes += 4;
					dst += 4;
					width--;
				}
			} else {
				width = -(char)width;
				dst += width;
				i -= width;
			}
		}
	}
}

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void CelDecodeClr(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataSize, nDataCap, w;
	BYTE *pRLEBytes, *dst;

	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;

	BYTE width;
	BYTE *end, *src;
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)&pCelBuff[4 * nCel];
	pRLEBytes = &pCelBuff[pFrameTable[0]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (CelCap == 8)
		nDataCap = 0;

	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize = pFrameTable[1] - pFrameTable[0] - nDataStart;

	src = pRLEBytes + nDataStart;
	end = &src[nDataSize];
	dst = &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]];

	for (; src != end; dst -= BUFFER_WIDTH + nWidth) {
		for (w = nWidth; w;) {
			width = *src++;
			if (!(width & 0x80)) {
				w -= width;
				while (width) {
					if (*src++) {
						dst[-BUFFER_WIDTH] = col;
						dst[-1] = col;
						dst[1] = col;
						dst[BUFFER_WIDTH] = col;
					}
					dst++;
					width--;
				}
			} else {
				width = -(char)width;
				dst += width;
				w -= width;
			}
		}
	}
}

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void CelDrawHdrClrHL(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataSize, nDataCap, w;
	BYTE *pRLEBytes, *dst;

	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;

	BYTE width;
	BYTE *end, *src;
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)&pCelBuff[4 * nCel];
	pRLEBytes = &pCelBuff[pFrameTable[0]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (CelCap == 8)
		nDataCap = 0;

	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize = pFrameTable[1] - pFrameTable[0] - nDataStart;

	src = pRLEBytes + nDataStart;
	end = &src[nDataSize];
	dst = &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]];

	for (; src != end; dst -= BUFFER_WIDTH + nWidth) {
		for (w = nWidth; w;) {
			width = *src++;
			if (!(width & 0x80)) {
				w -= width;
				if (dst < gpBufEnd) {
					if (dst >= gpBufEnd - BUFFER_WIDTH) {
						while (width) {
							if (*src++) {
								dst[-BUFFER_WIDTH] = col;
								dst[-1] = col;
								dst[1] = col;
							}
							dst++;
							width--;
						}
					} else {
						while (width) {
							if (*src++) {
								dst[-BUFFER_WIDTH] = col;
								dst[-1] = col;
								dst[1] = col;
								dst[BUFFER_WIDTH] = col;
							}
							dst++;
							width--;
						}
					}
				} else {
					src += width;
					dst += width;
				}
			} else {
				width = -(char)width;
				dst += width;
				w -= width;
			}
		}
	}
}
// 69CF0C: using guessed type int gpBufEnd;

void ENG_set_pixel(int sx, int sy, BYTE col)
{
	BYTE *dst;

	/// ASSERT: assert(gpBuffer);

	if (sy < 0 || sy >= SCREEN_HEIGHT + SCREEN_Y || sx < SCREEN_X || sx >= SCREEN_WIDTH + SCREEN_X)
		return;

	dst = &gpBuffer[sx + PitchTbl[sy]];

	if (dst < gpBufEnd)
		*dst = col;
}
// 69CF0C: using guessed type int gpBufEnd;

void engine_draw_pixel(int sx, int sy)
{
	BYTE *dst;

	/// ASSERT: assert(gpBuffer);

	if (gbRotateMap) {
		if (gbNotInView && (sx < 0 || sx >= SCREEN_HEIGHT + SCREEN_Y || sy < SCREEN_X || sy >= SCREEN_WIDTH + SCREEN_X))
			return;
		dst = &gpBuffer[sy + PitchTbl[sx]];
	} else {
		if (gbNotInView && (sy < 0 || sy >= SCREEN_HEIGHT + SCREEN_Y || sx < SCREEN_X || sx >= SCREEN_WIDTH + SCREEN_X))
			return;
		dst = &gpBuffer[sx + PitchTbl[sy]];
	}

	if (dst < gpBufEnd)
		*dst = gbPixelCol;
}
// 52B96C: using guessed type char gbPixelCol;
// 52B970: using guessed type int gbRotateMap;
// 52B99C: using guessed type int gbNotInView;
// 69CF0C: using guessed type int gpBufEnd;

// Exact copy from https://github.com/erich666/GraphicsGems/blob/dad26f941e12c8bf1f96ea21c1c04cd2206ae7c9/gems/DoubleLine.c
// Except:
// * not in view checks
// * global variable instead of reverse flag
// * condition for pixels_left < 0 removed

/*
Symmetric Double Step Line Algorithm
by Brian Wyvill
from "Graphics Gems", Academic Press, 1990
*/

#define GG_SWAP(A, B) \
	{                 \
		(A) ^= (B);   \
		(B) ^= (A);   \
		(A) ^= (B);   \
	}
#define GG_ABSOLUTE(I, J, K) (((I) - (J)) * ((K) = (((I) - (J)) < 0 ? -1 : 1)))

void DrawLine(int x0, int y0, int x1, int y1, BYTE col)
{
	int i, dx, dy, steps;
	float ix, iy, sx, sy;

	dx = x1 - x0;
	dy = y1 - y0;
	steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
	ix = dx / (float)steps;
	iy = dy / (float)steps;
	sx = x0;
	sy = y0;

	for(i = 0; i <= steps; i++, sx += ix, sy += iy) {
		ENG_set_pixel(sx, sy, col);
	}
}
// 52B96C: using guessed type char gbPixelCol;
// 52B970: using guessed type int gbRotateMap;
// 52B99C: using guessed type int gbNotInView;

int GetDirection(int x1, int y1, int x2, int y2)
{
	int mx, my;
	int md, ny;

	mx = x2 - x1;
	my = y2 - y1;

	if (mx >= 0) {
		if (my >= 0) {
			md = DIR_S;
			if (2 * mx < my)
				md = DIR_SW;
		} else {
			my = -my;
			md = DIR_E;
			if (2 * mx < my)
				md = DIR_NE;
		}
		if (2 * my < mx)
			return DIR_SE;
	} else {
		ny = -mx;
		if (my >= 0) {
			md = DIR_W;
			if (2 * ny < my)
				md = DIR_SW;
		} else {
			my = -my;
			md = DIR_N;
			if (2 * ny < my)
				md = DIR_NE;
		}
		if (2 * my < ny)
			return DIR_NW;
	}

	return md;
}

void SetRndSeed(int s)
{
	SeedCount = 0;
	sglGameSeed = s;
	orgseed = s;
}
// 52B974: using guessed type int orgseed;
// 52B97C: using guessed type int sglGameSeed;
// 52B998: using guessed type int SeedCount;

int GetRndSeed()
{
	SeedCount++;
	sglGameSeed = rand_multiplier * sglGameSeed + rand_increment;
	return abs(sglGameSeed);
}
// 52B97C: using guessed type int sglGameSeed;
// 52B998: using guessed type int SeedCount;

int random(BYTE idx, int v)
{
	if (v <= 0)
		return 0;
	if (v >= 0xFFFF)
		return GetRndSeed() % v;
	return (GetRndSeed() >> 16) % v;
}

void engine_debug_trap(BOOL show_cursor)
{
	/*
	TMemBlock *pCurr;

	sgMemCrit.Enter();
	while(sgpMemBlock != NULL) {
		pCurr = sgpMemBlock->pNext;
		SMemFree(sgpMemBlock, "C:\\Diablo\\Direct\\ENGINE.CPP", 1970);
		sgpMemBlock = pCurr;
	}
	sgMemCrit.Leave();
*/
}

unsigned char *DiabloAllocPtr(int dwBytes)
{
	BYTE *buf;

#ifdef __cplusplus
	sgMemCrit.Enter();
#endif
	buf = (BYTE *)SMemAlloc(dwBytes, "C:\\Src\\Diablo\\Source\\ENGINE.CPP", 2236, 0);
#ifdef __cplusplus
	sgMemCrit.Leave();
#endif

	if (buf == NULL) {
		ErrDlg(IDD_DIALOG2, GetLastError(), "C:\\Src\\Diablo\\Source\\ENGINE.CPP", 2269);
	}

	return buf;
}

void mem_free_dbg(void *p)
{
	if (p) {
#ifdef __cplusplus
		sgMemCrit.Enter();
#endif
		SMemFree(p, "C:\\Src\\Diablo\\Source\\ENGINE.CPP", 2317, 0);
#ifdef __cplusplus
		sgMemCrit.Leave();
#endif
	}
}

BYTE *LoadFileInMem(char *pszName, int *pdwFileLen)
{
	HANDLE file;
	BYTE *buf;
	int fileLen;

	WOpenFile(pszName, &file, FALSE);
	fileLen = WGetFileSize(file, NULL);

	if (pdwFileLen)
		*pdwFileLen = fileLen;

	if (!fileLen)
		app_fatal("Zero length SFILE:\n%s", pszName);

	buf = (BYTE *)DiabloAllocPtr(fileLen);

	WReadFile(file, buf, fileLen);
	WCloseFile(file);

	return buf;
}

DWORD LoadFileWithMem(const char *pszName, void *p)
{
	DWORD dwFileLen;
	HANDLE hsFile;

	/// ASSERT: assert(pszName);
	if (p == NULL) {
		app_fatal("LoadFileWithMem(NULL):\n%s", pszName);
	}

	WOpenFile(pszName, &hsFile, FALSE);

	dwFileLen = WGetFileSize(hsFile, NULL);
	if (dwFileLen == 0) {
		app_fatal("Zero length SFILE:\n%s", pszName);
	}

	WReadFile(hsFile, p, dwFileLen);
	WCloseFile(hsFile);

	return dwFileLen;
}

void Cl2ApplyTrans(BYTE *p, BYTE *ttbl, int nCel)
{
	int i, nDataSize;
	char width;
	BYTE *dst;
	DWORD *pFrameTable;

	/// ASSERT: assert(p != NULL);
	/// ASSERT: assert(ttbl != NULL);

	for (i = 1; i <= nCel; i++) {
		pFrameTable = (DWORD *)&p[4 * i];
		dst = &p[pFrameTable[0] + 10];
		nDataSize = pFrameTable[1] - pFrameTable[0] - 10;
		while (nDataSize) {
			width = *dst++;
			nDataSize--;
			/// ASSERT: assert(nDataSize >= 0);
			if (width < 0) {
				width = -width;
				if (width > 65) {
					nDataSize--;
					/// ASSERT: assert(nDataSize >= 0);
					*dst = ttbl[*dst];
					dst++;
				} else {
					nDataSize -= width;
					/// ASSERT: assert(nDataSize >= 0);
					while (width) {
						*dst = ttbl[*dst];
						dst++;
						width--;
					}
				}
			}
		}
	}
}

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cl2DecodeFrm1(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if (nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	if (CelCap == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[CelCap];
	if (!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	Cl2DecDatFrm1(
	    &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]],
	    pRLEBytes + nDataStart,
	    nDataSize - nDataStart,
	    nWidth);
}

void Cl2DecDatFrm1(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;
	char width;
	BYTE fill;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	w = nWidth;

	while (nDataSize) {
		width = *src++;
		nDataSize--;
		if (width < 0) {
			width = -width;
			if (width > 65) {
				width -= 65;
				nDataSize--;
				fill = *src++;
				w -= width;
				while (width) {
					*dst = fill;
					dst++;
					width--;
				}
				if (!w) {
					w = nWidth;
					dst -= BUFFER_WIDTH + w;
				}
				continue;
			} else {
				nDataSize -= width;
				w -= width;
				while (width) {
					*dst = *src;
					src++;
					dst++;
					width--;
				}
				if (!w) {
					w = nWidth;
					dst -= BUFFER_WIDTH + w;
				}
				continue;
			}
		}
		while (width) {
			if (width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if (!w) {
				w = nWidth;
				dst -= BUFFER_WIDTH + w;
			}
		}
	}
}

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cl2DecodeFrm2(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if (nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	if (CelCap == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[CelCap];
	if (!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	Cl2DecDatFrm2(
	    &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]],
	    pRLEBytes + nDataStart,
	    nDataSize - nDataStart,
	    nWidth,
	    col);
}

void Cl2DecDatFrm2(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, char col)
{
	int w;
	char width;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	w = nWidth;

	while (nDataSize) {
		width = *src++;
		nDataSize--;
		if (width < 0) {
			width = -width;
			if (width > 65) {
				width -= 65;
				nDataSize--;
				if (*src++) {
					w -= width;
					dst[-1] = col;
					dst[width] = col;
					while (width) {
						dst[-BUFFER_WIDTH] = col;
						dst[BUFFER_WIDTH] = col;
						dst++;
						width--;
					}
					if (!w) {
						w = nWidth;
						dst -= BUFFER_WIDTH + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				w -= width;
				while (width) {
					if (*src++) {
						dst[-1] = col;
						dst[1] = col;
						dst[-BUFFER_WIDTH] = col;
						dst[BUFFER_WIDTH] = col;
					}
					dst++;
					width--;
				}
				if (!w) {
					w = nWidth;
					dst -= BUFFER_WIDTH + w;
				}
				continue;
			}
		}
		while (width) {
			if (width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if (!w) {
				w = nWidth;
				dst -= BUFFER_WIDTH + w;
			}
		}
	}
}

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cl2DecodeFrm3(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap, char light)
{
	int nDataStart, nDataSize, idx, nSize;
	BYTE *pRLEBytes, *pDecodeTo;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if (nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	if (CelCap == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[CelCap];
	if (!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	nSize = nDataSize - nDataStart;
	pRLEBytes += nDataStart;
	pDecodeTo = &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]];

	idx = light4flag ? 1024 : 4096;
	if (light == 2)
		idx += 256;
	if (light >= 4)
		idx += (light - 1) << 8;

	Cl2DecDatLightTbl1(
	    pDecodeTo,
	    pRLEBytes,
	    nSize,
	    nWidth,
	    &pLightTbl[idx]);
}
// 525728: using guessed type int light4flag;

void Cl2DecDatLightTbl1(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, BYTE *pTable)
{
	int w;
	char width;
	BYTE fill;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	w = nWidth;
	sgnWidth = nWidth;

	while (nDataSize) {
		width = *src++;
		nDataSize--;
		if (width < 0) {
			width = -width;
			if (width > 65) {
				width -= 65;
				nDataSize--;
				fill = pTable[*src++];
				w -= width;
				while (width) {
					*dst = fill;
					dst++;
					width--;
				}
				if (!w) {
					w = sgnWidth;
					dst -= BUFFER_WIDTH + w;
				}
				continue;
			} else {
				nDataSize -= width;
				w -= width;
				while (width) {
					*dst = pTable[*src];
					src++;
					dst++;
					width--;
				}
				if (!w) {
					w = sgnWidth;
					dst -= BUFFER_WIDTH + w;
				}
				continue;
			}
		}
		while (width) {
			if (width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if (!w) {
				w = sgnWidth;
				dst -= BUFFER_WIDTH + w;
			}
		}
	}
}
// 52B978: using guessed type int sgnWidth;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cl2DecodeLightTbl(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataSize, nSize;
	BYTE *pRLEBytes, *pDecodeTo;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if (nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	if (CelCap == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[CelCap];
	if (!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	nSize = nDataSize - nDataStart;
	pRLEBytes += nDataStart;
	pDecodeTo = &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]];

	if (light_table_index)
		Cl2DecDatLightTbl1(pDecodeTo, pRLEBytes, nSize, nWidth, &pLightTbl[light_table_index * 256]);
	else
		Cl2DecDatFrm1(pDecodeTo, pRLEBytes, nSize, nWidth);
}
// 69BEF8: using guessed type int light_table_index;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cl2DecodeFrm4(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if (nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	if (CelCap == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[CelCap];
	if (!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	Cl2DecDatFrm4(
	    &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]],
	    pRLEBytes + nDataStart,
	    nDataSize - nDataStart,
	    nWidth);
}

void Cl2DecDatFrm4(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;
	char width;
	BYTE fill;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	w = nWidth;

	while (nDataSize) {
		width = *src++;
		nDataSize--;
		if (width < 0) {
			width = -width;
			if (width > 65) {
				width -= 65;
				nDataSize--;
				fill = *src++;
				if (dst < gpBufEnd) {
					w -= width;
					while (width) {
						*dst = fill;
						dst++;
						width--;
					}
					if (!w) {
						w = nWidth;
						dst -= BUFFER_WIDTH + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				if (dst < gpBufEnd) {
					w -= width;
					while (width) {
						*dst = *src;
						src++;
						dst++;
						width--;
					}
					if (!w) {
						w = nWidth;
						dst -= BUFFER_WIDTH + w;
					}
					continue;
				} else {
					src += width;
				}
			}
		}
		while (width) {
			if (width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if (!w) {
				w = nWidth;
				dst -= BUFFER_WIDTH + w;
			}
		}
	}
}
// 69CF0C: using guessed type int gpBufEnd;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cl2DecodeClrHL(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if (nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	if (CelCap == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[CelCap];
	if (!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	gpBufEnd -= BUFFER_WIDTH;
	Cl2DecDatClrHL(
	    &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]],
	    pRLEBytes + nDataStart,
	    nDataSize - nDataStart,
	    nWidth,
	    col);
	gpBufEnd += BUFFER_WIDTH;
}
// 69CF0C: using guessed type int gpBufEnd;

void Cl2DecDatClrHL(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, char col)
{
	int w;
	char width;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	w = nWidth;

	while (nDataSize) {
		width = *src++;
		nDataSize--;
		if (width < 0) {
			width = -width;
			if (width > 65) {
				width -= 65;
				nDataSize--;
				if (*src++ && dst < gpBufEnd) {
					w -= width;
					dst[-1] = col;
					dst[width] = col;
					while (width) {
						dst[-BUFFER_WIDTH] = col;
						dst[BUFFER_WIDTH] = col;
						dst++;
						width--;
					}
					if (!w) {
						w = nWidth;
						dst -= BUFFER_WIDTH + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				if (dst < gpBufEnd) {
					w -= width;
					while (width) {
						if (*src++) {
							dst[-1] = col;
							dst[1] = col;
							dst[-BUFFER_WIDTH] = col;
							dst[BUFFER_WIDTH] = col;
						}
						dst++;
						width--;
					}
					if (!w) {
						w = nWidth;
						dst -= BUFFER_WIDTH + w;
					}
					continue;
				} else {
					src += width;
				}
			}
		}
		while (width) {
			if (width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if (!w) {
				w = nWidth;
				dst -= BUFFER_WIDTH + w;
			}
		}
	}
}
// 69CF0C: using guessed type int gpBufEnd;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cl2DecodeFrm5(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap, char light)
{
	int nDataStart, nDataSize, idx, nSize;
	BYTE *pRLEBytes, *pDecodeTo;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if (nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	if (CelCap == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[CelCap];
	if (!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	nSize = nDataSize - nDataStart;
	pRLEBytes += nDataStart;
	pDecodeTo = &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]];

	idx = light4flag ? 1024 : 4096;
	if (light == 2)
		idx += 256;
	if (light >= 4)
		idx += (light - 1) << 8;

	Cl2DecDatLightTbl2(
	    pDecodeTo,
	    pRLEBytes,
	    nSize,
	    nWidth,
	    &pLightTbl[idx]);
}
// 525728: using guessed type int light4flag;

void Cl2DecDatLightTbl2(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, BYTE *pTable)
{
	int w;
	char width;
	BYTE fill;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	w = nWidth;
	sgnWidth = nWidth;

	while (nDataSize) {
		width = *src++;
		nDataSize--;
		if (width < 0) {
			width = -width;
			if (width > 65) {
				width -= 65;
				nDataSize--;
				fill = pTable[*src++];
				if (dst < gpBufEnd) {
					w -= width;
					while (width) {
						*dst = fill;
						dst++;
						width--;
					}
					if (!w) {
						w = sgnWidth;
						dst -= BUFFER_WIDTH + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				if (dst < gpBufEnd) {
					w -= width;
					while (width) {
						*dst = pTable[*src];
						src++;
						dst++;
						width--;
					}
					if (!w) {
						w = sgnWidth;
						dst -= BUFFER_WIDTH + w;
					}
					continue;
				} else {
					src += width;
				}
			}
		}
		while (width) {
			if (width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if (!w) {
				w = sgnWidth;
				dst -= BUFFER_WIDTH + w;
			}
		}
	}
}
// 52B978: using guessed type int sgnWidth;
// 69CF0C: using guessed type int gpBufEnd;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cl2DecodeFrm6(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataSize, nSize;
	BYTE *pRLEBytes, *pDecodeTo;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if (nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	if (CelCap == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[CelCap];
	if (!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	nSize = nDataSize - nDataStart;
	pRLEBytes += nDataStart;
	pDecodeTo = &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]];

	if (light_table_index)
		Cl2DecDatLightTbl2(pDecodeTo, pRLEBytes, nSize, nWidth, &pLightTbl[light_table_index * 256]);
	else
		Cl2DecDatFrm4(pDecodeTo, pRLEBytes, nSize, nWidth);
}
// 69BEF8: using guessed type int light_table_index;

void PlayInGameMovie(char *pszMovie)
{
	PaletteFadeOut(8);
	play_movie(pszMovie, 0);
	ClearScreenBuffer();
	drawpanflag = 255;
	scrollrt_draw_game_screen(1);
	PaletteFadeIn(8);
	drawpanflag = 255;
}
// 52571C: using guessed type int drawpanflag;

DEVILUTION_END_NAMESPACE
