// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// $Id: Display.hh,v 3.1 2003/08/06 14:37:47 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_BILLARD_DISPLAY_HH
#define CERTI_BILLARD_DISPLAY_HH

class Display
{
public:
    static Display *instance();

    int getHeight() const ;
    void setWindow(int, int);
    void show();

protected:
    Display();

private:
    static Display *myInstance ;

    const int XMAX ; // width
    const int YMAX ; // height

    int x, y ;
};

#endif // CERTI_BILLARD_DISPLAY_HH

// $Id: Display.hh,v 3.1 2003/08/06 14:37:47 breholee Exp $
