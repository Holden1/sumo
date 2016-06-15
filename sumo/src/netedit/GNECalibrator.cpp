/****************************************************************************/
/// @file    GNECalibrator.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: GNECalibrator.cpp 19861 2016-02-01 09:08:47Z palcraft $
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

#include "GNECalibrator.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNERouteProbe.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibrator::GNECalibrator(const std::string& id, GNEEdge* edge, GNEViewNet* viewNet, SUMOReal pos, SUMOTime frequency, const std::string& output, bool blocked) :
    GNEAdditional(id, viewNet, Position(pos, 0), SUMO_TAG_CALIBRATOR, NULL, blocked),
    myEdge(edge),
    myFrequency(frequency),
    myOutput(output) {
    // Update geometry;
    updateGeometry();
    // Set Colors
    myBaseColor = RGBColor(255, 255, 50, 0);
    myBaseColorSelected = RGBColor(255, 255, 125, 255);
}


GNECalibrator::~GNECalibrator() {
}


void
GNECalibrator::moveAdditional(SUMOReal, SUMOReal, GNEUndoList*) {
    // This additional cannot be moved
}


void
GNECalibrator::updateGeometry() {
    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();

    // clear Shape
    myShape.clear();

    // Iterate over lanes
    for(int i = 0; i < myEdge->getLanes().size(); i++) {

        // Get shape of lane parent
        myShape.push_back(myEdge->getLanes().at(i)->getShape().positionAtOffset(myEdge->getLanes().at(i)->getPositionRelativeToParametricLenght(myPosition.x())));

        // Obtain first position
        Position f = myShape[i] - Position(1, 0);

        // Obtain next position
        Position s = myShape[i] + Position(1, 0);

        // Save rotation (angle) of the vector constructed by points f and s
        myShapeRotations.push_back(myEdge->getLanes().at(i)->getShape().rotationDegreeAtOffset(myEdge->getLanes().at(i)->getPositionRelativeToParametricLenght(myPosition.x())) * -1);
    }
}


void
GNECalibrator::writeAdditional(OutputDevice& device) {
    // Write parameters
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_EDGE, myEdge->getID());
    device.writeAttr(SUMO_ATTR_POSITION, myPosition.x());
    device.writeAttr(SUMO_ATTR_FREQUENCY, myFrequency);
    device.writeAttr(SUMO_ATTR_OUTPUT, myOutput);
    // Close tag
    device.closeTag();
}


GUIParameterTableWindow*
GNECalibrator::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) {
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
GNECalibrator::drawGL(const GUIVisualizationSettings& s) const {
    // get values
    glPushName(getGlID());
    SUMOReal width = (SUMOReal) 2.0 * s.scale;
    glLineWidth(1.0);
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);

    glPushName(getGlID());
    for (size_t i = 0; i < myShape.size(); ++i) {
        const Position& pos = myShape[i];
        SUMOReal rot = myShapeRotations[i];
        glPushMatrix();
        glTranslated(pos.x(), pos.y(), getType());
        glRotated(rot, 0, 0, 1);
        glTranslated(0, 0, getType());
        glScaled(exaggeration, exaggeration, 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glBegin(GL_TRIANGLES);
        glColor3d(1, .8f, 0);
        // base
        glVertex2d(0 - 1.4, 0);
        glVertex2d(0 - 1.4, 6);
        glVertex2d(0 + 1.4, 6);
        glVertex2d(0 + 1.4, 0);
        glVertex2d(0 - 1.4, 0);
        glVertex2d(0 + 1.4, 6);
        glEnd();

        // draw text
        if (s.scale * exaggeration >= 1.) {
            glTranslated(0, 0, .1);
            glColor3d(0, 0, 0);
            pfSetPosition(0, 0);
            pfSetScale(3.f);
            SUMOReal w = pfdkGetStringWidth("C");
            glRotated(180, 0, 1, 0);
            glTranslated(-w / 2., 2, 0);
            pfDrawString("C");
            glTranslated(w / 2., -2, 0);
        }
        glPopMatrix();
    }
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    glPopName();
}


std::string
GNECalibrator::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_LANE:
            return toString(myEdge->getAttribute(SUMO_ATTR_ID));
        case SUMO_ATTR_POSITION:
            return toString(myPosition.x());
        case SUMO_ATTR_FREQUENCY:
            return toString(myFrequency);
        case SUMO_ATTR_OUTPUT:
            return myOutput;
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


void
GNECalibrator::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying " + toString(getType()) + " attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_FREQUENCY:
        case SUMO_ATTR_OUTPUT:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            updateGeometry();
            break;
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }

}


bool
GNECalibrator::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_POSITION:
            throw InvalidArgument("modifying " + toString(getType()) + " attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_FREQUENCY:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0);
        case SUMO_ATTR_OUTPUT:
            return isValidFileValue(value);
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNECalibrator::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying " + toString(getType()) + " attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_POSITION:
            myPosition = Position(parse<SUMOReal>(value), 0);
            updateGeometry();
            getViewNet()->update();
            break;
        case SUMO_ATTR_FREQUENCY:
            myFrequency = parse<SUMOReal>(value);
            break;
        case SUMO_ATTR_OUTPUT:
            myOutput = value;
            break;
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}

/****************************************************************************/