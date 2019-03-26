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

#include <QIcon>

int ControllerModel::rowCount( const QModelIndex& parent ) const
{
    return 2;
}

QVariant ControllerModel::data( const QModelIndex& index, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            switch ( index.row() ) {
                case 0: return "XBox One S (Wireless)";
                case 1: return "DualShock 4 (Wireless)";
                default: return QVariant();
            }
        case Qt::DecorationRole:
            return QIcon::fromTheme( "input-gaming" );
        default:
            return QVariant();
    }
}
