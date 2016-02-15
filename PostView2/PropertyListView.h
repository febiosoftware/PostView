#pragma once
#include <QWidget>
#include <vector>
#include <QtCore/QVariant>
#include <QPushButton>

//-----------------------------------------------------------------------------
class QTableWidget;
class QLabel;

#include <QPainter>
#include <QColorDialog>

//-----------------------------------------------------------------------------
class CColorButton : public QWidget
{
	Q_OBJECT

public:
	CColorButton(QWidget* parent) : QWidget(parent)
	{
		m_col = QColor(Qt::black);
	}

	void paintEvent(QPaintEvent* ev)
	{
		QPainter p(this);
		p.fillRect(rect(), m_col);
	}

	void mouseReleaseEvent(QMouseEvent* ev)
	{
		QColorDialog dlg;
		QColor col = dlg.getColor(m_col);
		if (col.isValid())
		{
			m_col = col;
			repaint();
			emit colorChanged(m_col);
		}
	}

	void setColor(const QColor& c) { m_col = c; }

signals:
	void colorChanged(QColor col);

private:
	QColor	m_col;
};

//-----------------------------------------------------------------------------
class CPropertyList
{
public:
	class CProperty
	{
	public:
		QString			m_name;		// the name of the property
		QString			m_info;		// description of the property
		QVariant::Type	m_type;		// type of property
		QStringList		m_values;	// set possible values for int properties. (separate by semicolon)

	public:
		CProperty(const QString& name, QVariant::Type type) : m_name(name), m_type(type), m_info(name) {}
		CProperty(const QString& name, QVariant::Type type, const QString& info) : m_name(name), m_type(type), m_info(info) {}
		CProperty(const QString& name, QVariant::Type type, const QString& info, const QStringList& val) : m_name(name), m_type(type), m_info(info), m_values(val) {}
	};

public:
	CPropertyList(){}
	virtual ~CPropertyList(){}

	int Properties() const { return (int) m_list.size(); }

	void AddProperty(const CProperty& p) { m_list.push_back(p); }
	void AddProperty(const QString& name, QVariant::Type type) { AddProperty(CProperty(name, type)); }
	void AddProperty(const QString& name, QVariant::Type type, const QString& info) { AddProperty(CProperty(name, type, info)); }
	void AddProperty(const QString& name, QVariant::Type type, const QString& info, const QStringList& vals) { AddProperty(CProperty(name, type, info, vals)); }

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
	void colorChanged(QColor c);
	void intChanged(const QString& s);
	void floatChanged(const QString& s);

private:
	CPropertyList*	m_list;
	QTableWidget*	m_prop;
	QLabel*			m_info;

	QWidget*	m_sel;
	int			m_selRow;
};
