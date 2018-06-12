#pragma once
#include "FEFileReader.h"
#include "PltArchive.h"

enum XPLT_READ_STATE_FLAG { 
	XPLT_READ_ALL_STATES, 
	XPLT_READ_LAST_STATE_ONLY, 
	XPLT_READ_STATES_FROM_LIST,
	XPLT_READ_FIRST_AND_LAST 
};

class xpltFileReader;

class xpltParser
{
public:
	xpltParser(xpltFileReader* xplt);
	virtual ~xpltParser();

	virtual bool Load(FEModel& fem) = 0;

	bool errf(const char* sz);

protected:
	xpltFileReader*	m_xplt;
	IArchive&		m_ar;
};

class xpltFileReader : public FEFileReader
{
protected:
	// file tags
	enum { 
		PLT_ROOT						= 0x01000000,
		PLT_HEADER						= 0x01010000,
			PLT_HDR_VERSION				= 0x01010001,
			PLT_HDR_NODES				= 0x01010002,	// obsolete in 2.0
			PLT_HDR_MAX_FACET_NODES		= 0x01010003,	// obsolete in 2.0 (redefined in each Surface section)
			PLT_HDR_COMPRESSION			= 0x01010004,	
			PLT_HDR_AUTHOR				= 0x01010005,	// new in 2.0
			PLT_HDR_SOFTWARE			= 0x01010006,	// new in 2.0
	};

	// size of name variables
	enum { DI_NAME_SIZE = 64 };

public:
	struct HEADER
	{
		int	nversion;
		int ncompression;				//!< compression method (or level)
		int	nn;							//!< nodes (not used for >= 2.0)
		int	nmax_facet_nodes;			//!< max nodes per facet (depends on version; not used >= 2.0)
		char author[DI_NAME_SIZE];		//!< name of author
		char software[DI_NAME_SIZE];	//!< name of software that generated the file
	};

public:
	xpltFileReader();
	~xpltFileReader();

	bool Load(FEModel& fem, const char* szfile);

	void SetReadStateFlag(int n) { m_read_state_flag = n; }
	void SetReadStatesList(vector<int>& l) { m_state_list = l; }

	int GetReadStateFlag() const { return m_read_state_flag; }
	vector<int> GetReadStates() const { return m_state_list; }

public:
	IArchive& GetArchive() { return m_ar; }

	const HEADER& GetHeader() const { return m_hdr; }

protected:
	bool ReadHeader();

private:
	xpltParser*	m_xplt;
	IArchive	m_ar;
	HEADER		m_hdr;

	// Options
	int			m_read_state_flag;	//!< flag setting option for reading states
	vector<int>	m_state_list;		//!< list of states to read (only when m_read_state_flag == XPLT_READ_STATES_FROM_LIST)
};
