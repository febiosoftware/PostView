#include "stdafx.h"
#include "FEDataManager.h"
#include "FEModel.h"
#include "constants.h"

FEDataManager::FEDataManager(FEModel* pm)
{
	m_pm = pm;
}

FEDataManager::~FEDataManager(void)
{
	Clear();
}

void FEDataManager::Clear()
{
	vector<FEDataField*>::iterator pi;
	for (pi = m_Node.begin(); pi != m_Node.end(); ++pi) delete (*pi);
	for (pi = m_Elem.begin(); pi != m_Elem.end(); ++pi) delete (*pi);
	for (pi = m_Face.begin(); pi != m_Face.end(); ++pi) delete (*pi);

	m_Node.clear();
	m_Elem.clear();
	m_Face.clear();
}

void FEDataManager::AddNodeData(FEDataField* pd)
{
	m_Node.push_back(pd);
	pd->SetFieldID(BUILD_FIELD(1, m_Node.size()-1, 0));
}

void FEDataManager::AddElemData(FEDataField* pd)
{
	m_Elem.push_back(pd);
	pd->SetFieldID(BUILD_FIELD(2, m_Elem.size()-1, 0));
}

void FEDataManager::AddFaceData(FEDataField* pd)
{
	m_Face.push_back(pd);
	pd->SetFieldID(BUILD_FIELD(4, m_Face.size()-1, 0));
}

void FEDataManager::DeleteNodeData(FEDataField* pd)
{
	FEDataFieldPtr it;
	for (it = m_Node.begin(); it != m_Node.end(); ++it)
	{
		if (*it == pd)
		{
			m_Node.erase(it);
			return;
		}
	}
}

void FEDataManager::DeleteFaceData(FEDataField* pd)
{
	FEDataFieldPtr it;
	for (it = m_Face.begin(); it != m_Face.end(); ++it)
	{
		if (*it == pd)
		{
			m_Face.erase(it);
			return;
		}
	}
}

void FEDataManager::DeleteElemData(FEDataField* pd)
{
	FEDataFieldPtr it;
	for (it = m_Elem.begin(); it != m_Elem.end(); ++it)
	{
		if (*it == pd)
		{
			m_Elem.erase(it);
			return;
		}
	}
}

int FEDataManager::FindElem(const char* szname)
{
	vector<FEDataField*>::iterator pe = m_Elem.begin();
	for (int i=0; i<(int) m_Elem.size(); ++i, ++pe)
	{
		if (strcmp((*pe)->GetName(), szname) == 0) return i;
	}

	return -1;
}

FEDataFieldPtr FEDataManager::NodeData(int i)
{
	return m_Node.begin() + i;
}

FEDataFieldPtr FEDataManager::FaceData(int i)
{
	return m_Face.begin() + i;
}

FEDataFieldPtr FEDataManager::ElementData(int i)
{
	return m_Elem.begin() + i;
}
