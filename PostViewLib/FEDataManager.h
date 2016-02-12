#pragma once

#include "FEMeshData.h"
#include <list>
#include <typeinfo>
#include <string.h>

using namespace std;

//-----------------------------------------------------------------------------
// forward declaration of the FEModel class
class FEModel;

//-----------------------------------------------------------------------------
// Base class describing a data field
class FEDataField
{
public:
	enum { MAX_DATA_NAME = 64 };

public:
	FEDataField(Data_Type ntype, Data_Format nfmt, Data_Class ncls) { m_szname[0] = 0; m_ntype = ntype; m_nfmt = nfmt; m_nclass = ncls; m_nref = 0;}
	virtual ~FEDataField(){}

	//! get the name of the field
	const char* GetName() const { return m_szname; }

	//! set the name of the field
	void SetName(const char* szname) { strcpy(m_szname, szname); }

	//! Create a copy
	virtual FEDataField* Clone() const = 0;

	//! FEMeshData constructor
	virtual FEMeshData* CreateData(FEModel* pm) = 0;

	//! type identifier
	Data_Type Type() { return m_ntype; }

	// Format identifier
	Data_Format Format() { return m_nfmt; }

	// Class Identifier
	Data_Class DataClass() { return m_nclass; }

	//! Set the field ID
	void SetFieldID(int nid) { m_nfield = nid; }

	//! get the field ID
	int GetFieldID() { return m_nfield; }

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

	virtual const std::type_info& TypeInfo() { return typeid(FEDataField); }

protected:
	char		m_szname[MAX_DATA_NAME];	//!< field name
	int			m_nfield;					//!< field ID
	Data_Type	m_ntype;					//!< data type
	Data_Format	m_nfmt;						//!< data format
	Data_Class  m_nclass;					//!< data class

public:
	// TODO: Add properties list for data fields (e.g. strains and curvature could use this)
	// strain parameters
	int		m_nref;	// reference state
};

//-----------------------------------------------------------------------------
template<typename T> class FEDataField_T : public FEDataField
{
public:
	FEDataField_T(const char* szname) : FEDataField(T::Type(), T::Format(), T::Class()) { SetName(szname); }
	FEMeshData* CreateData(FEModel* pm) { return new T(pm); }

	virtual FEDataField* Clone() const { return new FEDataField_T<T>(GetName()); }

	const std::type_info& TypeInfo() { return typeid(T); }
};

//-----------------------------------------------------------------------------
typedef list<FEDataField*>::iterator FEDataFieldPtr;

//-----------------------------------------------------------------------------
// The data manager stores the attributes (name and type) of the different
// data fields.
class FEDataManager
{
public:
	FEDataManager(FEModel* pm);
	~FEDataManager(void);

	//! add a nodal data field
	void AddNodeData(FEDataField* pd);

	//! add an element data field
	void AddElemData(FEDataField* pd);

	//! add a face data field
	void AddFaceData(FEDataField* pd);

	//! delete a nodal data field
	void DeleteNodeData(FEDataField* pd);

	//! delete a face data field
	void DeleteFaceData(FEDataField* pd);

	//! delete a element data field
	void DeleteElemData(FEDataField* pd);

	//! get the first nodal datafield
	FEDataFieldPtr FirstNode() { return m_Node.begin(); }
	int NodeFields() { return m_Node.size(); }

	//! get the first element datafield
	FEDataFieldPtr FirstElement() { return m_Elem.begin(); }
	int ElementFields() { return m_Elem.size(); }

	//! get the first face datafield
	FEDataFieldPtr FirstFace() { return m_Face.begin(); }
	int FaceFields() { return m_Face.size(); }

	//! clear data
	void Clear();

	//! find the index of a datafield
	int FindElem(const char* sz);

	//! find the data field given an index
	FEDataFieldPtr ElementData(int i);

protected:
	list<FEDataField*>	m_Node;		// nodal data
	list<FEDataField*>	m_Elem;		// element data
	list<FEDataField*>	m_Face;		// face data

	FEModel*	m_pm;
};

