#include "CommandPanel.h"
#include "MainWindow.h"

CCommandPanel::CCommandPanel(CMainWindow* pwnd, QWidget* parent) : QWidget(parent), m_wnd(pwnd)
{
}

void CCommandPanel::Update(bool breset)
{

}

CDocument* CCommandPanel::GetActiveDocument()
{
	return m_wnd->GetActiveDocument();
}
