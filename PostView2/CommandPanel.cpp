#include "CommandPanel.h"
#include "MainWindow.h"

CCommandPanel::CCommandPanel(CMainWindow* pwnd, QWidget* parent) : QWidget(parent), m_wnd(pwnd)
{
	CDocument* doc = pwnd->GetDocument();
	doc->AddObserver(this);
}

void CCommandPanel::Update(bool breset)
{

}
