#pragma once

#include "FEMeshData.h"
#include <vector>
#include <typeinfo>
#include <string>

using namespace std;

//-----------------------------------------------------------------------------
// forward declaration of the FEModel class
class FEModel;

//-----------------------------------------------------------------------------
// data field flags
enum DataFieldFlags {
	EXPORT_DATA = 1			// data field can be exported
};

//-----------------------------------------------------------------------------
// Base class describing a data field
class FEDataField
{
public:
	enum { MAX_DATA_NAME = 64 };

public:
	FEDataField(Data_Type ntype, Data_Format nfmt, Data_Class ncls, unsigned int flag = 0)
	{ 
		m_szname[0] = 0; 
		m_ntype = ntype; 
		m_nfmt = nfmt; 
		m_nclass = ncls; 
		m_nref = 0;
		m_flag = flag;
	}
	virtual ~FEDataField(){}

	//! get the name of the field
	const char* GetName() const { return m_szname; }

	//! set the name of the field
	void SetName(const char* szname);

	//! Create a copy
	virtual FEDataField* Clone() const = 0;

	//! FEMeshData constructor
	virtual FEMeshData* CreateData(FEState* pstate) = 0;

	//! type identifier
	Data_Type Type() { return m_ntype; }

	// Format identifier
	Data_Format Format() { return m_nfmt; }

	// Class Identifier
	Data_Class DataClass() { return m_nclass; }

	//! Set the field ID
	void SetFieldID(int nid) { m_nfield = nid; }

	//! get the field ID
	int GetFieldID() const { return m_nfield; }

	//! type string
	const char* TypeStr()
	{
		switch(m_ntype)
		{
		case DATA_FLOAT  : return "float"  ; break;
		case DATA_VEC3F  : return "vec3f"  ; break;
		case DATA_MAT3F  : return "mat3f"  ; break;
		case DATA_MAT3D  : return "mat3d"  ; break;
		case DATA_MAT3FS : return "mat3fs" ; break;
		case DATA_MAT3FD : return "mat3fd" ; break;
        case DATA_TENS4FS: return "tens4fs"; break;
		};
		return "unknown";
	}

	//! number of components
	int components(Data_Tensor_Type ntype);

	//! return the name of a component
	std::string componentName(int ncomp, Data_Tensor_Type ntype);

	virtual const std::type_info& TypeInfo() { return typeid(FEDataField); }

	unsigned int Flags() const { return m_flag; }

protected:
	char		m_szname[MAX_DATA_NAME];	//!< field name
	int			m_nfield;					//!< field ID
	Data_Type	m_ntype;					//!< data type
	Data_Format	m_nfmt;						//!< data format
	Data_Class  m_nclass;					//!< data class
	unsigned int	m_flag;					//!< flags

public:
	// TODO: Add properties list for data fields (e.g. strains and curvature could use this)
	// strain parameters
	int		m_nref;	// reference state
	
	// used by mesh data to see if an item has been assigned a value
	vector<int>		m_item;
};

//-----------------------------------------------------------------------------
template<typename T> class FEDataField_T : public FEDataField
{
public:
	FEDataField_T(const char* szname, unsigned int flag = 0) : FEDataField(T::Type(), T::Format(), T::Class(), flag) { SetName(szname); }
	FEMeshData* CreateData(FEState* pstate) { return new T(pstate, this); }

	virtual FEDataField* Clone() const 
	{ 
		FEDataField_T<T>* newData = new FEDataField_T<T>(GetName());
		newData->m_item = m_item;
		return newData;
	}

	const std::type_info& TypeInfo() { return typeid(T); }
};

//-----------------------------------------------------------------------------
typedef vector<FEDataField*>::iterator FEDataFieldPtr;

//-----------------------------------------------------------------------------
// The data manager stores the attributes (name and type) of the different
// data fields.
class FEDataManager
{
public:
	FEDataManager(FEModel* pm);
	~FEDataManager(void);

	//! add a data field
	void AddDataField(FEDataField* pd);

	//! delete a data field
	void DeleteDataField(FEDataField* pd);

	//! get the nodal datafield
	FEDataFieldPtr FirstDataField();
	int DataFields() const;

	//! clear data
	void Clear();

	//! find the index of a datafield
	int FindDataField(const char* sz);

	//! find the data field given an index
	FEDataFieldPtr DataField(int i);

	std::string getDataString(int nfield, Data_Tensor_Type ntype);

protected:
	vector<FEDataField*>	m_Data;
	FEModel*	m_pm;
};
