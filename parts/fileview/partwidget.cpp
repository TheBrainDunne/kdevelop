/***************************************************************************
 *   Copyright (C) 2003 by KDevelop authors                                *
 *   kdevelop-devel@kde.org                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <q3whatsthis.h>
#include <q3vbox.h>
#include <qlayout.h>
#include <qtoolbutton.h>
#include <qdom.h>
//Added by qt3to4:
#include <QFocusEvent>
#include <kcombobox.h>
#include <kaction.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>

#include "kdevversioncontrol.h"
#include "kdevcore.h"
#include "kdevproject.h"
#include "kdevmainwindow.h"

#include "fileviewpart.h"
#include "filetreewidget.h"
#include "partwidget.h"

///////////////////////////////////////////////////////////////////////////////
// class PartWidget
///////////////////////////////////////////////////////////////////////////////

PartWidget::PartWidget( FileViewPart *part, QWidget *parent )
    : Q3VBox( parent, "fileviewpartwidget" ), m_filetree( 0 ),
    m_filter( 0 ), m_btnFilter( 0 ), m_part( part )
{
    Q_ASSERT( part && parent );

    KDevVCSFileInfoProvider *infoProvider = 0;
    if (part && part->versionControl() && part->versionControl()->fileInfoProvider())
        infoProvider = part->versionControl()->fileInfoProvider();

    m_filetree = new FileTreeWidget( m_part, this, infoProvider );
    setCaption(i18n("File Tree"));
    m_filetree->setCaption(i18n("File Tree"));
    m_filetree->setIcon(SmallIcon("folder"));
    Q3WhatsThis::add(m_filetree, i18n("<b>File tree</b><p>"
                                    "The file viewer shows all files of the project "
                                    "in a tree layout."));

    Q3HBox* filterBox = new Q3HBox( this );
    m_btnFilter = new QToolButton( filterBox );
    m_btnFilter->setIconSet( SmallIconSet("filter" ) );
    m_btnFilter->setToggleButton( true );
    m_filter = new KHistoryCombo( true, filterBox, "filter");
    m_filter->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ));
    filterBox->setStretchFactor(m_filter, 2);

    connect( m_btnFilter, SIGNAL( clicked() ), this, SLOT( slotBtnFilterClick() ) );
    connect( m_filter, SIGNAL( activated(const QString&) ), this, SLOT( slotFilterChange(const QString&) ) );
    connect( m_filter, SIGNAL( returnPressed(const QString&) ),
             m_filter, SLOT( addToHistory(const QString&) ) );

    Q3WhatsThis::add
        ( m_filter,
                i18n("<p>Here you can enter a name filter to limit which files are <b>not displayed</b>."
                     "<p>To clear the filter, toggle off the filter button to the left."
                     "<p>To reapply the last filter used, toggle on the filter button." ) );
    Q3WhatsThis::add
        ( m_btnFilter,
                i18n("<p>This button clears the name filter when toggled off, or "
                     "reapplies the last filter used when toggled on.") );

    m_filter->insertItem( m_filetree->hidePatterns() );
}

///////////////////////////////////////////////////////////////////////////////

PartWidget::~PartWidget()
{
}

///////////////////////////////////////////////////////////////////////////////

void PartWidget::showProjectFiles()
{
    m_filetree->openDirectory( m_part->project()->projectDirectory() );
    m_filetree->applyHidePatterns( m_filetree->hidePatterns() );
}

///////////////////////////////////////////////////////////////////////////////

void PartWidget::slotFilterChange( const QString & nf )
{
    QString f = nf.stripWhiteSpace();
    bool empty = f.isEmpty() || f == "*";
    if ( empty )
    {
        m_filter->lineEdit()->setText( QString() );
        QToolTip::add( m_btnFilter, i18n("Apply last filter (\"%1\")").arg( m_lastFilter ) );
    }
    else
    {
        m_lastFilter = f;
        QToolTip::add( m_btnFilter, i18n("Clear filter") );
    }
    m_btnFilter->setOn( !empty );
    // this will be never true after the m_filter has been used;)
    m_btnFilter->setEnabled( !( empty && m_lastFilter.isEmpty() ) );

    m_filetree->applyHidePatterns( f );
}

///////////////////////////////////////////////////////////////////////////////
/*
   When the button in the filter box toggles:
   If off:
   If the name filer is anything but "" or "*", reset it.
   If on:
   Set last filter.
*/
void PartWidget::slotBtnFilterClick()
{
    if ( !m_btnFilter->isOn() )
    {
        slotFilterChange( QString() );
    }
    else
    {
        m_filter->lineEdit()->setText( m_lastFilter );
        slotFilterChange( m_lastFilter );
    }
}

void PartWidget::focusInEvent( QFocusEvent * )
{
    m_filetree->setFocus();
}

#include "partwidget.moc"
