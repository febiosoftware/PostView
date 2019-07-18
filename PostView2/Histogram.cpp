#include "stdafx.h"
#include "Histogram.h"
#include <PostViewLib/ImageModel.h>
#include <PostViewLib/3DImage.h>
#include <QtCharts/QLineSeries>
using namespace Post;

CHistogramViewer::CHistogramViewer(QWidget* parent) : QChartView(parent)
{
	QLineSeries* series = new QLineSeries();
	for (int i=0; i<256; ++i)
		series->append(i, 0);

	QChart* chart = new QChart;
	chart->addSeries(series);
	chart->createDefaultAxes();
	chart->legend()->hide();

	setChart(chart);
	setRenderHint(QPainter::Antialiasing);
}

void CHistogramViewer::SetImageModel(CImageModel* img)
{
	if (img == nullptr) return;

	C3DImage* im = img->Get3DImage();

	vector<double> h(256, 0.0);
	for (int k = 0; k < im->Depth(); ++k)
	{
		for (int j = 0; j < im->Height(); ++j)
			for (int i = 0; i < im->Width(); ++i)
			{
				int n = im->value(i, j, k);
				h[n]++;
			}
	}

	double N = im->Depth()*im->Width()*im->Height();

	QLineSeries* series = new QLineSeries();
	for (int i = 0; i<256; ++i)
		series->append(i, h[i] / N);

	QChart* chart = new QChart;
	chart->addSeries(series);
	chart->createDefaultAxes();
	chart->legend()->hide();

	setChart(chart);
}
