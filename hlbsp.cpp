
#include "hlbsp.h"
#include "pakfile.h"
#include "wadfile.h"
#include <math.h>
#include <stdlib.h>
#include <vector>
#include "QuickSort.h"
//#include "TextureCache.h"

CHLBsp::CHLBsp()
{
	// entities
	m_nEntityDataSize = 0;
	m_pEntityData = NULL;
	
	// planes
	m_nNumPlanes = 0;
	m_pPlanes = NULL;
	
	// textures
	m_nTexDataSize = 0;
	m_pTexData = NULL; // (dmiptexlump_t)
	
	// vertices
	m_nNumVertices = 0;
	m_pVertices = NULL;
	
	// visibility
	m_nVisDataSize = 0;
	m_pVisData = NULL;
	
	// nodes
	m_nNumNodes = 0;
	m_pNodes = NULL;
	
	// texinfo
	m_nNumTexInfo = 0;
	m_pTexInfo = NULL;
	
	// faces
	m_nNumFaces = 0;
	m_pFaces = NULL;
	
	// lighting
	m_nLightDataSize = 0;
	m_pLightMapData = NULL;
	
	// clipnodes
	m_nNumClipNodes = 0;
	m_pClipNodes = NULL;

	// leafs
	m_nNumLeafs = 0;
	m_pLeafs = NULL;
	
	// marksurfaces
	m_nNumMarkSurfaces = 0;
	m_pMarkSurfaces = NULL;
	
	// edges
	m_nNumEdges = 0;
	m_pEdges = NULL;
	
	// surfedges
	m_nNumSurfEdges = 0;
	m_pSurfEdges = NULL;
	
	// models
	m_nNumModels = 0;
	m_pModels = NULL;

	faces = NULL;
	numVerts = 0;
	verts = NULL;
	numIndices = 0;
	indices = NULL;
}

CHLBsp::~CHLBsp()
{
	// entities
	m_nEntityDataSize = 0;
	if(m_pEntityData)
	{
		delete [] m_pEntityData;
		m_pEntityData = NULL;
	}
	
	// planes
	m_nNumPlanes = 0;
	if(m_pPlanes)
	{
		delete [] m_pPlanes;
		m_pPlanes = NULL;
	}
	
	// textures
	m_nTexDataSize = 0;
	if(m_pTexData)
	{
		delete [] m_pTexData;
		m_pTexData = NULL;
	}
	
	// vertices
	m_nNumVertices = 0;
	if(m_pVertices)
	{
		delete [] m_pVertices;
		m_pVertices = NULL;
	}
	
	// visibility
	m_nVisDataSize = 0;
	if(m_pVisData)
	{
		delete [] m_pVisData;
		m_pVisData = NULL;
	}
	
	// nodes
	m_nNumNodes = 0;
	if(m_pNodes)
	{
		delete [] m_pNodes;
		m_pNodes = NULL;
	}
	
	// texinfo
	m_nNumTexInfo = 0;
	if(m_pTexInfo)
	{
		delete [] m_pTexInfo;
		m_pTexInfo = NULL;
	}
	
	// faces
	m_nNumFaces = 0;
	if(m_pFaces)
	{
		delete [] m_pFaces;
		m_pFaces = NULL;
	}
	
	// lighting
	m_nLightDataSize = 0;
	if(m_pLightMapData)
	{
		delete [] m_pLightMapData;
		m_pLightMapData = NULL;
	}
	
	// clipnodes
	m_nNumClipNodes = 0;
	if(m_pClipNodes)
	{
		delete [] m_pClipNodes;
		m_pClipNodes = NULL;
	}

	// leafs
	m_nNumLeafs = 0;
	if(m_pLeafs)
	{
		delete [] m_pLeafs;
		m_pLeafs = NULL;
	}
	
	// marksurfaces
	m_nNumMarkSurfaces = 0;
	if(m_pMarkSurfaces)
	{
		delete [] m_pMarkSurfaces;
		m_pMarkSurfaces = NULL;
	}
	
	// edges
	m_nNumEdges = 0;
	if(m_pEdges)
	{
		delete [] m_pEdges;
		m_pEdges = NULL;
	}
	
	// surfedges
	m_nNumSurfEdges = 0;
	if(m_pSurfEdges)
	{
		delete [] m_pSurfEdges;
		m_pSurfEdges = NULL;
	}
	
	// models
	m_nNumModels = 0;
	if(m_pModels)
	{
		delete [] m_pModels;
		m_pModels = NULL;
	}

	if(faces)
	{
		delete [] faces;
		faces = NULL;
	}

	numVerts = 0;
	if(verts)
	{
		delete [] verts;
		verts = NULL;
	}
	
	numIndices = 0;
	if(indices)
	{
		delete [] indices;
		indices = NULL;
	}

}

void CHLBsp::Init(LPDIRECT3DDEVICE9 pDevice, const char *strTexturePath)
{
	strcpy(m_strTexturePath, strTexturePath);
	m_pFont = new CD3DFont("Arial", 12, D3DFONT_BOLD);
	m_pFont->InitDeviceObjects(pDevice);
	m_pFont->RestoreDeviceObjects();

	m_pDevice = pDevice;
	pDevice->AddRef();

	m_pVertexCache = new CVertexCache<vertex_t, short>(pDevice, CUSTOM_FVF);
}

void CHLBsp::Shutdown()
{
	if (m_pFont) 
	{
		delete m_pFont;
		m_pFont = NULL;
	}

	if (m_pDevice)
	{
		m_pDevice->Release();
		m_pDevice = NULL;
	}

	if (m_pVertexCache)
	{
		delete m_pVertexCache;
		m_pVertexCache = NULL;
	}
}

