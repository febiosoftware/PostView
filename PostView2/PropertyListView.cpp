#include "PropertyListView.h"
#include <QBoxLayout>
#include <QTableView>
#include <QPushButton>
#include <QHeaderView>
#include <QLabel>
#include <QComboBox>
#include <QApplication>
#include <QLineEdit>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QtCore/QAbstractTableModel>
#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
class CIntInput : public QLineEdit
{
public:
	CIntInput(QWidget* parent = 0) : QLineEdit(parent)
	{
		setValidator(new QIntValidator);
	}

	void setValue(int m) { setText(QString("%1").arg(m)); }
	int value() const { return text().toInt(); }
};

//-----------------------------------------------------------------------------
class CFloatInput : public QLineEdit
{
public:
	CFloatInput(QWidget* parent = 0) : QLineEdit(parent)
	{
		QDoubleValidator* pv = new QDoubleValidator;
		pv->setRange(-1e99, 1e99, 3);
		setValidator(pv);
	}

	void setValue(double v) { setText(QString("%1").arg(v)); }
	double value() const { return text().toDouble(); }
};

//-----------------------------------------------------------------------------
class CPropertyListModel : public QAbstractTableModel
{
public:
	CPropertyListModel(QWidget* parent) : QAbstractTableModel(parent) { m_list = 0; }

	void setPropertyList(CPropertyList* plist)
	{
		beginResetModel();
		m_list = plist;
		endResetModel();
	}

	int rowCount(const QModelIndex& parent) const 
	{
		if (m_list) return m_list->Properties();
		return 0;
	}

	int columnCount(const QModelIndex& parent) const { return 2; }

	QVariant headerData(int section, Qt::Orientation orient, int role) const
	{
		if ((orient == Qt::Horizontal)&&(role == Qt::DisplayRole))
		{
			switch (section)
			{
			case 0: return QString("Property"); break;
			case 1: return QString("Value"); break;
			}
		}
		return QAbstractTableModel::headerData(section, orient, role);
	}

	QVariant data(const QModelIndex& index, int role) const
	{
		if ((m_list==0)||(!index.isValid())) return QVariant();

		const CPropertyList::CProperty& prop = m_list->Property(index.row());

		if (role == Qt::ToolTipRole)
		{
			QString tip = (tr("<p><b>%1</b></p><p>%2</p>").arg(prop.m_name).arg(prop.m_info));
			return tip;
		}
		if (index.column() == 0)
		{
			if ((role == Qt::DisplayRole)||(role==Qt::EditRole)) return prop.m_name;
		}
		else if (index.column() == 1)
		{
			QVariant v = m_list->GetPropertyValue(index.row());
			if (role == Qt::EditRole) return v;
			if (v.type() == QVariant::Color)
			{
				if (role == Qt::BackgroundRole) return v;
			}
			else if (role == Qt::DisplayRole)
			{
				if (v.type() == QVariant::Bool)
				{
					bool b = v.value<bool>();
					v = (b ? "Yes" : "No");
					return v;
				}
				else if ((v.type() == QVariant::Int)&&(prop.m_values.isEmpty()==false))
				{
					int n = v.toInt();
					return prop.m_values.at(n);
				}
				return v;
			}
		}

		return QVariant();
	}

	Qt::ItemFlags flags(const QModelIndex& index) const
	{
		if (!index.isValid()) return 0;
		if (index.column() == 1)
			return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
		else return QAbstractTableModel::flags(index);
	}

	bool setData(const QModelIndex& index, const QVariant& value, int role)
	{
		if (index.isValid() && (role == Qt::EditRole))
		{
			const CPropertyList::CProperty& prop = m_list->Property(index.row());
			if (prop.m_type == QVariant::Bool)
			{
				int n = value.toInt();
				bool b = (n==0? false : true);
				m_list->SetPropertyValue(index.row(), b);
			}
			else m_list->SetPropertyValue(index.row(), value);
			return true;
		}
		return false;
	}
	
private:
	CPropertyList*	m_list;
};

class CPropertyListDelegate : public QStyledItemDelegate
{
private:
	CPropertyListView*	m_view;

public:
	explicit CPropertyListDelegate(CPropertyListView* view) : QStyledItemDelegate(view), m_view(view) {}

