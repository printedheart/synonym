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
#ifndef PYTHONDATALOADER_H
#define PYTHONDATALOADER_H

#include "relation.h"
#include "iworddataloader.h"

#include "PythonQt.h"
#include <QObject>
#include <QStringList>        

class WordGraph;
class WordGraphDecorator;
/**
    @author Sergejs <sergey.melderis@gmail.com>
 */
class PythonDataLoader : public QObject, public IWordDataLoader
{
    Q_OBJECT
    public:
        PythonDataLoader(QObject *parent = 0);
    
        ~PythonDataLoader();
    
        virtual WordGraph * createWordGraph(const QString &searchWord, Relation::Types searchTypes);
     
        virtual QStringList words() const;
    
    public slots:
        void stdOut(const QString &str);    
    protected:
        QString m_curWord;
        
    private:
        PythonQtObjectPtr  m_mainContext;
        WordGraphDecorator *m_graphDecorator;    
                
};

#endif