bool CHLBsp::Load(const char *filename)
{
	FILE *pFile = NULL;
	
	pFile = fopen(filename, "rb");
	
	if(!pFile)
		return false;

	unsigned char *pData = NULL;

	fseek(pFile, 0, SEEK_END);
	int length = ftell(pFile);
	pData = new unsigned char[length];
	fseek(pFile, 0, SEEK_SET);
	fread(pData, sizeof(unsigned char), length, pFile);
	
	dheader_t *h = (dheader_t*)pData;

	// read entities
	m_nEntityDataSize = h->lumps[LUMP_ENTITIES].filelen;
	m_pEntityData = new char[m_nEntityDataSize];
	memcpy(m_pEntityData, pData + h->lumps[LUMP_ENTITIES].fileofs, m_nEntityDataSize);

	// read planes
	m_nNumPlanes = h->lumps[LUMP_PLANES].filelen / sizeof(dplane_t);
	m_pPlanes = new dplane_t[m_nNumPlanes];
	memcpy(m_pPlanes, pData + h->lumps[LUMP_PLANES].fileofs, h->lumps[LUMP_PLANES].filelen);

	// read textures
	m_nTexDataSize = h->lumps[LUMP_TEXTURES].filelen;
	m_pTexData = new unsigned char[m_nTexDataSize];
	memcpy(m_pTexData, pData + h->lumps[LUMP_TEXTURES].fileofs, m_nTexDataSize);

	// read vertices
	m_nNumVertices = h->lumps[LUMP_VERTEXES].filelen / sizeof(dvertex_t);
	m_pVertices = new dvertex_t[m_nNumVertices];
	memcpy(m_pVertices, pData + h->lumps[LUMP_VERTEXES].fileofs, h->lumps[LUMP_VERTEXES].filelen);

	// read visibility
	m_nVisDataSize = h->lumps[LUMP_VISIBILITY].filelen;
	m_pVisData = new unsigned char[m_nVisDataSize];
	memcpy(m_pVisData, pData + h->lumps[LUMP_VISIBILITY].fileofs, m_nVisDataSize);

	// read nodes
	m_nNumNodes = h->lumps[LUMP_NODES].filelen / sizeof(dnode_t);
	m_pNodes = new dnode_t[m_nNumNodes];
	memcpy(m_pNodes, pData + h->lumps[LUMP_NODES].fileofs, h->lumps[LUMP_NODES].filelen);

	// read texinfo
	m_nNumTexInfo = h->lumps[LUMP_TEXINFO].filelen / sizeof(texinfo_t);
	m_pTexInfo = new texinfo_t[m_nNumTexInfo];
	memcpy(m_pTexInfo, pData + h->lumps[LUMP_TEXINFO].fileofs, h->lumps[LUMP_TEXINFO].filelen);

	// read faces
	m_nNumFaces = h->lumps[LUMP_FACES].filelen / sizeof(dface_t);
	m_pFaces = new dface_t[m_nNumFaces];
	memcpy(m_pFaces, pData + h->lumps[LUMP_FACES].fileofs, h->lumps[LUMP_FACES].filelen);

	// read lighting
	m_nLightDataSize = h->lumps[LUMP_LIGHTING].filelen;
	m_pLightMapData = new unsigned char[m_nLightDataSize];
	memcpy(m_pLightMapData, pData + h->lumps[LUMP_LIGHTING].fileofs, m_nLightDataSize);

	// read clipnodes
	m_nNumClipNodes = h->lumps[LUMP_CLIPNODES].filelen / sizeof(dclipnode_t);
	m_pClipNodes = new dclipnode_t[m_nNumClipNodes];
	memcpy(m_pClipNodes, pData + h->lumps[LUMP_CLIPNODES].fileofs, h->lumps[LUMP_CLIPNODES].filelen);

	// read leafs
	m_nNumLeafs = h->lumps[LUMP_LEAFS].filelen / sizeof(dleaf_t);
	m_pLeafs = new dleaf_t[m_nNumLeafs];
	memcpy(m_pLeafs, pData + h->lumps[LUMP_LEAFS].fileofs, h->lumps[LUMP_LEAFS].filelen);

	// read marksurfaces
	m_nNumMarkSurfaces = h->lumps[LUMP_MARKSURFACES].filelen / sizeof(unsigned short);
	m_pMarkSurfaces = new unsigned short[m_nNumMarkSurfaces];
	memcpy(m_pMarkSurfaces, pData + h->lumps[LUMP_MARKSURFACES].fileofs, h->lumps[LUMP_MARKSURFACES].filelen);

	// read edges
	m_nNumEdges = h->lumps[LUMP_EDGES].filelen / sizeof(dedge_t);
	m_pEdges = new dedge_t[m_nNumEdges];
	memcpy(m_pEdges, pData + h->lumps[LUMP_EDGES].fileofs, h->lumps[LUMP_EDGES].filelen);

	// read surfedges
	m_nNumSurfEdges = h->lumps[LUMP_SURFEDGES].filelen / sizeof(int);
	m_pSurfEdges = new int[m_nNumSurfEdges];
	memcpy(m_pSurfEdges, pData + h->lumps[LUMP_SURFEDGES].fileofs, h->lumps[LUMP_SURFEDGES].filelen);

	// read models
	m_nNumModels = h->lumps[LUMP_MODELS].filelen / sizeof(dmodel_t);
	m_pModels = new dmodel_t[m_nNumModels];
	memcpy(m_pModels, pData + h->lumps[LUMP_MODELS].fileofs, h->lumps[LUMP_MODELS].filelen);

	LoadTextures();
	SetupFaces();
	
	// free file buffer
	if(pData)
		delete [] pData;

	fclose(pFile);
	
	return true;
}

bool CHLBsp::LoadFromPak(const char *pakfile, const char *filename)
{
	PakFile pfile;
	if(!pfile.Open(pakfile))
		return false;

	unsigned char *pData = NULL;

	if(!pfile.ReadFile(filename, pData))
		return false;

	dheader_t *h = (dheader_t*)pData;

	// read entities
	m_nEntityDataSize = h->lumps[LUMP_ENTITIES].filelen;
	m_pEntityData = new char[m_nEntityDataSize];
	memcpy(m_pEntityData, pData + h->lumps[LUMP_ENTITIES].fileofs, m_nEntityDataSize);

	// read planes
	m_nNumPlanes = h->lumps[LUMP_PLANES].filelen / sizeof(dplane_t);
	m_pPlanes = new dplane_t[m_nNumPlanes];
	memcpy(m_pPlanes, pData + h->lumps[LUMP_PLANES].fileofs, h->lumps[LUMP_PLANES].filelen);

	// read textures
	m_nTexDataSize = h->lumps[LUMP_TEXTURES].filelen;
	m_pTexData = new unsigned char[m_nTexDataSize];
	memcpy(m_pTexData, pData + h->lumps[LUMP_TEXTURES].fileofs, m_nTexDataSize);

	// read vertices
	m_nNumVertices = h->lumps[LUMP_VERTEXES].filelen / sizeof(dvertex_t);
	m_pVertices = new dvertex_t[m_nNumVertices];
	memcpy(m_pVertices, pData + h->lumps[LUMP_VERTEXES].fileofs, h->lumps[LUMP_VERTEXES].filelen);

	// read visibility
	m_nVisDataSize = h->lumps[LUMP_VISIBILITY].filelen;
	m_pVisData = new unsigned char[m_nVisDataSize];
	memcpy(m_pVisData, pData + h->lumps[LUMP_VISIBILITY].fileofs, m_nVisDataSize);

	// read nodes
	m_nNumNodes = h->lumps[LUMP_NODES].filelen / sizeof(dnode_t);
	m_pNodes = new dnode_t[m_nNumNodes];
	memcpy(m_pNodes, pData + h->lumps[LUMP_NODES].fileofs, h->lumps[LUMP_NODES].filelen);

	// read texinfo
	m_nNumTexInfo = h->lumps[LUMP_TEXINFO].filelen / sizeof(texinfo_t);
	m_pTexInfo = new texinfo_t[m_nNumTexInfo];
	memcpy(m_pTexInfo, pData + h->lumps[LUMP_TEXINFO].fileofs, h->lumps[LUMP_TEXINFO].filelen);

	// read faces
	m_nNumFaces = h->lumps[LUMP_FACES].filelen / sizeof(dface_t);
	m_pFaces = new dface_t[m_nNumFaces];
	memcpy(m_pFaces, pData + h->lumps[LUMP_FACES].fileofs, h->lumps[LUMP_FACES].filelen);

	// read lighting
	m_nLightDataSize = h->lumps[LUMP_LIGHTING].filelen;
	m_pLightMapData = new unsigned char[m_nLightDataSize];
	memcpy(m_pLightMapData, pData + h->lumps[LUMP_LIGHTING].fileofs, m_nLightDataSize);

	// read clipnodes
	m_nNumClipNodes = h->lumps[LUMP_CLIPNODES].filelen / sizeof(dclipnode_t);
	m_pClipNodes = new dclipnode_t[m_nNumClipNodes];
	memcpy(m_pClipNodes, pData + h->lumps[LUMP_CLIPNODES].fileofs, h->lumps[LUMP_CLIPNODES].filelen);

	// read leafs
	m_nNumLeafs = h->lumps[LUMP_LEAFS].filelen / sizeof(dleaf_t);
	m_pLeafs = new dleaf_t[m_nNumLeafs];
	memcpy(m_pLeafs, pData + h->lumps[LUMP_LEAFS].fileofs, h->lumps[LUMP_LEAFS].filelen);

	// read marksurfaces
	m_nNumMarkSurfaces = h->lumps[LUMP_MARKSURFACES].filelen / sizeof(unsigned short);
	m_pMarkSurfaces = new unsigned short[m_nNumMarkSurfaces];
	memcpy(m_pMarkSurfaces, pData + h->lumps[LUMP_MARKSURFACES].fileofs, h->lumps[LUMP_MARKSURFACES].filelen);

	// read edges
	m_nNumEdges = h->lumps[LUMP_EDGES].filelen / sizeof(dedge_t);
	m_pEdges = new dedge_t[m_nNumEdges];
	memcpy(m_pEdges, pData + h->lumps[LUMP_EDGES].fileofs, h->lumps[LUMP_EDGES].filelen);

	// read surfedges
	m_nNumSurfEdges = h->lumps[LUMP_SURFEDGES].filelen / sizeof(int);
	m_pSurfEdges = new int[m_nNumSurfEdges];
	memcpy(m_pSurfEdges, pData + h->lumps[LUMP_SURFEDGES].fileofs, h->lumps[LUMP_SURFEDGES].filelen);

	// read models
	m_nNumModels = h->lumps[LUMP_MODELS].filelen / sizeof(dmodel_t);
	m_pModels = new dmodel_t[m_nNumModels];
	memcpy(m_pModels, pData + h->lumps[LUMP_MODELS].fileofs, h->lumps[LUMP_MODELS].filelen);

	LoadTextures();
	SetupFaces();
	
	// free file buffer
	if(pData)
		delete [] pData;

	pfile.Close();

	return true;
}