	void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const
	{
		if (index.column() == 0)
		{
			QStyleOptionViewItem opt = option;
			initStyleOption(&opt, index);

			opt.font.setBold(true);
	        QStyledItemDelegate::paint(painter, opt, index);
		}

		if (index.column() == 1)
		{
	        QStyledItemDelegate::paint(painter, option, index);

			// Fill the background before calling the base class paint
			// otherwise selected cells would have a white background
			QVariant background = index.data(Qt::BackgroundRole);
			if (background.canConvert<QBrush>())
				painter->fillRect(option.rect, background.value<QBrush>());

		    // To draw a border on selected cells
	        if(option.state & QStyle::State_Selected)
			{
				painter->save();
				QPen pen(Qt::black, 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
				int w = pen.width()/2;
				painter->setPen(pen);
				painter->drawRect(option.rect.adjusted(w,w,-w,-w));
				painter->restore();
			}
		}
    }

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		const QAbstractItemModel* model = index.model();
		if ((model == 0)||(index.column()==0)) return QStyledItemDelegate::createEditor(parent, option, index);

		QVariant data = index.data(Qt::EditRole);
		if (data.type() == QVariant::Bool)
		{
			QComboBox* box = new QComboBox(parent);
			box->addItem("No");
			box->addItem("Yes");
			bool b = data.value<bool>();
			box->setCurrentIndex(b ? 1 : 0);
			return box;
		}
		if (data.type() == QVariant::Color)
		{
			CColorButton* pc = new CColorButton(parent);
			pc->setColor(data.value<QColor>());
			return pc;
		}

		return QStyledItemDelegate::createEditor(parent, option, index);
	}

	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
	{
		if (!index.isValid()) return;

		QComboBox* box = qobject_cast<QComboBox*>(editor);
		if (box) { model->setData(index, box->currentIndex(), Qt::EditRole); return; }

		CColorButton* col = qobject_cast<CColorButton*>(editor);
		if (col) { model->setData(index, col->color(), Qt::EditRole); return; }

		QStyledItemDelegate::setModelData(editor, model, index);
	}
};

class Ui::CPropertyListView
{
public:
	CPropertyList*			m_list;
	QTableView*				m_prop;
	CPropertyListDelegate*	m_delegate;
	CPropertyListModel*		m_data;
	QLabel*					m_info;

public:
	void setupUi(::CPropertyListView* parent)
	{
		QVBoxLayout* playout = new QVBoxLayout(parent);
		playout->setMargin(0);

		m_prop = new QTableView;
		m_prop->setObjectName(QStringLiteral("modelProps"));
		m_prop->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_prop->setSelectionMode(QAbstractItemView::SingleSelection);
		m_prop->horizontalHeader()->setStretchLastSection(true);
//		m_prop->horizontalHeader()->hide();
		m_prop->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
		m_prop->verticalHeader()->setDefaultSectionSize(24);
		m_prop->verticalHeader()->hide();

		m_delegate = new CPropertyListDelegate(parent);
		m_prop->setItemDelegate(m_delegate);

		m_data = new CPropertyListModel(m_prop);
		m_prop->setModel(m_data);

		m_info = new QLabel;
		m_info->setFrameStyle(QFrame::Panel);
		m_info->setMinimumHeight(50);
		
		playout->addWidget(m_prop);
		playout->addWidget(m_info);

		QMetaObject::connectSlotsByName(parent);
	}
};

//-----------------------------------------------------------------------------
CPropertyListView::CPropertyListView(QWidget* parent) : QWidget(parent), ui(new Ui::CPropertyListView)
{
	ui->setupUi(this);
}

//-----------------------------------------------------------------------------
void CPropertyListView::Update(CPropertyList* plist)
{
	ui->m_list = plist;
	ui->m_data->setPropertyList(plist);
}

//-----------------------------------------------------------------------------
void CPropertyListView::on_modelProps_clicked(const QModelIndex& index)
{
	if (ui->m_list)
	{
		if (index.isValid())
		{
			const CPropertyList::CProperty& pi = ui->m_list->Property(index.row());
			ui->m_info->setText(tr("<p><b>%1</b></p><p>%2</p>").arg(pi.m_name).arg(pi.m_info));
			return;
		}
	}
	ui->m_info->clear();
}
