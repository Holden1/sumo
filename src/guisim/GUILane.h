#ifndef GUILane_h
#define GUILane_h
//---------------------------------------------------------------------------//
//                        GUILane.h -
//  A MSLane extended by some values needed by the gui
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
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
// Revision 1.5  2003/07/16 15:24:55  dkrajzew
// GUIGrid now handles the set of things to draw in another manner than GUIEdgeGrid did; Further things to draw implemented
//
// Revision 1.4  2003/07/07 08:14:48  dkrajzew
// first steps towards the usage of a real lane and junction geometry implemented
//
// Revision 1.3  2003/04/15 09:09:13  dkrajzew
// documentation added
//
// Revision 1.2  2003/02/07 10:39:17  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <utility>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/qutils/NewQMutex.h>
#include "GUILaneWrapper.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSVehicle;
class MSNet;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * An extended MSLane. A mechanism to avoid concurrent
 * visualisation and simulation what may cause problems when vehicles
 * disappear is implemented using a mutex.
 */
class GUILane :
    public MSLane {
public:
    /// constructor
    GUILane( MSNet &net, std::string id, double maxSpeed,
        double length, MSEdge* egde, const Position2DVector &shape );

    /// destructor
    ~GUILane();

    /** returns the vector of vehicles locking the lane for simulation first */
    const MSLane::VehCont &getVehiclesLocked();

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
//    void moveNonCriticalSingle();

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
//    void moveCriticalSingle();

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
//    void moveNonCriticalMulti();

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
//    void moveCriticalMulti();

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    void moveNonCritical(/*
        const MSEdge::LaneCont::const_iterator &firstNeighLane,
        const MSEdge::LaneCont::const_iterator &lastNeighLane */);

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    void moveCritical(/*
        const MSEdge::LaneCont::const_iterator &firstNeighLane,
        const MSEdge::LaneCont::const_iterator &lastNeighLane */);

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    void setCritical();

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    bool emit( MSVehicle& newVeh );

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    bool isEmissionSuccess( MSVehicle* aVehicle );

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    void integrateNewVehicle();

    /// allows the processing of vehicles for threads
    void releaseVehicles();

    /// returns the vehicles closing their processing for other threads
    const VehCont &getVehiclesSecure();

    GUILaneWrapper *buildLaneWrapper(GUIGlObjectStorage &idStorage) ;

    friend class GUILaneChanger;

    friend class GUILaneWrapper;

protected:
    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    bool push( MSVehicle* veh );

    /// moves myTmpVehicles int myVehicles after a lane change procedure
    void swapAfterLaneChange();

private:
    /// The mutex used to avoid concurrent updates of the vehicle buffer
    NewQMutex _lock;

    /// The shape of the lane
    Position2DVector myShape;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUILane.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

