#ifndef GUIBaseLaneDrawer_h
#define GUIBaseLaneDrawer_h
//---------------------------------------------------------------------------//
//                        GUIBaseLaneDrawer.h -
//  A class for lane drawing; Lanes are drawn as straight lines
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

#include <map>
#include <utils/gfx/RGBColor.h>
#include <gui/GUISUMOAbstractView.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUILaneWrapper;
class Position2D;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * Draws lanes as simple, one-colored straights
 */
class GUIBaseLaneDrawer : public GUISUMOAbstractView::GUILaneDrawer {
public:
    /// constructor
    GUIBaseLaneDrawer(std::vector<GUIEdge*> &edges);

    /// destructor
    virtual ~GUIBaseLaneDrawer();

    void drawGLLanes(size_t *which, size_t maxEdges,
        double width, GUISUMOAbstractView::LaneColoringScheme scheme);

protected:
    /// initialises the drawing
    virtual void initStep();

    /// draws a single vehicle
    virtual void drawLane(const GUILaneWrapper &lane,
        GUISUMOAbstractView::LaneColoringScheme scheme, double width) = 0;

    /// sets the colour of the vehicle to draw
    void setLaneColor(const GUILaneWrapper &lane,
        GUISUMOAbstractView::LaneColoringScheme scheme);

    void drawBoxLine(const Position2D &beg, double rot,
        double visLength);
    void drawLine(const Position2D &beg, double rot,
        double visLength);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIBaseLaneDrawer.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

