#include "link-container.h"

#include <math.h>
#include <QDebug>

#include "misc.h" // FIXME-2 only debugging

/////////////////////////////////////////////////////////////////
// LinkContainer
/////////////////////////////////////////////////////////////////

LinkContainer::LinkContainer()
{
    //qDebug() << "Const LinkContainer this=" << this;
    init();
}

LinkContainer::~LinkContainer()
{
    //qDebug()<< "Destructor LC  this=" << this << " style=" << style << " l=" << l << " p =" << p<< "  segments=" << segments.count();
    delLink();

    // bottomLine is deleted indirectly as child of LinkContainer
}

void LinkContainer::init()
{
    containerType = Link;

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
    createBottomLine();

    visible = true;
}

void LinkContainer::createBottomLine()
{
    bottomLine = new QGraphicsLineItem(this);
    bottomLine->setPen(pen);
    //FIXME-2 needed? bottomLine->setZValue(dZ_LINK);
    bottomLine->setVisible(true); // FIXME-2 testing
}

void LinkContainer::deleteBottomLine()
{
    if (bottomLine) {
        delete bottomLine;
        bottomLine = nullptr;
    }
}

bool LinkContainer::hasBottomLine()
{
    if (bottomLine)
        return true;
    else
        return false;
}

void LinkContainer::delLink()
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
        default:
            break;
    }
}

void LinkContainer::setLinkStyle(Style newStyle)
{
    if (style == newStyle) return;

    delLink();

    style = newStyle;

    QGraphicsLineItem *cl;
    switch (style) {
        case Line:
            l = new QGraphicsLineItem(this);
            //l->setZValue(dZ_LINK);    // FIXME-2 check, needed?
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
                //FIXME-2 needed? cl->setZValue(dZ_LINK);
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
            //FIXME-2 needed? p->setZValue(dZ_LINK);
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
            //FIXME-2 needed? p->setZValue(dZ_LINK);
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
        default:
            break;
    }
}

LinkContainer::Style LinkContainer::getLinkStyle() { return style; }

LinkContainer::Style LinkContainer::styleFromString(const QString &s)
{
    LinkContainer::Style style;
     if (s == "StyleLine")
        return LinkContainer::Line;
    else if (s == "StyleParabel")
        return LinkContainer::Parabel;
    else if (s == "StylePolyLine")
        return LinkContainer::PolyLine;
    else if (s == "StylePolyParabel")
        return LinkContainer::PolyParabel;

    return  LinkContainer::Undefined;
}

QString LinkContainer::styleString(const Style &style)
{
    switch (style) {
        case LinkContainer::NoLink:
            return "StyleNoLink";
        case LinkContainer::Line:
            return "StyleLine";
        case LinkContainer::Parabel:
            return "StyleParabel";
        case LinkContainer::PolyLine:
            return "StylePolyLine";
        case LinkContainer::PolyParabel:
            return "StylePolyParabel";
        default:
            return "StyleUndefined";
    }
}

void LinkContainer::setLinkColorHint(ColorHint hint)
{
    colorHint = hint;
}

LinkContainer::ColorHint LinkContainer::getLinkColorHint()
{
    return colorHint;
}

void LinkContainer::setLinkColor(QColor col)
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
        default:
            break;
    }
}

QColor LinkContainer::getLinkColor() { return linkcolor; }

void LinkContainer::setVisibility(bool v)
{
    visible = v;
    updateVisibility();
}

void LinkContainer::updateVisibility()
{
    bool visnow = visible;

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
        default:
            break;
        }
    }
}

void LinkContainer::updateLinkGeometry()
{
    // needs:
    //	upLinkPosParent
    //	linkPosSelf
    //	orient   of parent
    //	style
    //
    // sets:
    //  bottomlineY
    //	drawing of the link itself

    if (style == NoLink)
        return;

    double p1x = upLinkPosParent.x(); // Link is drawn from P1 to P2
    double p1y = upLinkPosParent.y();

    double p2x = upLinkPosSelf.x();
    double p2y = upLinkPosSelf.y();

    double vx = p2x - p1x;
    double vy = p2y - p1y;

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

    // Draw the horizontal line below heading (from childRefPos to parentPos)
    if (bottomLine) {
        bottomLine->setLine(p2x,p2y, downLinkPos.x(), downLinkPos.y());
        //bottomLine->setZValue(z);
    }

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
            l->setLine(p1x, p1y, p2x, p2y);
            l->setZValue(z);
            break;
        case Parabel:
            parabel(pa0, p1x, p1y, p2x, p2y);
            for (int i = 0; i < segments.size(); ++i) {
                segments.at(i)->setLine(QLineF(
                            pa0.at(i).x(),
                            pa0.at(i).y(),
                            pa0.at(i + 1).x(),
                            pa0.at(i + 1).y()));
                segments.at(i)->setZValue(z);
            }
            break;
        case PolyLine:
            pa0.clear();
            pa0 << QPointF(p1x + tp.x(), p1y + tp.y());
            pa0 << QPointF(p1x - tp.x(), p1y - tp.y());
            pa0 << QPointF(p2x,p2y);
            p->setPolygon(QPolygonF(pa0));
            p->setZValue(z);
            break;
        case PolyParabel:
            parabel(pa1, p1x + tp.x(), p1y + tp.y(), p2x, p2y);
            parabel(pa2, p1x - tp.x(), p1y - tp.y(), p2x, p2y);
            pa0.clear();
            for (int i = 0; i <= arcsegs; i++)
                pa0 << QPointF(pa1.at(i));
            for (int i = 0; i <= arcsegs; i++)
                pa0 << QPointF(pa2.at(arcsegs - i));
            p->setPolygon(QPolygonF(pa0));
            p->setZValue(z);
            break;
        default:
            qWarning() << "LinkContainer::updateLinkGeometry - Unknown LinkStyle in " << __LINE__;
            break;
    }
    setPos(0,0);    // needed due to reposition()
}

void LinkContainer::setUpLinkPosParent(const QPointF& p)
{
    upLinkPosParent = p;
}

void LinkContainer::setUpLinkPosSelf(const QPointF& p)
{
    upLinkPosSelf = p;
}

void LinkContainer::setDownLinkPos(const QPointF& p)
{
    downLinkPos= p;
}

void LinkContainer::parabel(QPolygonF &ya, qreal p1x, qreal p1y, qreal p2x,
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

void LinkContainer::reposition()
{
    //qDebug() << "LC::reposition " << info();
    return;
}

