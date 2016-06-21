/****************************************************************************/
/// @file    GNERouteProbe.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2016
/// @version $Id$
///
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <utility>
#include <utils/geom/GeomConvHelper.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNEViewNet.h"
#include "GNERouteProbe.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNELogo_RouteProbe.cpp"
#include "GNELogo_RouteProbeSelected.cpp"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// static member definitions
// ===========================================================================
GUIGlID GNERouteProbe::myRouteProbeGlID = 0;
GUIGlID GNERouteProbe::myRouteProbeSelectedGlID = 0;
bool GNERouteProbe::myRouteProbeInitialized = false;
bool GNERouteProbe::myRouteProbeSelectedInitialized = false;

// ===========================================================================
// member method definitions
// ===========================================================================


GNERouteProbe::GNERouteProbe(const std::string& id, GNEViewNet* viewNet, GNEEdge *edge, int frequency, const std::string& filename, int begin, bool blocked) :
    GNEAdditional(id, viewNet, Position(), SUMO_TAG_ROUTEPROBE, NULL, blocked),
    myEdge(edge),
    myFrequency(frequency),
    myFilename(filename),
    myBegin(begin) {
    // Add additional to edge parent
    myEdge->addAdditional(this);
    // Update geometry;
    updateGeometry();
    // Set colors
    // load RouteProbe logo, if wasn't inicializated
    if (!myRouteProbeInitialized) {
        FXImage* i = new FXGIFImage(getViewNet()->getNet()->getApp(), GNELogo_RouteProbe, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
        myRouteProbeGlID = GUITexturesHelper::add(i);
        myRouteProbeInitialized = true;
        delete i;
    }

    // load RouteProbe selected logo, if wasn't inicializated
    if (!myRouteProbeSelectedInitialized) {
        FXImage* i = new FXGIFImage(getViewNet()->getNet()->getApp(), GNELogo_RouteProbeSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
        myRouteProbeSelectedGlID = GUITexturesHelper::add(i);
        myRouteProbeSelectedInitialized = true;
        delete i;
    }
}


GNERouteProbe::~GNERouteProbe() {
    if(myEdge)
        myEdge->removeAdditional(this);
}


void
GNERouteProbe::updateGeometry() {
    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();

    // clear Shape
    myShape.clear();

    // get lanes of edge
    GNELane* firstLane = myEdge->getLanes().at(0);

    // Save number of lanes
    numberOfLanes = myEdge->getLanes().size();

    // Get shape of lane parent
    myShape.push_back(firstLane->getShape().positionAtOffset(5));

    // Obtain first position
    Position f = myShape[0] - Position(1, 0);

    // Obtain next position
    Position s = myShape[0] + Position(1, 0);

    // Save rotation (angle) of the vector constructed by points f and s
    myShapeRotations.push_back(firstLane->getShape().rotationDegreeAtOffset(5) * -1);

    // Set offset of the block icon
    myBlockIconOffset = Position(1.1, -3.06);

    // Set block icon rotation, and using their rotation for logo
    setBlockIconRotation(firstLane);
}


Position 
GNERouteProbe::getPositionInView() const {
    Position A = myEdge->getLanes().front()->getShape().positionAtOffset(myPosition.x());
    Position B = myEdge->getLanes().back()->getShape().positionAtOffset(myPosition.x());

    // return Middle point
    return Position((A.x() + B.x()) / 2, (A.y() + B.y()) / 2); 
}


void
GNERouteProbe::moveAdditional(SUMOReal, SUMOReal, GNEUndoList*) {
    // This additional cannot be moved
}


void
GNERouteProbe::writeAdditional(OutputDevice& device) {
    // Write parameters
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_FREQUENCY, myFrequency);
    if(!myFilename.empty())
        device.writeAttr(SUMO_ATTR_FILE, myFilename);
    device.writeAttr(SUMO_ATTR_BEGIN, myBegin);
    // Close tag
    device.closeTag();
}


GNEEdge* 
GNERouteProbe::getEdge() const {
    return myEdge;
}

void 
GNERouteProbe::removeEdgeReference() {
    myEdge = NULL;
}


std::string 
GNERouteProbe::getFilename() const {
    return myFilename;
}


int 
GNERouteProbe::getFrequency() const {
    return myFrequency;
}


int 
GNERouteProbe::getBegin() const {
    return myBegin;
}


void 
GNERouteProbe::setFilename(std::string filename) {
    myFilename = filename;
}


void 
GNERouteProbe::setFrequency(int frequency) {
    myFrequency = frequency;
}


void 
GNERouteProbe::setBegin(int begin) {
    myBegin = begin;
}


GUIParameterTableWindow*
GNERouteProbe::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    /** NOT YET SUPPORTED **/
    // Ignore Warning
    UNUSED_PARAMETER(parent);
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, 2);
    // add items
    ret->mkItem("id", false, getID());
    /** @TODO complet with the rest of parameters **/
    // close building
    ret->closeBuilding();
    return ret;
}


