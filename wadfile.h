
#ifndef _WADFILE_H_
#define _WADFILE_H_

#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;

typedef struct
{
	char		identification[4];		// should be WAD2 or 2DAW
	int			numlumps;
	int			infotableofs;
} wadinfo_t;


typedef struct
{
	int			filepos;
	int			disksize;
	int			size;					// uncompressed
	char		type;
	char		compression;
	char		pad1, pad2;
	char		name[16];				// must be null terminated
} lumpinfo_t;

class WadFile
{
public:
	WadFile();
	~WadFile();
	bool Open(const char *filename);
	bool ReadFile(const char *filename, unsigned char *&data, int &length);
	void Close();
#ifdef _DEBUG
	friend ostream &operator<<(ostream &ostr, const WadFile &pf);
#endif

private:
	wadinfo_t m_header;
	bool m_bIsOpen;
	int m_nNumFiles;
	lumpinfo_t *m_pFiles;
	FILE *m_pFile;

};

#endif // _WADFILE_H_