#include <QDebug>

#include "container.h"

#include "branchitem.h"
#include "misc.h"

#define qdbg() qDebug().nospace().noquote()

Container::Container()
{
    //qdbg() << "* Const Container " << this;
    init();
}

Container::~Container()
{
    //qdbg() << "Destr Container" << info() << this;
}

void Container::copy(Container *other)
{
    containerType = other->containerType;

    originalPos = other->originalPos;
    name = other->name;

    layout = other->layout;
    horizontalDirection = other->horizontalDirection;
    verticalAlignment = other->verticalAlignment;
}

void Container::init()
{
    containerType = UndefinedType;
    layout = Horizontal;

    // subcontainers usually may influence position
    // Only mapCenters will stay where they are

    minimumWidth = 0;

    horizontalDirection = LeftToRight;

    centralContainer = nullptr;

    // Overlay is used for frames: Don't consider for sizes or repositioning
    overlay = false;

    // Not visible usually
    setBrush(Qt::NoBrush);
    setPen(Qt::NoPen);

    // But children are shown
    show();
}

Container::ContainerType Container::getContainerType()
{
    return containerType;
}

int Container::type() const
{
    return Type;
}

void Container::setContainerType(const Container::ContainerType &t)
{
    containerType = t;
}

void Container::setName(const QString &n)   // FIXME-3 debugging only
{
    name = n;
}

QString Container::getName()    // FIXME-3 debugging only
{
    QString t;
    switch (containerType) {
        case Branch:
            t = "Branch";
            break;
        case BranchesContainer:
            t = "BranchesContainer";
            break;
        case FlagCont:
            t = "FlagCont";
            break;
        case FlagRowCont:
            t = "FlagRowCont";
            break;
        case Frame:
            t = "Frame";
            break;
        case Heading:
            t = "Heading";
            break;
        case Image:
            t = "Image";
            break;
        case ImagesContainer:
            t = "ImagesContainer";
            break;
        case InnerContent:
            t = "InnerContent";
            break;
        case Link:
            t = "Link";
            break;
        case OrnamentsContainer:
            t = "OrnamentsContainer";
            break;
        case OuterContainer:
            t = "OuterContainer";
            break;
        case Selection:
            t = "Selection";
            break;
        case TmpParent:
            t = "TmpParent";
            break;
        case UndefinedType:
            t = "Undefined";
            break;
        default:
            t = "Unknown";
            break;
    }
    return QString("[%1]").arg(t);
}

QString Container::info (const QString &prefix)
{
    return prefix +
        getName() +
        QString(" Layout: %1").arg(getLayoutString()) +
        QString(" scenePos: %1").arg(qpointFToString(scenePos(), 0)) +
        QString(" pos: %1").arg(qpointFToString(pos(), 0)) +
        QString(" rect: %1").arg(qrectFToString(rect(), 0));
}

int Container::containerDepth()
{
    int i = 0;
    Container *pc = parentContainer();
    while (pc) {
        i++;
        pc = pc->parentContainer();
    }
    return i;
}

QString Container::ind()
{
    int i = 0;
    QString s;
    while (i++ < containerDepth()) {
        s += "  ";
        i++;
    }
    return s;
}

void Container::setLayout(const Layout &l)
{
    layout = l;
}

Container::Layout Container::getLayout()
{
    return layout;
}

Container::Layout Container::getLayoutFromString(const QString &s)
{
    if (s == "Horizontal") return Horizontal;
    if (s == "Vertical") return Vertical;
    if (s == "BoundingFloats") return BoundingFloats;
    if (s == "FloatingBounded") return FloatingBounded;
    if (s == "FloatingFree") return FloatingFree;
    return UndefinedLayout;
}

QString Container::getLayoutString(const Layout &l)
{
    QString r;
    switch (l) {
        case Horizontal:
            r = "Horizontal";
            break;
        case Vertical:
            r = "Vertical";
            break;
        case BoundingFloats:
            r = "BoundingFloats";
            break;
        case FloatingBounded:
            r = "FloatingBounded";
            break;
        case FloatingFree:
            r = "FloatingFree";
            break;
        case UndefinedLayout:
            r = "UndefinedLayout";
            break;
        default:
            r = QString("Container::getLayoutString unknown layout: %1").arg(l);
            qWarning () << r;
    }
    return r;
}

