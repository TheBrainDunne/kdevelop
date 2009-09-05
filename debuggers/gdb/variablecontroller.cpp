/*
 * GDB Debugger Support
 *
 * Copyright 2007 Hamish Rodda <rodda@kde.org>
 * Copyright 2008 Vladimir Prus <ghost@cs.msu.su>
 * Copyright 2009 Niko Sams <niko.sams@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "variablecontroller.h"

#include "gdbcommand.h"
#include "debugsession.h"
#include "stringhelpers.h"
#include "gdbvariable.h"

#include <debugger/variable/variablecollection.h>
#include <debugger/breakpoint/breakpointmodel.h>

#include <interfaces/icore.h>
#include <interfaces/idebugcontroller.h>

using namespace GDBDebugger;

VariableController::VariableController(DebugSession* parent)
    : KDevelop::IVariableController(parent)
{
    Q_ASSERT(parent);
    connect(parent, SIGNAL(programStopped(GDBMI::ResultRecord)), SLOT(programStopped(GDBMI::ResultRecord)));
}

DebugSession *VariableController::debugSession() const
{
    return static_cast<DebugSession*>(const_cast<QObject*>(QObject::parent()));
}

void VariableController::programStopped(const GDBMI::ResultRecord& r)
{
    if (r.hasField("reason") && r["reason"].literal() == "function-finished"
        && r.hasField("gdb-result-var"))
    {
        variableCollection()->watches()->addFinishResult(r["gdb-result-var"].literal());
    } else {
        variableCollection()->watches()->removeFinishResult();
    }
}

void VariableController::update()
{
    if (autoUpdate() & UpdateWatches) {
        variableCollection()->watches()->reinstall();
    }

    if (autoUpdate() & UpdateLocals) {
        updateLocals();
    }

    if ((autoUpdate() & UpdateLocals) ||
        ((autoUpdate() & UpdateWatches) && variableCollection()->watches()->childCount() > 0))
    {
        debugSession()->addCommand(
            new GDBCommand(GDBMI::VarUpdate, "--all-values *", this,
                       &VariableController::handleVarUpdate));
    }
}

void VariableController::handleVarUpdate(const GDBMI::ResultRecord& r)
{
    const GDBMI::Value& changed = r["changelist"];
    for (int i = 0; i < changed.size(); ++i)
    {
        const GDBMI::Value& var = changed[i];
        KDevelop::Variable* v = KDevelop::Variable::findByName(var["name"].literal());
        // v can be NULL here if we've already removed locals after step,
        // but the corresponding -var-delete command is still in the queue.
        if (v) {
            if (var.hasField("type_changed")
                && var["type_changed"].literal() == "true")
            {
                v->deleteChildren();
                v->setHasMore(var["new_num_children"].toInt() != 0);
                v->fetchMoreChildren();
            }

            if (var.hasField("in_scope") && var["in_scope"].literal() == "false")
            {
                v->setInScope(false);
            }
            else
            {
                v->setInScope(true);
                v->setValue(var["value"].literal());
            }
        }
    }
}
class StackListArgumentsHandler : public GDBCommandHandler
{
public:
    StackListArgumentsHandler(QStringList localsName)
        : m_localsName(localsName)
    {}

    virtual void handle(const GDBMI::ResultRecord &r)
    {
        // FIXME: handle error.

        const GDBMI::Value& locals = r["stack-args"][0]["args"];

        for (int i = 0; i < locals.size(); i++) {
            m_localsName << locals[i].literal();
        }
        KDevelop::ICore::self()->debugController()->variableCollection()
                ->locals()->updateLocals(m_localsName);
    }

private:
    QStringList m_localsName;
};

class StackListLocalsHandler : public GDBCommandHandler
{
public:
    StackListLocalsHandler(DebugSession *session)
        : m_session(session)
    {}

    virtual void handle(const GDBMI::ResultRecord &r)
    {
        // FIXME: handle error.

        const GDBMI::Value& locals = r["locals"];

        QStringList localsName;
        for (int i = 0; i < locals.size(); i++) {
            const GDBMI::Value& var = locals[i];
            localsName << var["name"].literal();
        }
        m_session->addCommand(                    //dont'show value, low-frame, high-frame
            new GDBCommand(GDBMI::StackListArguments, "0 0 0",
                        new StackListArgumentsHandler(localsName)));
    }

private:
    DebugSession *m_session;
};

void VariableController::updateLocals()
{
    debugSession()->addCommand(
        new GDBCommand(GDBMI::StackListLocals, "--all-values",
                        new StackListLocalsHandler(debugSession())));
}


class FetchMoreChildrenHandler : public GDBCommandHandler
{
public:
    FetchMoreChildrenHandler(KDevelop::Variable *variable, DebugSession *session)
        : m_variable(variable), m_session(session), m_activeCommands(1)
    {}

    virtual void handle(const GDBMI::ResultRecord &r)
    {
        if (!m_variable) return;
        --m_activeCommands;
        const GDBMI::Value& children = r["children"];
        for (int i = 0; i < children.size(); ++i) {
            const GDBMI::Value& child = children[i];
            const QString& exp = child["exp"].literal();
            if (exp == "public" || exp == "protected" || exp == "private") {
                ++m_activeCommands;
                m_session->addCommand(
                    new GDBCommand(GDBMI::VarListChildren,
                                QString("--all-values \"%1\"").arg(child["name"].literal()),
                                this/*use again as handler*/));
            } else {
                KDevelop::Variable* var = m_session->variableController()->
                    createVariable(m_variable->model(), m_variable, 
                                   child["exp"].literal());
                var->setTopLevel(false);
                var->setVarobj(child["name"].literal());
                var->setHasMoreInitial(child["numchild"].toInt());
                m_variable->appendChild(var);
                var->setValue(child["value"].literal());
            }
        }

        m_variable->setHasMore(m_activeCommands != 0);
        if (m_activeCommands == 0) {
            delete this;
        }
    }
    virtual bool handlesError() {
        // FIXME: handle error?
        return false;
    }
    virtual bool autoDelete() {
        // we delete ourselve
        return false;
    }

