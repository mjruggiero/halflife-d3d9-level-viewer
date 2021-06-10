////////////////////////////////////////////////////////////////////////////////
//
// VertexCache.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _VERTEXCACHE_H_
#define _VERTEXCACHE_H_

#include <d3d9.h>

#define DEFAULTSIZE 4096

template<class VertexType, class IndexType>
class CVertexCache
{
public:
	CVertexCache(LPDIRECT3DDEVICE9 pDevice, DWORD dwFVF, int nSize = DEFAULTSIZE)
	{
		m_pDevice = pDevice;
		m_pDevice->AddRef();
		m_dwFVF = dwFVF;
		
		m_pDevice->CreateVertexBuffer(nSize * sizeof(VertexType),
									D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
									m_dwFVF,
									D3DPOOL_DEFAULT,
									&m_pVertexBuffer,
									NULL);

		m_pDevice->CreateIndexBuffer(nSize * sizeof(IndexType),
								   D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
								   sizeof(IndexType) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32,
								   D3DPOOL_DEFAULT,
								   &m_pIndexBuffer,
								   NULL);

		m_nNumVertices = 0;
		m_nNumIndices = 0;
		m_nMaxSize = nSize;
	}

	virtual ~CVertexCache()
	{
		m_pVertexBuffer->Release();
		m_pIndexBuffer->Release();
		m_pDevice->Release();
	}
	
	void Render(VertexType *pVertices, int nNumVertices, IndexType *pIndices, int nNumIndices)
	{
		if(m_nNumVertices + nNumVertices > m_nMaxSize || m_nNumIndices + nNumIndices > m_nMaxSize)
			Flush();

		// add vertices
		VertexType *pVertexBufferPtr = NULL;

		m_pVertexBuffer->Lock(m_nNumVertices * sizeof(VertexType), nNumVertices * sizeof(VertexType), (void**)&pVertexBufferPtr, D3DLOCK_NOSYSLOCK | D3DLOCK_NOOVERWRITE);

		memcpy(pVertexBufferPtr, pVertices, nNumVertices * sizeof(VertexType));

		m_pVertexBuffer->Unlock();


		// add indices
		IndexType *pIndexBufferPtr = NULL;

		m_pIndexBuffer->Lock(m_nNumIndices * sizeof(VertexType), nNumIndices * sizeof(IndexType), (void**)&pIndexBufferPtr, D3DLOCK_NOSYSLOCK | D3DLOCK_NOOVERWRITE);

		//memcpy(pIndexBufferPtr, pIndices, nNumIndices * sizeof(IndexType));

		for(int i = 0; i < nNumIndices; ++i)
		{
			pIndexBufferPtr[i] = pIndices[i] + m_nNumVertices;
		}

		m_pIndexBuffer->Unlock();

		m_nNumVertices += nNumVertices;
		m_nNumIndices += nNumIndices;
	}

	void Flush()
	{
		if(m_nNumVertices == 0 || m_nNumIndices == 0)
			return;
		
		// setup and draw
		m_pDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(VertexType));
		m_pDevice->SetIndices(m_pIndexBuffer);
		m_pDevice->SetFVF(m_dwFVF);
		m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_nNumVertices,
			0, m_nNumIndices / 3);
		m_nNumVertices = 0;
		m_nNumIndices = 0;
	}
	
private:
	LPDIRECT3DDEVICE9 m_pDevice;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
	int m_nNumVertices;
	LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;
	int m_nNumIndices;
	int m_nMaxSize;
	DWORD m_dwFVF;

};

#endif // _VERTEXCACHE_H_
