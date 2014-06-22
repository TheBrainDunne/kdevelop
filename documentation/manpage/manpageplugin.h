/*  This file is part of KDevelop
    Copyright 2010 Yannick Motta <yannick.motta@gmail.com>
    Copyright 2010 Benjamin Port <port.benjamin@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef MANPAGEPLUGIN_H
#define MANPAGEPLUGIN_H

#include "manpagemodel.h"

#include <interfaces/iplugin.h>
#include <interfaces/idocumentationprovider.h>

#include <QVariantList>
#include <QProgressBar>

class ManPageDocumentation;

class ManPagePlugin : public KDevelop::IPlugin, public KDevelop::IDocumentationProvider
{
    Q_OBJECT
    Q_INTERFACES( KDevelop::IDocumentationProvider )
    public:
        ManPagePlugin(QObject *parent, const QVariantList & args= QVariantList());
        virtual ~ManPagePlugin();
        virtual KSharedPtr< KDevelop::IDocumentation > documentationForDeclaration (KDevelop::Declaration* dec) const;
        virtual QAbstractListModel* indexModel() const;
        virtual KSharedPtr< KDevelop::IDocumentation > documentationForIndex(const QModelIndex& index) const;
        virtual QIcon icon() const;
        virtual QString name() const;
        virtual KSharedPtr< KDevelop::IDocumentation > homePage() const;
        void deleteProgressBar();

        ManPageModel* model() const;
        QProgressBar* progressBar() const;

    signals:
        void addHistory( const KSharedPtr< KDevelop::IDocumentation >& doc ) const;

    private:
        ManPageModel *m_model;
};

#endif // MANPAGEPLUGIN_H
