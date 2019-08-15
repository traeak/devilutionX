#ifndef __RENDER_H__
#define __RENDER_H__

void RenderTile(BYTE *pBuff);
#define drawUpperScreen(p) RenderTile(p)
#define drawLowerScreen(p) RenderTile(p)
void trans_rect(int x, int y, int w, int h);

#endif /* __RENDER_H__ */