void CHLBsp::LoadTextures()
{
	int i, j;
	
	//m_pTextures = new LPDIRECT3DTEXTURE9[m_nNumTexInfo];
	m_pTextures = new CTexture[m_nNumTexInfo];

	dmiptexlump_t *pMipTexLump = (dmiptexlump_t*)m_pTexData;
	textures = new texture_t[pMipTexLump->nummiptex];

	m_nNumTextures = pMipTexLump->nummiptex;
	for(i = 0; i < pMipTexLump->nummiptex; ++i)
	{
		miptex_t *pMipTex = (miptex_t*)(m_pTexData + pMipTexLump->dataofs[i]);
		
		textures[i].width = pMipTex->width;
		textures[i].height = pMipTex->height;
		
		WadFile wf;
		
		wf.Open(m_strTexturePath);
		unsigned char *pRawData;
		unsigned char *pData = new unsigned char[pMipTex->width * pMipTex->height * 4];
		// read the raw texture from the wad file
		int length;
		cout << "Reading " << pMipTex->name << " from halflife.wad" << endl;
		char textureName[80];
		strcpy(textureName, pMipTex->name);
		
		CTexture *pTexture = &m_pTextures[i];
		pTexture->m_pCurrent = pTexture;

		//masked
		if(pMipTex->name[0] == '{')
		{
			m_pTextures[i].m_bMasked = true;
			pTexture->m_pNext = &m_pTextures[i];
			unsigned char *pData = new unsigned char[pMipTex->width * pMipTex->height * 4];
			if(!wf.ReadFile(pMipTex->name, pRawData, length))
			{
				cout << "Couldn't find " << pMipTex->name << " in halflife.wad" << endl;
				continue;
			}

			miptex_t *pmt = (miptex_t*)pRawData;
			
			unsigned char *palette = pRawData + (pmt->offsets[3] + 
				(pmt->width / 8 * pmt->height / 8) + 2);
			unsigned char *pImage = pRawData + pmt->offsets[0];
			for(unsigned int index = 0; index < pmt->width * pmt->height; ++index)
			{
				pData[4 * index + 0] = palette[3 * pImage[index] + 2]; //red
				pData[4 * index + 1] = palette[3 * pImage[index] + 1]; //green
				pData[4 * index + 2] = palette[3 * pImage[index] + 0]; //blue
				if(pData[4 * index + 0] == 255 &&	pData[4 * index + 1] == 0 && pData[4 * index + 2] == 0)
					pData[4 * index + 3] = 0;
				else
					pData[4 * index + 3] = 255;
			}

			HRESULT hr = D3DXCreateTexture(m_pDevice, pMipTex->width, pMipTex->height, 0,
				0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTextures[i].m_pTexture);
			
			LPDIRECT3DSURFACE9 pSurface;
			hr = m_pTextures[i].m_pTexture->GetSurfaceLevel(0, &pSurface);

			RECT rect = {0, 0, pMipTex->width, pMipTex->height};
			hr = D3DXLoadSurfaceFromMemory(pSurface,
				NULL,
				NULL,
				pData,
				D3DFMT_A8R8G8B8,
				pMipTex->width*4,
				NULL,
				&rect,
				D3DX_FILTER_LINEAR,
				0);
			
			D3DXFilterTexture(m_pTextures[i].m_pTexture, NULL, 0, D3DX_FILTER_LINEAR);

			pSurface->Release();
			
			// DEBUG
			char name[80];
			strcpy(name, pMipTex->name);
			strcat(name, ".bmp");
			//D3DXSaveTextureToFile(name, D3DXIFF_BMP, m_pTextures[i], NULL);
			if(pRawData)
				delete [] pRawData;
			delete [] pData;
		}
		// animating
		else if(pMipTex->name[0] == '+' && pMipTex->name[1] == '0')
		{
			unsigned char *pData = new unsigned char[pMipTex->width * pMipTex->height * 4];
			m_pTextures[i].m_bAnimated = true;

			if(!wf.ReadFile(pMipTex->name, pRawData, length))
			{
				cout << "Couldn't find " << pMipTex->name << " in halflife.wad" << endl;
				continue;
			}

			miptex_t *pmt = (miptex_t*)pRawData;
			
			unsigned char *palette = pRawData + (pmt->offsets[3] + 
				(pmt->width / 8 * pmt->height / 8) + 2);
			unsigned char *pImage = pRawData + pmt->offsets[0];
			for(unsigned int index = 0; index < pmt->width * pmt->height; ++index)
			{
				pData[4 * index + 0] = palette[3 * pImage[index] + 2]; //red
				pData[4 * index + 1] = palette[3 * pImage[index] + 1]; //green
				pData[4 * index + 2] = palette[3 * pImage[index] + 0]; //blue
				pData[4 * index + 3] = 255;
			}

			HRESULT hr = D3DXCreateTexture(m_pDevice, pMipTex->width, pMipTex->height, 0,
				0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTextures[i].m_pTexture);
			
			LPDIRECT3DSURFACE9 pSurface;
			hr = m_pTextures[i].m_pTexture->GetSurfaceLevel(0, &pSurface);

			RECT rect = {0, 0, pMipTex->width, pMipTex->height};
			hr = D3DXLoadSurfaceFromMemory(pSurface,
				NULL,
				NULL,
				pData,
				D3DFMT_A8R8G8B8,
				pMipTex->width*4,
				NULL,
				&rect,
				D3DX_FILTER_LINEAR,
				0);
			
			D3DXFilterTexture(m_pTextures[i].m_pTexture, NULL, 0, D3DX_FILTER_LINEAR);

			pSurface->Release();
			
			// DEBUG
			char name[80];
			strcpy(name, pMipTex->name);
			strcat(name, ".bmp");
			//D3DXSaveTextureToFile(name, D3DXIFF_BMP, m_pTextures[i], NULL);
			if(pRawData)
				delete [] pRawData;
			delete [] pData;

			for(j = 1; j < 10; ++j)
			{
				pData = new unsigned char[pMipTex->width * pMipTex->height * 4];
				textureName[1] = j + 48; // 48 is ASCII 0
				if(!wf.ReadFile(textureName, pRawData, length))
				{
					cout << "Couldn't find " << pMipTex->name << " in halflife.wad" << endl;
					pTexture->m_pNext = &m_pTextures[i];
					break;
				}

				pTexture->m_pNext = new CTexture;
				pTexture = pTexture->m_pNext;
				
				miptex_t *pmt = (miptex_t*)pRawData;
				
				unsigned char *palette = pRawData + (pmt->offsets[3] + 
					(pmt->width / 8 * pmt->height / 8) + 2);
				unsigned char *pImage = pRawData + pmt->offsets[0];
				for(unsigned int index = 0; index < pmt->width * pmt->height; ++index)
				{
					pData[4 * index + 0] = palette[3 * pImage[index] + 2]; // red
					pData[4 * index + 1] = palette[3 * pImage[index] + 1]; // green
					pData[4 * index + 2] = palette[3 * pImage[index] + 0]; // blue
					pData[4 * index + 3] = 255; // alpha
				}

				HRESULT hr = D3DXCreateTexture(m_pDevice, pMipTex->width, pMipTex->height, 0,
					0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture->m_pTexture);
				
				LPDIRECT3DSURFACE9 pSurface;
				hr = pTexture->m_pTexture->GetSurfaceLevel(0, &pSurface);

				RECT rect = {0, 0, pMipTex->width, pMipTex->height};
				hr = D3DXLoadSurfaceFromMemory(pSurface,
					NULL,
					NULL,
					pData,
					D3DFMT_A8R8G8B8,
					pMipTex->width*4,
					NULL,
					&rect,
					D3DX_FILTER_LINEAR,
					0);
				
				D3DXFilterTexture(pTexture->m_pTexture, NULL, 0, D3DX_FILTER_LINEAR);

				pSurface->Release();

				if(pRawData)
					delete [] pRawData;
				delete [] pData;
			}
		}
		// switchable
		else if(pMipTex->name[0] == '+' && pMipTex->name[1] == 'a')
		{
			unsigned char *pData = new unsigned char[pMipTex->width * pMipTex->height * 4];
			m_pTextures[i].m_bSwitchable = true;

			if(!wf.ReadFile(pMipTex->name, pRawData, length))
			{
				cout << "Couldn't find " << pMipTex->name << " in halflife.wad" << endl;
				continue;
			}

			miptex_t *pmt = (miptex_t*)pRawData;
			
			unsigned char *palette = pRawData + (pmt->offsets[3] + 
				(pmt->width / 8 * pmt->height / 8) + 2);
			unsigned char *pImage = pRawData + pmt->offsets[0];
			for(unsigned int index = 0; index < pmt->width * pmt->height; ++index)
			{
				pData[4 * index + 0] = palette[3 * pImage[index] + 2]; //red
				pData[4 * index + 1] = palette[3 * pImage[index] + 1]; //green
				pData[4 * index + 2] = palette[3 * pImage[index] + 0]; //blue
				pData[4 * index + 3] = 255;
			}

			HRESULT hr = D3DXCreateTexture(m_pDevice, pMipTex->width, pMipTex->height, 0,
				0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTextures[i].m_pTexture);
			
			LPDIRECT3DSURFACE9 pSurface;
			hr = m_pTextures[i].m_pTexture->GetSurfaceLevel(0, &pSurface);

			RECT rect = {0, 0, pMipTex->width, pMipTex->height};
			hr = D3DXLoadSurfaceFromMemory(pSurface,
				NULL,
				NULL,
				pData,
				D3DFMT_A8R8G8B8,
				pMipTex->width*4,
				NULL,
				&rect,
				D3DX_FILTER_LINEAR,
				0);
			
			D3DXFilterTexture(m_pTextures[i].m_pTexture, NULL, 0, D3DX_FILTER_LINEAR);

			pSurface->Release();
			
			// DEBUG
			char name[80];
			strcpy(name, pMipTex->name);
			strcat(name, ".bmp");
			//D3DXSaveTextureToFile(name, D3DXIFF_BMP, m_pTextures[i], NULL);
			if(pRawData)
				delete [] pRawData;
			delete [] pData;

			pData = new unsigned char[pMipTex->width * pMipTex->height * 4];
			textureName[1] = '0';
			if(!wf.ReadFile(textureName, pRawData, length))
			{
				cout << "Couldn't find " << pMipTex->name << " in halflife.wad" << endl;
				pTexture->m_pNext = &m_pTextures[i];
				continue;
			}

			pTexture->m_pNext = new CTexture;
			pTexture = pTexture->m_pNext;
			pTexture->m_pNext = &m_pTextures[i];
			
			pmt = (miptex_t*)pRawData;
			
			palette = pRawData + (pmt->offsets[3] + 
				(pmt->width / 8 * pmt->height / 8) + 2);
			pImage = pRawData + pmt->offsets[0];
			for(unsigned int index = 0; index < pmt->width * pmt->height; ++index)
			{
				pData[4 * index + 0] = palette[3 * pImage[index] + 2]; // red
				pData[4 * index + 1] = palette[3 * pImage[index] + 1]; // green
				pData[4 * index + 2] = palette[3 * pImage[index] + 0]; // blue
				pData[4 * index + 3] = 255; // alpha
			}

			hr = D3DXCreateTexture(m_pDevice, pMipTex->width, pMipTex->height, 0,
				0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture->m_pTexture);
			
			hr = pTexture->m_pTexture->GetSurfaceLevel(0, &pSurface);

			RECT rect2 = {0, 0, pMipTex->width, pMipTex->height};
			hr = D3DXLoadSurfaceFromMemory(pSurface,
				NULL,
				NULL,
				pData,
				D3DFMT_A8R8G8B8,
				pMipTex->width*4,
				NULL,
				&rect2,
				D3DX_FILTER_LINEAR,
				0);
			
			D3DXFilterTexture(pTexture->m_pTexture, NULL, 0, D3DX_FILTER_LINEAR);

			pSurface->Release();

			if(pRawData)
				delete [] pRawData;
			delete [] pData;
		}
		else
		{
			if(!wf.ReadFile(pMipTex->name, pRawData, length))
			{
				cout << "Couldn't find " << pMipTex->name << " in halflife.wad" << endl;
				continue;
			}

			pTexture->m_pNext = &m_pTextures[i];

			miptex_t *pmt = (miptex_t*)pRawData;
			
			unsigned char *palette = pRawData + (pmt->offsets[3] + 
				(pmt->width / 8 * pmt->height / 8) + 2);
			unsigned char *pImage = pRawData + pmt->offsets[0];
			for(unsigned int index = 0; index < pmt->width * pmt->height; ++index)
			{
				pData[4 * index + 0] = palette[3 * pImage[index] + 2]; //red
				pData[4 * index + 1] = palette[3 * pImage[index] + 1]; //green
				pData[4 * index + 2] = palette[3 * pImage[index] + 0]; //blue
				if(pData[4 * index + 0] == 255 &&	pData[4 * index + 1] == 0 && pData[4 * index + 2] == 0)
					pData[4 * index + 3] = 0;
				else
					pData[4 * index + 3] = 255;
			}

			HRESULT hr = D3DXCreateTexture(m_pDevice, pMipTex->width, pMipTex->height, 0,
				0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTextures[i].m_pTexture);
			
			LPDIRECT3DSURFACE9 pSurface;
			hr = m_pTextures[i].m_pTexture->GetSurfaceLevel(0, &pSurface);

			RECT rect = {0, 0, pMipTex->width, pMipTex->height};
			hr = D3DXLoadSurfaceFromMemory(pSurface,
				NULL,
				NULL,
				pData,
				D3DFMT_A8R8G8B8,
				pMipTex->width*4,
				NULL,
				&rect,
				D3DX_FILTER_LINEAR,
				0);
			
			D3DXFilterTexture(m_pTextures[i].m_pTexture, NULL, 0, D3DX_FILTER_LINEAR);

			pSurface->Release();
			
			// DEBUG
			char name[80];
			strcpy(name, pMipTex->name);
			strcat(name, ".bmp");
			//D3DXSaveTextureToFile(name, D3DXIFF_BMP, m_pTextures[i], NULL);
			if(pRawData)
				delete [] pRawData;
			delete [] pData;
		}

		wf.Close();
	}
}

