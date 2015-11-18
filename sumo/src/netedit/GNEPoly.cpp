/****************************************************************************/
/// @file    GNEPoly.cpp
/// @author  Jakob Erdmann
/// @date    Sept 2012
/// @version $Id$
///
// A class for visualizing and editing POIS in netedit (adapted from
// GUIPolygon and NLHandler)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <utility>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/geom/Position.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <netimport/NIImporter_SUMO.h>
#include <netwrite/NWWriter_SUMO.h>
#include "GNENet.h"
#include "GNEEdge.h"
#include "GNEUndoList.h"
#include "GNEViewNet.h"
#include "GNEChange_Attribute.h"
#include "GNEPoly.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================

// ===========================================================================
// method definitions
// ===========================================================================
GNEPoly::GNEPoly(GNENet* net, const std::string& id, const std::string& type, const PositionVector& shape, bool fill,
           const RGBColor& color, SUMOReal layer,
           SUMOReal angle, const std::string& imgFile) :
    GUIPolygon(id, type, color, shape, fill, layer, angle, imgFile),
    GNEAttributeCarrier(SUMO_TAG_POLY),
    myNet(net)
{}


GNEPoly::~GNEPoly() { }


void
GNEPoly::drawGL(const GUIVisualizationSettings& s) const {
    const SUMOReal hintSize = 0.8;
    GUIPolygon::drawGL(s);
    // draw geometry hints
    if (s.scale * hintSize > 1.) { // check whether it is not too small
        RGBColor current = GLHelper::getColor();
        RGBColor darker = current.changedBrightness(-32);
		GLHelper::setColor(darker);
        glPushName(getGlID());
        for (int i = 0; i < (int)myShape.size() - 1; i++) {
            Position pos = myShape[i];
            glPushMatrix();
            glTranslated(pos.x(), pos.y(), GLO_POLYGON + 0.01);            
            GLHelper:: drawFilledCircle(hintSize, 32);
            glPopMatrix();
        }
        glPopName();
    }

}

Position
GNEPoly::moveGeometry(const Position& oldPos, const Position& newPos, bool relative) {
    PositionVector geom = myShape;
    bool changed = GNEEdge::changeGeometry(geom, getMicrosimID(), oldPos, newPos, relative, true);
    if (changed) {
        myShape = geom;
        myNet->refreshElement(this);
        return newPos;
    } else {
        return oldPos;
    }
}



std::string
GNEPoly::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
            break;
        case SUMO_ATTR_TYPE:
            return toString(Polygon::getType());
            break;
        default:
            throw InvalidArgument("POI attribute '" + toString(key) + "' not allowed");
    }
}


void
GNEPoly::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* /* undoList */) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    //switch (key) {
    //case SUMO_ATTR_ID:
    //case SUMO_ATTR_POSITION:
    //case GNE_ATTR_MODIFICATION_STATUS:
    //    undoList->add(new GNEChange_Attribute(this, key, value), true);
    //    break;
    //case SUMO_ATTR_TYPE: {
    //    undoList->p_begin("change junction type");
    //    bool resetConnections = false;
    //    if (SUMOXMLDefinitions::NodeTypes.get(value) == NODETYPE_TRAFFIC_LIGHT) {
    //        // create new traffic light
    //        undoList->add(new GNEChange_TLS(this, 0, true), true);
    //    } else if (myNBNode.getType() == NODETYPE_TRAFFIC_LIGHT) {
    //        // delete old traffic light
    //        // make a copy because we will modify the original
    //        const std::set<NBTrafficLightDefinition*> tls = myNBNode.getControllingTLS();
    //        for (std::set<NBTrafficLightDefinition*>::iterator it=tls.begin(); it!=tls.end(); it++) {
    //            undoList->add(new GNEChange_TLS(this, *it, false), true);
    //        }
    //    }
    //    // must be the final step, otherwise we do not know which traffic lights to remove via GNEChange_TLS
    //    undoList->add(new GNEChange_Attribute(this, key, value), true);
    //    undoList->p_end();
    //    break;
    //}
    //default:
    throw InvalidArgument("POI attribute '" + toString(key) + "' not allowed");
    //}
}


bool
GNEPoly::isValid(SumoXMLAttr key, const std::string& /* value */) {
    //switch (key) {
    //case SUMO_ATTR_ID:
    //    return isValidID(value) && myNet->retrieveJunction(value, false) == 0;
    //    break;
    //case SUMO_ATTR_TYPE:
    //    return SUMOXMLDefinitions::NodeTypes.hasString(value);
    //    break;
    //case SUMO_ATTR_POSITION:
    //    bool ok;
    //    return GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, false).size() == 1;
    //    break;
    //default:
    throw InvalidArgument("POI attribute '" + toString(key) + "' not allowed");
    //}
}


// ===========================================================================
// private
// ===========================================================================

void
GNEPoly::setAttribute(SumoXMLAttr key, const std::string& /* value */) {
    //switch (key) {
    //case SUMO_ATTR_ID:
    //    myNet->renameJunction(this, value);
    //    break;
    //case SUMO_ATTR_TYPE: {
    //    myNBNode.reinit(myNBNode.getPosition(), SUMOXMLDefinitions::NodeTypes.get(value));
    //    break;
    //}
    //case SUMO_ATTR_POSITION:
    //    bool ok;
    //    myOrigPos = GeomConvHelper::parseShapeReporting(value, "netedit-given", 0, ok, false)[0];
    //    move(myOrigPos);
    //    break;
    //default:
    throw InvalidArgument("POI attribute '" + toString(key) + "' not allowed");
    //}
}


// ===========================================================================
// GNEPolyHandler methods
// ===========================================================================


/****************************************************************************/
