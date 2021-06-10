
#ifndef _PAKFILE_H_
#define _PAKFILE_H_

#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;

typedef struct
{
	char	name[56];
	int		filepos, filelen;
} packfile_t;

typedef struct
{
	char	id[4];
	int		dirofs;
	int		dirlen;
} packheader_t;

class PakFile
{
public:
	PakFile();
	~PakFile();
	bool Open(const char *filename);
	bool FileExists(const char *filename);
	bool ReadFile(const char *filename, unsigned char *&data);
	void Close();
#ifdef _DEBUG
	friend ostream &operator<<(ostream &ostr, const PakFile &pf);
#endif

private:
	packheader_t m_header;
	bool m_bIsOpen;
	int m_nNumFiles;
	packfile_t *m_pFiles;
	FILE *m_pFile;

};

#endif // _PAKFILE_H_