/*
void CHLBsp::LoadTextures()
{
	int i;
	
	//m_pTextures = new LPDIRECT3DTEXTURE9[m_nNumTexInfo];
	m_pTextures = new CTexture[m_nNumTexInfo];

	dmiptexlump_t *pMipTexLump = (dmiptexlump_t*)m_pTexData;
	textures = new texture_t[pMipTexLump->nummiptex];

	for(i = 0; i < pMipTexLump->nummiptex; ++i)
	{
		miptex_t *pMipTex = (miptex_t*)(m_pTexData + pMipTexLump->dataofs[i]);
		
		textures[i].width = pMipTex->width;
		textures[i].height = pMipTex->height;
		
		WadFile wf;
		
		wf.Open(m_strTexturePath);
		unsigned char *pRawData;
		unsigned char *pData = new unsigned char[pMipTex->width * pMipTex->height * 4];
		// read the raw texture from the wad file
		int length;
		cout << "Reading " << pMipTex->name << " from halflife.wad" << endl;
		if(!wf.ReadFile(pMipTex->name, pRawData, length))
		{
			cout << "Couldn't find " << pMipTex->name << " in halflife.wad" << endl;
			continue;
		}

		miptex_t *pmt = (miptex_t*)pRawData;
		
		unsigned char *palette = pRawData + (pmt->offsets[3] + 
			(pmt->width / 8 * pmt->height / 8) + 2);
		unsigned char *pImage = pRawData + pmt->offsets[0];
		for(unsigned int index = 0; index < pmt->width * pmt->height; ++index)
		{
			pData[4 * index + 0] = palette[3 * pImage[index] + 2]; //red
			pData[4 * index + 1] = palette[3 * pImage[index] + 1]; //green
			pData[4 * index + 2] = palette[3 * pImage[index] + 0]; //blue
			if(pData[4 * index + 0] == 255 &&	pData[4 * index + 1] == 0 && pData[4 * index + 2] == 0)
				pData[4 * index + 3] = 0;
			else
				pData[4 * index + 3] = 255;
		}

		HRESULT hr = D3DXCreateTexture(m_pDevice, pMipTex->width, pMipTex->height, 0,
			0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTextures[i].m_pTexture);
		
		LPDIRECT3DSURFACE9 pSurface;
		hr = m_pTextures[i].m_pTexture->GetSurfaceLevel(0, &pSurface);

		RECT rect = {0, 0, pMipTex->width, pMipTex->height};
		hr = D3DXLoadSurfaceFromMemory(pSurface,
			NULL,
			NULL,
			pData,
			D3DFMT_A8R8G8B8,
			pMipTex->width*4,
			NULL,
			&rect,
			D3DX_FILTER_LINEAR,
			0);
		
		D3DXFilterTexture(m_pTextures[i].m_pTexture, NULL, 0, D3DX_FILTER_LINEAR);

		pSurface->Release();
		
		// DEBUG
		char name[80];
		strcpy(name, pMipTex->name);
		strcat(name, ".bmp");
		//D3DXSaveTextureToFile(name, D3DXIFF_BMP, m_pTextures[i], NULL);

		if(pRawData)
			delete [] pRawData;
		delete [] pData;
		wf.Close();
	}
}
*/

