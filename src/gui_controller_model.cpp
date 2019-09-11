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

int ControllerModel::rowCount( const QModelIndex& parent ) const
{
    std::lock_guard<std::recursive_mutex> lg( m_mutex );
    return m_bindings.size();
}

QVariant ControllerModel::data( const QModelIndex& index, int role ) const
{
    std::lock_guard<std::recursive_mutex> lg( m_mutex );
    assert( index.row() < m_bindings.size() );

    switch ( role ) {
        case Qt::DisplayRole:
            assert( m_bindings[ index.row() ] );
            return QString( "%1\n0x%2" )
                .arg( m_bindings[ index.row() ]->gamepadName().c_str() )
                .arg( QString::number( m_bindings[ index.row() ]->m_gamepadId, 16 ).toUpper() )
            ;
        case Qt::DecorationRole:
            return QIcon::fromTheme( "input-gaming" );

        default:
            return QVariant();
    }
}

void ControllerModel::refreshViews( std::vector<Binding*> b )
{
    std::lock_guard<std::recursive_mutex> lg( m_mutex );
    m_bindings = std::move( b );
    emit dataChanged( QModelIndex(), QModelIndex() );
}

void ControllerModel::selectionChanged( const QModelIndex& index )
{
    std::lock_guard<std::recursive_mutex> lg( m_mutex );
    assert( index.row() < m_bindings.size() );
    m_currentBinding = m_bindings[ index.row() ];
    QString text;
    if ( m_currentBinding && m_currentBinding->m_script ) {
        text = m_currentBinding->m_script->text().c_str();
    }
    emit emitText( text );
}

Binding* ControllerModel::currentSelection() const
{
    return m_currentBinding;
}
