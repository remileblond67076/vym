#include "linkobj.h"

#include <math.h>

#include <QDebug>
#include <QGraphicsEllipseItem>

#include "misc.h" // FIXME-2 only debugging

/////////////////////////////////////////////////////////////////
// LinkObj
/////////////////////////////////////////////////////////////////

LinkObj::LinkObj(QGraphicsItem *parent) : MapObj(parent)
{
    //qDebug() << "Const LinkObj this=" << this;
    init();
}

LinkObj::~LinkObj()
{
    //qDebug()<< "Destructor LO  this=" << this;
    delLink();
    deleteBottomLine();
}

void LinkObj::init()
{
    l = nullptr;
    p = nullptr;
    linkcolor = Qt::black;
    thickness_start = 8;
    style = NoLink;
    arcsegs = 13;

    // FIXME-2 instead of linkcolor pen.color() could be used all around
    pen.setWidth(1);
    pen.setColor(linkcolor);
    pen.setCapStyle(Qt::RoundCap);

    bottomLine = nullptr;
}

void LinkObj::createBottomLine()
{
    if (bottomLine) return;

    bottomLine = new QGraphicsLineItem(this);
    bottomLine->setPen(pen);
}

void LinkObj::deleteBottomLine()
{
    if (bottomLine) {
        delete bottomLine;
        bottomLine = nullptr;
    }
}

bool LinkObj::hasBottomLine()
{
    if (bottomLine)
        return true;
    else
        return false;
}

void LinkObj::delLink()
{
    switch (style) {
        case Line:
            delete (l);
            break;
        case Parabel:
            while (!segments.isEmpty())
                delete segments.takeFirst();
            break;
        case PolyLine:
            delete (p);
            break;
        case PolyParabel:
            delete (p);
            break;
        case ListDash:
            delete (p);
            break;
        default:
            break;
    }
}

void LinkObj::setLinkStyle(Style newStyle)
{
    if (style == newStyle) return;

    delLink();

    style = newStyle;

    QGraphicsLineItem *cl;
    switch (style) {
        case Line:
            l = new QGraphicsLineItem(this);
            //if (visible)
                l->show();
            //else
            //    l->hide();
            l->setPen(pen);
            break;
        case Parabel:
            for (int i = 0; i < arcsegs; i++) {
                cl = new QGraphicsLineItem(this);
                cl->setLine(QLineF(i * 5, 0, i * 10, 100));
                cl->setPen(pen);
                if (visible)
                    cl->show();
                else
                    cl->hide();
                segments.append(cl);
            }
            pa0.resize(arcsegs + 1);
            break;
        case PolyLine:
            p = new QGraphicsPolygonItem(this);
            p->setPen(pen);
            p->setBrush(QBrush(pen.color()));
            if (visible)
                p->show();
            else
                p->hide();
            pa0.resize(3);
            break;
        case PolyParabel:
            p = new QGraphicsPolygonItem(this);
            p->setPen(pen);
            p->setBrush(QBrush(pen.color()));
            if (visible)
                p->show();
            else
                p->hide();
            pa0.resize(arcsegs * 2 + 2);
            pa1.resize(arcsegs + 1);
            pa2.resize(arcsegs + 1);
            break;
        case ListDash:
            p = new QGraphicsPolygonItem(this);
            p->setPen(pen);
            p->setBrush(QBrush(pen.color()));
            if (visible)
                p->show();
            else
                p->hide();
            pa0.resize(3);
            break;
        default:
            break;
    }
}

LinkObj::Style LinkObj::getLinkStyle() { return style; }

LinkObj::Style LinkObj::styleFromString(const QString &s)
{
    LinkObj::Style style;
     if (s == "StyleLine")
        return LinkObj::Line;
    else if (s == "StyleParabel")
        return LinkObj::Parabel;
    else if (s == "StylePolyLine")
        return LinkObj::PolyLine;
    else if (s == "StylePolyParabel")
        return LinkObj::PolyParabel;
    else if (s == "ListDash")
        return LinkObj::ListDash;

    return  LinkObj::Undefined;
}