//#define LMWIDTH 256
//#define LMHEIGHT 256

void CHLBsp::SetupFaces()
{
	int i;

	m_pLightmaps = new LPDIRECT3DTEXTURE9[m_nNumFaces];

	// loop through faces and count verts and indices
	for(i = 0; i < m_nNumFaces; ++i)
	{
		dface_t *pFace = &m_pFaces[i];

		numVerts += pFace->numedges;
		numIndices += 3 * (pFace->numedges - 2);
	}

	// setup arrays
	faces = new face_t[m_nNumFaces];
	verts = new vertex_t[numVerts];
	indices = new short[numIndices];
	
	// reset the counts
	numVerts = 0;
	numIndices = 0;

	// loop through faces and set arrays
	for(i = 0; i < m_nNumFaces; ++i)
	{
		float mins = 999999;
		float mint = 999999;
		float maxs = -99999;
		float maxt = -99999;

		dface_t *pFace = &m_pFaces[i];
		texinfo_t *pTexInfo = &m_pTexInfo[pFace->texinfo];

		faces[i].tex = pFace->texinfo;
		faces[i].startVerts = numVerts;
		faces[i].numVerts = pFace->numedges;
		faces[i].startIndices = numIndices;
		faces[i].numIndices = 3 * (pFace->numedges - 2);

		// setup verts
		int j;
		for(j = 0; j < pFace->numedges - 1; ++j)
		{
			// calculate vertices
			int index = m_pSurfEdges[pFace->firstedge + j];
			int v1, v2;

			dedge_t *pEdge = &m_pEdges[(index > 0) ? index : -index];

			if(index > 0)
			{
				v1 = pEdge->v[0];
				v2 = pEdge->v[1];
			}
			else
			{
				v1 = pEdge->v[1];
				v2 = pEdge->v[0];
			}

			dvertex_t *pVert = NULL;

			if(!j)
			{
				pVert = &m_pVertices[v1];
				verts[numVerts + j].x = pVert->point[0];
				verts[numVerts + j].y = pVert->point[1];
				verts[numVerts + j].z = pVert->point[2];
				
				verts[numVerts + j].s = (pTexInfo->vecs[0][0] * pVert->point[0] + 
					pTexInfo->vecs[0][1] * pVert->point[1] + 
					pTexInfo->vecs[0][2] * pVert->point[2] + pTexInfo->vecs[0][3]);
				if(verts[numVerts + j].s < mins)
					mins = verts[numVerts + j].s;
				if(verts[numVerts + j].s > maxs)
					maxs = verts[numVerts + j].s;
				verts[numVerts + j].s /= textures[m_pTexInfo[pFace->texinfo].miptex].width;
				
				verts[numVerts + j].t = (pTexInfo->vecs[1][0] * pVert->point[0] + 
					pTexInfo->vecs[1][1] * pVert->point[1] + 
					pTexInfo->vecs[1][2] * pVert->point[2] + pTexInfo->vecs[1][3]);
				if(verts[numVerts + j].t < mint)
					mint = verts[numVerts + j].t;
				if(verts[numVerts + j].t > maxt)
					maxt = verts[numVerts + j].t;
				verts[numVerts + j].t /= textures[m_pTexInfo[pFace->texinfo].miptex].height;
			}

			pVert = &m_pVertices[v2];
			verts[numVerts + j + 1].x = pVert->point[0];
			verts[numVerts + j + 1].y = pVert->point[1];
			verts[numVerts + j + 1].z = pVert->point[2];
			
			verts[numVerts + j + 1].s = (pTexInfo->vecs[0][0] * pVert->point[0] + 
				pTexInfo->vecs[0][1] * pVert->point[1] + 
				pTexInfo->vecs[0][2] * pVert->point[2] + pTexInfo->vecs[0][3]);
			if(verts[numVerts + j + 1].s < mins)
				mins = verts[numVerts + j + 1].s;
			if(verts[numVerts + j + 1].s > maxs)
				maxs = verts[numVerts + j + 1].s;
			verts[numVerts + j + 1].s /= textures[m_pTexInfo[pFace->texinfo].miptex].width;

			verts[numVerts + j + 1].t = (pTexInfo->vecs[1][0] * pVert->point[0] + 
				pTexInfo->vecs[1][1] * pVert->point[1] + 
				pTexInfo->vecs[1][2] * pVert->point[2] + pTexInfo->vecs[1][3]);
			if(verts[numVerts + j + 1].t < mint)
				mint = verts[numVerts + j + 1].t;
			if(verts[numVerts + j + 1].t > maxt)
				maxt = verts[numVerts + j + 1].t;
			verts[numVerts + j + 1].t /= textures[m_pTexInfo[pFace->texinfo].miptex].height;
		}

		// setup indices
		int i1 = 0;
		int i2 = 1;
		int i3 = 2;
		for(j = 0; j < pFace->numedges - 2; ++j)
		{
			indices[numIndices + (3 * j)] = i1;
			indices[numIndices + (3 * j + 1)] = i2;
			indices[numIndices + (3 * j + 2)] = i3;
			++i2;
			++i3;
		}

		// load lightmap
		int extentss = ceil(maxs / 16) - floor(mins / 16);
		int lmwidth = extentss + 1;

		int extentst = ceil(maxt / 16) - floor(mint / 16);
		int lmheight = extentst + 1;

		if(pFace->styles[0] == -1)
		{
			m_pLightmaps[i] = NULL;
		}
		else
		{
			unsigned char *data = new unsigned char[3 * lmwidth * lmheight];
			int lmsize = 3 * lmwidth * lmheight;
			int offset = 0;
			//if(pFace->styles[3] != -1)
			//	int offset = 3 * lmsize;

			//memcpy(data, &m_pLightmapData[pFace->lightofs], 3 * lmwidth * lmheight);
			for(int index = 0; index < lmwidth * lmheight; ++index)
			{
				float multiplier = 1.5;
				int temp = multiplier * m_pLightMapData[pFace->lightofs + offset + index * 3 + 0];
				if(temp > 255)
					temp = 255;
				data[3 * index + 2] = temp;
				temp = multiplier * m_pLightMapData[pFace->lightofs + offset + index * 3 + 1];
				if(temp > 255)
					temp = 255;
				data[3 * index + 1] = temp;
				temp = multiplier * m_pLightMapData[pFace->lightofs + offset + index * 3 + 2];
				if(temp > 255)
					temp = 255;
				data[3 * index + 0] = temp;
			}

			HRESULT hr = D3DXCreateTexture(m_pDevice, lmwidth, lmheight, 0,
				D3DUSAGE_RENDERTARGET, D3DFMT_R8G8B8, D3DPOOL_DEFAULT, &m_pLightmaps[i]);
			
			LPDIRECT3DSURFACE9 pSurface;
			hr = m_pLightmaps[i]->GetSurfaceLevel(0, &pSurface);

			RECT rect = {0, 0, lmwidth, lmheight};
			hr = D3DXLoadSurfaceFromMemory(pSurface,
				NULL,
				NULL,
				data,
				D3DFMT_R8G8B8,
				lmwidth * 3,
				NULL,
				&rect,
				D3DX_FILTER_LINEAR,
				0);

			D3DXFilterTexture(m_pLightmaps[i], NULL, 0, D3DX_FILTER_LINEAR);

			pSurface->Release();

			int lmnum = -1;
			bool textureFound = false;

			delete [] data;
		}

		for(j = 0; j < pFace->numedges - 1; ++j)
		{
			// calculate vertices
			int index = m_pSurfEdges[pFace->firstedge + j];
			int v1, v2;

			dedge_t *pEdge = &m_pEdges[(index > 0) ? index : -index];

			if(index > 0)
			{
				v1 = pEdge->v[0];
				v2 = pEdge->v[1];
			}
			else
			{
				v1 = pEdge->v[1];
				v2 = pEdge->v[0];
			}

			dvertex_t *pVert = NULL;

			if(!j)
			{
				pVert = &m_pVertices[v1];
                float mid_poly_s = (mins + maxs)/2.0f;
                float mid_poly_t = (mint + maxt)/2.0f;
                float mid_tex_s = (float)lmwidth / 2.0f;
                float mid_tex_t = (float)lmheight / 2.0f;
                float s = (pTexInfo->vecs[0][0] * pVert->point[0] + 
					pTexInfo->vecs[0][1] * pVert->point[1] + 
					pTexInfo->vecs[0][2] * pVert->point[2] + pTexInfo->vecs[0][3]);
				float t = (pTexInfo->vecs[1][0] * pVert->point[0] + 
					pTexInfo->vecs[1][1] * pVert->point[1] + 
					pTexInfo->vecs[1][2] * pVert->point[2] + pTexInfo->vecs[1][3]);
				float lms = mid_tex_s + (s - mid_poly_s) / 16.0f;
                float lmt = mid_tex_t + (t - mid_poly_t) / 16.0f;
                lms /= (float) lmwidth;
                lmt /= (float) lmheight;
				
				verts[numVerts + j].lms = lms;
                verts[numVerts + j].lmt = lmt;
			}

			pVert = &m_pVertices[v2];
            float mid_poly_s = (mins + maxs)/2.0f;
            float mid_poly_t = (mint + maxt)/2.0f;
            float mid_tex_s = (float)lmwidth / 2.0f;
            float mid_tex_t = (float)lmheight / 2.0f;
            float s = (pTexInfo->vecs[0][0] * pVert->point[0] + 
				pTexInfo->vecs[0][1] * pVert->point[1] + 
				pTexInfo->vecs[0][2] * pVert->point[2] + pTexInfo->vecs[0][3]);
			float t = (pTexInfo->vecs[1][0] * pVert->point[0] + 
				pTexInfo->vecs[1][1] * pVert->point[1] + 
				pTexInfo->vecs[1][2] * pVert->point[2] + pTexInfo->vecs[1][3]);
			float lms = mid_tex_s + (s - mid_poly_s) / 16.0f;
            float lmt = mid_tex_t + (t - mid_poly_t) / 16.0f;
            lms /= (float) lmwidth;
            lmt /= (float) lmheight;
			
			verts[numVerts + j + 1].lms = lms;
            verts[numVerts + j + 1].lmt = lmt;
		}

		numVerts += pFace->numedges;
		numIndices += 3 * (pFace->numedges - 2);

	}

	cache.faces = new cacheface_t[m_nNumFaces];
	faceinc = new int[m_nNumFaces];
}

