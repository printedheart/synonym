/***************************************************************************
 *   Copyright (C) 2007 by Sergejs Melderis                                *
 *   sergey.melderis@gmail.com                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef LAYOUT_H
#define LAYOUT_H

#include "graphnode.h"
#include "graphedge.h"
#include "graphscene.h"




/**
    @author Sergey Melderis <sergey.melderis@gmail.com>
*/
class Layout
{
public:
    Layout();

    virtual ~Layout() {};

    /**
     * Layouts nodes and edges.
     */
    virtual bool layout() = 0;
    
    virtual void stop() = 0;
    
    virtual void setRestDistance(int distance) { m_restDistance = distance; }
    
    void setScene(GraphScene *scene);
    
    
    // Used by TGLayout
    virtual void startDamper() {};
    virtual void stopDamper() {};
    virtual void resetDamper() {};
    
    
protected:    
    GraphScene *m_scene;
    QRectF m_sceneRect;
    int m_restDistance;
};



 

  

#endif

