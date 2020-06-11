/*This file is part of the PostView source code and is licensed under the MIT license
listed below.

See Copyright-PostView.txt for details.

Copyright (c) 2020 University of Utah, The Trustees of Columbia University in 
the City of New York, and others.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

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
