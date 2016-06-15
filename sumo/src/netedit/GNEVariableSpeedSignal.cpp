/****************************************************************************/
/// @file    GNEVariableSpeedSignal.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: GNEVariableSpeedSignal.cpp 19861 2016-02-01 09:08:47Z palcraft $
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

#include "GNEVariableSpeedSignal.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNELogo_VariableSpeedSignal.cpp"
#include "GNELogo_VariableSpeedSignalSelected.cpp"
#include "GNEVariableSpeedSignalDialog.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// static member definitions
// ===========================================================================
GUIGlID GNEVariableSpeedSignal::variableSpeedSignalGlID = 0;
GUIGlID GNEVariableSpeedSignal::variableSpeedSignalSelectedGlID = 0;
bool GNEVariableSpeedSignal::variableSpeedSignalInitialized = false;
bool GNEVariableSpeedSignal::variableSpeedSignalSelectedInitialized = false;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEVariableSpeedSignal::GNEVariableSpeedSignal(const std::string& id, GNEViewNet* viewNet, Position pos, std::vector<GNELane*> lanes, const std::string& filename, bool blocked) :
    GNEAdditionalSet(id, viewNet, pos, SUMO_TAG_VSS, blocked, std::vector<GNEAdditional*>(), std::vector<GNEEdge*>(), lanes),
    myFilename(filename) {
    // Update geometry;
    updateGeometry();
    // Set colors
    myBaseColor = RGBColor(76, 170, 50, 255);
    myBaseColorSelected = RGBColor(161, 255, 135, 255);
    // load rerouter logo, if wasn't inicializated
    if (!variableSpeedSignalInitialized) {
        FXImage* i = new FXGIFImage(getViewNet()->getNet()->getApp(), GNELogo_VariableSpeedSignal, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
        variableSpeedSignalGlID = GUITexturesHelper::add(i);
        variableSpeedSignalInitialized = true;
        delete i;
    }

    // load rerouter selected logo, if wasn't inicializated
    if (!variableSpeedSignalSelectedInitialized) {
        FXImage* i = new FXGIFImage(getViewNet()->getNet()->getApp(), GNELogo_VariableSpeedSignalSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
        variableSpeedSignalSelectedGlID = GUITexturesHelper::add(i);
        variableSpeedSignalSelectedInitialized = true;
        delete i;
    }
}


GNEVariableSpeedSignal::~GNEVariableSpeedSignal() {
}


void
GNEVariableSpeedSignal::updateGeometry() {
    // Clear shape
    myShape.clear();

    // Set position
    myShape.push_back(myPosition);

    // Set block icon offset
    myBlockIconOffset = Position(-0.5, -0.5);

    // Set block icon rotation, and using their rotation for draw logo
    setBlockIconRotation();

    // Update connections
    updateConnections();
}


void 
GNEVariableSpeedSignal::openAdditionalDialog() {
    GNEVariableSpeedSignalDialog variableSpeedSignalDialog(this);
}


void
GNEVariableSpeedSignal::moveAdditional(SUMOReal posx, SUMOReal posy, GNEUndoList *undoList) {
    // if item isn't blocked
    if(myBlocked == false) {
        // change Position
        undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(Position(posx, posy, 0))));
    }
}


void
GNEVariableSpeedSignal::writeAdditional(OutputDevice& device) {

}


std::string 
GNEVariableSpeedSignal::getFilename() const {
    return myFilename;
}


void 
GNEVariableSpeedSignal::setFilename(std::string filename) {
    myFilename = filename;
}


GUIParameterTableWindow*
GNEVariableSpeedSignal::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) {
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
GNEVariableSpeedSignal::drawGL(const GUIVisualizationSettings& s) const {
    // Start drawing adding an gl identificator
    glPushName(getGlID());

    // Add a draw matrix for drawing logo
    glPushMatrix();
    glTranslated(myShape[0].x(), myShape[0].y(), getType());
    glColor3d(1, 1, 1);
    glRotated(180, 0, 0, 1);

    // Draw icon depending of rerouter is or isn't selected
    if(isAdditionalSelected()) 
        GUITexturesHelper::drawTexturedBox(variableSpeedSignalSelectedGlID, 1);
    else
        GUITexturesHelper::drawTexturedBox(variableSpeedSignalGlID, 1);

    // Pop draw icon matrix
    glPopMatrix();

    // Push matrix to draw every symbol over lane
    glPushMatrix();

    // Traslate to 0,0
    glTranslated(0, 0, getType());

    // Obtain exaggeration
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);

    // Iterate over lanes
    for (childLanes::const_iterator i = myChildLanes.begin(); i != myChildLanes.end(); i++) {
        // Draw every signal over Lane
        glPushMatrix();
        glScaled(exaggeration, exaggeration, 1);
        glTranslated(i->positionOverLane.x(), i->positionOverLane.y(), 0);
        glRotated(i->rotationOverLane, 0, 0, 1);
        glTranslated(0, -1.5, 0);

        int noPoints = 9;
        if (s.scale > 25) {
            noPoints = (int)(9.0 + s.scale / 10.0);
            if (noPoints > 36) {
                noPoints = 36;
            }
        }
        glColor3d(1, 0, 0);
        GLHelper::drawFilledCircle((SUMOReal) 1.3, noPoints);
        if (s.scale >= 5) {
            glTranslated(0, 0, .1);
            glColor3d(0, 0, 0);
            GLHelper::drawFilledCircle((SUMOReal) 1.1, noPoints);
            // Draw speed
            SUMOReal speed = i->lane->getSpeed();
            // Show as Km/h
            speed *= 3.6f;
            if (((int) speed + 1) % 10 == 0)
                speed = (SUMOReal)(((int) speed + 1) / 10 * 10);
            // draw the speed string
            std::string speedToDraw = toString<SUMOReal>(speed);
            glColor3d(1, 1, 0);
            glTranslated(0, 0, .1);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            pfSetPosition(0, 0);
            pfSetScale(1.2f);
            SUMOReal w = pfdkGetStringWidth(speedToDraw.c_str());
            glRotated(180, 0, 1, 0);
            glTranslated(-w / 2., 0.3, 0);
            pfDrawString(speedToDraw.c_str());
        }
        glPopMatrix();
    }

    glPopMatrix();

    // Show Lock icon depending of the Edit mode
    //if(dynamic_cast<GNEViewNet*>(parent)->showLockIcon())
        drawLockIcon(0.4);

    // Draw connections
    drawConnections();

    // Draw name
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);

    // Pop name
    glPopName();
}


std::string
GNEVariableSpeedSignal::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_LANES:
            /** completar **/
            return "";
        case SUMO_ATTR_POSITION:
            return toString(myPosition);
        case SUMO_ATTR_FILE:
            return myFilename;
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


void
GNEVariableSpeedSignal::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying " + toString(getType()) + " attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_LANES:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_FILE:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            updateGeometry();
            break;
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


bool
GNEVariableSpeedSignal::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying " + toString(getType()) + " attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_POSITION:
            bool ok;
            return GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, false).size() == 1;
        case SUMO_ATTR_LANES:
            /** completar **/
        case SUMO_ATTR_FILE:
            return isValidFileValue(value);
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


void
GNEVariableSpeedSignal::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying " + toString(getType()) + " attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_LANES:
            /** completar **/
            break;
        case SUMO_ATTR_POSITION:
            bool ok;
            myPosition = GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, false)[0];
            updateGeometry();
            getViewNet()->update();
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}

/****************************************************************************/