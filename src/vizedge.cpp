#include "vizedge.h"
#include "vizgraph.h"
#include "vecops.h"
#include <QPainterPathStroker>

VizEdge::VizEdge(GfaEdge* _gfa_edge, VizGraph* _vg) :
                                          VizElement(VIZ_EDGE, _vg, _gfa_edge) {
  gfa_edge = _gfa_edge;
  isDovetail = gfa_edge->isDovetail();
  highlights[0] = NULL;
  highlights[1] = NULL;
  viz_nodes[0] = vg->getNode(gfa_edge->getSegment(0));
  viz_nodes[1] = vg->getNode(gfa_edge->getSegment(1));
  if (isDovetail) {
    connected_subnodes[0] = (gfa_edge->isInedge(0) ?
        viz_nodes[0]->getStart() : viz_nodes[0]->getEnd());
    connected_subnodes[1] = (gfa_edge->isInedge(1) ?
        viz_nodes[1]->getStart() : viz_nodes[1]->getEnd());
  } else {
    connected_subnodes[0] = viz_nodes[0]->getNodeAtBase(
        (gfa_edge->getBegin(0)+gfa_edge->getEnd(0))/2);
    connected_subnodes[1] = viz_nodes[1]->getNodeAtBase(
        (gfa_edge->getBegin(1)+gfa_edge->getEnd(1))/2);
  }
  if (gfa_edge->positionsSet()) {
    highlights[0] = viz_nodes[0]->registerHighlight(gfa_edge->getBegin(0),
        gfa_edge->getEnd(0));
    highlights[1] = viz_nodes[1]->registerHighlight(gfa_edge->getBegin(1),
        gfa_edge->getEnd(1));
    highlights[0]->setVisible(false);
    highlights[1]->setVisible(false);
  }

  ogdf_edge = vg->G.searchEdge(connected_subnodes[0], connected_subnodes[1]);
  if (ogdf_edge == 0)
    ogdf_edge = vg->G.newEdge(connected_subnodes[0], connected_subnodes[1]);
  vg->edgeLengths[ogdf_edge] = 0.2;
  if (isDovetail) {
    vg->GA.doubleWeight(ogdf_edge) = 10;
  } else {
    vg->GA.doubleWeight(ogdf_edge) = 80;
  }

  setAcceptHoverEvents(true);
  setFlags(ItemIsSelectable);
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(ItemAcceptsInputMethod, true);
  vg->scene->addItem(this);
}

VizEdge::~VizEdge() {
  /* empty */
}

QPainterPath VizEdge::getPath(VizGroup* group) {
  QPainterPath path;
  if (!isDovetail) {
    QPointF p1 = viz_nodes[0]->getCoordForBase(
        (gfa_edge->getBegin(0)+gfa_edge->getEnd(0))/2);
    QPointF p2 = viz_nodes[1]->getCoordForBase(
        (gfa_edge->getBegin(1)+gfa_edge->getEnd(1))/2);
    path.moveTo(p1);
    path.lineTo(p2);
  } else {
    QPointF p1 = vg->getNodePos(connected_subnodes[0]);
    QPointF p2 = vg->getNodePos(connected_subnodes[1]);
    QPointF d1 = (gfa_edge->isInedge(0) ?
        viz_nodes[0]->getStartDir() : viz_nodes[0]->getEndDir());
    QPointF d2 = (gfa_edge->isInedge(1) ?
        viz_nodes[1]->getStartDir() : viz_nodes[1]->getEndDir());
    path.moveTo(p1);
    path.cubicTo(p1+5*d1, p2+5*d2, p2);
  }
  if (group != NULL) {
    QPainterPathStroker stroker;
    double width = (isDovetail ? getOption(VIZ_DOVETAILWIDTH).toDouble():
                                 getOption(VIZ_INTERNALWIDTH).toDouble());
    for (long unsigned int idx = 0; idx < groups.size(); idx++) {
      if (groups[idx] == group) {
        break;
      }
      width += groups[idx]->getOption(VIZ_GROUPWIDTH).toDouble() * 2;
    }
    width += group->getOption(VIZ_GROUPWIDTH).toDouble() * 0.5;
    stroker.setWidth(width);
    return stroker.createStroke(path);
  }
  return path;
}

