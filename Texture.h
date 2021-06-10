////////////////////////////////////////////////////////////////////////////////
//
// Texture.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <d3d9.h>

class CTexture
{
public:
	CTexture();
	~CTexture();
	void Update();
	void Switch();

	int m_nWidth;
	int m_nHeight;
	LPDIRECT3DTEXTURE9 m_pTexture;
	CTexture *m_pCurrent;
	CTexture *m_pNext;
	bool m_bMasked;
	bool m_bAnimated;
	bool m_bSwitchable;

private:

};

#endif
