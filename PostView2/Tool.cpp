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

#include "Tool.h"
#include "PropertyListForm.h"
#include "MainWindow.h"
#include <QApplication>
#include <QBoxLayout>
#include <QPushButton>

//-----------------------------------------------------------------------------
CAbstractTool::CAbstractTool(const QString& s, CMainWindow* wnd) : m_name(s), m_wnd(wnd) 
{
}

//-----------------------------------------------------------------------------
void CAbstractTool::updateUi()
{
	QWidget* w = QApplication::activeWindow();
	if (w) w->repaint();
}

//-----------------------------------------------------------------------------
// get the active document
CDocument* CAbstractTool::GetActiveDocument()
{
	return m_wnd->GetActiveDocument();
}

//-----------------------------------------------------------------------------
void CToolUI::hideEvent(QHideEvent* ev)
{
	if (m_tool) m_tool->deactivate();
}

//-----------------------------------------------------------------------------
void CToolUI::showEvent(QShowEvent* ev)
{
	if (m_tool) m_tool->activate();
}

//-----------------------------------------------------------------------------
CBasicTool::CBasicTool(const QString& s, CMainWindow* wnd, unsigned int flags) : CAbstractTool(s, wnd)
{
	m_list = 0;
	m_form = 0;
	m_flags = flags;
}

//-----------------------------------------------------------------------------
QWidget* CBasicTool::createUi()
{
	m_list = getPropertyList();
	if (m_list == 0) return 0;

	CToolUI* pw = new CToolUI(this);
	QVBoxLayout* pl = new QVBoxLayout(pw);
	pw->setLayout(pl);

	m_form = new CPropertyListForm;
	m_form->setPropertyList(m_list);
	pl->addWidget(m_form);

	if (m_flags & HAS_APPLY_BUTTON)
	{
		QHBoxLayout* pg = new QHBoxLayout;
		pg->addStretch();
		
		QPushButton* pb = new QPushButton("Apply");
		pg->addWidget(pb);
		pl->addLayout(pg);

		QObject::connect(pb, SIGNAL(clicked(bool)), this, SLOT(on_button_clicked()));
	}

	return pw;
}

//-----------------------------------------------------------------------------
void CBasicTool::on_button_clicked()
{
	OnApply();
}

//-----------------------------------------------------------------------------
void CBasicTool::updateUi()
{
	if (m_form) 
	{
		m_form->updateData();
	}
	CAbstractTool::updateUi();
}
