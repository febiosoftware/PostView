#include "stdafx.h"
#include "FEDataField.h"
#include "FEMeshData_T.h"
using namespace Post;

FEDataField::FEDataField(const std::string& name, Data_Type ntype, Data_Format nfmt, Data_Class ncls, unsigned int flag)
{
	m_ntype = ntype;
	m_nfmt = nfmt;
	m_nclass = ncls;
	m_nref = 0;
	m_flag = flag;
	m_name = name;
	m_arraySize = 0;
}


void FEDataField::SetName(const std::string& newName)
{
	m_name = newName;
}

void FEDataField::SetArrayNames(vector<string>& n)
{
	m_arrayNames = n;
}

vector<string> FEDataField::GetArrayNames() const
{
	return m_arrayNames;
}

const char* FEDataField::TypeStr() const
{
	switch (m_ntype)
	{
	case DATA_FLOAT: return "float"; break;
	case DATA_VEC3F: return "vec3f"; break;
	case DATA_MAT3F: return "mat3f"; break;
	case DATA_MAT3D: return "Mat3d"; break;
	case DATA_MAT3FS: return "mat3fs"; break;
	case DATA_MAT3FD: return "mat3fd"; break;
	case DATA_TENS4FS: return "tens4fs"; break;
	case DATA_ARRAY: return "array"; break;
	case DATA_ARRAY_VEC3F: return "array<vec3>"; break;
	};
	return "unknown";
}

int FEDataField::components(Data_Tensor_Type ntype)
{
	if (ntype == DATA_SCALAR)
	{
		switch (m_ntype)
		{
		case DATA_FLOAT: return 1; break;
		case DATA_VEC3F: return 7; break;
		case DATA_MAT3F: return 9; break;
		case DATA_MAT3D: return 9; break;
		case DATA_MAT3FS: return 14; break;
		case DATA_MAT3FD: return 3; break;
		case DATA_TENS4FS: return 21; break;
		case DATA_ARRAY: return GetArraySize(); break;
		case DATA_ARRAY_VEC3F: return GetArraySize()*4; break;
		}
	}
	else if (ntype == DATA_VECTOR)
	{
		switch (m_ntype)
		{
		case DATA_FLOAT: return 0; break;
		case DATA_VEC3F: return 1; break;
		case DATA_MAT3F: return 3; break;
		case DATA_MAT3D: return 3; break;
		case DATA_MAT3FS: return 3; break;
		case DATA_MAT3FD: return 0; break;
		case DATA_TENS4FS: return 0; break;
		case DATA_ARRAY: return 0; break;
		case DATA_ARRAY_VEC3F: return GetArraySize(); break;
		}
	}
	else if (ntype == DATA_TENSOR2)
	{
		switch (m_ntype)
		{
		case DATA_FLOAT: return 0; break;
		case DATA_VEC3F: return 0; break;
		case DATA_MAT3F: return 1; break;
		case DATA_MAT3D: return 1; break;
		case DATA_MAT3FS: return 1; break;
		case DATA_MAT3FD: return 1; break;
		case DATA_TENS4FS: return 0; break;
		case DATA_ARRAY: return 0; break;
		case DATA_ARRAY_VEC3F: return 0; break;
		}
	}
	return 0;
}