void CHLBsp::DrawVisibleFaces()
{
	int i;
	
	cache.numFaces = 0;
	
	for(i = 0; i < m_nNumFaces; ++i)
	{
		if(faceinc[i])
		{
			cache.faces[cache.numFaces].face = i;
			cache.faces[cache.numFaces++].sortkey = m_pTexInfo[m_pFaces[i].texinfo].miptex;
		}
	}

	QuickSort<cacheface_t>(cache.faces, 0, cache.numFaces - 1);

	int texture = 0;

	m_pVertexCache;
	
	for(i = 0; i < cache.numFaces; ++i)
	{
		face_t *pFace = &faces[cache.faces[i].face];
		//if(cache.faces[i].sortkey != texture)
		{
			m_pVertexCache->Flush();
			m_pDevice->SetTexture(0, m_pTextures[cache.faces[i].sortkey].m_pCurrent->m_pTexture);
			m_pDevice->SetTexture(1, m_pLightmaps[cache.faces[i].face]);
		}

		// send verts and indices to vertex cache
		m_pVertexCache->Render(
			&verts[pFace->startVerts],
			pFace->numVerts,
			&indices[pFace->startIndices],
			pFace->numIndices);
	}

	m_pVertexCache->Flush();
}

bool operator<(cacheface_t &f1, cacheface_t &f2)
{
	if(f1.sortkey < f2.sortkey)
		return true;
	else
		return false;
}

