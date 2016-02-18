#include "FEFileReader.h"
#include <stdarg.h>
using namespace std;

FEFileReader::FEFileReader(const char* sztype) : m_sztype(sztype)
{
	m_fp = 0;
}

FEFileReader::~FEFileReader()
{
}

bool FEFileReader::Open(const char* szfile, const char* szmode)
{
	if (m_fp) Close();
	m_fp = fopen(szfile, szmode);
	m_fileName = szfile;
	return (m_fp != 0);
}

void FEFileReader::Close()
{
	if (m_fp) fclose(m_fp);
	m_fp = 0;
}

const std::string& FEFileReader::GetErrorMessage()
{
	return m_err;
}
	
int FEFileReader::Errors()
{
	return m_nerrors;
}

bool FEFileReader::errf(const char* szerr, ...)
{
	// get a pointer to the argument list
	va_list	args;

	// copy to string
	va_start(args, szerr);
	char sz[512] = {0};
	vsprintf_s(sz, 511, szerr, args);
	va_end(args);

	// append to the error string
	if (m_err.empty())
	{
		m_err = string(sz);
	}
	else m_err.append("\n").append(sz);
	
	m_nerrors++;

	// close the file
	Close();

	return false;
}
