/*
 * This file is part of KDevelop
 * Copyright 2013 Milian Wolff <mail@milianw.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "testdeclarations.h"

#include "../parsesession.h"
#include "../declarationbuilder.h"

#include <qtest_kde.h>

#include <tests/testcore.h>
#include <tests/autotestshell.h>
#include <tests/testhelpers.h>

QTEST_KDEMAIN(TestDeclarations, NoGUI);

using namespace KDevelop;

void TestDeclarations::initTestCase()
{
    AutoTestShell::init();
    TestCore::initialize(Core::NoUi);
}

void TestDeclarations::cleanupTestCase()
{
    TestCore::shutdown();
}

void TestDeclarations::testFunction()
{
    const IndexedString file("functionArgs.js");
    //                          0         1         2         3
    //                          01234567890123456789012345678901234567890
    ParseSession session(file, "// some comment\n"
                               "function foo(arg1, arg2, arg3) {}");
    QVERIFY(session.ast());
    QCOMPARE(session.language(), QmlJS::Document::JavaScriptLanguage);

    DeclarationBuilder builder(&session);
    ReferencedTopDUContext top = builder.build(file, session.ast());
    QVERIFY(top);

    DUChainReadLocker lock;

    QCOMPARE(top->localDeclarations().size(), 1);

    FunctionDeclaration* fooDec = dynamic_cast<FunctionDeclaration*>(top->localDeclarations().at(0));
    QVERIFY(fooDec);
    QCOMPARE(fooDec->range(), RangeInRevision(1, 9, 1, 12));
    QCOMPARE(fooDec->comment(), QByteArray(" some comment"));

    QVERIFY(fooDec->internalContext());
    QVERIFY(fooDec->internalFunctionContext());
}

#include "testdeclarations.moc"