void CHLBsp::Render(D3DXVECTOR3 &position)
{
	cache.numFaces = 0;
	memset(faceinc, 0, m_nNumFaces * sizeof(int));

	int leaf = FindLeaf(position);

	// if we're outside of the level then draw all the faces in the first model
	if(leaf == 0)
	{
		int index = m_pModels[0].firstface;
		for(int i = 0; i < m_nNumFaces/*m_pModels[0].numfaces*/; ++i)
		{
			AddFace(index);
			++index;
		}

		//qsort(cache.faces, cache.numFaces, sizeof(cacheface_t), facecomp);
		QuickSort<cacheface_t>(cache.faces, 0, cache.numFaces - 1);
		
		DrawVisibleFaces();
		
		char buffer[40];
		sprintf(buffer, "leaf=%d", leaf);
		//m_pFont->DrawText(0, 0, 0xff00ff00, buffer);

		return;
	}

	unsigned char *pPVS = &m_pVisData[m_pLeafs[leaf].visofs];

	int i = 1;
	while(i < m_pModels[0].visleafs)
	{
		if(*pPVS == 0)
		{
			i += *(pPVS + 1) * 8;
			pPVS += 2;
		}
		else
		{
			for(int j = 0; j < 8; ++j)
			{
				if((i < m_nNumLeafs) && (*pPVS & (1 << j)))
				{
					DrawLeaf(i);
				}
				++i;
			}
			++pPVS;
		}
	}
	
	//qsort(cache.faces, cache.numFaces, sizeof(cacheface_t), facecomp);
	QuickSort<cacheface_t>(cache.faces, 0, cache.numFaces - 1);

	DrawVisibleFaces();

	char buffer[40];
	sprintf(buffer, "leaf=%d", leaf);
	//m_pFont->DrawText(0, 0, 0xff00ff00, buffer);
}

int CHLBsp::FindLeaf(D3DXVECTOR3 &position)
{
	int node = 0;
	node = m_pModels[0].headnode[0];
	while(node >= 0)
	{
		dnode_t *pNode = &m_pNodes[node];
		dplane_t *pPlane = &m_pPlanes[pNode->planenum];
		float dot = pPlane->normal[0] * position.x + pPlane->normal[1] * position.y +
			pPlane->normal[2] * position.z - pPlane->dist;

		// if in front go down front node else go down back
		if(dot >= 0)
		{
			node = pNode->children[0];
		}
		else
		{
			node = pNode->children[1];
		}
	}

	return ~node;
}

void CHLBsp::DrawLeaf(int num)
{
	int index = m_pLeafs[num].firstmarksurface;
	for(int i = 0; i < m_pLeafs[num].nummarksurfaces; ++i)
	{
		AddFace(m_pMarkSurfaces[index]);
		++index;
	}
}

void CHLBsp::AddFace(int num)
{
	if(!faceinc[num])
	{
		cache.faces[cache.numFaces].face = num;
		cache.faces[cache.numFaces++].sortkey = m_pTexInfo[m_pFaces[num].texinfo].miptex;
		faceinc[num] = 1;
	}
}

void CHLBsp::DrawFace(int num)
{
	dface_t *pFace = &m_pFaces[num];
	texinfo_t *pTexInfo = &m_pTexInfo[pFace->texinfo];
	
	if(pTexInfo->flags & CONTENTS_TRANSLUCENT)
		;//glBlendFunc(GL_ONE, GL_ONE);
	else
		;//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	
	int index = pFace->firstedge;
	for(int i = 0; i < pFace->numedges; ++i)
	{
		int sedge = m_pSurfEdges[index];
		bool reverse = false;
		if(sedge < 0)
		{
			reverse = true;
			sedge = -sedge;
		}

		if(reverse)
		{
			dvertex_t *pVert = &m_pVertices[m_pEdges[sedge].v[1]];
			float u = (pTexInfo->vecs[0][0] * pVert->point[0] + pTexInfo->vecs[0][1] * pVert->point[1] + 
				pTexInfo->vecs[0][2] * pVert->point[2] + pTexInfo->vecs[0][3]) / textures[pTexInfo->miptex].width;
			float v = (pTexInfo->vecs[1][0] * pVert->point[0] + pTexInfo->vecs[1][1] * pVert->point[1] + 
				pTexInfo->vecs[1][2] * pVert->point[2] + pTexInfo->vecs[1][3]) / textures[pTexInfo->miptex].height;
			pVert = &m_pVertices[m_pEdges[sedge].v[0]];
			u = (pTexInfo->vecs[0][0] * pVert->point[0] + pTexInfo->vecs[0][1] * pVert->point[1] + 
				pTexInfo->vecs[0][2] * pVert->point[2] + pTexInfo->vecs[0][3]) / textures[pTexInfo->miptex].width;
			v = (pTexInfo->vecs[1][0] * pVert->point[0] + pTexInfo->vecs[1][1] * pVert->point[1] + 
				pTexInfo->vecs[1][2] * pVert->point[2] + pTexInfo->vecs[1][3]) / textures[pTexInfo->miptex].height;
		}
		else
		{
			dvertex_t *pVert = &m_pVertices[m_pEdges[sedge].v[0]];
			float u = (pTexInfo->vecs[0][0] * pVert->point[0] + pTexInfo->vecs[0][1] * pVert->point[1] + 
				pTexInfo->vecs[0][2] * pVert->point[2] + pTexInfo->vecs[0][3]) / textures[pTexInfo->miptex].width;
			float v = (pTexInfo->vecs[1][0] * pVert->point[0] + pTexInfo->vecs[1][1] * pVert->point[1] + 
				pTexInfo->vecs[1][2] * pVert->point[2] + pTexInfo->vecs[1][3]) / textures[pTexInfo->miptex].height;
			pVert = &m_pVertices[m_pEdges[sedge].v[1]];
			u = (pTexInfo->vecs[0][0] * pVert->point[0] + pTexInfo->vecs[0][1] * pVert->point[1] + 
				pTexInfo->vecs[0][2] * pVert->point[2] + pTexInfo->vecs[0][3]) / textures[pTexInfo->miptex].width;
			v = (pTexInfo->vecs[1][0] * pVert->point[0] + pTexInfo->vecs[1][1] * pVert->point[1] + 
				pTexInfo->vecs[1][2] * pVert->point[2] + pTexInfo->vecs[1][3]) / textures[pTexInfo->miptex].height;
		}
		++index;
	}
}

void CHLBsp::Update()
{
	for(int i = 0; i < m_nNumTextures; ++i)
	{
		m_pTextures[i].Update();
	}
}

void CHLBsp::Switch()
{
	for(int i = 0; i < m_nNumTextures; ++i)
	{
		m_pTextures[i].Switch();
	}
}

