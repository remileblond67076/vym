#include "mapdesign.h"

#include <QDebug>

/////////////////////////////////////////////////////////////////
// MapDesign
/////////////////////////////////////////////////////////////////

MapDesign::MapDesign()
{
    //qDebug() << "Constr. MapDesign";
    init();
}

void MapDesign::init()
{
    // NewBranch: Layout of children branches 
    bcNewBranchLayouts << Container::FloatingBounded;
    bcNewBranchLayouts << Container::Vertical;

    // RelinkBranch: Layout of children branches 
    bcRelinkBranchLayouts << Container::FloatingBounded;
    bcRelinkBranchLayouts << Container::Vertical;

    // NewBranch: Layout of children images 
    icNewBranchLayouts << Container::FloatingFree;

    // RelinkBranch: Layout of children images
    icRelinkBranchLayouts << Container::FloatingFree;

    // Should links of branches use a default color or the color of heading?
    linkColHint = LinkContainer::DefaultColor;
    defaultLinkCol = Qt::blue;
}

void MapDesign::setName(const QString &s)
{
    name = s;
}

QString MapDesign::getName()
{
    return name;
}

Container::Layout MapDesign::branchesContainerLayout(
        const BranchContainer::StyleUpdateMode &mode,
        int depth)
{
    //qDebug() << "MD  mode=" << mode << " d=" << depth;
    if (mode == BranchContainer::NewBranch) {
        // Relinked branch
        int max = bcNewBranchLayouts.count();
        if (depth < max)
            return bcNewBranchLayouts.at(depth);
        else {
            if (max > 0)
                // Return last entry, if it exists
                return bcNewBranchLayouts.at(max - 1);
            else
                // Don't change 
                return Container::UndefinedLayout;
        }
    } else {
        // New branch or anyway updating 
        int max = bcRelinkBranchLayouts.count();
        if (depth < max)
            return bcRelinkBranchLayouts.at(depth);
        else {
            if (max > 0)
                // Return last entry, if it exists
                return bcRelinkBranchLayouts.at(max - 1);
            else
                // Don't change 
                return Container::UndefinedLayout;
        }
    }
}

Container::Layout MapDesign::imagesContainerLayout(
        const BranchContainer::StyleUpdateMode &mode,
        int depth)
{
    //qDebug() << "MD  mode=" << mode << " d=" << depth;
    if (mode == BranchContainer::NewBranch) {
        // Relinked branch
        int max = icNewBranchLayouts.count();
        if (depth < max)
            return icNewBranchLayouts.at(depth);
        else {
            if (max > 0)
                // Return last entry, if it exists
                return icNewBranchLayouts.at(max - 1);
            else
                // Don't change 
                return Container::UndefinedLayout;
        }
    } else {
        // New branch or anyway updating 
        int max = icRelinkBranchLayouts.count();
        if (depth < max)
            return icRelinkBranchLayouts.at(depth);
        else {
            if (max > 0)
                // Return last entry, if it exists
                return icRelinkBranchLayouts.at(max - 1);
            else
                // Don't change 
                return Container::UndefinedLayout;
        }
    }
}

LinkContainer::ColorHint MapDesign::linkColorHint()
{
    return linkColHint;
}

void MapDesign::setLinkColorHint(const LinkContainer::ColorHint &lch)
{
    linkColHint = lch;
}

QColor MapDesign::defaultLinkColor()
{
    return defaultLinkCol;
}

void MapDesign::setDefaultLinkColor(const QColor &col)
{
    defaultLinkCol = col;
}
