
#ifndef _HLBSP_H_
#define _HLBSP_H_

#ifdef _DEBUG
#include <iostream>
#endif

using namespace std;

#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>
#include "bspfile.h"
#include "VertexCache.h"
#include "Texture.h"
#include "d3dfont.h"

#define CUSTOM_FVF (D3DFVF_XYZ | D3DFVF_TEX2)

typedef struct
{
	int width;
	int height;
} texture_t;

typedef struct
{
	float x, y, z;
	float s, t;
	float lms, lmt;
} vertex_t;

typedef struct
{
	int face;
	int sortkey;
} cacheface_t;

typedef struct
{
	int numFaces;
	cacheface_t *faces;
} cache_t;

typedef struct
{
	int tex;
	int lm;
	int numVerts;
	int startVerts;
	int numIndices;
	int startIndices;
} face_t;

class CHLBsp
{
public:
	CHLBsp();
	~CHLBsp();
	void Init(LPDIRECT3DDEVICE9 pDevice, const char *strTexturePath);
	void Shutdown();
	bool LoadFromPak(const char *pakfile, const char *filename);
	bool Load(const char *filename);
	void LoadTextures();
	void Render(D3DXVECTOR3 &position);
	void DrawLeaf(int num);
	void DrawFace(int num);
	int FindLeaf(D3DXVECTOR3 &position);
	void SetupFaces();
	void AddFace(int num);
	void DrawVisibleFaces();
	void Update();
	void Switch();

#ifdef _DEBUG
	friend ostream &operator<<(ostream &ostr, const CHLBsp &bsp);
#endif
	int m_texnum;
	
private:
	LPDIRECT3DDEVICE9 m_pDevice;
	//LPDIRECT3DTEXTURE9 *m_pTextures;
	int m_nNumTextures;
	CTexture *m_pTextures;
	
	LPDIRECT3DTEXTURE9 *m_pLightmaps;

	CVertexCache<vertex_t, short> *m_pVertexCache;

	// entities
	int	m_nEntityDataSize;
	char *m_pEntityData;
	
	// planes
	int m_nNumPlanes;
	dplane_t *m_pPlanes;
	
	// textures
	int m_nTexDataSize;
	unsigned char *m_pTexData; // (dmiptexlump_t)
	
	// vertices
	int m_nNumVertices;
	dvertex_t *m_pVertices;
	
	// visibility
	int m_nVisDataSize;
	unsigned char *m_pVisData;
	
	// nodes
	int m_nNumNodes;
	dnode_t *m_pNodes;
	
	// texinfo
	int m_nNumTexInfo;
	texinfo_t *m_pTexInfo;
	
	// faces
	int m_nNumFaces;
	dface_t *m_pFaces;
	
	// lighting
	int m_nLightDataSize;
	unsigned char *m_pLightMapData;
	
	// clipnodes
	int m_nNumClipNodes;
	dclipnode_t *m_pClipNodes;
	
	// leafs
	int m_nNumLeafs;
	dleaf_t *m_pLeafs;
	
	// marksurfaces
	int m_nNumMarkSurfaces;
	unsigned short *m_pMarkSurfaces;
	
	// edges
	int m_nNumEdges;
	dedge_t *m_pEdges;
	
	// surfedges
	int m_nNumSurfEdges;
	int *m_pSurfEdges;
	
	// models
	int m_nNumModels;
	dmodel_t *m_pModels;

	//unsigned char palette[256][3];
	texture_t *textures;

	face_t *faces;

	int numVerts;
	vertex_t *verts;

	int numIndices;
	short *indices;

	cache_t cache;
	int *faceinc;

	CD3DFont *m_pFont;

	char m_strTexturePath[80];

};

#ifdef _DEBUG
ostream &operator<<(ostream &ostr, const dplane_t &p);
ostream &operator<<(ostream &ostr, const dvertex_t &v);
ostream &operator<<(ostream &ostr, const dnode_t &n);
ostream &operator<<(ostream &ostr, const texinfo_t &ti);
ostream &operator<<(ostream &ostr, const dface_t &f);
ostream &operator<<(ostream &ostr, const dclipnode_t &cn);
ostream &operator<<(ostream &ostr, const dleaf_t &l);
ostream &operator<<(ostream &ostr, const dedge_t &e);
ostream &operator<<(ostream &ostr, const dmodel_t &m);
ostream &operator<<(ostream &ostr, const miptex_t &m);
#endif

#endif // _HLBSP_H_