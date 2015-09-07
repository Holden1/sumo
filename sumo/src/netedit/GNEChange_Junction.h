/****************************************************************************/
/// @file    GNEChange_Junction.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id: GNEChange_Junction.h 3359 2014-02-22 09:38:00Z behr_mi $
///
// A network change in which a single junction is created or deleted
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#ifndef GNEChange_Junction_h
#define GNEChange_Junction_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <utils/foxtools/fxexdefs.h>
#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;
class GNEJunction;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_Junction
 * A network change in which a single junction is created or deleted
 */
class GNEChange_Junction : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_Junction)

public:
    /** @brief Constructor for creating/deleting a junction
     * @param[in] net The net on which to apply changes
     * @param[in] junction The junction to be created/deleted
     * @param[in] forward Whether to create/delete (true/false)
     */
    GNEChange_Junction(GNENet* net, GNEJunction* junction, bool forward);

    /// @brief Destructor
    ~GNEChange_Junction();

    FXString undoName() const;
    FXString redoName() const;
    void undo();
    void redo();


private:

    /** @brief full information regarding the junction that is to be created/deleted
     * we assume shared responsibility for the pointer (via reference counting)
     */
    GNEJunction* myJunction;
};

#endif
/****************************************************************************/