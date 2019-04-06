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

#include "gui.hpp"

#include <QFontDatabase>
#include <QSplitter>

Gui::Gui( ControllerModel* model )
: QMainWindow( 0 )
, m_list( this )
, m_scriptText( this )
{
    QSplitter* splitterMain = new QSplitter( this );
    setCentralWidget( splitterMain );
    splitterMain->addWidget( &m_list );
    splitterMain->addWidget( &m_scriptText );
    m_list.setModel( model );
    connect( model, &ControllerModel::emitText, &m_scriptText, &QTextEdit::setText );
    connect( &m_list, &QListView::clicked, model, &ControllerModel::selectionChanged );
    m_scriptText.setFont( QFontDatabase::systemFont( QFontDatabase::FixedFont ) );
    resize( 1280, 720 );
    show();
}
