/***************************************************************************
 *   Copyright (C) 2001 by Daniel Engelschalt                              *
 *   daniel.engelschalt@gmx.net                                            *
 *   Copyright (C) 2004 Jonas Jacobi<j.jacobi@gmx.de>                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// qt includes
#include <qtabwidget.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qmultilineedit.h>
#include <qslider.h>
#include <qheader.h>
#include <qcolor.h>
#include <qlabel.h>

// kde includes
#include <kdevproject.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kfileitem.h>
#include <kurlrequester.h>
#include <klistview.h>
#include <knuminput.h>
#include <kmainwindow.h>
#include <klineedit.h>
// kdevelop includes
#include <domutil.h>
#include <kdevcoderepository.h>
#include <kdevmainwindow.h>
#include <kdevcoderepository.h>
#include <catalog.h>

#include "cppsupportfactory.h"
#include "ccconfigwidget.h"
#include "cppsupportpart.h"
#include "cppcodecompletionconfig.h"
#include "createpcsdialog.h"
#include "creategettersetterconfiguration.h"

using namespace std;


CCConfigWidget::CCConfigWidget( CppSupportPart* part, QWidget* parent, const char* name )
    : CCConfigWidgetBase( parent, name )
{
    m_pPart = part;
    connect( m_pPart->codeRepository(), SIGNAL(catalogRegistered(Catalog* )), this, SLOT(catalogRegistered(Catalog* )) );
    connect( m_pPart->codeRepository(), SIGNAL(catalogUnregistered(Catalog* )), this, SLOT(catalogUnregistered(Catalog* )) );

    initGeneralTab( );
    initCodeCompletionTab( );
	initGetterSetterTab( );
    inputCodeCompletion->setRange( 0, 2000, 100, false );
    inputArgumentsHint->setRange( 0, 2000, 100, false );
}

void CCConfigWidget::initGeneralTab( )
{
    QDomDocument dom = *m_pPart->projectDom();
    interface_suffix->setText(DomUtil::readEntry(dom, "/cppsupportpart/filetemplates/interfacesuffix", ".h"));
    implementation_suffix->setText(DomUtil::readEntry(dom, "/cppsupportpart/filetemplates/implementationsuffix", ".cpp"));

	KConfig *config = CppSupportFactory::instance()->config();
	if (config)
	{
		config->setGroup("General");
		m_switchShouldMatch->setChecked( config->readBoolEntry("SwitchShouldMatch", true ) );
		m_showContextMenuExplosion->setChecked( config->readBoolEntry("ShowContextMenuExplosion", false ) );
	}
}

CCConfigWidget::~CCConfigWidget( )
{
}

void CCConfigWidget::accept( )
{
    saveFileTemplatesTab();
    saveCodeCompletionTab();
	saveGetterSetterTab();
}

void CCConfigWidget::saveFileTemplatesTab( )
{
    QDomDocument dom = *m_pPart->projectDom();
    DomUtil::writeEntry(dom, "/cppsupportpart/filetemplates/interfacesuffix",interface_suffix->text());
    DomUtil::writeEntry(dom, "/cppsupportpart/filetemplates/implementationsuffix",implementation_suffix->text());

	KConfig *config = CppSupportFactory::instance()->config();
	if (config)
	{
		config->setGroup("General");
		config->writeEntry("SwitchShouldMatch", m_switchShouldMatch->isChecked() );
		config->writeEntry("ShowContextMenuExplosion", m_showContextMenuExplosion->isChecked() );
	}
}

void CCConfigWidget::initCodeCompletionTab( )
{
    advancedOptions->header()->hide();

    CppCodeCompletionConfig* c = m_pPart->codeCompletionConfig();

    inputCodeCompletion->setValue( c->codeCompletionDelay() );
    inputArgumentsHint->setValue( c->argumentsHintDelay() );
    checkAutomaticCodeCompletion->setChecked( c->automaticCodeCompletion() );
    checkAutomaticArgumentsHint->setChecked( c->automaticArgumentsHint() );

    m_includeGlobalFunctions->setChecked( c->includeGlobalFunctions() );
    m_includeTypes->setChecked( c->includeTypes() );
    m_includeEnums->setChecked( c->includeEnums() );
    m_includeTypedefs->setChecked( c->includeTypedefs() );
 
    QValueList<Catalog*> catalogs = m_pPart->codeRepository()->registeredCatalogs();
    for( QValueList<Catalog*>::Iterator it=catalogs.begin(); it!=catalogs.end(); ++it )
    {
        Catalog* c = *it;
        QFileInfo dbInfo( c->dbName() );
        QCheckListItem* item = new QCheckListItem( advancedOptions, dbInfo.baseName(), QCheckListItem::CheckBox );
        item->setOn( c->enabled() );

        m_catalogs[ item ] = c;
    }
}

void CCConfigWidget::saveCodeCompletionTab( )
{
    CppCodeCompletionConfig* c = m_pPart->codeCompletionConfig();

    c->setCodeCompletionDelay( inputCodeCompletion->value() );
    c->setArgumentsHintDelay( inputArgumentsHint->value() );

    c->setAutomaticCodeCompletion( checkAutomaticCodeCompletion->isChecked() );
    c->setAutomaticArgumentsHint( checkAutomaticArgumentsHint->isChecked() );

    c->setIncludeGlobalFunctions( m_includeGlobalFunctions->isChecked() );
    c->setIncludeTypes( m_includeTypes->isChecked() );
    c->setIncludeEnums( m_includeEnums->isChecked() );
    c->setIncludeTypedefs( m_includeTypedefs->isChecked() );

    for( QMap<QCheckListItem*, Catalog*>::Iterator it=m_catalogs.begin(); it!=m_catalogs.end(); ++it )
    {
	it.data()->setEnabled( it.key()->isOn() );
    }
    
    c->store();
}

void CCConfigWidget::slotNewPCS( )
{
    CreatePCSDialog dlg( m_pPart, m_pPart->mainWindow()->main() );
    dlg.importerListView->setFocus();
    dlg.exec();
}

void CCConfigWidget::slotRemovePCS()
{
	if ( !advancedOptions->selectedItem() ) return;
	
	QString db = advancedOptions->selectedItem()->text(0);
	QString question = i18n("Are you sure you want to remove the \"%1\" database?").arg( db );
	
	KStandardDirs *dirs =  m_pPart->instance()->dirs();
	QString dbName = dirs->saveLocation( "data", "kdevcppsupport/pcs" ) + "/" + db + ".db";
	
        if ( KMessageBox::Continue == KMessageBox::warningContinueCancel( 0, question, i18n("Remove Database"), KStdGuiItem::del() ) )
	{
		m_pPart->removeCatalog( dbName );
	}
}

void CCConfigWidget::catalogRegistered( Catalog * c )
{
    QFileInfo dbInfo( c->dbName() );
    QCheckListItem* item = new QCheckListItem( advancedOptions, dbInfo.baseName(), QCheckListItem::CheckBox );
    item->setOn( c->enabled() );

    m_catalogs[ item ] = c;
}

void CCConfigWidget::catalogUnregistered( Catalog * c )
{
    for( QMap<QCheckListItem*, Catalog*>::Iterator it=m_catalogs.begin(); it!=m_catalogs.end(); ++it )
    {
	if( it.data() == c ){
	    QCheckListItem* item = it.key();
	    delete( item );
	    m_catalogs.remove( it );
	    break;
	}
    }
}

void CCConfigWidget::initGetterSetterTab( )
{
	CreateGetterSetterConfiguration* config = m_pPart->createGetterSetterConfiguration();
	
	if (config == 0)
		return;
	
	m_edtGet->setText(config->prefixGet());
	m_edtSet->setText(config->prefixSet());
	m_edtRemovePrefix->setText(config->prefixVariable().join(","));
	m_edtParameterName->setText(config->parameterName());
	
	slotGetterSetterValuesChanged();
}

void CCConfigWidget::slotGetterSetterValuesChanged( )
{
	bool hasError = false;
	if (m_edtParameterName->text().isEmpty())
	{
		m_lblParameterName->setPaletteForegroundColor(QColor("red"));
		m_edtExampleGet->setText("error, missing parametername");
		m_edtExampleSet->setText("error, missing parametername");
		hasError = true;
	}
	
	QString name = m_edtVariableName->text();
	if (name.isEmpty())
	{
		m_lblVariableName->setPaletteForegroundColor(QColor("red"));
		m_edtExampleGet->setText("error, missing variablename");
		m_edtExampleSet->setText("error, missing variablename");
		hasError = true;
	}
	
	if (hasError)
	{
		m_edtExampleGet->setPaletteForegroundColor(QColor("red"));
		m_edtExampleSet->setPaletteForegroundColor(QColor("red"));
		
		return;
	} else
	{
		m_lblVariableName->setPaletteForegroundColor(QColor("black"));
		m_lblParameterName->setPaletteForegroundColor(QColor("black"));
		m_edtExampleGet->setPaletteForegroundColor(QColor("black"));
		m_edtExampleSet->setPaletteForegroundColor(QColor("black"));
	}
	
	QStringList prefixes = QStringList::split(",",m_edtRemovePrefix->text().replace(" ", ""));
	unsigned int len = 0;
	QStringList::ConstIterator theend = prefixes.end();
	for(QStringList::ConstIterator ci = prefixes.begin(); ci != theend; ++ci){
		if (name.startsWith(*ci) && (*ci).length() > len)
			len = (*ci).length();
	}
	
	if (len > 0)
		name.remove(0,len);
	
	QString getName = name;
	if (!m_edtGet->text().isEmpty())
	{
		getName[0] = getName[0].upper();
		getName.prepend(m_edtGet->text());
	}
	
	QString setName = name;
	if (!m_edtSet->text().isEmpty())
	{
		setName[0] = setName[0].upper();
		setName.prepend(m_edtSet->text());
	}
	
	m_edtExampleGet->setText("string " + getName + "() const;");
	m_edtExampleSet->setText("void " + setName + "(const string& " + m_edtParameterName->text() + ");");
}

void CCConfigWidget::saveGetterSetterTab( )
{
	if (m_edtParameterName->text().isEmpty() || m_edtGet->text() == m_edtSet->text())
		return;
	
	CreateGetterSetterConfiguration* config = m_pPart->createGetterSetterConfiguration();
	if (config == 0)
		return;
	
	config->setPrefixGet(m_edtGet->text());
	config->setPrefixSet(m_edtSet->text());
	config->setPrefixVariable(QStringList::split(",",m_edtRemovePrefix->text().replace(" ", "")));
	config->setParameterName(m_edtParameterName->text());
	config->store();
}


#include "ccconfigwidget.moc"
