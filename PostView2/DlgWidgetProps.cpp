#include "stdafx.h"
#include "DlgWidgetProps.h"
#include <QTabWidget>
#include <QBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QValidator>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QFontComboBox>
#include <QSpinBox>
#include <QGroupBox>
#include "GLWidget.h"
#include "CIntInput.h"
#include "CColorButton.h"
#include "convert.h"

class CFontWidget : public QGroupBox
{
public:
	QFontComboBox*	pfontStyle;
	QSpinBox*		pfontSize;
	CColorButton*	pfontColor;
	QCheckBox		*pfontBold, *pfontItalic;

public:
	CFontWidget(QWidget* parent = 0) : QGroupBox("Font", parent)
	{
		QGridLayout* pgrid = new QGridLayout;
		pfontStyle = new QFontComboBox;
		QLabel* label = new QLabel("Font:"); label->setBuddy(pfontStyle);
		pgrid->addWidget(label, 0, 0); pgrid->addWidget(pfontStyle, 0, 1, 1, 3);
		pfontSize = new QSpinBox;
		label = new QLabel("Size:"); label->setBuddy(pfontSize);
		pgrid->addWidget(label, 1, 0); pgrid->addWidget(pfontSize, 1, 1);
		pfontColor = new CColorButton;
		label = new QLabel("Color:"); label->setBuddy(pfontColor);
		pgrid->addWidget(label, 0, 4, 1, 1, Qt::AlignRight); pgrid->addWidget(pfontColor, 0, 5);
		pgrid->addWidget(pfontBold   = new QCheckBox("bold"  ), 1, 2);
		pgrid->addWidget(pfontItalic = new QCheckBox("italic"), 1, 3);
		setLayout(pgrid);
	}
};

class CPositionWidget : public QWidget
{
public:
	CIntInput *px, *py;
	CIntInput *pw, *ph;

	void setPosition(int x, int y, int w, int h)
	{
		px->setValue(x);
		py->setValue(y);
		pw->setValue(w);
		ph->setValue(h);
	}

	int x() const { return px->value(); }
	int y() const { return py->value(); }
	int w() const { return pw->value(); }
	int h() const { return ph->value(); }

public:
	CPositionWidget(QWidget* parent = 0) : QWidget(parent)
	{
		QFormLayout* pform = new QFormLayout;
		pform->addRow("X:", px = new CIntInput);
		pform->addRow("Y:", py = new CIntInput);
		pform->addRow("Width:", pw = new CIntInput);
		pform->addRow("Height:", ph = new CIntInput);
		setLayout(pform);
	}
};

class Ui::CDlgBoxProps
{
public:
	CPositionWidget* ppos;
	QLineEdit* ptext;
	QCheckBox* pshadow;
	CColorButton*	pshadowCol;
	QComboBox* pbgstyle;
	CColorButton *bgCol1, *bgCol2;
	CFontWidget* pfont;

public:
	void setupUi(QDialog* parent)
	{
		// main layout
		QVBoxLayout* pv = new QVBoxLayout(parent);

		// create tab widget
		QTabWidget* tab = new QTabWidget;

		// text properties
		QWidget* textPage = new QWidget;
		QVBoxLayout* textPageLayout = new QVBoxLayout;

		QGroupBox* pg = new QGroupBox("Text");
		QVBoxLayout* pvg = new QVBoxLayout;
		QHBoxLayout* phb = new QHBoxLayout;
			QLabel* label = new QLabel("Text:");
			ptext = new QLineEdit;
			label->setBuddy(ptext);
			phb->addWidget(label);
			phb->addWidget(ptext);
		pvg->addLayout(phb);

		phb = new QHBoxLayout;
			pshadow = new QCheckBox("text shadow");
			pshadowCol = new CColorButton;
			phb->addWidget(pshadow);
			phb->addWidget(pshadowCol);
			phb->addStretch();
		pvg->addLayout(phb);
		pg->setLayout(pvg);
		textPageLayout->addWidget(pg);

		pfont = new CFontWidget;
		textPageLayout->addWidget(pfont);

		pg = new QGroupBox("Background");
		QFormLayout* pform = new QFormLayout;
		QStringList items; items << "None" << "Color 1" << "Color 2" << "Horizontal gradient" << "Vertical gradient";
		pbgstyle = new QComboBox;
		pbgstyle->addItems(items);

		pform->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
		pform->addRow("Background style:", pbgstyle);
		pform->addRow("Color1:", bgCol1 = new CColorButton);
		pform->addRow("Color2:", bgCol2 = new CColorButton);
		pg->setLayout(pform);
		textPageLayout->addWidget(pg);

		textPage->setLayout(textPageLayout);

		// position properties
		ppos = new CPositionWidget;

		// add all the tabs
		tab->addTab(textPage, "Text");
		tab->addTab(ppos, "Position");
		pv->addWidget(tab);

		// create the dialog button box
		QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		pv->addWidget(buttonBox);
		QObject::connect(buttonBox, SIGNAL(accepted()), parent, SLOT(accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), parent, SLOT(reject()));
	}
};

