#ifndef LINK_H
#define LINK_H

#include <QColor>

#include "xmlobj.h"

class QPointF;
class QGraphicsScene;
class QString;

class BranchItem;
class LinkableMapObj;
class VymModel;
class XLinkItem;
class XLinkObj;


class Link:public XMLObj
{
public:
    enum XLinkState {undefinedXLink,initXLink,activeXLink,deleteXLink};	

    Link (VymModel *m);
    virtual ~Link();
    virtual void init ();
    void setBeginBranch (BranchItem*);
    BranchItem* getBeginBranch();
    void setEndBranch   (BranchItem*);
    void setEndPoint(QPointF);
    BranchItem* getEndBranch();
    void setBeginLinkItem (XLinkItem*);
    XLinkItem* getBeginLinkItem();
    void setEndLinkItem (XLinkItem*);
    XLinkItem* getEndLinkItem ();
    XLinkItem* getOtherEnd (XLinkItem*);
    void setColor(QColor);
    QColor getColor();
    void setWidth (int);
    int getWidth ();
    bool activate ();		
    void deactivate ();		
    XLinkState getState();
    void removeXLinkItem (XLinkItem *xli);
    void updateLink();
    QString saveToDir ();
    XLinkObj* getXLinkObj();
    XLinkObj* createMapObj(QGraphicsScene *scene);

private:
    XLinkState xLinkState;  // init during drawing or active
    QColor color;
    int width;

    XLinkObj *xlo;
    VymModel *model;

    BranchItem *beginBranch;
    BranchItem *endBranch;
    XLinkItem *beginLinkItem;
    XLinkItem *endLinkItem;
};


#endif

