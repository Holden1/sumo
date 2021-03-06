/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    TraCI_POI.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @date    30.05.2012
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
#ifndef TraCI_POI_h
#define TraCI_POI_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <traci-server/TraCIDefs.h>


// ===========================================================================
// class declarations
// ===========================================================================
class PointOfInterest;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCI_POI
 * @brief C++ TraCI client API implementation
 */
class TraCI_POI {
public:
    static std::vector<std::string> getIDList();
    static int getIDCount();
    static std::string getType(const std::string& poiID);
    static TraCIPosition getPosition(const std::string& poiID);
    static TraCIColor getColor(const std::string& poiID);
    static std::string getParameter(const std::string& poiID, const std::string& param);

    static void setType(const std::string& poiID, const std::string& setType);
    static void setColor(const std::string& poiID, const TraCIColor& c);
    static void setPosition(const std::string& poiID, const TraCIPosition& pos);
    static bool add(const std::string& poiID, const TraCIPosition& pos, const TraCIColor& c, const std::string& type, int layer);
    static bool remove(const std::string& poiID, int layer = 0);

    static void subscribe(const std::string& objID, SUMOTime beginTime, SUMOTime endTime, const std::vector<int>& vars);
    static void subscribeContext(const std::string& objID, SUMOTime beginTime, SUMOTime endTime, int domain, double range, const std::vector<int>& vars);
    static void setParameter(const std::string& poiID, const std::string& param, const std::string& value);

private:
    static PointOfInterest* getPoI(const std::string& id);

    /// @brief invalidated standard constructor
    TraCI_POI();

    /// @brief invalidated copy constructor
    TraCI_POI(const TraCI_POI& src);

    /// @brief invalidated assignment operator
    TraCI_POI& operator=(const TraCI_POI& src);
};


#endif

/****************************************************************************/

