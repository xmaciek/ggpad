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
#include <QString>

#include <memory>
#include <mutex>
class Binding;

class ControllerModel : public QAbstractListModel {
    Q_OBJECT

    std::mutex* m_mutex;
    std::vector<std::unique_ptr<Binding>>* m_bindings;
    Binding* m_currentBinding;

public:
    ControllerModel( std::mutex*, std::vector<std::unique_ptr<Binding>>* );

    virtual int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
    virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;

    void selectionChanged( const QModelIndex& );
    void refreshViews();

    Binding* currentSelection() const;

signals:
    void emitText( const QString& );
};
