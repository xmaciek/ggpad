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

#include "log.hpp"
#include "logview.hpp"

#include <QFileDialog>
#include <QFontDatabase>
#include <QIcon>
#include <QSplitter>
#include <QVBoxLayout>

Gui::Gui( ControllerModel* model )
: QMainWindow( 0 )
, m_list( this )
, m_scriptText( this )
, m_syntaxHighlight( m_scriptText.document() )
, m_toolbar( this )
{
    LogView* logView = new LogView( this );
    logView->setFont( QFontDatabase::systemFont( QFontDatabase::FixedFont ) );

    QSplitter* splitterMain = new QSplitter( this );
    QSplitter* splitterLog = new QSplitter( Qt::Vertical, this );
    setCentralWidget( splitterLog );
    splitterLog->addWidget( splitterMain );
    splitterLog->addWidget( logView );
    splitterLog->setStretchFactor( 0, 4 );
    splitterLog->setStretchFactor( 1, 1 );

    m_toolbar.setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
    m_actionOpen = m_toolbar.addAction( QIcon::fromTheme( "document-open" ), "Open" );
    connect( m_actionOpen, &QAction::triggered, this, &Gui::onClickOpen );

    m_actionSave = m_toolbar.addAction( QIcon::fromTheme( "document-save" ), "Save" );
    connect( m_actionSave, &QAction::triggered,
             [this]() { if ( m_saveScriptCb ) { m_saveScriptCb(); } }
            );

    m_actionRun = m_toolbar.addAction( QIcon::fromTheme( "media-playback-start"), "Run" );
    connect( m_actionRun, &QAction::triggered,
             [this]() { if ( m_runScriptCb ) { m_runScriptCb(); } }
            );

    m_actionStop = m_toolbar.addAction( QIcon::fromTheme( "media-playback-stop"), "Stop" );
    connect( m_actionStop, &QAction::triggered,
             [this]() { if ( m_stopScriptCb ) { m_stopScriptCb(); } }
            );
    addToolBar( Qt::TopToolBarArea, &m_toolbar );


    splitterMain->addWidget( &m_list );
    splitterMain->addWidget( &m_scriptText );
    splitterMain->setStretchFactor( 0, 1 );
    splitterMain->setStretchFactor( 1, 3 );

    m_list.setModel( model );
    connect( model, &ControllerModel::emitText, &m_scriptText, &QTextEdit::setText );
    connect( &m_list, &QListView::clicked, model, &ControllerModel::selectionChanged );
    m_scriptText.setFont( QFontDatabase::systemFont( QFontDatabase::FixedFont ) );
    resize( 1280, 720 );
    show();
}

void Gui::setOpenCb( const std::function<void()>& foo )
{
    m_openScriptCb = foo;
}

void Gui::setSaveCb( const std::function<void()>& foo )
{
    m_saveScriptCb = foo;
}

void Gui::setRunCb( const std::function<void()>& foo )
{
    m_runScriptCb = foo;
}

void Gui::setStopCb( const std::function<void()>& foo )
{
    m_stopScriptCb = foo;
}

void Gui::setUpdateCb( const std::function<void(const std::string&)>& foo )
{
    m_updateScriptCb = foo;
}

void Gui::onClickOpen()
{
    QPointer<QFileDialog> ptr = new QFileDialog(
        this
        , "Open .lua script"
        , QDir::homePath()
        , "Lua (*.lua)"
    );
    ptr->setViewMode( QFileDialog::Detail );
    if ( !ptr->exec() ) {
        return;
    }
    QStringList fileNames = ptr->selectedFiles();
    ptr.clear();
    assert( m_updateScriptCb );
    for ( const QString& it : fileNames ) {
        m_updateScriptCb( it.toStdString() );
    }
}
