#ifndef GUIMessageWindow_h
#define GUIMessageWindow_h
//---------------------------------------------------------------------------//
//                        GUIMessageWindow.h -
//  A logging window for the gui
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 25 Nov 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.1  2003/11/26 09:39:13  dkrajzew
// added a logging windows to the gui (the passing of more than a single lane to come makes it necessary)
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <qtextview.h>
#include <qdialog.h>
#include <qmainwindow.h>
#include <string>
#include "GUIEvents.h"


 /* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIMessageWindow
 * This class displays messages incoming to the gui from either the load or
 * the run thread.
 * The text is colored in dependence to its type (messages: green,
 * warnings: yellow, errors: red)
 *
 * Each time a new message is passed, the window is reopened.
 */
class GUIMessageWindow : public QTextView {
    /// is a q-object
    Q_OBJECT
public:
    /// Constructor
    GUIMessageWindow(QWidget *parent);

    /// Destructor
    ~GUIMessageWindow();

    /// Adds a a separator to this log window
    void addSeparator();

    /** @brief Adds new text to the window
        The type of the text is determined by the first parameter */
    void appendText(GUIEvent eType, const std::string &msg);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
