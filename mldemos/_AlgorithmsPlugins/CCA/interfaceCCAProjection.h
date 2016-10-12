#ifndef INTERFACECCAPROJECTION_H
#define INTERFACECCAPROJECTION_H
#include <vector>
#include <interfaces.h>
#include "ui_paramsCCA.h"
#include <QTableWidget>

class CCAProjection : public QObject, public ProjectorInterface
{
    Q_OBJECT
    Q_INTERFACES(ProjectorInterface)
private:
    Ui::paramsCCA *params;
    QWidget *widget;
    QTableWidget* table;
    QTableWidget* table_root;
    QWidget *table1, *table2;
public:
    CCAProjection();
    ~CCAProjection();
    // virtual functions to manage the algorithm creation
    Projector *GetProjector();
    void DrawInfo(Canvas *canvas, QPainter &painter, Projector *projector);
    void DrawModel(Canvas *canvas, QPainter &painter, Projector *projector);
    void DrawGL(Canvas *canvas, GLWidget *glw, Projector *projector){}

    // virtual functions to manage the GUI and I/O
    QString GetName(){return QString("CCA");}
    QString GetAlgoString();
    QString GetInfoFile(){return "CCA.html";}
    QWidget *GetParameterWidget(){return widget;}
    void SetParams(Projector *projector);
    void SaveOptions(QSettings &settings);
    bool LoadOptions(QSettings &settings);
    void SaveParams(QTextStream &stream);
    bool LoadParams(QString name, float value);
    void SetParams(Projector *projector, fvec parameters);
    fvec GetParams();
    void GetParameterList(std::vector<QString> &parameterNames,
                                 std::vector<QString> &parameterTypes,
                                 std::vector< std::vector<QString> > &parameterValues);
public slots:
    void Undock1();
    void Undock2();
};


#endif // INTERFACECCAPROJECTION_H
