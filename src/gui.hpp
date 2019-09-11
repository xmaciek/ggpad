// GGPAD Copyright 2019 Maciej Latocha
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <QAbstractListModel>
#include <QAction>
#include <QListView>
#include <QMainWindow>
#include <QPointer>
#include <QTextEdit>
#include <QToolBar>

#include <functional>
#include <string>

#include "binding.hpp"
#include "gui_controller_model.hpp"
#include "gui_syntax_highlight.hpp"

class Gui : public QMainWindow {
    Q_OBJECT

private:
    QListView m_list;
    QTextEdit m_scriptText;
    SyntaxHighlight m_syntaxHighlight;

    QToolBar m_toolbar;
    QPointer<QAction> m_actionOpen;
    QPointer<QAction> m_actionSave;
    QPointer<QAction> m_actionRun;
    QPointer<QAction> m_actionStop;

    std::function<void()> m_openScriptCb;
    std::function<void()> m_saveScriptCb;
    std::function<void()> m_runScriptCb;
    std::function<void()> m_stopScriptCb;
    std::function<void( const std::string& )> m_updateScriptCb;

    void selectionChanged( Binding* );

    void onClickOpen();

public:
    Gui( ControllerModel* );

    void setOpenCb( const std::function<void()>& );
    void setSaveCb( const std::function<void()>& );
    void setRunCb( const std::function<void()>& );
    void setStopCb( const std::function<void()>& );
    void setUpdateCb( const std::function<void(const std::string&)>& );
};
