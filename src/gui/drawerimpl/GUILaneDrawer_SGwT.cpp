//---------------------------------------------------------------------------//
//                        GUILaneDrawer_SGwT.cpp -
//  A class for vehicle drawing; Vehicles are drawn as triangles
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 02.09.2003
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//


//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.1  2003/09/05 14:50:39  dkrajzew
// implementations of artefact drawers moved to folder "drawerimpl"
//
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <iostream> // !!!
#include <string> // !!!
#include <microsim/MSEdge.h>
#include <guisim/GUIVehicle.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILaneWrapper.h>
#include "GUILaneDrawer_SGwT.h"
#include <utils/geom/GeomHelper.h>

#include <qgl.h>



/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUILaneDrawer_SGwT::GUILaneDrawer_SGwT(std::vector<GUIEdge*> &edges)
    : GUIBaseLaneDrawer(edges)
{
}


GUILaneDrawer_SGwT::~GUILaneDrawer_SGwT()
{
}


void
GUILaneDrawer_SGwT::drawLane(const GUILaneWrapper &lane,
                             GUISUMOAbstractView::LaneColoringScheme scheme, 
                             double width)
{
    setLaneColor(lane, scheme);
    glPushName(lane.getGlID());
    if(width>1.0) {
        glPushMatrix();
        const Position2D &beg = lane.getBegin();
        glTranslated(beg.x(), beg.y(), 0);
        glRotated( lane.getRotation(), 0, 0, 1 );
        double visLength = -lane.visLength();
        glBegin( GL_QUADS );
        glVertex2f(0-1.5, 0);
        glVertex2f(-1.5, visLength);
        glVertex2f(1.5, visLength);
        glVertex2f(1.5, 0);
        glEnd();
        glBegin( GL_LINES);
        // without the substracted offsets, lines are partially longer
        //  than the boxes
        glVertex2f(0, 0-.1);
        glVertex2f(0, visLength-.1);
        glEnd();
        glPopMatrix();
    } else {
        const Position2D &begin = lane.getBegin();
        const Position2D &end = lane.getEnd();
        glBegin( GL_LINES);
        glVertex2f(begin.x(), begin.y());
        glVertex2f(end.x(), end.y());
        glEnd();
    }
    glPopName();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUILaneDrawer_SGwT.icc"
//#endif

// Local Variables:
// mode:C++
// End:


