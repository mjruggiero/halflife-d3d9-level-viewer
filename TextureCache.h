
#ifndef _TEXTURECACHE_H_
#define _TEXTURECACHE_H_

#include <gl/gl.h>

#define WIDTH 256
#define HEIGHT 256

struct Rect
{
	Rect(int l = 0, int t = 0, int r = 0, int b = 0)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}
	int left, top, right, bottom;
};

class Node
{
public:
	Node()
	{
		child[0] = NULL;
		child[1] = NULL;
		imageID = -1;
	}

	virtual ~Node()
	{
		if(child[0])
			delete child[0];
		if(child[1])
			delete child[1];
	}

	Node *Insert(unsigned char *data, int width, int height)
	{
		//if we're not a leaf then
		if(child[0] != NULL && child[1] != NULL)
		{
			//(try inserting into first child)
			Node *newNode = child[0]->Insert(data, width, height);
			if(newNode != NULL)
				return newNode;

			//(no room, insert into second)
			return child[1]->Insert(data, width, height);
		}
		else
		{
			//(if there's already a lightmap here, return)
			if(imageID != -1)
				return NULL;

			//(if we're too small, return)
			//if img doesn't fit in pnode->rect
			if(width > (rc.right - rc.left) || height > (rc.bottom - rc.top))
				return NULL;

			//(if we're just right, accept)
			//if img fits perfectly in pnode->rect
			if(width == (rc.right - rc.left) && height ==(rc.bottom - rc.top))
			{
				imageID = 1;
				return this;//pnode;
			}

			//(otherwise, gotta split this node and create some kids)
			/*pnode->*/child[0] = new Node;
			/*pnode->*/child[1] = new Node;

			//(decide which way to split)
			int dw = (rc.right - rc.left) - width;
			int dh = (rc.bottom - rc.top) - height;

			if(dw > dh)
			{
				child[0]->rc = Rect(rc.left, rc.top, rc.left + width , rc.bottom);
				child[1]->rc = Rect(rc.left+width, rc.top, rc.right, rc.bottom);
			}
			else
			{
				child[0]->rc = Rect(rc.left, rc.top, rc.right, rc.top + height);
				child[1]->rc = Rect(rc.left, rc.top+height, rc.right, rc.bottom);
			}

			//(insert into first child we created)
			return /*pnode->*/child[0]->Insert(data, width, height);
		}
	}

	Node* child[2];
	Rect rc;
	int imageID;

private:

};

class TextureCache
{
public:
	TextureCache()
	{
		m_root = new Node();
		m_root->rc.top = 0;
		m_root->rc.left = 0;
		m_root->rc.right = WIDTH;
		m_root->rc.bottom = HEIGHT;

		glGenTextures(1, &m_tex);
		glBindTexture(GL_TEXTURE_2D, m_tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		m_pData = new unsigned char[WIDTH * HEIGHT * 3];
		glTexImage2D(GL_TEXTURE_2D, 0, 3, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pData);
		if(m_pData)
			delete [] m_pData;
	}
	
	virtual ~TextureCache()
	{
		if(m_root)
			delete m_root;
	}

	Node *TextureCache::Insert(unsigned char *data, int width, int height)
	{
		Node *pnode = m_root->Insert(data, width, height);
		if(pnode != NULL)
		{
			glBindTexture(GL_TEXTURE_2D, m_tex);
			cout << "pnode->rc.left=" << pnode->rc.left << endl;
			cout << "pnode->rc.top=" << pnode->rc.top << endl;
			cout << "width=" << width << endl;
			cout << "height=" << height << endl;
			cout << &data << endl;
			glTexSubImage2D(GL_TEXTURE_2D, 0, pnode->rc.left, pnode->rc.top, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
			pnode->imageID = 1;
			return pnode;//m_tex;
			//**copy pixels over from img into pnode->rc part of texture;
			//**pnode->imageID = new handle;
		}
		else
		{
			return NULL;//-1; //INVALID_HANDLE;
		}
	}

	unsigned int GetName()
	{
		return m_tex;
	}


private:
	Node *m_root;
	unsigned int m_tex;
	unsigned char *m_pData;

};

#endif // _TEXTURECACHE_H_
