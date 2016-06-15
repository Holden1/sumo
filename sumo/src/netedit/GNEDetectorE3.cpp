/****************************************************************************/
/// @file    GNEDetectorE3.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: GNEDetectorE3.cpp 19861 2016-02-01 09:08:47Z palcraft $
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
#include <utils/common/MsgHandler.h>

#include "GNEDetectorE3.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNELogo_E3.cpp"
#include "GNELogo_E3Selected.cpp"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// static member definitions
// ===========================================================================
GUIGlID GNEDetectorE3::myDetectorE3GlID = 0;
GUIGlID GNEDetectorE3::myDetectorE3SelectedGlID = 0;
bool GNEDetectorE3::myDetectorE3Initialized = false;
bool GNEDetectorE3::myDetectorE3SelectedInitialized = false;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetectorE3::GNEDetectorE3(const std::string& id, GNEViewNet* viewNet, Position pos, SUMOReal freq, const std::string& filename, bool blocked) :
    GNEAdditionalSet(id, viewNet, pos, SUMO_TAG_E3DETECTOR, blocked),
    myFreq(freq),
    myFilename(filename),
    myCounterId(0) {
    // Update geometry;
    updateGeometry();
    // Set colors
    myBaseColor = RGBColor(76, 170, 50, 255);
    myBaseColorSelected = RGBColor(161, 255, 135, 255);
    // load detector E3 logo, if wasn't inicializated
    if (!myDetectorE3Initialized) {
        FXImage* i = new FXGIFImage(getViewNet()->getNet()->getApp(), GNELogo_E3, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
        myDetectorE3GlID = GUITexturesHelper::add(i);
        myDetectorE3Initialized = true;
        delete i;
    }
    // load detector E3 selected logo, if wasn't inicializated
    if (!myDetectorE3SelectedInitialized) {
        FXImage* i = new FXGIFImage(getViewNet()->getNet()->getApp(), GNELogo_E3Selected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
        myDetectorE3SelectedGlID = GUITexturesHelper::add(i);
        myDetectorE3SelectedInitialized = true;
        delete i;
    }
}


GNEDetectorE3::~GNEDetectorE3() {
}


void
GNEDetectorE3::updateGeometry() {
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
GNEDetectorE3::moveAdditional(SUMOReal posx, SUMOReal posy, GNEUndoList *undoList) {
    // if item isn't blocked
    if(myBlocked == false) {
        // change Position
        undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(Position(posx, posy, 0))));
    }
}


void
GNEDetectorE3::writeAdditional(OutputDevice& device) {
    // Only save E3 if have Entry/Exits
    if(getNumberOfAdditionalChilds() > 0) {
        // Write parameters
        device.openTag(getTag());
        device.writeAttr(SUMO_ATTR_ID, getID());
        device.writeAttr(SUMO_ATTR_FREQUENCY, myFreq);
        if(!myFilename.empty())
            device.writeAttr(SUMO_ATTR_FILE, myFilename);
        device.writeAttr(SUMO_ATTR_X, myPosition.x());
        device.writeAttr(SUMO_ATTR_Y, myPosition.y());
        writeAdditionalChildrens(device);
        // Close tag
        device.closeTag();
    }
    else
        WRITE_WARNING(toString(getTag()) + " with ID = '" + getID() + "' cannot be writed in additional file because don't have childs.");
}


GUIParameterTableWindow*
GNEDetectorE3::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) {
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
GNEDetectorE3::drawGL(const GUIVisualizationSettings& s) const {
    // Start drawing adding an gl identificator
    glPushName(getGlID());

    // Add a draw matrix for drawing logo
    glPushMatrix();
    glTranslated(myShape[0].x(), myShape[0].y(), getType());
    glColor3d(1, 1, 1);
    glRotated(180, 0, 0, 1);

    // Draw icon depending of detector is or isn't selected
    if(isAdditionalSelected()) 
        GUITexturesHelper::drawTexturedBox(myDetectorE3SelectedGlID, 1);
    else
        GUITexturesHelper::drawTexturedBox(myDetectorE3GlID, 1);

    // Pop logo matrix
    glPopMatrix();

    // Show Lock icon depending of the Edit mode
    //if(dynamic_cast<GNEViewNet*>(parent)->showLockIcon())
        drawLockIcon(0.4);

    // Draw connections
    drawConnections();

    // Pop name
    glPopName();

    // Draw name
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
}


std::string
GNEDetectorE3::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_POSITION:
            return toString(myPosition);
        case SUMO_ATTR_FREQUENCY:
            return toString(myFreq);
        case SUMO_ATTR_FILE:
            return myFilename;
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


void
GNEDetectorE3::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying " + toString(getType()) + " attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_FREQUENCY:
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
GNEDetectorE3::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying " + toString(getType()) + " attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_POSITION:
            bool ok;
            return GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, false).size() == 1;
        case SUMO_ATTR_FREQUENCY:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0);
        case SUMO_ATTR_FILE:
            return isValidFileValue(value);
        case SUMO_ATTR_LINES:
            return isValidStringVector(value);
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


void
GNEDetectorE3::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying " + toString(getType()) + " attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_POSITION:
            bool ok;
            myPosition = GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, false)[0];
            updateGeometry();
            getViewNet()->update();
            break;
        case SUMO_ATTR_FREQUENCY:
            myFreq = parse<SUMOReal>(value);
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}

/****************************************************************************/