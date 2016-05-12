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

class Ui::CDlgBoxProps
{
public:
	CIntInput *px, *py;
	CIntInput *pw, *ph;
	QLineEdit* ptext;
	QCheckBox* pshadow;
	CColorButton*	pshadowCol;
	QComboBox* pbgstyle;
	CColorButton *bgCol1, *bgCol2;
	QFontComboBox* pfont;
	QSpinBox* pfontSize;
	CColorButton* pfontColor;
	QCheckBox *pfontBold, *pfontItalic;

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

		pg = new QGroupBox("Font");
		QGridLayout* pgrid = new QGridLayout;
		pfont = new QFontComboBox;
		label = new QLabel("Font:"); label->setBuddy(pfont);
		pgrid->addWidget(label, 0, 0); pgrid->addWidget(pfont, 0, 1, 1, 3);
		pfontSize = new QSpinBox;
		label = new QLabel("Size:"); label->setBuddy(pfontSize);
		pgrid->addWidget(label, 1, 0); pgrid->addWidget(pfontSize, 1, 1);
		pfontColor = new CColorButton;
		label = new QLabel("Color:"); label->setBuddy(pfontColor);
		pgrid->addWidget(label, 0, 4, 1, 1, Qt::AlignRight); pgrid->addWidget(pfontColor, 0, 5);
		pgrid->addWidget(pfontBold   = new QCheckBox("bold"  ), 1, 2);
		pgrid->addWidget(pfontItalic = new QCheckBox("italic"), 1, 3);
		pg->setLayout(pgrid);
		textPageLayout->addWidget(pg);

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
		QWidget* posPage = new QWidget;
		pform = new QFormLayout;
		pform->addRow("X:", px = new CIntInput);
		pform->addRow("Y:", py = new CIntInput);
		pform->addRow("Width:", pw = new CIntInput);
		pform->addRow("Height:", ph = new CIntInput);
		posPage->setLayout(pform);

		// add all the tabs
		tab->addTab(textPage, "Text");
		tab->addTab(posPage , "Position");
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

	ui->px->setValue(widget->x());
	ui->py->setValue(widget->y());
	ui->pw->setValue(widget->w());
	ui->ph->setValue(widget->h());

	ui->ptext->setText(widget->get_label());
	ui->pshadow->setChecked(pb->m_bshadow);
	ui->pshadowCol->setColor(toQColor(pb->m_shc));

	ui->pbgstyle->setCurrentIndex(pb->get_bg_style());
	ui->bgCol1->setColor(toQColor(pb->get_bg_color(0)));
	ui->bgCol2->setColor(toQColor(pb->get_bg_color(1)));

	ui->pfont->setCurrentFont(QFont(pb->get_font()));
	ui->pfontSize->setValue(pb->get_font_size());
	ui->pfontBold->setChecked(pb->m_font_bold);
	ui->pfontItalic->setChecked(pb->m_font_italic);
	ui->pfontColor->setColor(toQColor(pb->get_fg_color()));
}

void CDlgBoxProps::accept()
{
	int x = ui->px->value();
	int y = ui->py->value();
	int w = ui->pw->value();
	int h = ui->ph->value();

	std::string text = ui->ptext->text().toStdString();

	pw->resize(x, y, w, h);
	pw->copy_label(text.c_str());

	GLBox* pb = dynamic_cast<GLBox*>(pw);
	pb->m_bshadow = ui->pshadow->isChecked();
	pb->m_shc = toGLColor(ui->pshadowCol->color());

	pb->set_bg_style(ui->pbgstyle->currentIndex());
	pb->set_bg_color(toGLColor(ui->bgCol1->color()), toGLColor(ui->bgCol2->color()));

	pb->set_font(ui->pfont->currentFont().family());
	pb->set_font_size(ui->pfontSize->value());
	pb->m_font_bold = ui->pfontBold->isChecked();
	pb->m_font_italic = ui->pfontItalic->isChecked();
	pb->set_fg_color(toGLColor(ui->pfontColor->color()));

	QDialog::accept();
}
