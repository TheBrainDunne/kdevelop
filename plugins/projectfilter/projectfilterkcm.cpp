/* This file is part of KDevelop
    Copyright 2008 Alexander Dymo <adymo@kdevelop.org>

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
#include "projectfilterkcm.h"

#include <QLayout>
#include <QStandardItemModel>

#include <kgenericfactory.h>
#include <KConfigDialogManager>
#include <KSettings/Dispatcher>

#include <interfaces/icore.h>
#include <interfaces/iplugincontroller.h>
#include <interfaces/iprojectcontroller.h>
#include <interfaces/iproject.h>
#include <project/interfaces/iprojectfilemanager.h>
#include <project/projectmodel.h>

#include "ui_projectfiltersettings.h"

#include "projectfilterdebug.h"
#include "filtermodel.h"
#include "comboboxdelegate.h"

using namespace KDevelop;

K_PLUGIN_FACTORY(ProjectFilterKCMFactory, registerPlugin<ProjectFilterKCM>();)
K_EXPORT_PLUGIN(ProjectFilterKCMFactory("kcm_kdevprojectfilter"))

ProjectFilterKCM::ProjectFilterKCM(QWidget* parent, const QVariantList& args)
    : ProjectKCModule<ProjectFilterSettings>(ProjectFilterKCMFactory::componentData(), parent, args)
    , m_model(new FilterModel(this))
    , m_ui(new Ui::ProjectFilterSettings)
{
    QVBoxLayout *l = new QVBoxLayout(this);
    QWidget *w = new QWidget;

    m_ui->setupUi(w);
    m_ui->filters->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ui->filters->setModel(m_model);
    m_ui->filters->header()->setResizeMode(FilterModel::Pattern, QHeaderView::Stretch);
    m_ui->filters->header()->setResizeMode(FilterModel::MatchOn, QHeaderView::ResizeToContents);
    m_ui->filters->header()->setResizeMode(FilterModel::Targets, QHeaderView::ResizeToContents);
    m_ui->filters->header()->setResizeMode(FilterModel::Inclusive, QHeaderView::ResizeToContents);
    m_ui->filters->setItemDelegateForColumn(FilterModel::MatchOn,
        new ComboBoxDelegate(QVector<ComboBoxDelegate::Item>()
                << ComboBoxDelegate::Item(i18n("relative path"), static_cast<int>(Filter::RelativePath))
                << ComboBoxDelegate::Item(i18n("basename"), static_cast<int>(Filter::Basename))
            , this));
    m_ui->filters->setItemDelegateForColumn(FilterModel::Targets,
        new ComboBoxDelegate(QVector<ComboBoxDelegate::Item>()
                << ComboBoxDelegate::Item(i18n("files"), static_cast<int>(Filter::Files))
                << ComboBoxDelegate::Item(i18n("folders"), static_cast<int>(Filter::Folders))
                << ComboBoxDelegate::Item(i18n("files and folders"), static_cast<int>(Filter::Folders | Filter::Files))
            , this));
    m_ui->filters->setItemDelegateForColumn(FilterModel::Inclusive,
        new ComboBoxDelegate(QVector<ComboBoxDelegate::Item>()
                << ComboBoxDelegate::Item(i18n("exclusive"), false)
                << ComboBoxDelegate::Item(i18n("inclusive"), true)
            , this));
    l->addWidget(w);

    addConfig( ProjectFilterSettings::self(), w );
    load();
    selectionChanged();

    connect(m_ui->filters->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(selectionChanged()));
    connect(this, SIGNAL(changed(bool)), SLOT(selectionChanged()));
    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(emitChanged()));

    connect(m_ui->add, SIGNAL(clicked(bool)), SLOT(add()));
    connect(m_ui->remove, SIGNAL(clicked(bool)), SLOT(remove()));
    connect(m_ui->moveUp, SIGNAL(clicked(bool)), SLOT(moveUp()));
    connect(m_ui->moveDown, SIGNAL(clicked(bool)), SLOT(moveDown()));
}

ProjectFilterKCM::~ProjectFilterKCM()
{
}

void ProjectFilterKCM::save()
{
    writeFilters(m_model->filters(), project()->projectConfiguration());
    project()->projectConfiguration()->sync();

    KSettings::Dispatcher::reparseConfiguration("kdevprojectfilter");
}

void ProjectFilterKCM::load()
{
    m_model->setFilters(readFilters(project()->projectConfiguration()));
}

void ProjectFilterKCM::selectionChanged()
{
    bool hasSelection = m_ui->filters->currentIndex().isValid();
    int row = -1;
    if (hasSelection) {
        row = m_ui->filters->currentIndex().row();
    }
    m_ui->remove->setEnabled(hasSelection);

    m_ui->moveDown->setEnabled(hasSelection && row != m_model->rowCount() - 1);
    m_ui->moveUp->setEnabled(hasSelection && row != 0);
}

void ProjectFilterKCM::add()
{
    m_model->addFilter();
    emit changed(true);
}

void ProjectFilterKCM::remove()
{
    Q_ASSERT(m_ui->filters->currentIndex().isValid());
    m_model->removeFilter(m_ui->filters->currentIndex().row());
    emit changed(true);
}

void ProjectFilterKCM::moveUp()
{
    Q_ASSERT(m_ui->filters->currentIndex().isValid());
    m_model->moveFilterUp(m_ui->filters->currentIndex().row());
    emit changed(true);
}

void ProjectFilterKCM::moveDown()
{
    Q_ASSERT(m_ui->filters->currentIndex().isValid());
    m_model->moveFilterDown(m_ui->filters->currentIndex().row());
    emit changed(true);
}

void ProjectFilterKCM::emitChanged()
{
    emit changed(true);
}

#include "projectfilterkcm.moc"
