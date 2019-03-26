#pragma once
#include <vector>
#include <string>

class CDocument;
class CMainWindow;

class CDocManager
{
public:
	CDocManager(CMainWindow* wnd);

	// return the number of documents
	int Documents() const;

	// add a document
	void AddDocument(CDocument* doc);

	// remove a document
	void RemoveDocument(int i);

	// get a document
	CDocument* GetDocument(int i);

	// save the current session to file
	bool SaveSession(const std::string& szfile);

	// open a session from file
	bool OpenSession(const std::string& szfile);

private:
	CMainWindow*	m_wnd;
	std::vector<CDocument*>	m_docList;
};
