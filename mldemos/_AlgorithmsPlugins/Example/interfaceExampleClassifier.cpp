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
#include "interfaceExampleClassifier.h"
#include "drawUtils.h"
#include <basicMath.h>
#include <QPixmap>
#include <QBitmap>
#include <QPainter>
#include <QDebug>

using namespace std;

ClassExample::ClassExample()
{
    // we initialize the hyperparameter widget
    params = new Ui::ParametersExample();
	params->setupUi(widget = new QWidget());
}

void ClassExample::SetParams(Classifier *classifier)
{
	if(!classifier) return;
    // the dynamic cast ensures that the pointer we received is really a classifierExample
    ClassifierExample * myExample = dynamic_cast<ClassifierExample *>(classifier);
    // if it isnt, we return
    if(!myExample) return;

    // here we gather the different hyperparameters from the interface
    double param1 = params->param1Spin->value();
    int param2 = params->param2Combo->currentIndex();
    bool param3 = params->param3Check->isChecked();

    // and finally we set the parameters of the algorithm
    myExample->SetParams(param1, param2, param3);
}

QString ClassExample::GetAlgoString()
{
    // here we gather the different hyperparameters from the interface
    double param1 = params->param1Spin->value();
    int param2 = params->param2Combo->currentIndex();
    bool param3 = params->param3Check->isChecked();

    // and we generate the algorithm string with something that is understandable
    QString algo = QString("MyExample %1").arg(param1);
    switch(param2)
	{
	case 0:
        algo += " Low";
		break;
	case 1:
        algo += " Med";
		break;
	case 2:
        algo += " Hig";
		break;
	}

    if(param3) algo += " Opt";
	return algo;
}

Classifier *ClassExample::GetClassifier()
{
    // we instanciate the algorithm object
    ClassifierExample *classifier = new ClassifierExample();
    // we set its parameters
	SetParams(classifier);
    // we return it to the main program
	return classifier;
}

void ClassExample::DrawInfo(Canvas *canvas, QPainter &painter, Classifier *classifier)
{
	if(!canvas || !classifier) return;
	painter.setRenderHint(QPainter::Antialiasing);

    ClassifierExample * myExample = dynamic_cast<ClassifierExample*>(classifier);
    if(!myExample) return;

    // to give an example, we use the QPainter interface to paint a circle close to the center of the data space


    // first we need to know which 2 dimensions are currently being displayed (in case of multi-dimensional data)
    // if the data is 2-dimensional it will be 0 and 1
    int xIndex = canvas->xIndex;
	int yIndex = canvas->yIndex;

    // now we get the current position of the center of the dataspace
    fvec sample = canvas->center;

    // and we add a random noise around it
    sample[xIndex] += (drand48()-0.5f)*0.1;
    sample[yIndex] += (drand48()-0.5f)*0.1;

    // we need to convert the sample coordinates from dataspace (N-dimensional in R) to the canvas coordinates (2D pixel by pixel)
    QPointF pointInCanvas = canvas->toCanvasCoords(sample);

    // we make the painter paint nicely (work well with forms, not so much with text)
    painter.setRenderHint(QPainter::Antialiasing);

    // we set the brush and pen, in our case no brush (hollow circle) and a thick red edge
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(Qt::red, 4));

    // and we finally draw it with a radius of 10
    painter.drawEllipse(pointInCanvas, 10, 10);

}

void ClassExample::DrawModel(Canvas *canvas, QPainter &painter, Classifier *classifier)
{
	// we want to draw the samples

    // we start by making the painter paint things nicely
    painter.setRenderHint(QPainter::Antialiasing);

    // for every point in the current dataset
    FOR(i, canvas->data->GetCount())
	{
        // we get the sample
		fvec sample = canvas->data->GetSample(i);
        // and its label
		int label = canvas->data->GetLabel(i);

        // and we test it using the classifier (TestMulti is for multi-class classification)
		fvec res = classifier->TestMulti(sample);

        // we get the point in canvas coordinates (2D pixel by pixel) corresponding to the sample (N-dimensional in R)
        QPointF point = canvas->toCanvasCoords(canvas->data->GetSample(i));

        // if we only have one response it is a binary classification
        if(res.size()==1)
		{
            // if the response is positive
            if(res[0] > 0)
			{
                // if the sample is also positive then we draw it with the corresponding color
                if(classifier->classMap[label] == 1) Canvas::drawSample(painter, point, 9, 1);
                // else we draw a cross
				else Canvas::drawCross(painter, point, 6, 2);
			}
            // vice-versa here if the response is negative
            else
			{
                if(classifier->classMap[label] != 1) Canvas::drawSample(painter, point, 9, 0);
				else Canvas::drawCross(painter, point, 6, 0);
			}
		}
        // if we have multiple responses
        else
		{
            // we look for the class with the maximum response
			int max = 0;
			for(int i=1; i<res.size(); i++) if(res[max] < res[i]) max = i;
            int resp = classifier->inverseMap[max];
            // if it corresponds to the actual label we draw it as a circle
			if(label == resp) Canvas::drawSample(painter, point, 9, label);
            // otherwise we draw a cross
			else Canvas::drawCross(painter, point, 6, label);
		}
	}
}

void ClassExample::SaveOptions(QSettings &settings)
{
    // we save to the system registry each parameter value
    settings.setValue("Param1", params->param1Spin->value());
    settings.setValue("Param2", params->param2Combo->currentIndex());
    settings.setValue("Param3", params->param3Check->isChecked());
}

bool ClassExample::LoadOptions(QSettings &settings)
{
    // we load the parameters from the registry so that when we launch the program we keep all values
    if(settings.contains("Param1")) params->param1Spin->setValue(settings.value("Param1").toFloat());
    if(settings.contains("Param2")) params->param2Combo->setCurrentIndex(settings.value("Param2").toInt());
    if(settings.contains("Param3")) params->param3Check->setChecked(settings.value("Param3").toBool());
	return true;
}

void ClassExample::SaveParams(QTextStream &file)
{
    // same as above but for files/string saving
    file << "classificationOptions" << ":" << "Param1" << " " << params->param1Spin->value() << "\n";
    file << "classificationOptions" << ":" << "Param2" << " " << params->param2Combo->currentIndex() << "\n";
    file << "classificationOptions" << ":" << "Param3" << " " << params->param3Check->isChecked() << "\n";
}

bool ClassExample::LoadParams(QString name, float value)
{
    // same as above but for files/string saving
    if(name.endsWith("Param1")) params->param1Spin->setValue((int)value);
    if(name.endsWith("Param2")) params->param2Combo->setCurrentIndex((int)value);
    if(name.endsWith("Param3")) params->param3Check->setChecked((int)value);
	return true;
}