private:
    QPointer<KDevelop::Variable> m_variable;
    DebugSession *m_session;
    int m_activeCommands;
};


void VariableController::fetchMoreChildren(KDevelop::Variable* variable)
{
    debugSession()->addCommand(
        new GDBCommand(GDBMI::VarListChildren,
                    QString("--all-values \"%1\"").arg(variable->varobj()),
                    new FetchMoreChildrenHandler(variable, debugSession())));
}


void VariableController::deleteVar(KDevelop::Variable* variable)
{
    debugSession()->addCommand(
        new GDBCommand(GDBMI::VarDelete, QString("\"%1\"").arg(variable->varobj())));
}

QString VariableController::expressionUnderCursor(KTextEditor::Document* doc, const KTextEditor::Cursor& cursor)
{
    QString line = doc->line(cursor.line());
    int index = cursor.column();
    QChar c = line[index];
    if (!c.isLetterOrNumber() && c != '_')
        return QString();

    int start = Utils::expressionAt(line, index);
    int end = index;
    for (; end < line.size(); ++end)
    {
        QChar c = line[end];
        if (!(c.isLetterOrNumber() || c == '_'))
            break;
    }
    if (!(start < end))
        return QString();

    QString expression(line.mid(start, end-start));
    expression = expression.trimmed();
    return expression;
}


void VariableController::addWatch(KDevelop::Variable* variable)
{
    debugSession()->addCommand(
        new GDBCommand(GDBMI::VarInfoPathExpression,
                        variable->varobj(),
                        this,
                        &VariableController::addWatch));
}

void VariableController::addWatchpoint(KDevelop::Variable* variable)
{
    debugSession()->addCommand(
        new GDBCommand(GDBMI::VarInfoPathExpression,
                        variable->varobj(),
                        this,
                        &VariableController::addWatchpoint));
}

void VariableController::addWatch(const GDBMI::ResultRecord& r)
{
    // FIXME: handle error.
    if (r.reason == "done" &&  !r["path_expr"].literal().isEmpty()) {
        variableCollection()->watches()->add(r["path_expr"].literal());
    }
}

void VariableController::addWatchpoint(const GDBMI::ResultRecord& r)
{
    if (r.reason == "done" && !r["path_expr"].literal().isEmpty()) {
        KDevelop::ICore::self()->debugController()->breakpointModel()->addWatchpoint(r["path_expr"].literal());
    }
}

KDevelop::Variable* VariableController::
createVariable(TreeModel* model, TreeItem* parent, 
               const QString& expression, const QString& display)
{
    return new GdbVariable(model, parent, expression, display);
}


#include "variablecontroller.moc"