std::string FEDataField::componentName(int ncomp, Data_Tensor_Type ntype)
{
	const std::string& name = GetName();
	const char* sz = name.c_str();

	char szline[256] = { 0 };

	if (ntype == DATA_SCALAR)
	{
		switch (m_ntype)
		{
		case DATA_FLOAT: return name; break;
		case DATA_VEC3F:
		{
			if (ncomp == 0) sprintf(szline, "X - %s", sz);
			else if (ncomp == 1) sprintf(szline, "Y - %s", sz);
			else if (ncomp == 2) sprintf(szline, "Z - %s", sz);
			else if (ncomp == 3) sprintf(szline, "XY - %s", sz);
			else if (ncomp == 4) sprintf(szline, "YZ - %s", sz);
			else if (ncomp == 5) sprintf(szline, "XZ - %s", sz);
			else if (ncomp == 6) sprintf(szline, "Total %s", sz);
			return szline;
		}
		break;
		case DATA_MAT3F:
		case DATA_MAT3D:
		{
			if (ncomp == 0) sprintf(szline, "XX - %s", sz);
			else if (ncomp == 1) sprintf(szline, "XY - %s", sz);
			else if (ncomp == 2) sprintf(szline, "XZ - %s", sz);
			else if (ncomp == 3) sprintf(szline, "YX - %s", sz);
			else if (ncomp == 4) sprintf(szline, "YY - %s", sz);
			else if (ncomp == 5) sprintf(szline, "YZ - %s", sz);
			else if (ncomp == 6) sprintf(szline, "ZX - %s", sz);
			else if (ncomp == 7) sprintf(szline, "ZY - %s", sz);
			else if (ncomp == 8) sprintf(szline, "ZZ - %s", sz);
			return szline;
		}
		break;
		case DATA_MAT3FS:
		{
			if (ncomp == 0) sprintf(szline, "X - %s", sz);
			else if (ncomp == 1) sprintf(szline, "Y - %s", sz);
			else if (ncomp == 2) sprintf(szline, "Z - %s", sz);
			else if (ncomp == 3) sprintf(szline, "XY - %s", sz);
			else if (ncomp == 4) sprintf(szline, "YZ - %s", sz);
			else if (ncomp == 5) sprintf(szline, "XZ - %s", sz);
			else if (ncomp == 6) sprintf(szline, "Effective %s", sz);
			else if (ncomp == 7) sprintf(szline, "1 Principal %s", sz);
			else if (ncomp == 8) sprintf(szline, "2 Principal %s", sz);
			else if (ncomp == 9) sprintf(szline, "3 Principal %s", sz);
			else if (ncomp == 10) sprintf(szline, "1 Dev Principal %s", sz);
			else if (ncomp == 11) sprintf(szline, "2 Dev Principal %s", sz);
			else if (ncomp == 12) sprintf(szline, "3 Dev Principal %s", sz);
			else if (ncomp == 13) sprintf(szline, "Max Shear %s", sz);
			return szline;
		}
		break;
		case DATA_MAT3FD:
		{
			if (ncomp == 0) sprintf(szline, "1 - %s", sz);
			else if (ncomp == 1) sprintf(szline, "2 - %s", sz);
			else if (ncomp == 2) sprintf(szline, "3 - %s", sz);
			return szline;
		}
		break;
		case DATA_TENS4FS:
		{
			if (ncomp == 0) sprintf(szline, "XXXX - %s", sz);
			else if (ncomp == 1) sprintf(szline, "XXYY - %s", sz);
			else if (ncomp == 2) sprintf(szline, "YYYY - %s", sz);
			else if (ncomp == 3) sprintf(szline, "XXZZ - %s", sz);
			else if (ncomp == 4) sprintf(szline, "YYZZ - %s", sz);
			else if (ncomp == 5) sprintf(szline, "ZZZZ - %s", sz);
			else if (ncomp == 6) sprintf(szline, "XXXY - %s", sz);
			else if (ncomp == 7) sprintf(szline, "YYXY - %s", sz);
			else if (ncomp == 8) sprintf(szline, "ZZXY - %s", sz);
			else if (ncomp == 9) sprintf(szline, "XYXY - %s", sz);
			else if (ncomp == 10) sprintf(szline, "XXYZ - %s", sz);
			else if (ncomp == 11) sprintf(szline, "YYYZ - %s", sz);
			else if (ncomp == 12) sprintf(szline, "ZZYZ - %s", sz);
			else if (ncomp == 13) sprintf(szline, "XYYZ - %s", sz);
			else if (ncomp == 14) sprintf(szline, "YZYZ - %s", sz);
			else if (ncomp == 15) sprintf(szline, "XXXZ - %s", sz);
			else if (ncomp == 16) sprintf(szline, "YYXZ - %s", sz);
			else if (ncomp == 17) sprintf(szline, "ZZXZ - %s", sz);
			else if (ncomp == 18) sprintf(szline, "XYXZ - %s", sz);
			else if (ncomp == 19) sprintf(szline, "YZXZ - %s", sz);
			else if (ncomp == 20) sprintf(szline, "XZXZ - %s", sz);
			return szline;
		}
		break;
		case DATA_ARRAY:
			{
				if (m_arrayNames.size() == m_arraySize)
				{
					sprintf(szline, "%s (%s)", sz, m_arrayNames[ncomp].c_str());
				}
				else
				{
					sprintf(szline, "%s[%d]", sz, ncomp);
				}
				return szline;
			}
			break;
		case DATA_ARRAY_VEC3F:
			{
				int index = ncomp / 4;
				int m = ncomp % 4;

				if (m_arrayNames.size() == m_arraySize)
				{
					const string& arr = m_arrayNames[index];
					switch (m)
					{
					case 0: sprintf(szline, "X - %s (%s)", sz, arr.c_str()); break;
					case 1: sprintf(szline, "Y - %s (%s)", sz, arr.c_str()); break;
					case 2: sprintf(szline, "Z - %s (%s)", sz, arr.c_str()); break;
					case 3: sprintf(szline, "Total %s (%s)", sz, arr.c_str()); break;
					}
				}
				else
				{
					switch (m)
					{
					case 0: sprintf(szline, "X - %s [%d]", sz, index); break;
					case 1: sprintf(szline, "Y - %s [%d]", sz, index); break;
					case 2: sprintf(szline, "Z - %s [%d]", sz, index); break;
					case 3: sprintf(szline, "Total %s [%d]", sz, index); break;
					}
				}
				return szline;
			}
			break;
		}
	}
	else if (ntype == DATA_VECTOR)
	{
		switch (m_ntype)
		{
		case DATA_VEC3F: return name; break;
		case DATA_MAT3FS:
			{
				if      (ncomp == 0) sprintf(szline, "1 Principal %s", sz);
				else if (ncomp == 1) sprintf(szline, "2 Principal %s", sz);
				else if (ncomp == 2) sprintf(szline, "3 Principal %s", sz);
				return szline;
			}
		break;
		case DATA_MAT3F:
		case DATA_MAT3D:
			{
				if      (ncomp == 0) sprintf(szline, "column 1 %s", sz);
				else if (ncomp == 1) sprintf(szline, "column 2 %s", sz);
				else if (ncomp == 2) sprintf(szline, "column 3 %s", sz);
				return szline;
			}
		break;
		case DATA_ARRAY_VEC3F:
			{
				const string& arr = m_arrayNames[ncomp];
				if (m_arrayNames.size() == m_arraySize)
				{
					sprintf(szline, "%s (%s)", sz, arr.c_str());
				}
				else
				{
					sprintf(szline, "%s [%d]", sz, ncomp);
				}
				return szline;
			}
		break;
		}
	}
	else if (ntype == DATA_TENSOR2)
	{
		switch (m_ntype)
		{
		case DATA_MAT3FS: return name; break;
		}
	}

	return "(invalid)";
}

