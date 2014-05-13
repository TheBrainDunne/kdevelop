/*
 * This file is part of KDevelop
 *
 * Copyright 2014 Sergey Kalinichev <kalinichev.so.0@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <language/interfaces/idefinesandincludesmanager.h>

class KConfig;
namespace KDevelop
{
class IProject;
}

using KDevelop::ConfigEntry;
/// @brief: Class responsible for reading/writing custom defines and includes from/to the disk.
class SettingsManager
{
public:
    SettingsManager();

    QList<ConfigEntry> readSettings(KConfig* cfg) const;

    void writeSettings(KConfig* cfg, const QList<ConfigEntry>& paths) const;

    /// @return name of the selected compiler, empty string if none is selected.
    QString currentCompiler(KConfig* cfg);

    ~SettingsManager(){}
};

#endif // SETTINGSMANAGER_H
