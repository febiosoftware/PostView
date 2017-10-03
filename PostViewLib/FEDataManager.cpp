#include "stdafx.h"
#include "FEDataManager.h"
#include "FEModel.h"
#include "constants.h"
#include <string>
#include <cstdio>

FEDataField::FEDataField(Data_Type ntype, Data_Format nfmt, Data_Class ncls, unsigned int flag)
{
	m_ntype = ntype;
	m_nfmt = nfmt;
	m_nclass = ncls;
	m_nref = 0;
	m_flag = flag;
}


void FEDataField::SetName(const std::string& newName)
{ 
	m_name = newName; 
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
		}
	}
	return 0;
}

std::string FEDataField::componentName(int ncomp, Data_Tensor_Type ntype)
{
	const std::string& name = GetName();
	const char* sz = name.c_str();

	char szline[256] = {0};

	if (ntype == DATA_SCALAR)
	{
		switch (m_ntype)
		{
		case DATA_FLOAT: return name; break;
		case DATA_VEC3F:
			{
				if      (ncomp == 0) sprintf(szline, "X - %s"  , sz);
				else if (ncomp == 1) sprintf(szline, "Y - %s"  , sz);
				else if (ncomp == 2) sprintf(szline, "Z - %s"  , sz);
				else if (ncomp == 3) sprintf(szline, "XY - %s" , sz);
				else if (ncomp == 4) sprintf(szline, "YZ - %s" , sz);
				else if (ncomp == 5) sprintf(szline, "XZ - %s" , sz);
				else if (ncomp == 6) sprintf(szline, "Total %s", sz);
				return szline;
			}
			break;
		case DATA_MAT3F:
		case DATA_MAT3D:
			{
				if      (ncomp == 0) sprintf(szline, "XX - %s", sz);
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
				if      (ncomp ==  0) sprintf(szline, "X - %s", sz);
				else if (ncomp ==  1) sprintf(szline, "Y - %s", sz);
				else if (ncomp ==  2) sprintf(szline, "Z - %s", sz);
				else if (ncomp ==  3) sprintf(szline, "XY - %s", sz);
				else if (ncomp ==  4) sprintf(szline, "YZ - %s", sz);
				else if (ncomp ==  5) sprintf(szline, "XZ - %s", sz);
				else if (ncomp ==  6) sprintf(szline, "Effective %s", sz);
				else if (ncomp ==  7) sprintf(szline, "1 Principal %s", sz);
				else if (ncomp ==  8) sprintf(szline, "2 Principal %s", sz);
				else if (ncomp ==  9) sprintf(szline, "3 Principal %s", sz);
				else if (ncomp == 10) sprintf(szline, "1 Dev Principal %s", sz);
				else if (ncomp == 11) sprintf(szline, "2 Dev Principal %s", sz);
				else if (ncomp == 12) sprintf(szline, "3 Dev Principal %s", sz);
				else if (ncomp == 13) sprintf(szline, "Max Shear %s", sz);
				return szline;
			}
			break;
		case DATA_MAT3FD:
			{
				if      (ncomp ==  0) sprintf(szline, "1 - %s", sz);
				else if (ncomp ==  1) sprintf(szline, "2 - %s", sz);
				else if (ncomp ==  2) sprintf(szline, "3 - %s", sz);			
				return szline;
			}
			break;
		case DATA_TENS4FS:
			{
				if      (ncomp ==  0) sprintf(szline, "XXXX - %s" , sz);
				else if (ncomp ==  1) sprintf(szline, "XXYY - %s" , sz);
				else if (ncomp ==  2) sprintf(szline, "YYYY - %s" , sz);
				else if (ncomp ==  3) sprintf(szline, "XXZZ - %s" , sz);
				else if (ncomp ==  4) sprintf(szline, "YYZZ - %s" , sz);
				else if (ncomp ==  5) sprintf(szline, "ZZZZ - %s" , sz);
				else if (ncomp ==  6) sprintf(szline, "XXXY - %s" , sz);
				else if (ncomp ==  7) sprintf(szline, "YYXY - %s" , sz);
				else if (ncomp ==  8) sprintf(szline, "ZZXY - %s" , sz);
				else if (ncomp ==  9) sprintf(szline, "XYXY - %s" , sz);
				else if (ncomp == 10) sprintf(szline, "XXYZ - %s" , sz);
				else if (ncomp == 11) sprintf(szline, "YYYZ - %s" , sz);
				else if (ncomp == 12) sprintf(szline, "ZZYZ - %s" , sz);
				else if (ncomp == 13) sprintf(szline, "XYYZ - %s" , sz);
				else if (ncomp == 14) sprintf(szline, "YZYZ - %s" , sz);
				else if (ncomp == 15) sprintf(szline, "XXXZ - %s" , sz);
				else if (ncomp == 16) sprintf(szline, "YYXZ - %s" , sz);
				else if (ncomp == 17) sprintf(szline, "ZZXZ - %s" , sz);
				else if (ncomp == 18) sprintf(szline, "XYXZ - %s" , sz);
				else if (ncomp == 19) sprintf(szline, "YZXZ - %s" , sz);
				else if (ncomp == 20) sprintf(szline, "XZXZ - %s" , sz);
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
				if      (ncomp == 0) sprintf(szline, "1 Principal %s", sz);
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

FEDataManager::FEDataManager(FEModel* pm)
{
	m_pm = pm;
}

FEDataManager::~FEDataManager(void)
{
	Clear();
}

FEDataFieldPtr FEDataManager::FirstDataField() 
{ 
	return m_Data.begin(); 
}

int FEDataManager::DataFields() const
{ 
	return (int) m_Data.size(); 
}

void FEDataManager::Clear()
{
	vector<FEDataField*>::iterator pi;
	for (pi = m_Data.begin(); pi != m_Data.end(); ++pi) delete (*pi);
	m_Data.clear();
}

void FEDataManager::AddDataField(FEDataField* pd)
{
	m_Data.push_back(pd);
	pd->SetFieldID(BUILD_FIELD(pd->DataClass(), DataFields()-1, 0));
}

void FEDataManager::DeleteDataField(FEDataField* pd)
{
	FEDataFieldPtr it;
	for (it = m_Data.begin(); it != m_Data.end(); ++it)
	{
		if (*it == pd)
		{
			m_Data.erase(it);
			return;
		}
	}
}

int FEDataManager::FindDataField(const std::string& fieldName)
{
	vector<FEDataField*>::iterator pe = m_Data.begin();
	for (int i=0; i<(int) m_Data.size(); ++i, ++pe)
	{
		if ((*pe)->GetName() == fieldName) return i;
	}

	return -1;
}

FEDataFieldPtr FEDataManager::DataField(int i)
{
	return m_Data.begin() + i;
}

std::string FEDataManager::getDataString(int nfield, Data_Tensor_Type ntype)
{
	if (IS_VALID(nfield))
	{
		int ndata = FIELD_CODE(nfield);
		int ncomp = FIELD_COMP(nfield);
		if ((ndata>=0) && (ndata< m_Data.size()))
		{
			FEDataField* pd = m_Data[ndata];
			return pd->componentName(ncomp, ntype);
		}
	}
	return "";
}
