#ifndef BRANCH_CONTAINER_H
#define BRANCH_CONTAINER_H

#include "container.h"

#include "link-container.h"

class BranchItem;
class HeadingContainer;
class LinkContainer;

class BranchContainer : public Container {
  public:
    /*! Orientation relative to parent branch container */
    enum Orientation {
        UndefinedOrientation,
        LeftOfParent,
        RightOfParent
    };

    BranchContainer (QGraphicsScene *scene, QGraphicsItem *parent = NULL, BranchItem *bi = NULL);
    virtual ~BranchContainer();
    virtual void init();

    BranchContainer* parentBranchContainer();

    void setBranchItem(BranchItem *);
    BranchItem *getBranchItem() const;

    virtual QString getName();

    void setOrientation(const Orientation &);
    void setOriginalOrientation();
    Orientation getOriginalOrientation();
    Orientation getOrientation();
    QPointF getOriginalParentPos();
    bool isOriginalFloating();

  private:
    bool temporaryLinked;   //! True, while moved as child of tmpParentContainer and linked temporary
    QPointF upLinkBeginScenePos;
  public:
    void setTemporaryLinked(const QPointF &sp);  // scene pos, where uplink starts
    void unsetTemporaryLinked();
    bool isTemporaryLinked();

    int childrenCount();

    int branchCount();
    void createBranchesContainer();
    void addToBranchesContainer(Container *c, bool keepScenePos = false);
    Container* getBranchesContainer();

    int imageCount();
    void createImagesContainer();
    void addToImagesContainer(Container *c, bool keepScenePos = false);
    Container* getImagesContainer();

    HeadingContainer* getHeadingContainer();
    LinkContainer* getLinkContainer();

    // Convenience functions to access children
    QList <BranchContainer*> childBranches();
    QList <ImageContainer*> childImages();

    /*! Get suggestion where new child could be positioned (scene coord) */
    QPointF getPositionHintNewChild(Container*);

    /*! Get suggestion where a relinked child could be positioned (scene coord) */
    QPointF getPositionHintRelink(Container*, int d_pos = 0, const QPointF & p_scene = QPointF());

    /*! Get position where link to children starts (scene coord) */
    QPointF getDownLinkScenePos();

    /*! Update "upwards" links in LinkContainer */
    void updateUpLink();


    virtual void setLayoutType(const LayoutType &ltype);

    virtual void setBranchesContainerLayoutType(const LayoutType &ltype);
    virtual void setBranchesContainerVerticalAlignment(const VerticalAlignment &valign);

    QRectF getHeadingRect();  //! Return rectangle of HeadingContainer in absolute coordinates

    bool isInClickBox(const QPointF &p);

    void updateVisuals();

    void reposition();

  protected:
    static qreal linkWidth;
    BranchItem *branchItem; //! Crossreference to "parent" BranchItem 
    
    // Save layout and alignment of branchesContainer even before container is created on demand
    LayoutType branchesContainerLayoutType;
    VerticalAlignment branchesContainerVerticalAlignment;

    HeadingContainer *headingContainer; // Heading of this branch
    HeadingContainer *linkSpaceContainer; // space for downLinks
    LinkContainer *linkContainer;       // uplink to parent
    Container *branchesContainer;       // Container with children branches
    Container *imagesContainer;         // Container with children images
    Container *ornamentsContainer;      // Flags and heading
    Container *innerContainer;          // Ornaments (see above) and children branches

  private:
    Orientation orientation;
    Orientation originalOrientation;            //! Save orientation before move for undo
    bool originalFloating;                      //! Save, if floating before linked temporary
    QPointF originalParentPos;  // FIXME-0 Currently used in ME to determine orientation during move: scene coord of orig, parent

};

#endif
