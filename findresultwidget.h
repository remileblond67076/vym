#ifndef FINDRESULTWIDGET_H 
#define FINDRESULTWIDGET_H

#include <QItemSelection>
#include <QWidget>

class FindResultModel;
class TreeItem;
class VymModel;
class QTreeView;
class QPushButton;
class FindWidget;

class FindResultWidget: public QWidget
{
    Q_OBJECT

public:
    FindResultWidget (QWidget *parent=NULL);
    FindResultModel* getResultModel();
    void addItem (TreeItem *ti);
    void addItem (const QString &s);
    QString getFindText();

public slots:	
    void popup();
    void cancelPressed();
    void nextButtonPressed (QString);
    void updateSelection(QItemSelection ,QItemSelection);

signals:
    void hideFindResultWidget();
    void noteSelected (QString , int );
    void findPressed (QString);

private:
    FindResultModel *resultsModel;
    QTreeView *view;
    FindWidget *findWidget;
};

#endif

