#pragma once
#include "TimePanel.h"
#include "TimeController.h"
#include <QBoxLayout>
#include <QLabel>

class Ui::CTimePanel
{
public:
	CTimeController*	timer;

public:
	void setupUi(QWidget* parent)
	{
		QHBoxLayout* mainLayout = new QHBoxLayout;
		mainLayout->setMargin(0);
		mainLayout->setSpacing(0);
//		QLabel* tmp = new QLabel("Hello");
//		tmp->setFixedWidth(100);
//		mainLayout->addWidget(tmp);
		mainLayout->addWidget(timer = new CTimeController); timer->setObjectName("timer");
		parent->setLayout(mainLayout);

		QMetaObject::connectSlotsByName(parent);
	}
};
