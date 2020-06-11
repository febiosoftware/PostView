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

#pragma once
#include "Tool.h"

class CDocument;
class CPlaneToolUI;
class CPlaneDecoration;

class CPlaneTool : public CAbstractTool
{
	Q_OBJECT

public:
	CPlaneTool(CMainWindow* wnd);

	// get the UI widget
	QWidget* createUi();

	// activate the tool
	void activate();

	// deactive the tool
	void deactivate();

	void update(bool breset);

private:
	void UpdateNormal();

private slots:
	void on_change_node1();
	void on_change_node2();
	void on_change_node3();
	void onAlignView();

private:
	CPlaneToolUI*		ui;
	CPlaneDecoration*	m_dec;
};
