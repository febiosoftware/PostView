#include "stdafx.h"
#include "DocManager.h"
#include "Document.h"

CDocManager::CDocManager(CMainWindow* wnd) : m_wnd(wnd)
{

}

// return the number of documents
int CDocManager::Documents() const
{
	return (int)m_docList.size();
}

// add a document
void CDocManager::AddDocument(CDocument* doc)
{
	m_docList.push_back(doc);
}

// remove a document
void CDocManager::RemoveDocument(int i)
{
	CDocument* doc = m_docList[i];
	m_docList.erase(m_docList.begin() + i);
	delete doc;
}

CDocument* CDocManager::GetDocument(int i)
{
	if ((i >= 0) && (i < m_docList.size()))
		return m_docList[i];
	else
		return nullptr;
}
