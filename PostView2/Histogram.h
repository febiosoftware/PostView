#pragma once
#include <QtCharts/QChartView>
using namespace QtCharts;

namespace Post {
	class CImageModel;
}

class CHistogramViewer : public QChartView
{
public:
	CHistogramViewer(QWidget* parent = nullptr);

	void SetImageModel(Post::CImageModel* img);
};
