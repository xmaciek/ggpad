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
#include <QStackedWidget>
#include <QTextEdit>
#include <QTimer>
#include <QToolBar>

#include <functional>
#include <string>

#include "comm.hpp"
#include "gui_controller_model.hpp"
#include "binding.hpp"

class Gui : public QMainWindow {
    Q_OBJECT

private:
    GuiControllerModel m_model{};
    GuiControllerModel::GamepadInfo* m_currentInfo = nullptr;
    Binding* m_currentBinding = nullptr;
    QListView m_list{ this };
    QStackedWidget m_editorStack{ this };

    QToolBar m_toolbar{ this };
    QPointer<QAction> m_actionOpen;
    QPointer<QAction> m_actionSave;
    QPointer<QAction> m_actionRun;
    QPointer<QAction> m_actionStop;

    Comm* m_serverComm = nullptr;
    QTimer m_timerServerMessages{ this };
    void processServerMessages();

    std::map<uint64_t, QTextEdit*> m_editorMapStack;
    void selectionChanged( GuiControllerModel::GamepadInfo* );

private slots:
    void onOpen();
    void onSave();
    void onRun();
    void onStop();
    void onUpdate();

public:
    ~Gui();
    Gui( Comm* );

};