CDlgBoxProps::CDlgBoxProps(GLWidget* widget, QWidget* parent) : QDialog(parent), ui(new Ui::CDlgBoxProps)
{
	ui->setupUi(this);

	pw = widget;
	GLBox* pb = dynamic_cast<GLBox*>(pw);

	ui->ppos->setPosition(widget->x(), widget->y(), widget->w(), widget->h());

	ui->ptext->setText(widget->get_label());
	ui->pshadow->setChecked(pb->m_bshadow);
	ui->pshadowCol->setColor(toQColor(pb->m_shc));

	ui->pbgstyle->setCurrentIndex(pb->get_bg_style());
	ui->bgCol1->setColor(toQColor(pb->get_bg_color(0)));
	ui->bgCol2->setColor(toQColor(pb->get_bg_color(1)));

	ui->pfont->pfontStyle->setCurrentFont(QFont(pb->get_font()));
	ui->pfont->pfontSize->setValue(pb->get_font_size());
	ui->pfont->pfontBold->setChecked(pb->m_font_bold);
	ui->pfont->pfontItalic->setChecked(pb->m_font_italic);
	ui->pfont->pfontColor->setColor(toQColor(pb->get_fg_color()));
}

void CDlgBoxProps::accept()
{
	int x = ui->ppos->x();
	int y = ui->ppos->y();
	int w = ui->ppos->w();
	int h = ui->ppos->h();

	std::string text = ui->ptext->text().toStdString();

	pw->resize(x, y, w, h);
	pw->copy_label(text.c_str());

	GLBox* pb = dynamic_cast<GLBox*>(pw);
	pb->m_bshadow = ui->pshadow->isChecked();
	pb->m_shc = toGLColor(ui->pshadowCol->color());

	pb->set_bg_style(ui->pbgstyle->currentIndex());
	pb->set_bg_color(toGLColor(ui->bgCol1->color()), toGLColor(ui->bgCol2->color()));

	pb->set_font(ui->pfont->pfontStyle->currentFont().family());
	pb->set_font_size(ui->pfont->pfontSize->value());
	pb->m_font_bold = ui->pfont->pfontBold->isChecked();
	pb->m_font_italic = ui->pfont->pfontItalic->isChecked();
	pb->set_fg_color(toGLColor(ui->pfont->pfontColor->color()));

	QDialog::accept();
}

class Ui::CDlgLegendProps
{
public:
	// Labels page
	QCheckBox* pshowLabels;
	QSpinBox* pprec;
	CFontWidget* plabelFont;

	// title page
	QCheckBox* pshowTitle;
	QLineEdit* ptitleText;
	CFontWidget* ptitleFont;

