#pragma once
#include "headers.h"

#include "gfa/graph.h"
#include "gfa/edge.h"

#include "vizedge.h"
#include "viznode.h"
#include "vizgap.h"
#include "vizgroup.h"
#include "vizfragment.h"
#include "vizsettings.h"
#include "ui_graphwidget.h"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/energybased/FMMMLayout.h>

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsItem>
#include <QGraphicsSceneMouseEvent>


using namespace ogdf;

typedef unordered_map<GfaSegment*,VizNode*> NodeMap;
typedef unordered_map<GfaEdge*,VizEdge*> EdgeMap;
typedef unordered_map<GfaGap*,VizGap*> GapMap;
typedef unordered_map<GfaGroup*,VizGroup*> GroupMap;
typedef unordered_map<GfaFragment*,VizFragment*> FragmentMap;

class VizScene : public QGraphicsScene {
  public:
    virtual void drawItems(QPainter * painter, int numItems, QGraphicsItem* items[], const QStyleOptionGraphicsItem options[], QWidget * widget = 0) {
      //cout << "Scene!" << endl;
      QGraphicsScene::drawItems(painter,numItems,items,options,widget);
    }
};


class VizGraph : public QWidget {
  //Q_OBJECT
  public:
    VizGraph(const QString& filename, const VizAppSettings& appSettings, QWidget *parent = 0);
    void setDisplaySize(unsigned int width, unsigned int height);
    //~VizGraph();
    
    Graph G;
    GraphAttributes GA;
    EdgeArray<double> edgeLengths;
    
    VizGraphSettings settings;
    
    //QGraphicsScene *scene;
    QGraphicsScene* scene;
    
    VizElement* getElement(GfaLine* line) const;
    VizNode* getNode(GfaSegment* seg) const;
    VizEdge* getEdge(GfaEdge* edge) const;
    VizFragment* getFragment(GfaFragment* fragment) const;
    VizGap* getGap(GfaGap* gap) const;
    VizGroup* getGroup(GfaGroup* group) const;
    void draw();
    void renderToFile(QString filename, QString format);
    
    QPointF getNodePos(node n);

  public slots:
    void zoomIn();
    void zoomOut();
    void zoomDefault();
  
  private:
    void addNode(GfaSegment* seg);
    void addEdge(GfaEdge* edge);
    void addGap(GfaGap* gap);
    void addGroup(GfaGroup* group);
    void addFragment(GfaFragment* fragment);
    void determineParams();
    void calcLayout();
    
    void setCacheMode(QGraphicsItem::CacheMode mode);
  
    GfaGraph* gfa;
    NodeMap nodes;
    EdgeMap edges;
    GapMap gaps;
    GroupMap groups;
    FragmentMap fragments;
    
    Ui::GraphWidget form;
    QGraphicsView* view;
    int viewWidth, viewHeight;
};
