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
#ifndef DBDATALOADER_H
#define DBDATALOADER_H

#include <iworddataloader.h>
#include "relation.h"
#include <QHash>
/**
	@author Sergey Melderis <sergey.melderis@gmail.com>
*/
class DbDataLoader : public QObject, public IWordDataLoader
{
Q_OBJECT
public:
    DbDataLoader(QObject *parent = 0);

    ~DbDataLoader();
    
    virtual WordGraph * createWordGraph(const QString &searchWord, Relation::Types searchTypes);
     
    virtual QStringList words() const;

private:    
    QHash<Relation::Type, int> relTypeLinkId;
    QHash<int, Relation::Type> linkIdRelType;
    
    void checkDatabase();
private slots:    
    void selectDbFile();
};

#endif
