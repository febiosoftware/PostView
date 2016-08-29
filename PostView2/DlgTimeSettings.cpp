#include "stdafx.h"
#include "DlgTimeSettings.h"
#include <QFormLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include "Document.h"

class Ui::CDlgTimeSettings
{
public:
	QComboBox*			mode;
	QDoubleSpinBox*		fps;
	QSpinBox*			start;
	QSpinBox*			end;	
	QCheckBox*			loop;
	QCheckBox*			fix;
	QDoubleSpinBox*		step;

public:
	void setupUi(QDialog* parent)
	{
		QVBoxLayout* mainLayout = new QVBoxLayout;
		QFormLayout* pform = new QFormLayout;
		pform->addRow("Mode:", mode = new QComboBox);
		pform->addRow("FPS:" , fps = new QDoubleSpinBox);
		pform->addRow("Start:", start = new QSpinBox);
		pform->addRow("End:", end = new QSpinBox);
		mainLayout->addLayout(pform);

		mode->addItem("Forward");
		mode->addItem("Reverse");
		mode->addItem("Cycle");

		fps->setRange(1.0, 100.0);
		fps->setSingleStep(0.5);
		fps->setDecimals(1);

		mainLayout->addWidget(loop = new QCheckBox("loop"));
		mainLayout->addWidget(fix  = new QCheckBox("fixed time step"));

		pform = new QFormLayout;
		pform->addRow("Time step size:", step = new QDoubleSpinBox);
		mainLayout->addLayout(pform);

		step->setRange(0.01, 100.0);
		step->setSingleStep(0.01);

		QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		mainLayout->addWidget(buttonBox);

		parent->setLayout(mainLayout);

		QObject::connect(buttonBox, SIGNAL(accepted()), parent, SLOT(accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), parent, SLOT(reject()));
	}
};

CDlgTimeSettings::CDlgTimeSettings(CDocument* doc, QWidget* parent) : QDialog(parent), ui(new Ui::CDlgTimeSettings), m_doc(doc)
{
	ui->setupUi(this);

	TIMESETTINGS& time = doc->GetTimeSettings();
	int N = doc->GetTimeSteps();
	ui->mode->setCurrentIndex(time.m_mode);
	ui->fps->setValue(time.m_fps);
	ui->start->setRange(1, N);
	ui->start->setValue(time.m_start + 1);
	ui->end->setRange(1, N);
	ui->end->setValue(time.m_end + 1);
	ui->loop->setChecked(time.m_bloop);
	ui->fix->setChecked(time.m_bfix);
	ui->step->setValue(time.m_dt);
}

void CDlgTimeSettings::accept()
{
	TIMESETTINGS& time = m_doc->GetTimeSettings();
	int N = m_doc->GetTimeSteps();

	time.m_mode = ui->mode->currentIndex();
	time.m_fps  = ui->fps->value();
	time.m_start = ui->start->value() - 1;
	time.m_end   = ui->end->value() - 1;
	time.m_bloop = ui->loop->isChecked();
	time.m_bfix  = ui->fix->isChecked();
	time.m_dt    = ui->step->value();

	if ((time.m_start < 0) || (time.m_end >= N) || (time.m_start > time.m_end))
	{
		QMessageBox::critical(this, "Time Settings", "Invalid time range");
		return;
	}

	QDialog::accept();
}