#ifdef _DEBUG
ostream &operator<<(ostream &ostr, const CHLBsp &bsp)
{
	int i;
	ostr << bsp.m_nEntityDataSize << " bytes of Entity Data" << endl;
	ostr << bsp.m_nLightDataSize << " bytes of Light Data" << endl;
	ostr << bsp.m_nNumClipNodes << " Clip Nodes" << endl;
	ostr << bsp.m_nNumEdges << " Edges" << endl;
	ostr << bsp.m_nNumFaces << " Faces" << endl;
	ostr << bsp.m_nNumLeafs << " Leafs" << endl;
	ostr << bsp.m_nNumMarkSurfaces << " Mark Surfaces" << endl;
	ostr << bsp.m_nNumModels << " Models" << endl;
	ostr << bsp.m_nNumNodes << " Nodes" << endl;
	ostr << bsp.m_nNumPlanes << " Planes" << endl;
	ostr << bsp.m_nNumSurfEdges << " Surface Edges" << endl;
	ostr << bsp.m_nNumTexInfo << " Tex Infos" << endl;
	ostr << bsp.m_nNumVertices << " Vertices" << endl;
	ostr << bsp.m_nTexDataSize << " bytes of Texture Data" << endl;
	ostr << bsp.m_nVisDataSize << " bytes of Vis Data" << endl;

	ostr << "Entity Data" << endl;
	ostr << bsp.m_pEntityData << endl;
	
	ostr << "Planes" << endl;
	for(i = 0; i < bsp.m_nNumPlanes; ++i)
	{
		ostr << bsp.m_pPlanes[i] << endl;
	}
	
	ostr << "Vertices" << endl;
	for(i = 0; i < bsp.m_nNumVertices; ++i)
	{
		ostr << bsp.m_pVertices[i] << endl;
	}

	ostr << "Nodes" << endl;
	for(i = 0; i < bsp.m_nNumVertices; ++i)
	{
		ostr << bsp.m_pVertices[i] << endl;
	}

	ostr << "Texinfo" << endl;
	for(i = 0; i < bsp.m_nNumTexInfo; ++i)
	{
		ostr << bsp.m_pTexInfo[i] << endl;
	}

	ostr << "Faces" << endl;
	for(i = 0; i < bsp.m_nNumFaces; ++i)
	{
		ostr << bsp.m_pFaces[i] << endl;
	}
	
	ostr << "Clipnodes" << endl;
	for(i = 0; i < bsp.m_nNumClipNodes; ++i)
	{
		ostr << bsp.m_pClipNodes[i] << endl;
	}

	ostr << "Leafs" << endl;
	for(i = 0; i < bsp.m_nNumLeafs; ++i)
	{
		ostr << bsp.m_pLeafs[i] << endl;
	}

	ostr << "Marksurfaces" << endl;
	for(i = 0; i < bsp.m_nNumMarkSurfaces; ++i)
	{
		ostr << bsp.m_pMarkSurfaces[i] << endl;
	}

	ostr << "Edges" << endl;
	for(i = 0; i < bsp.m_nNumEdges; ++i)
	{
		ostr << bsp.m_pEdges[i] << endl;
	}

	ostr << "Surfedges" << endl;
	for(i = 0; i < bsp.m_nNumSurfEdges; ++i)
	{
		ostr << bsp.m_pSurfEdges[i] << endl;
	}
	
	ostr << "Models" << endl;
	for(i = 0; i < bsp.m_nNumModels; ++i)
	{
		ostr << bsp.m_pModels[i] << endl;
	}

	return ostr;
}

ostream &operator<<(ostream &ostr, const dplane_t &p)
{
	ostr << "normal=(" << p.normal[0] << "," << p.normal[1] << "," << p.normal[2] << ")" << endl;
	ostr << "dist=" << p.dist << endl;
	ostr << "type=" << p.type << endl;
	return ostr;
}

ostream &operator<<(ostream &ostr, const dvertex_t &v)
{
	ostr << "point=(" << v.point[0] << "," << v.point[1] << "," << v.point[2] << ")" << endl;
	return ostr;
}

ostream &operator<<(ostream &ostr, const dnode_t &n)
{
	ostr << "planenum=" << n.planenum << endl;
	ostr << "children={" << n.children[0] << "," << n.children[1] << "}" << endl;
	ostr << "mins=(" << n.mins[0] << "," << n.mins[1] << "," << n.mins[2] << ")" << endl;
	ostr << "maxs=(" << n.maxs[0] << "," << n.maxs[1] << "," << n.maxs[2] << ")" << endl;
	ostr << "firstface=" << n.firstface << endl;
	ostr << "numfaces=" << n.numfaces << endl;
	return ostr;
}

ostream &operator<<(ostream &ostr, const texinfo_t &ti)
{
	ostr << "vecs[0]=(" << ti.vecs[0][0] << "," << ti.vecs[0][1] << "," 
		<< ti.vecs[0][2] << "," << ti.vecs[0][3] << ")" << endl;
	ostr << "vecs[1]=(" << ti.vecs[1][0] << "," << ti.vecs[1][1] << ","
		<< ti.vecs[1][2] << "," << ti.vecs[1][3] << ")" << endl;
	ostr << "miptex=" << ti.miptex << endl;
	ostr << "flags=" << ti.flags << endl;
	return ostr;
}

ostream &operator<<(ostream &ostr, const dface_t &f)
{
	ostr << "planenum=" << f.planenum << endl;
	ostr << "side=" << f.side << endl;
	ostr << "firstedge=" << f.firstedge << endl;
	ostr << "numedges=" << f.numedges << endl;
	ostr << "texinfo=" << f.texinfo << endl;
	ostr << "styles={" << (short)f.styles[0] << "," << (short)f.styles[1] << ","
		<< (short)f.styles[2] << "," << (short)f.styles[3] << "}" << endl;
	ostr << "lightofs=" << f.lightofs << endl;
	return ostr;
}

ostream &operator<<(ostream &ostr, const dclipnode_t &cn)
{
	ostr << "planenum=" << cn.planenum << endl;
	ostr << "children={" << cn.children[0] << "," << cn.children[0] << "}" << endl;
	return ostr;
}

ostream &operator<<(ostream &ostr, const dleaf_t &l)
{
	ostr << "contents=" << l.contents << endl;
	ostr << "visofs=" << l.visofs << endl;
	ostr << "mins=(" << l.mins[0] << "," << l.mins[1] << "," << l.mins[2] << ")" << endl;
	ostr << "maxs=(" << l.maxs[0] << "," << l.maxs[1] << "," << l.maxs[2] << ")" << endl;
	ostr << "firstmarksurface=" << l.firstmarksurface << endl;
	ostr << "nummarksurfaces=" << l.nummarksurfaces << endl;
	ostr << "ambient_level={" << (short)l.ambient_level[0] << "," << (short)l.ambient_level[1] << ","
		<< (short)l.ambient_level[2] << "," << (short)l.ambient_level[3] << "}" << endl;
	return ostr;
}

ostream &operator<<(ostream &ostr, const dedge_t &e)
{
	ostr << "v={" << e.v[0] << "," << e.v[1] << "}" << endl;
	return ostr;
}

ostream &operator<<(ostream &ostr, const dmodel_t &m)
{
	ostr << "mins=(" << m.mins[0] << "," << m.mins[1] << "," << m.mins[2] << ")" << endl;
	ostr << "maxs=(" << m.maxs[0] << "," << m.maxs[1] << "," << m.maxs[2] << ")" << endl;
	ostr << "origin=(" << m.origin[0] << "," << m.origin[1] << "," << m.origin[2] << ")" << endl;
	ostr << "headnode={" << m.headnode[0] << "," << m.headnode[1] << "," 
		<< m.headnode[2] << "," << m.headnode[3] << "}" << endl;
	ostr << "visleafs=" << m.visleafs << endl;
	ostr << "firstface=" << m.firstface << endl;
	ostr << "numfaces=" << m.numfaces << endl;
	return ostr;
}

ostream &operator<<(ostream &ostr, const miptex_t &m)
{
	ostr << "name=" << m.name << endl;
	ostr << "width=" << m.width << endl;
	ostr << "height=" << m.height << endl;
	ostr << "offsets={" << m.offsets[0] << "," << m.offsets[1] << "," <<
		m.offsets[2] << "," << m.offsets[3] << "}" << endl;
	return ostr;
}

#endif
