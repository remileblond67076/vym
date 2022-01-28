#include <QDebug>

#include "branch-container.h"

#include "branchitem.h"
#include "heading-container.h"
#include "mapobj.h"

BranchContainer::BranchContainer(QGraphicsScene *scene, QGraphicsItem *parent, BranchItem *bi) : Container(parent)
{
    //qDebug() << "* Const BranchContainer begin this = " << this << "  branchitem = " << bi;
    scene->addItem(this);
    branchItem = bi;
    init();
}

BranchContainer::~BranchContainer()
{
    QString h;
    if (branchItem) h = branchItem->getHeadingPlain();
    //qDebug() << "* Destr BranchContainer" << name << h << this;

    if (branchItem)
    {
        // Unlink containers in my own subtree from related treeItems
        // That results in deleting all containers in subtree first 
        // and later deleting subtree of treeItems
        branchItem->unlinkBranchContainer();
    }
}

void BranchContainer::init()
{
    type = Container::Branch;

    setBrush(Qt::NoBrush);
    setLayoutType(Container::Horizontal);
    setHorizontalDirection(Container::LeftToRight);

    headingContainer = new HeadingContainer ();
    headingContainer->setBrush(Qt::NoBrush);
    scene()->addItem (headingContainer);

    childrenContainer = new Container ();
    childrenContainer->setBrush(Qt::NoBrush);
    childrenContainer->setLayoutType(Container::Vertical);      // Default, usually depends on depth
    childrenContainer->setVerticalAlignment(Container::Left);   // Default, usually depends on position
    childrenContainer->type = Container::Children;
    scene()->addItem (childrenContainer);

    innerContainer = new Container ();
    innerContainer->setBrush(Qt::NoBrush);
    innerContainer->type = InnerContent;
    scene()->addItem (innerContainer);

    addContainer(innerContainer);
    innerContainer->addContainer(headingContainer);
    innerContainer->addContainer(childrenContainer);            // Default, probably depends on depth
}

void BranchContainer::setBranchItem(BranchItem *bi) { branchItem = bi; }

BranchItem *BranchContainer::getBranchItem() const { return branchItem; }

QString BranchContainer::getName() {
    if (branchItem)
        return Container::getName() + " + " + branchItem->getHeadingPlain();
    else
        return Container::getName() + " - ?";
}

void BranchContainer::addToChildrenContainer(Container *c)
{
    c->setParentItem(childrenContainer);
}

Container* BranchContainer::getChildrenContainer()
{
    return childrenContainer;
}

QRectF BranchContainer::getHeadingRect()
{
    QPointF p = headingContainer->scenePos();
    return QRectF(p.x(), p.y(), headingContainer->rect().width(), headingContainer->rect().height());
}

void BranchContainer::setTmpParentContainer(BranchItem* dstBI, QPointF mousePos, int offset)
{
    //qDebug() << "BC::setTmpParentContainer " << dstBI << mousePos << offset;
}

void BranchContainer::unsetTmpParentContainer(QPointF absPos)
{
    qDebug() << " a) BC::unsetTmpParentContainer absPos = " << absPos << " BC=" << this;
    qDebug() << " b) relPos before: " << pos() << "scenePos: " << scenePos();
    setPos (absPos);   // FIXME-2 only for floating layout
    qDebug() << " c) relPos  after: " << pos() << "scenePos: " << scenePos();
}

bool BranchContainer::isInClickBox(const QPointF &p)
{
    return headingContainer->rect().contains(headingContainer->mapFromScene(p));
}

void BranchContainer::updateVisuals()
{
    headingContainer->setText(branchItem->getHeadingText());
}

void BranchContainer::reposition()
{
    // FIXME-2 temporary:   Let mainbranches float. Needs to go to central Layout class later
    if (branchItem)
    {
        bool leftOfCenter = false;
        if (branchItem->depth() == 0)
        {
            qDebug() << "BC::reposition d == 0 BFloatLayout! " << branchItem->getHeadingPlain() << this << "children: " << childrenContainer;
            //boundsType = BoundedFloating;
            setLayoutType(BFloat);
            childrenContainer->setLayoutType(BFloat);
        } else if (branchItem->depth() == 1) {
            if (pos().x() < 0) 
                leftOfCenter = true;
            else
                leftOfCenter = false;

            qDebug() << "BC::reposition d == 1  loc=" << leftOfCenter << branchItem->getHeadingPlain() << this << "children: " << childrenContainer;
            //boundsType = BoundedStacked;
            setLayoutType(Container::Horizontal);
            childrenContainer->setLayoutType(Vertical);

            if (leftOfCenter) {
                // Left of center
                setHorizontalDirection(RightToLeft);
                childrenContainer->setVerticalAlignment(Right);
            } else {
                // Right of center
                setHorizontalDirection(LeftToRight);
                childrenContainer->setVerticalAlignment(Left);
            }
        } else {
            leftOfCenter = branchItem->parentBranch()->getBranchContainer()->getHorizontalDirection();
            qDebug() << "BC::reposition d == 2  loc=" << leftOfCenter << branchItem->getHeadingPlain() << this << "children: " << childrenContainer;
            //boundsType = BoundedStacked;
            setLayoutType(Container::Horizontal);
            childrenContainer->setLayoutType(Vertical);

            if (leftOfCenter) {
                // Left of center
                //setHorizontalDirection(RightToLeft);
                innerContainer->setHorizontalDirection(RightToLeft);
                childrenContainer->setVerticalAlignment(Right);
            } else {
                // Right of center
                //setHorizontalDirection(LeftToRight);
                innerContainer->setHorizontalDirection(LeftToRight);
                childrenContainer->setVerticalAlignment(Left);
            }
        }
    } //else
        //qDebug() << "BC::reposition  no branchItem!!!!  tmpParentContainer?  this = " << this;

    Container::reposition();
}

