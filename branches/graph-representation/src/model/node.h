/***************************************************************************
 *   Copyright (C) 2007 by Sergejs   *
 *   sergey.melderis@gmail.com   *
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
#ifndef NODE_H
#define NODE_H

#include <QObject>
        
class WordGraph;        
class Edge;
/**
    @author Sergejs Melderis <sergey.melderis@gmail.com>
*/

class Node : public QObject
{
Q_OBJECT

public:
    Node(const QString &id, WordGraph *parent);
    virtual ~Node();
    
    friend class WordGraph;
    
    WordGraph * graph() const;

    QString id() const;

    QList<Edge *> edges() const;
    void addEdge(Edge *edge);
    
    unsigned int degree() const;

    void setFixed(bool fixed);
    bool fixed() const;
    
    virtual QString toString() const;
    
private:
    QList<Edge *> m_edges;
    
    bool m_fixed;
    QString m_id;
};



class WordNode : public Node
{
Q_OBJECT
public:
    WordNode(const QString &id, WordGraph *parent);
    ~WordNode();

    QString word() const;
private:
    
};

class MeaningNode : public Node
{
Q_OBJECT
public:
    MeaningNode(const QString &id, WordGraph *parent);
    ~MeaningNode();

    void setMeaning(const QString &meaning);
    QString meaning() const;
    
    int partOfSpeech() const;
    void setPartOfSpeech(int pos);

//    virtual QString toString() const;
private:
    QString m_meaning;
    int m_pos;
};






#endif