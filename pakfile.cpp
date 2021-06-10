
#include <string.h>
#include "pakfile.h"

PakFile::PakFile()
{
	m_bIsOpen = false;
	int m_nNumFiles = 0;
	m_pFiles = 0;
}

PakFile::~PakFile()
{
	if(m_bIsOpen)
		Close();
	if(m_pFiles)
		delete [] m_pFiles;
}

bool PakFile::Open(const char *filename)
{
	if(m_bIsOpen)
		Close();
	
	m_pFile = fopen(filename, "rb");
	
	if(!m_pFile)
		return false;

	// read header
	fread(&m_header, sizeof(packheader_t), 1, m_pFile);

	// read directories
	m_nNumFiles = m_header.dirlen / sizeof(packfile_t);
	m_pFiles = new packfile_t[m_nNumFiles];
	fseek(m_pFile, m_header.dirofs, SEEK_SET);
	fread(m_pFiles, sizeof(packfile_t), m_nNumFiles, m_pFile);

	m_bIsOpen = true;
	
	return true;
}

bool PakFile::FileExists(const char *filename)
{
	int i;
	bool found = false;
	for(i = 0; i < m_nNumFiles; ++i)
	{
		if(!strcmp(filename, m_pFiles[i].name))
		{
			found = true;
			break;
		}
	}
	return found;
}

bool PakFile::ReadFile(const char *filename, unsigned char *&data)
{
	if(!m_bIsOpen)
		return false;
	
	// find file in list
	int i;
	bool found = false;
	for(i = 0; i < m_nNumFiles; ++i)
	{
		if(!strcmp(filename, m_pFiles[i].name))
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

	fseek(m_pFile, m_pFiles[i].filepos, SEEK_SET);
	data = new unsigned char[m_pFiles[i].filelen];
	fread(data, sizeof(unsigned char), m_pFiles[i].filelen, m_pFile);
	return true;
}

void PakFile::Close()
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
ostream &operator<<(ostream &ostr, const PakFile &pf)
{
	for(int i = 0; i < pf.m_nNumFiles; ++i)
	{
		ostr << pf.m_pFiles[i].name << endl;
	}
	return ostr;
}
#endif
