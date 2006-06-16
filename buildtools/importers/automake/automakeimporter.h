/* KDevelop Automake Support
 *
 * Copyright (C)  2005  Matt Rogers <mattr@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef AUTOMAKEIMPORTER_H
#define AUTOMAKEIMPORTER_H

#include <QList>
#include <QString>
#include "kdevbuildmanager.h"

class QObject;
class KDevProject;
class KDevProjectItem;
class AutoMakeItem;
class MakefileInterface;
class QDir;
class KDevProjectFolderItem;
class KDevProjectFileItem;
class KDevProjectTargetItem;
class KDevProjectBuilder;


class AutoMakeImporter : public KDevBuildManager
{
public:
	AutoMakeImporter( QObject* parent = 0, const QStringList& args = QStringList() );

	virtual ~AutoMakeImporter();

	virtual KDevProject* project() const;
    virtual KDevProjectBuilder* builder() const { return 0; }
    virtual QStringList includeDirectories() const { return QStringList(); }
    virtual KUrl::List preprocessorDefines() const { return KUrl::List(); }

	virtual KDevProjectFolderItem* addFolder( const KUrl& /*folder */,
	                        KDevProject* /*parent*/ ) { return false; }

    virtual KDevProjectTargetItem* createTarget( const QString&,
                                                 KDevProjectFolderItem* ) { return false; }

    virtual KDevProjectFileItem* addFile( const KUrl&,
	                                     KDevProjectFolderItem* ) { return false; }

    virtual bool addFileToTarget( KDevProjectFileItem*, KDevProjectTargetItem* ) { return false; }

	virtual bool removeFolder( KDevProjectFolderItem* ) { return false; }

    virtual bool removeTarget( KDevProjectTargetItem* ) { return false; }

    virtual bool removeFile( KDevProjectFileItem*,
	                         KDevProjectFolderItem* ) { return false; }
	virtual bool removeFileFromTarget( KDevProjectFileItem*,
	                                   KDevProjectTargetItem* ) { return false; }

    QList<KDevProjectTargetItem*> targets() const;

	virtual QList<KDevProjectFolderItem*> parse( KDevProjectFolderItem* dom );
	virtual KDevProjectItem* import( KDevProjectModel* model,
	                                 const QString& fileName );
	virtual KUrl findMakefile( KDevProjectFolderItem* dom ) const;
    virtual KUrl::List findMakefiles( KDevProjectFolderItem* dom ) const;

	void createProjectItems( const KUrl&, KDevProjectItem* );

private:
	KDevProject* m_project;
	KDevProjectItem* m_rootItem;

	MakefileInterface* m_interface;
};

#endif
// kate: indent-mode cstyle; space-indent off; tab-width 4; auto-insert-doxygen on;

