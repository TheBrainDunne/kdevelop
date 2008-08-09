/***************************************************************************
 *   Copyright 2001 Bernd Gehrmann <bernd@kdevelop.org>                    *
 *   Copyright 2004-2005 Sascha Cunz <sascha@kdevelop.org>                 *
 *   Copyright 2005 Ian Reinhart Geiser <ian@geiseri.com>                  *
 *   Copyright 2007 Alexander Dymo <adymo@kdevelop.org>                    *
 *   Copyright 2008 Evgeniy Ivanov <powerfox@kde.ru>                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "appwizardplugin.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextCodec>
#include <QTextStream>

#include <ktar.h>
#include <kzip.h>
#include <kdebug.h>
#include <klocale.h>
#include <kaction.h>
#include <ktempdir.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kmacroexpander.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <kactioncollection.h>
#include <kmimetype.h>
#include <kio/copyjob.h>
#include <kio/netaccess.h>

#include <interfaces/icore.h>
#include <interfaces/iprojectcontroller.h>
#include <interfaces/iplugincontroller.h>
#include <vcs/vcsmapping.h>
#include <vcs/vcslocation.h>
#include <vcs/vcsjob.h>
#include <vcs/interfaces/icentralizedversioncontrol.h>
#include <vcs/interfaces/idistributedversioncontrol.h>

#include "appwizarddialog.h"
#include "projectselectionpage.h"
#include "projectvcspage.h"
#include "projecttemplatesmodel.h"
#include "importproject.h"

K_PLUGIN_FACTORY(AppWizardFactory,
    registerPlugin<AppWizardPlugin>();
    KComponentData compData = componentData();
    KStandardDirs *dirs = compData.dirs();
    dirs->addResourceType("apptemplates", "data", "kdevappwizard/templates/");
    dirs->addResourceType("apptemplate_descriptions","data", "kdevappwizard/template_descriptions/");
    dirs->addResourceType("appimports", "data", "kdevappwizard/imports/");
    dirs->addResourceType("appimportfiles", "data", "kdevappwizard/importfiles/");
    setComponentData(compData);
)
K_EXPORT_PLUGIN(AppWizardFactory("kdevappwizard"))

AppWizardPlugin::AppWizardPlugin(QObject *parent, const QVariantList &)
    :KDevelop::IPlugin(AppWizardFactory::componentData(), parent)
{
    setXMLFile("kdevappwizard.rc");

    QAction *action = actionCollection()->addAction("project_new");
    action->setIcon(KIcon("window-new"));
    action->setText(i18n("&New Project..."));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(slotNewProject()));
    action->setToolTip( i18n("Generate a new project from a template") );
    action->setWhatsThis( i18n("<b>New project</b><p>"
                               "This starts KDevelop's application wizard. "
                               "It helps you to generate a skeleton for your "
                               "application from a set of templates.</p>") );

    action = actionCollection()->addAction( "project_import" );
    action->setIcon(KIcon("document-import"));
    action->setText(i18n( "&Import Existing Project..." ));
    connect( action, SIGNAL( triggered( bool ) ), SLOT( slotImportProject() ) );
    action->setToolTip( i18n( "Import existing project" ) );

    m_templatesModel = new ProjectTemplatesModel(this);
}

AppWizardPlugin::~AppWizardPlugin()
{
}

void AppWizardPlugin::slotNewProject()
{
    m_templatesModel->refresh();
    AppWizardDialog dlg(core()->pluginController(), m_templatesModel);


    if (dlg.exec() == QDialog::Accepted)
    {
        QString project = createProject( dlg.appInfo() );
        if (!project.isEmpty())
            core()->projectController()->openProject(KUrl::fromPath(project));
    }
}

void AppWizardPlugin::slotImportProject()
{
    ImportProject import(this, QApplication::activeWindow());
    import.exec();
}

QString AppWizardPlugin::createProject(const ApplicationInfo& info)
{
    QFileInfo templateInfo(info.appTemplate);
    if (!templateInfo.exists())
        return "";

    QString templateName = templateInfo.baseName();
    kDebug(9010) << "creating project for template:" << templateName << " with VCS:" << info.vcsPluginName;

    QString templateArchive = componentData().dirs()->findResource("apptemplates", templateName + ".zip");
    if( templateArchive.isEmpty() )
    {
        templateArchive = componentData().dirs()->findResource("apptemplates", templateName + ".tar.bz2");
    }

    kDebug(9010) << "Using archive:" << templateArchive;

    if (templateArchive.isEmpty())
        return "";


    KUrl dest = info.location;

    //prepare variable substitution hash
    m_variables.clear();
    m_variables["APPNAME"] = info.name;
    m_variables["APPNAMEUC"] = info.name.toUpper();
    m_variables["APPNAMELC"] = info.name.toLower();
    m_variables["PROJECTDIRNAME"] = dest.fileName();
    m_variables["VERSIONCONTROLPLUGIN"] = info.vcsPluginName;

    KArchive* arch = 0;
    if( templateArchive.endsWith(".zip") )
    {
        arch = new KZip(templateArchive);
    }
    else
    {
        arch = new KTar(templateArchive, "application/x-bzip");
    }
    if (arch->open(QIODevice::ReadOnly))
    {
        KTempDir tmpdir;
        QString unpackDir = tmpdir.name(); //the default value for all Centralized VCS
        IPlugin* plugin = core()->pluginController()->loadPlugin( info.vcsPluginName );
        if( info.vcsPluginName.isEmpty() || ( plugin && plugin->extension<KDevelop::IDistributedVersionControl>() ) )
        {
            if( !QFileInfo( dest.toLocalFile() ).exists() )
            {
                QDir::root().mkdir( dest.toLocalFile() );
            }
            unpackDir = dest.toLocalFile(); //in DVCS we unpack template directly to the project's directory
        }
        else
        {
            KUrl parentdir = dest;
            parentdir.cd( ".." );
            if( !QFileInfo( parentdir.toLocalFile() ).exists() )
            {
                QDir::root().mkdir( parentdir.toLocalFile() );
            }
        }

        if ( !unpackArchive( arch->directory(), unpackDir ) )
        {
            QString errorMsg = i18n("Could not create new project");
            return vcsError(errorMsg, tmpdir, KUrl(unpackDir));
        }

        if( !info.vcsPluginName.isEmpty() && plugin )
        {
            //If We use DVCS?
            if (plugin->extension<KDevelop::IDistributedVersionControl>() )
            {
                KDevelop::IDistributedVersionControl* iface =
                        plugin->extension<KDevelop::IDistributedVersionControl>();
                kDebug(9010) << "DVCS system is used, just initializing DVCS";

                //TODO: check if we want to handle KDevelop project files (like now) or only SRC dir
                KDevelop::VcsJob* job = iface->init(dest.toLocalFile());
                if (!job || !job->exec() || job->status() != KDevelop::VcsJob::JobSucceeded)
                {
                    QString errorMsg = i18n("Could not initialize DVCS repository");
                    return vcsError(errorMsg, tmpdir, dest);
                }
                kDebug(9010) << "Initializing DVCS repository:" << dest.toLocalFile();

                job = iface->add(KUrl::List(dest),
                                 KDevelop::IBasicVersionControl::Recursive);
                if (!job || !job->exec() || job->status() != KDevelop::VcsJob::JobSucceeded)
                {
                    QString errorMsg = i18n("Could not add files to the DVCS repository");
                    return vcsError(errorMsg, tmpdir, dest);
                }
                job = iface->commit(QString("initial project import from KDevelop"), KUrl::List(dest),
                                    KDevelop::IBasicVersionControl::Recursive);
                if (!job || !job->exec() || job->status() != KDevelop::VcsJob::JobSucceeded)
                {
                    QString errorMsg = i18n("Could not import project into");
                    return vcsError(errorMsg + iface->name(), tmpdir, dest);
                }
            }
            else
            {

                KDevelop::VcsMapping import;
                KDevelop::VcsLocation srcloc = info.importInformation.sourceLocations().first();
                import.addMapping( KDevelop::VcsLocation( KUrl( tmpdir.name() ) ),
                                    info.importInformation.destinationLocation( srcloc ),
                                    info.importInformation.mappingFlag( srcloc ) );

                KDevelop::ICentralizedVersionControl* iface = plugin->extension<KDevelop::ICentralizedVersionControl>();
                kDebug(9010) << "importing" << srcloc.localUrl() << "to" << import.destinationLocation(  KDevelop::VcsLocation( KUrl( tmpdir.name()))).repositoryServer();
                kDebug(9010) << "Using temp dir" << tmpdir.name() << import.sourceLocations().first().localUrl();
                kDebug(9010) << "Checking out" << info.checkoutInformation.sourceLocations().first().repositoryServer() << "To" << info.checkoutInformation.destinationLocation(info.checkoutInformation.sourceLocations().first()).localUrl();
                
                KDevelop::VcsJob* job = iface->import( import, info.importCommitMessage );
                if(job && job->exec() && job->status() == KDevelop::VcsJob::JobSucceeded )
                {
                    KDevelop::VcsJob* job = iface->checkout( info.checkoutInformation );
                    if (!job || !job->exec() || job->status() != KDevelop::VcsJob::JobSucceeded )
                    {
                        QString errorMsg = i18n("Could not checkout imported project");
                        return vcsError(errorMsg, tmpdir, dest);
                    }
                }
                else
                {
                    QString errorMsg = i18n("Could not import project");
                    return vcsError(errorMsg, tmpdir, dest);
                }
            }
        }
        else
        {
            //This should never happen, the vcs dialog presented a list of vcs
            //systems and now the chosen system doesn't exist anymore??
            tmpdir.unlink();
            return "";
        }
        tmpdir.unlink();
    }else
    {
        kDebug(9010) << "failed to open template archive";
        return "";
    }

    kDebug(9010) << "Returning" << QDir::cleanPath(dest.toLocalFile() + '/' + info.name.toLower() + ".kdev4");
    return QDir::cleanPath(dest.toLocalFile() + '/' + dest.fileName() + ".kdev4");
}

bool AppWizardPlugin::unpackArchive(const KArchiveDirectory *dir, const QString &dest)
{
    kDebug(9010) << "unpacking dir:" << dir->name() << "to" << dest;
    QStringList entries = dir->entries();
    kDebug(9010) << "entries:" << entries.join(",");

    //This extra tempdir is needed just for the files files have special names,
    //which may contain macros also files contain content with macros. So the
    //easiest way to extract the files from the archive and then rename them
    //and replace the macros is to use a tempdir and copy the file (and
    //replacing while copying). This also allows to easily remove all files, by
    //just unlinking the tempdir
    KTempDir tdir;

    bool ret = true;

    foreach (QString entry, entries)
    {
        if (entry.endsWith(".kdevtemplate"))
            continue;
        if (dir->entry(entry)->isDirectory())
        {
            const KArchiveDirectory *file = (KArchiveDirectory *)dir->entry(entry);
            QString newdest = dest+"/"+file->name();
            if( !QFileInfo( newdest ).exists() )
            {
                QDir::root().mkdir( newdest  );
            }
            ret |= unpackArchive(file, newdest);
        }
        else if (dir->entry(entry)->isFile())
        {
            const KArchiveFile *file = (KArchiveFile *)dir->entry(entry);
            file->copyTo(tdir.name());
            QString destName = dest + '/' + file->name();
            if (!copyFileAndExpandMacros(QDir::cleanPath(tdir.name()+'/'+file->name()),
                    KMacroExpander::expandMacros(destName, m_variables)))
            {
                KMessageBox::sorry(0, i18n("The file %1 cannot be created.", dest));
                return false;
            }
        }
    }
    tdir.unlink();
    return ret;
}

bool AppWizardPlugin::copyFileAndExpandMacros(const QString &source, const QString &dest)
{
    kDebug(9010) << "copy:" << source << "to" << dest;
    if( KMimeType::isBinaryData(source) ) 
    {
        KIO::CopyJob* job = KIO::copy( KUrl(source), KUrl(dest), KIO::HideProgressInfo );
        if( !job->exec() )
        {
            return false;
        }
        return true;
    } else 
    {
        QFile inputFile(source);
        QFile outputFile(dest);
    
    
        if (inputFile.open(QFile::ReadOnly) && outputFile.open(QFile::WriteOnly))
        {
            QTextStream input(&inputFile);
            input.setCodec(QTextCodec::codecForName("UTF-8"));
            QTextStream output(&outputFile);
            output.setCodec(QTextCodec::codecForName("UTF-8"));
            while(!input.atEnd())
            {
                QString line = input.readLine();
                output << KMacroExpander::expandMacros(line, m_variables) << "\n";
            }
            // Preserve file mode...
            struct stat fmode;
            ::fstat(inputFile.handle(), &fmode);
            ::fchmod(outputFile.handle(), fmode.st_mode);
            return true;
        }
        else
        {
            inputFile.close();
            outputFile.close();
            return false;
        }
    }
}

QString AppWizardPlugin::vcsError(const QString &errorMsg, KTempDir &tmpdir, const KUrl &dest)
{
    KMessageBox::error(0, errorMsg);
    KIO::NetAccess::del(dest, 0);
    tmpdir.unlink();
    return "";
}

#include "appwizardplugin.moc"