QString Container::getLayoutString()
{
    return getLayoutString(layout);
}

bool Container::isFloating()
{
    Container *pc = parentContainer();
    if (pc && pc->hasFloatingLayout())
        return true;
    else
        return false;
}

bool Container::hasFloatingLayout() {
    if (layout == FloatingBounded || layout == FloatingFree)
        return true;
    else
        return false;
}

void Container::setCentralContainer(Container *cc)
{
    centralContainer = cc;
}

void Container::setHorizontalDirection(const HorizontalDirection &hdir)
{
    horizontalDirection = hdir;
}

Container::HorizontalDirection Container::getHorizontalDirection()
{
    return horizontalDirection;
}

void Container::setVerticalAlignment(const VerticalAlignment &a)
{
    verticalAlignment = a;
}

bool Container::isVisibleContainer()
{
    return visible;
}

void Container::setVisibility(bool v)
{
    // qDebug() << "C:setVis v=" << v << info ();
    visible = v;
    setVisible(visible);
}

void Container::addContainer(Container *c)
{
    c->setParentItem(this);
}

QList <Container*> Container::childContainers()
{
    //Return list of children, but ignore QGraphicsItems, which are not Containers
    QList <Container*> list;
    Container *c;
    foreach (QGraphicsItem *child, childItems()) {
        if (child->type() > UserType)
            list << (Container*) child;
    }
    return list;
}

void Container::setAnimation(const AnimPoint &ap) { animatedPos = ap; }

void Container::stopAnimation()
{
    animatedPos.stop();
}

bool Container::animate()
{
    animatedPos.animate();
    if (animatedPos.isAnimated()) {
        setPos(animatedPos);
        return true;
    }
    return false;
}

bool Container::isAnimated()
{
    return animatedPos.isAnimated();
}

QVariant Container::itemChange(GraphicsItemChange change, const QVariant &value)    // FIXME-2 needed? not used so far...
{
    //qDebug() << "Container::itemChange of " << this << ": " << change << value;
    /*
    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();
        QRectF rect = scene()->sceneRect();
        if (!rect.contains(newPos)) {
            // Keep the item inside the scene rect.
            newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
            newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
            return newPos;
        }
    }
    */
    return  QGraphicsItem::itemChange(change, value);
}

Container* Container::parentContainer()
{
    return (Container*)parentItem();
}

void Container::setPos(QPointF p)
{
    if (animatedPos.isAnimated())
        QGraphicsItem::setPos(animatedPos);
    else
        QGraphicsItem::setPos(p);
}

void Container::setPos(qreal x, qreal y)
{
    setPos(QPointF(x, y));
}

void Container::setOriginalPos()    // FIXME-3 Only used for BranchContainer and ImageContainer -> maybe move to LinkableContainer?
{
    originalPos = pos();
}

QPointF Container::getOriginalPos()
{
    return originalPos;
}

