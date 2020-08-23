// GGPAD Copyright 2020 Maciej Latocha ( latocha.maciek@gmail.com )
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
#include <QString>

#include <filesystem>
#include <list>
#include <memory>
#include <mutex>
#include <string>

class GuiControllerModel : public QAbstractListModel {
    Q_OBJECT

public:
    struct GamepadInfo {
        uint64_t m_id = 0;
        std::string m_name;
        std::filesystem::path m_scriptPath;
        bool m_isConnected = false;
    };

private:
    std::list<GamepadInfo> m_gamepadInfoList;

public:
    ~GuiControllerModel() = default;
    GuiControllerModel() = default;

    void selectionChanged( const QModelIndex& );
    virtual int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
    virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;
    GamepadInfo& operator [] ( uint64_t );
    void append( GamepadInfo&& );
    void refresh();

signals:
    void selectionChanged( GamepadInfo* );
};