	// Position page
	CPositionWidget* ppos;

public:
	void setupUi(QDialog* parent)
	{
		// main layout
		QVBoxLayout* pv = new QVBoxLayout(parent);

		// create tab widget
		QTabWidget* tab = new QTabWidget;

		// Labels page
		QWidget* labelsPage = new QWidget;
			QVBoxLayout* labelsPageLayout = new QVBoxLayout;
				QHBoxLayout* ph = new QHBoxLayout;
					ph->addWidget(pshowLabels = new QCheckBox("show labels"));
					QLabel* plabel = new QLabel("precision");
					ph->addWidget(plabel);
					ph->addWidget(pprec = new QSpinBox); plabel->setBuddy(pprec); pprec->setRange(1, 7);
					ph->addStretch();
				labelsPageLayout->addLayout(ph);
				plabelFont = new CFontWidget;
				labelsPageLayout->addWidget(plabelFont);
			labelsPage->setLayout(labelsPageLayout);

		// Titles page
		QWidget* ptitlePage = new QWidget;
			QVBoxLayout* titlePageLayout = new QVBoxLayout;
				titlePageLayout->addWidget(pshowTitle = new QCheckBox("show title"));
				ph = new QHBoxLayout;
					ph->addWidget(plabel = new QLabel("Title:"));
					ph->addWidget(ptitleText = new QLineEdit); plabel->setBuddy(ptitleText);
				titlePageLayout->addLayout(ph);
				titlePageLayout->addWidget(ptitleFont = new CFontWidget);
			ptitlePage->setLayout(titlePageLayout);

		// Position page
		ppos = new CPositionWidget;

		// add all the tabs
		tab->addTab(labelsPage, "Labels");
		tab->addTab(ptitlePage, "Title");
		tab->addTab(ppos, "Position");
		pv->addWidget(tab);

		// create the dialog button box
		QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		pv->addWidget(buttonBox);
		QObject::connect(buttonBox, SIGNAL(accepted()), parent, SLOT(accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), parent, SLOT(reject()));
	}
};

CDlgLegendProps::CDlgLegendProps(GLWidget* widget, QWidget* parent) : QDialog(parent), ui(new Ui::CDlgLegendProps)
{
	ui->setupUi(this);

	pw = widget;
	GLLegendBar* pb = dynamic_cast<GLLegendBar*>(pw);

	ui->pshowLabels->setChecked(pb->ShowLabels());
	ui->pprec->setValue(pb->GetPrecision());
	ui->plabelFont->pfontStyle->setCurrentFont(pb->get_font());
	ui->plabelFont->pfontSize->setValue(pb->get_font_size());
	ui->plabelFont->pfontColor->setColor(toQColor(pb->get_fg_color()));
	ui->plabelFont->pfontBold->setChecked(pb->m_font_bold);
	ui->plabelFont->pfontItalic->setChecked(pb->m_font_italic);

	ui->pshowTitle->setChecked(pb->ShowTitle());
	ui->ptitleText->setText(pb->get_label());
	ui->ptitleFont->pfontStyle->setCurrentFont(pb->get_font());
	ui->ptitleFont->pfontSize->setValue(pb->get_font_size());
	ui->ptitleFont->pfontColor->setColor(toQColor(pb->get_fg_color()));
	ui->ptitleFont->pfontBold->setChecked(pb->m_font_bold);
	ui->ptitleFont->pfontItalic->setChecked(pb->m_font_italic);

	ui->ppos->setPosition(widget->x(), widget->y(), widget->w(), widget->h());
}

void CDlgLegendProps::accept()
{
	int x = ui->ppos->x();
	int y = ui->ppos->y();
	int w = ui->ppos->w();
	int h = ui->ppos->h();
	pw->resize(x, y, w, h);

	std::string text = ui->ptitleText->text().toStdString();
	pw->copy_label(text.c_str());

	GLLegendBar* pb = dynamic_cast<GLLegendBar*>(pw);
	pb->ShowLabels(ui->pshowLabels->isChecked());
	pb->SetPrecision(ui->pprec->value());
	pb->set_font(ui->plabelFont->pfontStyle->currentFont().family());
	pb->set_font_size(ui->plabelFont->pfontSize->value());
	pb->m_font_bold = ui->plabelFont->pfontBold->isChecked();
	pb->m_font_italic = ui->plabelFont->pfontItalic->isChecked();
	pb->set_fg_color(toGLColor(ui->plabelFont->pfontColor->color()));

	QDialog::accept();
}
