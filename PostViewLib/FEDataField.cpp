#include "stdafx.h"
#include "FEDataField.h"
#include "FEMeshData_T.h"

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

vector<string> FEDataField::GetArrayNames()
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
	case DATA_MAT3D: return "mat3d"; break;
	case DATA_MAT3FS: return "mat3fs"; break;
	case DATA_MAT3FD: return "mat3fd"; break;
	case DATA_TENS4FS: return "tens4fs"; break;
	case DATA_ARRAY: return "array"; break;
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
		}
	}
	else if (ntype == DATA_VECTOR)
	{
		switch (m_ntype)
		{
		case DATA_FLOAT: return 0; break;
		case DATA_VEC3F: return 1; break;
		case DATA_MAT3F: return 0; break;
		case DATA_MAT3D: return 0; break;
		case DATA_MAT3FS: return 3; break;
		case DATA_MAT3FD: return 0; break;
		case DATA_TENS4FS: return 0; break;
		case DATA_ARRAY: return 0; break;
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
		}
	}
	return 0;
}

std::string FEDataField::componentName(int ncomp, Data_Tensor_Type ntype, bool bshort)
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
			const string& var = GetName();
			if (m_arrayNames.size() == m_arraySize)
			{
				if (bshort)
					sprintf(szline, "%s", m_arrayNames[ncomp].c_str());
				else
					sprintf(szline, "%s (%s)", var.c_str(), m_arrayNames[ncomp].c_str());
			}
			else
			{
				sprintf(szline, "%s[%d]", var.c_str(), ncomp);
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
		case DATA_VEC3F: return string(sz); break;
		case DATA_MAT3FS:
		{
			if (ncomp == 0) sprintf(szline, "1 Principal %s", sz);
			else if (ncomp == 1) sprintf(szline, "2 Principal %s", sz);
			else if (ncomp == 2) sprintf(szline, "3 Principal %s", sz);
			return szline;
		}
		break;
		}
	}
	else if (ntype == DATA_TENSOR2)
	{
		switch (m_ntype)
		{
		case DATA_TENS4FS: return string(sz); break;
		}
	}

	return "(invalid)";
}

//=================================================================================================
FEArrayDataField::FEArrayDataField(const std::string& name, Data_Class c, unsigned int flag) : FEDataField(name, DATA_ARRAY, DATA_ITEM, c, flag)
{
}

FEDataField* FEArrayDataField::Clone() const
{
	FEArrayDataField* newData = new FEArrayDataField(GetName(), DataClass(), m_flag);
	newData->SetArraySize(GetArraySize());
	return newData;
}

FEMeshData* FEArrayDataField::CreateData(FEState* pstate)
{
	switch (DataClass())
	{
	case CLASS_NODE: return new FENodeArrayData(pstate, GetArraySize()); break;
	case CLASS_ELEM: return new FEElemArrayData(pstate, GetArraySize(), this); break;
	}
	assert(false);
	return 0;
}
