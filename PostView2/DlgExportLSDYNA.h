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
#include <QDialog>

class CDlgExportLSDYNA_UI;

class CDlgExportLSDYNA : public QDialog
{
public:
	CDlgExportLSDYNA(QWidget* parent);

	void accept();

public:
	bool	m_bsel;
	bool	m_bsurf;
	bool	m_bnode;

private:
	CDlgExportLSDYNA_UI*	ui;
};

class CDlgExportLSDYNAPlot_UI;

namespace Post {
	class FEPostModel;
}

class CDlgExportLSDYNAPlot : public QDialog
{
public:
	CDlgExportLSDYNAPlot(Post::FEPostModel* fem, QWidget* parent);

	void accept();

public:
	bool	m_flag[6];
	int		m_code[6];

private:
	CDlgExportLSDYNAPlot_UI*	ui;
};