void
GNERouteProbe::drawGL(const GUIVisualizationSettings& s) const {
    // get values
    glPushName(getGlID());
    SUMOReal width = (SUMOReal) 2.0 * s.scale;
    glLineWidth(1.0);
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);

    // draw shape
    glColor3ub(255, 216, 0);
    glPushMatrix();
    glTranslated(0, 0, getType());
    glTranslated(myShape[0].x(), myShape[0].y(), 0);
    glRotated(myShapeRotations[0], 0, 0, 1);
    glScaled(exaggeration, exaggeration, 1);
    glTranslated(-1.6, -1.6, 0);
    glBegin(GL_QUADS);
    glVertex2d(0,  0.25);
    glVertex2d(0, -0.25);
    glVertex2d((numberOfLanes * 3.3), -0.25);
    glVertex2d((numberOfLanes * 3.3),  0.25);
    glEnd();
    glTranslated(0, 0, .01);
    glBegin(GL_LINES);
    glVertex2d(0, 0.25 - .1);
    glVertex2d(0, -0.25 + .1);
    glEnd();

    // position indicator (White)
    if (width * exaggeration > 1) {
        glRotated(90, 0, 0, -1);
        glColor3d(1, 1, 1);
        glBegin(GL_LINES);
        glVertex2d(0, 0);
        glVertex2d(0, (numberOfLanes * 3.3));
        glEnd();
    }

    // Pop shape matrix
    glPopMatrix();

    // Add a draw matrix for drawing logo
    glPushMatrix();
    glTranslated(myShape[0].x(), myShape[0].y(), getType());
    glRotated(myShapeRotations[0], 0, 0, 1);
    glTranslated(-2.56, - 1.6, 0);
    glColor3d(1, 1, 1);
    glRotated(-90, 0, 0, 1);

    // Draw icon depending of detector is or isn't selected
    if(isAdditionalSelected()) 
        GUITexturesHelper::drawTexturedBox(myRouteProbeSelectedGlID, 1);
    else
        GUITexturesHelper::drawTexturedBox(myRouteProbeGlID, 1);

    // Pop logo matrix
    glPopMatrix();

    // Check if the distance is enought to draw details
    if (s.scale * exaggeration >= 10) {        
        // Show Lock icon depending of the Edit mode
        //if(dynamic_cast<GNEViewNet*>(parent)->showLockIcon())
            drawLockIcon(0.4);
    }

    // Finish draw
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    glPopName();
}


std::string
GNERouteProbe::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_EDGE:
            return myEdge->getID();
        case SUMO_ATTR_FILE:
            return myFilename;
        case SUMO_ATTR_FREQUENCY:
            return toString(myFrequency);
        case SUMO_ATTR_BEGIN:
            return toString(myBegin);
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


void
GNERouteProbe::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGE:
            throw InvalidArgument("modifying " + toString(getType()) + " attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_FREQUENCY:
        case SUMO_ATTR_BEGIN:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            updateGeometry();
            break;
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


bool
GNERouteProbe::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGE:
            throw InvalidArgument("modifying " + toString(getType()) + " attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_FILE:
            return isValidFileValue(value);
        case SUMO_ATTR_FREQUENCY:
            return canParse<int>(value);
        case SUMO_ATTR_BEGIN:
            return canParse<int>(value);
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


void
GNERouteProbe::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGE:
            throw InvalidArgument("modifying " + toString(getType()) + " attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        case SUMO_ATTR_FREQUENCY:
            myFrequency = parse<int>(value);
            break;
        case SUMO_ATTR_BEGIN:
            myBegin = parse<int>(value);
            break;
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}

/****************************************************************************/