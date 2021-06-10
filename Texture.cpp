////////////////////////////////////////////////////////////////////////////////
//
// Texture.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include "Texture.h"

CTexture::CTexture()
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_pTexture = NULL;
	m_pNext = NULL;
	m_bMasked = false;
	m_bAnimated = false;
	m_bSwitchable = false;
}

CTexture::~CTexture()
{
	if(m_pTexture)
	{
		m_pTexture->Release();
		m_pTexture = NULL;
	}
}

void CTexture::Update()
{
	if(m_bAnimated)
		m_pCurrent = m_pCurrent->m_pNext;
}

void CTexture::Switch()
{
	if(m_bSwitchable)
		m_pCurrent = m_pCurrent->m_pNext;

	if(!m_pCurrent)
		int x = 0;
}