//=================================================================================================
FEArrayDataField::FEArrayDataField(const std::string& name, Data_Class c, Data_Format f, unsigned int flag) : FEDataField(name, DATA_ARRAY, f, c, flag)
{
}

FEDataField* FEArrayDataField::Clone() const
{
	FEArrayDataField* newData = new FEArrayDataField(GetName(), DataClass(), Format(), m_flag);
	newData->SetArraySize(GetArraySize());
    vector<string> arrnames = GetArrayNames();
	newData->SetArrayNames(arrnames);
	return newData;
}

FEMeshData* FEArrayDataField::CreateData(FEState* pstate)
{
	switch (DataClass())
	{
	case CLASS_NODE: return new FENodeArrayData(pstate, GetArraySize()); break;
	case CLASS_ELEM: 
		switch (Format())
		{
		case DATA_ITEM: return new FEElemArrayDataItem(pstate, GetArraySize(), this); break;
		case DATA_NODE: return new FEElemArrayDataNode(pstate, GetArraySize(), this); break;
		}
		break;
	}
	assert(false);
	return 0;
}

//=================================================================================================
FEArrayVec3DataField::FEArrayVec3DataField(const std::string& name, Data_Class c, unsigned int flag) : FEDataField(name, DATA_ARRAY_VEC3F, DATA_ITEM, c, flag)
{
}

FEDataField* FEArrayVec3DataField::Clone() const
{
	FEArrayVec3DataField* newData = new FEArrayVec3DataField(GetName(), DataClass(), m_flag);
	newData->SetArraySize(GetArraySize());
    vector<string> arrnames = GetArrayNames();
    newData->SetArrayNames(arrnames);
	return newData;
}

FEMeshData* FEArrayVec3DataField::CreateData(FEState* pstate)
{
	switch (DataClass())
	{
	case CLASS_ELEM: return new FEElemArrayVec3Data(pstate, GetArraySize(), this); break;
	}
	assert(false);
	return 0;
}
