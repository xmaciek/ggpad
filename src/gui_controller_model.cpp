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

#include "gui_controller_model.hpp"

#include "binding.hpp"

#include <QIcon>

ControllerModel::ControllerModel( std::mutex* mtx, std::vector<std::unique_ptr<Binding>>* binds )
: m_mutex( mtx )
, m_bindings( binds )
, m_currentBinding( nullptr )
{
}

int ControllerModel::rowCount( const QModelIndex& parent ) const
{
    if ( !m_mutex ) {
        return 0;
    }
    std::lock_guard<std::mutex> lg( *m_mutex );
    return m_bindings ? m_bindings->size() : 0;
}

QVariant ControllerModel::data( const QModelIndex& index, int role ) const
{
    if ( !m_mutex ) {
        return QVariant();
    }
    std::lock_guard<std::mutex> lg( *m_mutex );
    assert( index.row() < m_bindings->size() );

    switch ( role ) {
        case Qt::DisplayRole: {
            return m_bindings->at( index.row() )->gamepadName().c_str();
        }
        case Qt::DecorationRole:
            return QIcon::fromTheme( "input-gaming" );
        default:
            return QVariant();
    }
}

void ControllerModel::refreshViews()
{
    emit dataChanged( QModelIndex(), QModelIndex() );
}

void ControllerModel::selectionChanged( const QModelIndex& index )
{
    assert( m_mutex );
    std::lock_guard<std::mutex>( *m_mutex );
    assert( m_bindings );
    assert( index.row() < m_bindings->size() );
    QString text;
    if ( m_bindings->at( index.row() )->m_script ) {
        m_currentBinding = m_bindings->at( index.row() ).get();
        text = m_currentBinding->m_script->text().c_str();
    }
    emit emitText( text );
}

Binding* ControllerModel::currentSelection() const
{
    return m_currentBinding;
}
