/*********************************************************************
MLDemos: A User-Friendly visualization toolkit for machine learning
Copyright (C) 2010  Basilio Noris
Contact: mldemos@b4silio.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License,
version 3 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*********************************************************************/
#include "interfaceKNNRegress.h"
#include <QPixmap>
#include <QBitmap>
#include <QPainter>
#include <datasetManager.h>
#include <canvas.h>

using namespace std;

RegrKNN::RegrKNN()
{
	params = new Ui::ParametersKNNRegress();
	params->setupUi(widget = new QWidget());
    connect(params->knnNormCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeOptions()));
    ChangeOptions();
}

RegrKNN::~RegrKNN()
{
    delete params;
}

void RegrKNN::ChangeOptions()
{
    params->knnNormSpin->setVisible(params->knnNormCombo->currentIndex() == 2);
    params->labelPower->setVisible(params->knnNormCombo->currentIndex() == 2);
}

void RegrKNN::SetParams(Regressor *regressor)
{
	if(!regressor) return;
	int k = params->knnKspin->value();
	int metricType = params->knnNormCombo->currentIndex();
	int metricP = params->knnNormSpin->value();

	((RegressorKNN *)regressor)->SetParams(k, metricType, metricP);
}

fvec RegrKNN::GetParams()
{
    fvec par(3);
    par[0] = params->knnKspin->value();
    par[1] = params->knnNormCombo->currentIndex();
    par[2] = params->knnNormSpin->value();
    return par;
}

void RegrKNN::SetParams(Regressor *regressor, fvec parameters)
{
    if(!regressor) return;
    int k = parameters.size() > 0 ? parameters[0] : 1;
    int metricType = parameters.size() > 1 ? parameters[1] : 0;
    int metricP = parameters.size() > 2 ? parameters[2] : 0;
    ((RegressorKNN *)regressor)->SetParams(k, metricType, metricP);
}

void RegrKNN::GetParameterList(std::vector<QString> &parameterNames,
                             std::vector<QString> &parameterTypes,
                             std::vector< std::vector<QString> > &parameterValues)
{
    parameterNames.push_back("K");
    parameterNames.push_back("Metric Type");
    parameterNames.push_back("Metric Power");
    parameterTypes.push_back("Integer");
    parameterTypes.push_back("List");
    parameterTypes.push_back("Integer");
    parameterValues.push_back(vector<QString>());
    parameterValues.back().push_back("1");
    parameterValues.back().push_back("999");
    parameterValues.push_back(vector<QString>());
    parameterValues.back().push_back("L1");
    parameterValues.back().push_back("L2");
    parameterValues.back().push_back("Lp");
    parameterValues.back().push_back("L-Inf");
    parameterValues.push_back(vector<QString>());
    parameterValues.back().push_back("1");
    parameterValues.back().push_back("150");
}

QString RegrKNN::GetAlgoString()
{
	int k = params->knnKspin->value();
	int metricType = params->knnNormCombo->currentIndex();
	int metricP = params->knnNormSpin->value();
	QString algo = QString("KNN %1 %2").arg(k).arg(metricType==3? 0 : metricType == 2 ? metricP : metricType+1);
	return algo;
}

Regressor *RegrKNN::GetRegressor()
{
	RegressorKNN *regressor = new RegressorKNN();
	SetParams(regressor);
	return regressor;
}

void RegrKNN::DrawConfidence(Canvas *canvas, Regressor *regressor)
{
	canvas->maps.confidence = QPixmap();
}

void RegrKNN::DrawModel(Canvas *canvas, QPainter &painter, Regressor *regressor)
{
	if(!regressor || !canvas) return;
	int w = canvas->width();
	int h = canvas->height();
	painter.setRenderHint(QPainter::Antialiasing, true);

    fvec sample = canvas->toSampleCoords(0,0);
    int dim = sample.size();
    if(dim > 2) return;
    int steps = w;
	QPointF oldPoint(-FLT_MAX,-FLT_MAX);
	QPointF oldPointUp(-FLT_MAX,-FLT_MAX);
	QPointF oldPointDown(-FLT_MAX,-FLT_MAX);
	FOR(x, steps)
	{
		sample = canvas->toSampleCoords(x,0);
		fvec res = regressor->Test(sample);
		if(res[0] != res[0] || res[1] != res[1]) continue;
		QPointF point = canvas->toCanvasCoords(sample[0], res[0]);
		QPointF pointUp = canvas->toCanvasCoords(sample[0],res[0] + res[1]);
		pointUp.setX(0);
		pointUp.setY(pointUp.y() - point.y());
		QPointF pointDown = -pointUp;
		if(x)
		{
			painter.setPen(QPen(Qt::black, 1));
			painter.drawLine(point, oldPoint);
			painter.setPen(QPen(Qt::black, 0.5));
			painter.drawLine(pointUp, oldPointUp);
			painter.drawLine(pointDown, oldPointDown);
		}
		oldPoint = point;
		oldPointUp = pointUp;
		oldPointDown = pointDown;
	}
}

void RegrKNN::SaveOptions(QSettings &settings)
{
	settings.setValue("knnK", params->knnKspin->value());
	settings.setValue("knnNorm", params->knnNormCombo->currentIndex());
	settings.setValue("knnPower", params->knnNormSpin->value());
}

bool RegrKNN::LoadOptions(QSettings &settings)
{
	if(settings.contains("knnK")) params->knnKspin->setValue(settings.value("knnK").toFloat());
	if(settings.contains("knnNorm")) params->knnNormCombo->setCurrentIndex(settings.value("knnNorm").toInt());
	if(settings.contains("knnPower")) params->knnNormSpin->setValue(settings.value("knnPower").toFloat());
	return true;
}

void RegrKNN::SaveParams(QTextStream &file)
{
	file << "regressionOptions" << ":" << "knnK" << " " << params->knnKspin->value() << "\n";
	file << "regressionOptions" << ":" << "knnNorm" << " " << params->knnNormCombo->currentIndex() << "\n";
	file << "regressionOptions" << ":" << "knnPower" << " " << params->knnNormSpin->value() << "\n";
}

bool RegrKNN::LoadParams(QString name, float value)
{
	if(name.endsWith("knnK")) params->knnKspin->setValue((int)value);
	if(name.endsWith("knnNorm")) params->knnNormCombo->setCurrentIndex((int)value);
	if(name.endsWith("knnPower")) params->knnNormSpin->setValue((int)value);
	return true;
}