void VizEdge::draw() {
  //if (scene())
    //vg->scene->removeItem(this);

  setPath(getPath());
  if (!isDovetail) {
    QPen pen(getOption(VIZ_INTERNALCOLOR).value<QColor>());
    pen.setWidthF(getOption(VIZ_INTERNALWIDTH).toDouble());
    setPen(pen);
  } else {
    QPen pen(getOption(VIZ_DOVETAILCOLOR).value<QColor>());
    pen.setWidthF(getOption(VIZ_DOVETAILWIDTH).toDouble());
    setPen(pen);
  }

  bool visible = getOption(VIZ_EDGEHIGHLIGHTSHOW).toBool();
  QColor color = getOption(VIZ_EDGEHIGHLIGHTCOLOR).value<QColor>();
  if (highlights[0]) {
    highlights[0]->setVisibility(visible);
    highlights[0]->setColor(color);
  }
  if (highlights[1]) {
    highlights[1]->setVisibility(visible);
    highlights[1]->setColor(color);
  }

  if (getOption(VIZ_EDGELABELSHOW).toBool()) {
    drawLabel(getOption(VIZ_EDGELABELFONT).toString(),
              getOption(VIZ_EDGELABELFONTSIZE).toDouble(),
              getOption(VIZ_EDGELABELCOLOR).value<QColor>(),
              getOption(VIZ_EDGELABELOUTLINEWIDTH).toDouble(),
              getOption(VIZ_EDGELABELOUTLINECOLOR).value<QColor>());
    /*QString text = QString::fromStdString(gfa_edge->getName());
    if (getOption(VIZ_EDGELABELSHOWLENGTH).toBool() && gfa_edge->isLengthSet())
      text += "\n" + QString(gfa_node->getLength()) + " bp";
    setLabelText(text);*/
    setLabelVisible(true);
  } else {
    setLabelVisible(false);
  }
  setVisible(!getOption(VIZ_DISABLEEDGES).toBool());
}

QPointF VizEdge::getCenterCoord() {
  QPointF p1 = vg->getNodePos(connected_subnodes[0]);
  QPointF p2 = vg->getNodePos(connected_subnodes[1]);
  return 0.5 * p1 + 0.5 * p2;
}

GfaLine* VizEdge::getGfaElement() {
  return gfa_edge;
}

void VizEdge::hoverEnterEvent(QGraphicsSceneHoverEvent *e) {
  QPen pen;
  if (!isDovetail) {
    pen.setColor(getOption(VIZ_INTERNALCOLOR).value<QColor>());
    pen.setWidthF(getOption(VIZ_INTERNALWIDTH).toDouble() * 1.5);
  } else {
    pen.setColor(getOption(VIZ_DOVETAILCOLOR).value<QColor>());
    pen.setWidthF(getOption(VIZ_DOVETAILWIDTH).toDouble() * 1.5);
  }
  setPen(pen);
  update();

  if (highlights[0]) {
    highlights[0]->setVisibility(true);
    highlights[0]->draw();
  }
  if (highlights[1]) {
    highlights[1]->setVisibility(true);
    highlights[1]->draw();
  }

  VizElement::hoverEnterEvent(e);
}

void VizEdge::hoverLeaveEvent(QGraphicsSceneHoverEvent *e) {
  QPen pen;
  if (!isDovetail) {
    pen.setColor(getOption(VIZ_INTERNALCOLOR).value<QColor>());
    pen.setWidthF(getOption(VIZ_INTERNALWIDTH).toDouble());
  } else {
    pen.setColor(getOption(VIZ_DOVETAILCOLOR).value<QColor>());
    pen.setWidthF(getOption(VIZ_DOVETAILWIDTH).toDouble());
  }
  setPen(pen);
  update();

  if (!getOption(VIZ_EDGEHIGHLIGHTSHOW).toBool()) {
    if (highlights[0])
      highlights[0]->setVisibility(false);
    if (highlights[1])
      highlights[1]->setVisibility(false);
  }

  VizElement::hoverLeaveEvent(e);
}
