#pragma once
#include "CommandPanel.h"
#include <vector>
using namespace std;

//-----------------------------------------------------------------------------
class QTreeWidgetItem;

//-----------------------------------------------------------------------------
namespace Ui {
	class CModelViewer;
}

//-----------------------------------------------------------------------------
class CPropertyList
{
public:
	class CProperty
	{
	public:
		QString	m_name;
		QString	m_val;

	public:
		CProperty(const QString& name, const QString& val) : m_name(name), m_val(val){}
	};

public:
	CPropertyList(){}
	virtual ~CPropertyList(){}

	int Properties() const { return (int) m_list.size(); }

	void AddProperty(const CProperty& p) { m_list.push_back(p); }

	const CProperty& Property(int i) const { return m_list[i]; }

private:
	vector<CProperty>	m_list;
};

//-----------------------------------------------------------------------------
class CModelViewer : public CCommandPanel
{
	Q_OBJECT

public:
	CModelViewer(CMainWindow* pwnd, QWidget* parent = 0);

public:
	void Update();

private slots:
	void on_modelTree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* prev);

private:
	Ui::CModelViewer*	ui;
};
