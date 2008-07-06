/***************************************************************************
 *   Copyright (C) 2008 by Sergejs Melderis                                *
 *   sergey.melderis@gmail.com                                             *
 *                                                                         *
 *   The following code is heavily based on TGLayout.java             *
 *   from TouchGraph project http://www.touchgraph.com.                    *
 *   The original java code can be downloaded from                         *
 *   http://sourceforge.net/project/showfiles.php?group_id=30469           *
 *                                                                         * 
 *                                                                         *
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
 **************************************************************************/
 
#ifndef TGGRAPH_LAYOUT_H
#define TGGRAPH_LAYOUT_H

#include "layout.h"

#include <QThread>


class TGLayout : public QThread, public Layout
{
Q_OBJECT    
public:
    TGLayout(QObject *parent = 0);

    ~TGLayout();
    
    static const double REPULSION = 10.0;

    virtual bool layout();

    virtual void stop();
    
    void run();
    
    void resetDamper();
    void startDamper();
    void stopDamper();
signals:
    void updateNodes();        
private:   
    void relax();
    void relaxEdges();
    inline void relaxEdge(GraphicsEdge *edge);

    void avoidLabels();

    void moveNodes();
    inline void moveNode(GraphicsNode *node);
    void damp();
    
    double m_maxMotion;
    double m_lastMaxMotion;
    double m_damper;
    double m_motionRatio; 
    bool m_damping;
    
    volatile bool stopped;
    
private slots:
    void advanceNodes();    
};






#endif


