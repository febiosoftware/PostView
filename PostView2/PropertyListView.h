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
		QColorDialog dlg(this);
		QColor col = dlg.getColor(m_col);
		if (col.isValid())
		{
			m_col = col;
			repaint();
			emit colorChanged(m_col);
		}
	}

	void setColor(const QColor& c) { m_col = c; }

	QColor color() { return m_col; }

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
		bool			m_bedit;	// editable flag

	public:
		CProperty(const QString& name, QVariant::Type type) : m_name(name), m_type(type), m_info(name) { m_bedit = true; }
		CProperty(const QString& name, QVariant::Type type, const QString& info) : m_name(name), m_type(type), m_info(info) { m_bedit = true; }
		CProperty(const QString& name, QVariant::Type type, const QString& info, const QStringList& val) : m_name(name), m_type(type), m_info(info), m_values(val) { m_bedit = true; }
	};

public:
	CPropertyList(){}
	virtual ~CPropertyList(){}

	int Properties() const { return (int) m_list.size(); }

	CProperty* AddProperty(const CProperty& p) { m_list.push_back(p); return &m_list[m_list.size()-1]; }
	CProperty* AddProperty(const QString& name, QVariant::Type type) { AddProperty(CProperty(name, type)); return &m_list[m_list.size()-1]; }
	CProperty* AddProperty(const QString& name, QVariant::Type type, const QString& info) { AddProperty(CProperty(name, type, info)); return &m_list[m_list.size()-1]; }
	CProperty* AddProperty(const QString& name, QVariant::Type type, const QString& info, const QStringList& vals) { AddProperty(CProperty(name, type, info, vals)); return &m_list[m_list.size()-1]; }

	const CProperty& Property(int i) { return m_list[i]; }

	virtual QVariant GetPropertyValue(int i) = 0;
	virtual void SetPropertyValue(int i, const QVariant& v) = 0;

private:
	std::vector<CProperty>	m_list;
};

//-----------------------------------------------------------------------------

namespace Ui {
	class CPropertyListView;
};

class CPropertyListView : public QWidget
{
	Q_OBJECT

public:
	CPropertyListView(QWidget* parent = 0);

	void Update(CPropertyList* plist);

private slots:
	void on_modelProps_clicked(const QModelIndex& index);
	void onDataChanged();

private:
	Ui::CPropertyListView*	ui;
};
