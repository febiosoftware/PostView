#pragma once
#include <QtCharts/QChartView>
using namespace QtCharts;

class CImageModel;

class CHistogramViewer : public QChartView
{
public:
	CHistogramViewer(QWidget* parent = nullptr);

	void SetImageModel(CImageModel* img);
};