QString LinkObj::styleString(const Style &style)
{
    switch (style) {
        case LinkObj::NoLink:
            return "StyleNoLink";
        case LinkObj::Line:
            return "StyleLine";
        case LinkObj::Parabel:
            return "StyleParabel";
        case LinkObj::PolyLine:
            return "StylePolyLine";
        case LinkObj::PolyParabel:
            return "StylePolyParabel";
        case LinkObj::ListDash:
            return "ListDash";
        default:
            return "StyleUndefined";
    }
}

void LinkObj::setLinkColorHint(ColorHint hint)
{
    colorHint = hint;
}

LinkObj::ColorHint LinkObj::getLinkColorHint()
{
    return colorHint;
}

void LinkObj::setLinkColor(QColor col)
{
    if (linkcolor == col) return;

    linkcolor = col;
    pen.setColor(col);

    if (bottomLine)
        bottomLine->setPen(pen);

    switch (style) {
        case Line:
            l->setPen(pen);
            break;
        case Parabel:
            for (int i = 0; i < segments.size(); ++i)
                segments.at(i)->setPen(pen);
            break;
        case PolyLine:
            p->setBrush(QBrush(linkcolor));
            p->setPen(pen);
            break;
        case PolyParabel:
            p->setBrush(QBrush(linkcolor));
            p->setPen(pen);
            break;
        case ListDash:
            p->setBrush(QBrush(linkcolor));
            p->setPen(pen);
            break;
        default:
            break;
    }
}

QColor LinkObj::getLinkColor() { return linkcolor; }

void LinkObj::setVisibility(bool v)
{
    visible = v;
    updateVisibility();
}

void LinkObj::updateVisibility()
{
    bool visnow = visible;

    qDebug() << "LO::updateVis  visnow=" << visnow << "  vis=" << isVisible();
    // FIXME-2 Hide links of unselected objects (if wanted)
    /*
    if (((MapItem *)treeItem)->getHideLinkUnselected() &&
        !treeItem->getModel()->isSelected(treeItem))
        visnow = false;
    */

    if (visnow) {
        if (bottomLine)
            bottomLine->show();

        switch (style) {
        case Line:
            if (l)
                l->show();
            break;
        case Parabel:
            for (int i = 0; i < segments.size(); ++i)
                segments.at(i)->show();
            break;
        case PolyLine:
            if (p)
                p->show();
            else
                qDebug() << "LC::updateVis p==0 (PolyLine)"; // FIXME-4
            break;
        case PolyParabel:
            if (p)
                p->show();
            else
                qDebug() << "LC::updateVis p==0 (PolyParabel) ";
                         //<< treeItem->getHeadingPlain(); // FIXME-4
            break;
        case ListDash:
            if (p)
                p->show();
            else
                qDebug() << "LC::updateVis p==0 (PolyLine)"; // FIXME-4
            break;
        default:
            break;
        }
    }
    else {
        if (bottomLine)
            bottomLine->hide();
        switch (style) {
        case Line:
            if (l)
                l->hide();
            break;
        case Parabel:
            for (int i = 0; i < segments.size(); ++i)
                segments.at(i)->hide();
            break;
        case PolyLine:
            if (p)
                p->hide();
            break;
        case PolyParabel:
            if (p)
                p->hide();
            break;
        case ListDash:
            if (p)
                p->hide();
            break;
        default:
            break;
        }
    }
}

