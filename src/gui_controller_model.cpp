// GGPAD Copyright 2020 Maciej Latocha
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

#include "gui_controller_model.hpp"

#include <QIcon>

int GuiControllerModel::rowCount( const QModelIndex& parent ) const
{
    return m_gamepadInfoList.size();
}

QVariant GuiControllerModel::data( const QModelIndex& index, int role ) const
{
    switch ( role ) {
    case Qt::DisplayRole: {
        assert( index.row() < m_gamepadInfoList.size() );
        std::list<GamepadInfo>::const_iterator it = m_gamepadInfoList.begin();
        std::advance( it, index.row() );
        return QString( "%1%2" )
            .arg( it->m_name.c_str() )
            .arg( it->m_isConnected ? "" : "\n(disconnected)" )
        ;
    }
    case Qt::DecorationRole:
        return QIcon::fromTheme( "input-gaming" );

    default:
        return QVariant();
    }
}

GuiControllerModel::GamepadInfo& GuiControllerModel::operator [] ( uint64_t id )
{
    const std::list<GamepadInfo>::iterator it = std::find_if(
        m_gamepadInfoList.begin()
        , m_gamepadInfoList.end()
        , [id]( const GamepadInfo& gp ) { return gp.m_id == id; }
    );
    if ( it != m_gamepadInfoList.end() ) {
        return *it;
    }
    return m_gamepadInfoList.emplace_back();
}

void GuiControllerModel::refresh()
{
    emit dataChanged( QModelIndex(), QModelIndex() );
}

void GuiControllerModel::selectionChanged( const QModelIndex& index )
{
    assert( index.row() < m_gamepadInfoList.size() );
    std::list<GamepadInfo>::iterator it = m_gamepadInfoList.begin();
    std::advance( it, index.row() );
    emit selectionChanged( &(*it) );
}
