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
#include <QListView>
#include <QMainWindow>
#include <QPointer>
#include <QTextEdit>

class Gui : public QMainWindow {
    Q_OBJECT

private:
    QListView m_list;
    QTextEdit m_scriptText;

public:
    Gui( QPointer<QAbstractListModel> );
};
