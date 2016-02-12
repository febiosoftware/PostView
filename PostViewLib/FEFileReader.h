#pragma once
#include <stdio.h>
#include <string>

class FEModel;

class FEFileReader
{
public:
	FEFileReader(const char* sztype);
	virtual ~FEFileReader();

	// This function must be overloaded in derived classes
	virtual bool Load(FEModel& fem, const char* szfile) = 0;

public:
	// get the error string
	const std::string& GetErrorMessage();
	
	// get the number of errors
	int Errors();

protected:
	// open the file
	bool Open(const char* szfile, const char* szmode);

	// close the file
	void Close();

	// helper function that sets the error string
	bool errf(const char* szerr, ...);

protected:
	FILE*	m_fp;

private:
	const char*		m_sztype;	//!< type identifier
	std::string		m_err;		//!< error messages (separated by \n)
	int				m_nerrors;	//!< number of errors
};
