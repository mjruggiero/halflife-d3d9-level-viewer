
#include <string.h>
#include "wadfile.h"

WadFile::WadFile()
{
	m_bIsOpen = false;
	int m_nNumFiles = 0;
	m_pFiles = 0;
}

WadFile::~WadFile()
{
	if(m_bIsOpen)
		Close();
	if(m_pFiles)
		delete [] m_pFiles;
}

bool WadFile::Open(const char *filename)
{
	if(m_bIsOpen)
		Close();
	
	m_pFile = fopen(filename, "rb");
	
	if(!m_pFile)
		return false;

	// read header
	fread(&m_header, sizeof(wadinfo_t), 1, m_pFile);

	// read directories
	m_nNumFiles = m_header.numlumps;
	m_pFiles = new lumpinfo_t[m_nNumFiles];
	fseek(m_pFile, m_header.infotableofs, SEEK_SET);
	fread(m_pFiles, sizeof(lumpinfo_t), m_nNumFiles, m_pFile);

	m_bIsOpen = true;
	
	return true;
}

bool WadFile::ReadFile(const char *filename, unsigned char *&data, int &length)
{
	if(!m_bIsOpen)
		return false;
	
	// find file in list
	int i;
	bool found = false;
	for(i = 0; i < m_nNumFiles; ++i)
	{
		if(!_stricmp(filename, m_pFiles[i].name))
		{
			found = true;
			break;
		}
	}

	if(!found)
	{
		data = NULL;
		return false;
	}

	length = m_pFiles[i].size;
	fseek(m_pFile, m_pFiles[i].filepos, SEEK_SET);
	data = new unsigned char[m_pFiles[i].size];
	fread(data, sizeof(unsigned char), m_pFiles[i].disksize, m_pFile);
	return true;
}

void WadFile::Close()
{
	if(m_pFile)
	{
		if(m_pFiles)
		{
			delete [] m_pFiles;
			m_pFiles= NULL;
		}
		m_nNumFiles = 0;
		fclose(m_pFile);
	}
	m_bIsOpen = false;
}

#ifdef _DEBUG
ostream &operator<<(ostream &ostr, const WadFile &pf)
{
	for(int i = 0; i < pf.m_nNumFiles; ++i)
	{
		ostr << pf.m_pFiles[i].name << endl;
	}
	return ostr;
}
#endif