void Container::reposition()    // FIXME-3 Remove comment code used for debugging
{
    qdbg() << ind() << QString("### Reposition of %1").arg(info()) << " childCount=" << childContainers().count();
    /*
    foreach (Container *c, childContainers())
        qdbg() << ind() << " * child: " << c->info();
    */

    // Repositioning is done recursively:
    // First the size sizes of subcontainers are calculated,
    // Container::reposition is overloaded, so for example HeadingContainer
    // will return correct size of heading!
    //
    // Then the subcontainers are positioned.
    //
    // a) Do we have any children after all?
    if (!isVisible() || childContainers().count() == 0)
    {
        if (!overlay)
            setRect(QRectF());
            // The overlay case is handled later in parent container
        return;
    }

    // b) calc sizes and reposition subcontainers first based on their layouts
    //    (overloaded: Leaf containers like HeadingContainer will not recurse)

    foreach (Container *c, childContainers())
        c->reposition();

    // c) Align my own containers

    switch (layout) {
        case BoundingFloats:
            {
                //qdbg() << ind() << " - BF a) info=" << info();

                // BoundingFloats is special case:
                // Only used for innerContainer or outerContainer
                // First child container is ornamentsContainer (or innerContainer),
                // next children are imagesContainer and/or branchesContainer

                if (childContainers().count() > 4 ) {
                    qWarning() << "Container::reposition " << info();
                    qWarning() << "Wrong number of children containers: " << childItems().count();
                    foreach (Container *c, childContainers())
                        qdbg() << "  " << c->info();

                    return;
                }

                // Calc space required
                QRectF c_bbox;  // bbox of container in my own coord
                QRectF bbox;    // United bboxes

                foreach (Container *c, childContainers()) {
                    c_bbox = mapRectFromItem(c, c->rect());
                    //qdbg() << ind() << " - BF c=" << c->info();
                    bbox = bbox.united(c_bbox);
                }


                // Translate, so that total bbox and contents move, so that
                // first container (ornaments container) is centered in origin
                // (could also be Innercont. within Outercontainer )
                Container *oc = (Container*)(childContainers().first());
                QPointF t = oc->rect().center();    // FIXME-2 t seems to be always (0,0) ?!?  Check again with flags!
                if (t != QPointF(0,0)) {
                    qdbg() << ind()
                        << " - BF bbox=" << qrectFToString(bbox, 0)
                        << " oc.pos=" << qpointFToString(oc->pos())
                        << " t_oc= " << qpointFToString(t,0)
                        << " oc=" << oc->info();
                    /* FIXME-3 innerContainer now correctly rotates around headingContainer, but with images the corners might go outside of bounding OuterContainer...
                    bbox.translate(t);
                    foreach (QGraphicsItem *child, childContainers()) {
                        Container *c = (Container*) child;
                        c->setPos(c->pos() + t);
                    }
                    */
                }

                setRect(bbox);

                // qdbg() << ind() << " - BF b) info=" << info();
            } // BoundingFloats layout
            break;

        case FloatingBounded:
            {
                // Creates rect from unite of all children
                // Will not move any children, but keep their
                // (relative) positions

                // Calc bbox of all children to prepare calculating rect()
                QRectF r;
                if (childContainers().count() > 0) {
                    bool first_iteration = true;

                    // Consider other children
                    foreach (Container *c, childContainers()) {
                        QRectF c_bbox = mapRectFromItem(c, c->rect());

                        if (first_iteration) {
                            first_iteration = false;
                            r = c_bbox;
                        } else
                            r = r.united(c_bbox);
                    }
                }

                setRect(r);
                //qdbg() << ind() << " + FloatingBounded r=" << qrectFToString(r) << "  pos=" << pos() << getName();
            }
            break;

        case FloatingFree:
            setRect(QRectF()); // Empty rectangle
            break;

        case Horizontal: {
                // Calc space required
                qreal h_max = 0;
                qreal w_total = 0;

                //qdbg() << ind() << " * HL starting for " << info();
                //foreach (Container *c, childContainers())
                //   qdbg() << ind() << "   HL: child: " << c->info() << " ovly:" << c->overlay;
                foreach (Container *c, childContainers()) {
                    if (!c->overlay) {
                        QRectF c_bbox = mapRectFromItem(c, c->rect());

                        w_total += c_bbox.width();
                        qreal h = c_bbox.height();
                        h_max = (h_max < h) ? h : h_max;
                    }
                }
/*
*/
                if (centralContainer)
                    qdbg() << ind() << "    HL: Found central container: " << centralContainer->info();

                // Left (or right) line, where next children will be aligned to
                qreal x = - w_total / 2;
                if (horizontalDirection == RightToLeft)
                    x = -x;

                // Position children initially. (So far only centered vertically)
                foreach (Container *c, childContainers()) {
                    if (!c->overlay) {
                        QRectF c_bbox = mapRectFromItem(c, c->rect());
                        QPointF origin_mapped = mapFromItem(c, QPointF());
                        qreal offset;

                        // Pre alignment
                        if (horizontalDirection == LeftToRight)
                            offset = - (c_bbox.left() - origin_mapped.x());
                        else
                            offset = - (c_bbox.right() - origin_mapped.x());

                        //qdbg() << ind() << "    HL x=" << x << "offset=" << offset << "c: " << c->info();

                        // Align vertically centered
                        c->setPos (x + offset, - c->rect().height() / 2 - c->rect().top());

                        // Align vertically to top
                        // c->setPos (x + offset, - h_max / 2 - c->rect().top());

                        // Align vertically to bottom
                        // c->setPos (x + offset, h_max / 2 - c->rect().bottom());

                        // Post alignment
                        if (horizontalDirection == LeftToRight) {
                            x += c_bbox.width();
                        } else
                            x -= c_bbox.width();

                        //qdbg() << ind() << "    HL Done positioning: " << c->info();
                    }   // No overlay container
                }   // Position children 

                // Move everything, so that center of central container will be in origin
                QPointF v_central;

                if (centralContainer) {
                    v_central = mapFromItem(centralContainer, centralContainer->rect().center());
                    qdbg() << ind() << "    HL central container a:  => v_central=" << qpointFToString(v_central, 0) << " cc=" << centralContainer->info();
                    if (parentContainer()) {
                        if (parentContainer()->hasFloatingLayout())  {
                            qdbg() << ind() << "pC has floating layout. this=" << info();
                            if (!v_central.isNull())
                                foreach (Container *c, childContainers()) {
                                    if (!c->overlay) {
                                        //qdbg() << ind() << "    HL moving c:" << c->info();
                                        c->setPos(c->pos() - v_central);
                                    }
                                }
                        }
                    } else {
                        qdbg() << ind() << " centralContainer but no pC " << info();
                        // No parentContainer, adjust mainBranch, so that heading keeps position
                        // FIXME-00 Make mapCenter positions absolute and independent of container size 
                        //setPos(-v_central);
                    }
                }

                setRect(QRectF(- w_total / 2 - v_central.x(),  - h_max / 2 - v_central.y(), w_total, h_max));

                //qdbg() << ind() << " * HL Finished for " << info();
            } // Horizontal layout
            break;

        case Vertical: {    // FIXME-2 Does not support overlay (FrameContainer) yet
                qreal h_total = 0;
                qreal w_max = 0;

                // Calc space required
                foreach (Container *c, childContainers()) {
                    QRectF c_bbox = mapRectFromItem(c, c->rect());

                    // For width and height we can use the already mapped dimensions
                    qreal w = c_bbox.width();
                    w_max = (w_max < w) ? w : w_max;
                    h_total += c_bbox.height();
                }

                // Top line, where next children will be aligned to
                qreal y = - h_total / 2;

                // Position children initially
                foreach (Container *c, childContainers()) {
                    y += - c->rect().top();
		    switch (verticalAlignment) {
			case AlignedLeft:
                            c->setPos(- w_max / 2 - c->rect().left(), y);
			    break;
			case AlignedRight:
                            c->setPos(w_max / 2 - c->rect().right(), y);
			    break;
			case AlignedCentered:
			    c->setPos (- c->rect().width() / 2 - c->rect().left(), y);
			    break;
                        default:
                            qWarning() << "Container::reposition vertically - undefined alignment:" << verticalAlignment << " in " << info();
                            if (containerType == BranchesContainer)
                                qWarning() << "  orient=" << ((BranchContainer*)this)->getOrientation();
		    }

                    y += c->rect().bottom();
                }

                // Set rect
                setRect(-w_max / 2, -h_total / 2, w_max, h_total);

            } // Vertical layout
            break;
        default:
            qWarning() << "Container::reposition  unknown layout type for container: " << info();
            break;
    }

    // Now we have calculated our own size, adjust depending overlay containers
    foreach (Container *c, childContainers()) {
        if (c->overlay) {
            c->setRect(rect());
        }
    }
}