void LinkObj::updateLinkGeometry()
{
    // needs:
    //	upLinkPosParent
    //	linkPosSelf
    //	orient   of parent
    //	style
    //
    // sets:
    //	drawing of bottomLine and upLink

    setPos(0,0);    // needed due to reposition()

    // Draw the horizontal line below heading (from childRefPos to parentPos)
    if (bottomLine)
        bottomLine->setLine(QLineF(upLinkPosSelf, downLinkPos));

    if (style == NoLink)
        return;

    double par_x = upLinkPosParent.x(); // Link is drawn from P1 to P2
    double par_y = upLinkPosParent.y();

    double self_x = upLinkPosSelf.x();
    double self_y = upLinkPosSelf.y();

    double vx = self_x - par_x;
    double vy = self_y - par_y;

    int z = -20000;
    // FIXME-3 Hack to z-move links to MapCenter (d==1) below MCOs frame (d==0)
    // no longer used?
    /*
    if (treeItem->depth() < 2)
        // z = (treeItem->depth() -2) * dZ_DEPTH + dZ_LINK;
        z = -dZ_LINK;
    else
        z = dZ_LINK;
    */
    double a; // angle
    if (vx > -0.000001 && vx < 0.000001)
        a = M_PI_2;
    else
        a = atan(vy / vx);
    // "turning point" for drawing polygonal links
    QPointF tp(-qRound(sin(a) * thickness_start),   // FIXME-2 qround needed?
                qRound(cos(a) * thickness_start));

    // Draw the link
    switch (style) {
        case Line:
            l->setLine(par_x, par_y, self_x, self_y);
            break;
        case Parabel:
            parabel(pa0, par_x, par_y, self_x, self_y);
            for (int i = 0; i < segments.size(); ++i) {
                segments.at(i)->setLine(QLineF(
                            pa0.at(i).x(),
                            pa0.at(i).y(),
                            pa0.at(i + 1).x(),
                            pa0.at(i + 1).y()));
            }
            break;
        case PolyLine:
            pa0.clear();
            pa0 << QPointF(par_x + tp.x(), par_y + tp.y());
            pa0 << QPointF(par_x - tp.x(), par_y - tp.y());
            pa0 << QPointF(self_x, self_y);
            p->setPolygon(QPolygonF(pa0));
            break;
        case PolyParabel:
            parabel(pa1, par_x + tp.x(), par_y + tp.y(), self_x, self_y);
            parabel(pa2, par_x - tp.x(), par_y - tp.y(), self_x, self_y);
            pa0.clear();
            for (int i = 0; i <= arcsegs; i++)
                pa0 << QPointF(pa1.at(i));
            for (int i = 0; i <= arcsegs; i++)
                pa0 << QPointF(pa2.at(arcsegs - i));
            p->setPolygon(QPolygonF(pa0));
            break;
        case ListDash: {  // FIXME-2 WIP, cont here. Not used?
                          // Width defined in BC:  linkSPaceContainer->setHeading("   ")
            pa0.clear();
            // center of LinkContainer, which will contain the list
            // FIXME-2 qDebug() << "LO::updateLG for ListDash bPP=" << bulletPointPos;
            //pa0 << QPointF(bulletPointPos + QPointF(-4, 0) + QPointF(self_x, self_y));
            pa0 << QPointF(bulletPointPos + QPointF( 4, 0));
            pa0 << QPointF(bulletPointPos + QPointF(-4, 0));
            p->setPolygon(QPolygonF(pa0));
            }
            break;
        default:
            qWarning() << "LinkObj::updateLinkGeometry - Unknown LinkStyle in " << __LINE__;
            break;
    }
}

void LinkObj::setUpLinkPosParent(const QPointF& p)
{
    upLinkPosParent = p;
}

void LinkObj::setUpLinkPosSelf(const QPointF& p)
{
    upLinkPosSelf = p;
}

void LinkObj::setDownLinkPos(const QPointF& p)
{
    downLinkPos= p;
}

void LinkObj::setBulletPointPos(const QPointF& p)
{
    bulletPointPos= p;
}

void LinkObj::parabel(QPolygonF &ya, qreal p1x, qreal p1y, qreal p2x,
                             qreal p2y)
{
    qreal vx = p2x - p1x;
    qreal vy = p2y - p1y;

    qreal dx; // delta x during calculation of parabel

    qreal pnx; // next point
    qreal pny;
    qreal m;

    if (vx > -0.0001 && vx < 0.0001)
        m = 0;
    else
        m = - (vy / (vx * vx));
    dx = vx / (arcsegs);
    ya.clear();
    ya << QPointF(p1x, p1y);
    for (int i = 1; i <= arcsegs; i++) {
        pnx = p1x + dx;
        pny = m * (pnx - p2x) * (pnx - p2x) + p2y;
        ya << QPointF(pnx, pny);
        p1x = pnx;
        p1y = pny;
    }
}

void LinkObj::reposition()
{
    //qDebug() << "LC::reposition " << info();
    return;
}

