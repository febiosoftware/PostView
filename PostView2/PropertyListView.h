#pragma once
#include <QWidget>
#include <vector>

//-----------------------------------------------------------------------------
class QTableWidget;
class QLabel;

//-----------------------------------------------------------------------------
class CPropertyList
{
public:
	class CProperty
	{
	public:
		QString		m_name;
		QString		m_val;
		QString		m_info;

	public:
		CProperty(const QString& name, const QString& val) : m_name(name), m_val(val), m_info(name) {}
		CProperty(const QString& name, const QString& val, const QString& info) : m_name(name), m_val(val), m_info(info) {}
	};

public:
	CPropertyList(){}
	virtual ~CPropertyList(){}

	int Properties() const { return (int) m_list.size(); }

	void AddProperty(const CProperty& p) { m_list.push_back(p); }

	const CProperty& Property(int i) { return m_list[i]; }

	virtual QVariant GetPropertyValue(int i) = 0;
	virtual void SetPropertyValue(int i, const QVariant& v) = 0;

private:
	std::vector<CProperty>	m_list;
};

//-----------------------------------------------------------------------------
class CPropertyListView : public QWidget
{
	Q_OBJECT

public:
	CPropertyListView(QWidget* parent = 0);

	void Update(CPropertyList* plist);

	void Clear();

private slots:
	void on_modelProps_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
	void on_modelProps_cellClicked(int row, int column);
	void comboChanged(int);

private:
	CPropertyList*	m_list;
	QTableWidget*	m_prop;
	QLabel*			m_info;

	QWidget*	m_sel;
	int			m_selRow, m_selCol;
};
