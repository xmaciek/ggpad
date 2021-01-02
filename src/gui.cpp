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
#include "gui_syntax_highlight.hpp"

#include <QFileDialog>
#include <QFontDatabase>
#include <QIcon>
#include <QLabel>
#include <QSplitter>
#include <QTextEdit>
#include <QVBoxLayout>

#include <cassert>
#include <filesystem>

Gui::~Gui()
{
    for ( auto& it : m_editorMapStack ) {
        delete it.second;
    }
}

static QByteArray getFileContent( const QString& str )
{
    QFile f( str );
    if ( !f.open( QIODevice::ReadOnly ) ) {
        return {};
    }

    QByteArray ret( f.size(), '\0' );
    f.read( ret.data(), ret.size() );
    return ret;
}

Gui::Gui( Comm* serverComm )
: m_serverComm{ serverComm }
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
    connect( m_actionOpen, &QAction::triggered, this, &Gui::onOpen );

    m_actionSave = m_toolbar.addAction( QIcon::fromTheme( "document-save" ), "Save" );
    connect( m_actionSave, &QAction::triggered, this, &Gui::onSave );

    m_actionRun = m_toolbar.addAction( QIcon::fromTheme( "media-playback-start" ), "Run" );
    connect( m_actionRun, &QAction::triggered, this, &Gui::onRun );

    m_actionStop = m_toolbar.addAction( QIcon::fromTheme( "media-playback-stop" ), "Stop" );
    connect( m_actionStop, &QAction::triggered, this, &Gui::onStop );

    addToolBar( Qt::TopToolBarArea, &m_toolbar );


    splitterMain->addWidget( &m_list );
    splitterMain->addWidget( &m_editorStack );
    splitterMain->setStretchFactor( 0, 1 );
    splitterMain->setStretchFactor( 1, 3 );

    {
        QTextEdit* tmp = new QTextEdit( this );
        tmp->setText( "Nya there!" );
        m_editorStack.addWidget( tmp );
    }

    m_list.setModel( &m_model );

    connect(
        &m_list
        , &QListView::clicked
        , &m_model
        , qOverload<const QModelIndex&>( &GuiControllerModel::selectionChanged )
    );

    connect(
        &m_model
        , qOverload<GuiControllerModel::GamepadInfo*>( &GuiControllerModel::selectionChanged )
        , this
        , &Gui::selectionChanged
    );

    if ( serverComm ) {
        connect( &m_timerServerMessages, &QTimer::timeout, this, &Gui::processServerMessages );
        m_timerServerMessages.start( 100 );
    }

    resize( 1280, 720 );
    show();
}

void Gui::onSave()
{
    if ( !m_currentInfo ) {
        LOG( LOG_DEBUG, "<font color=orange>No selection, this btn should be disabled</font>" );
        return;
    }

    bool isNewScript = false;
    std::string savePath = m_currentInfo->m_scriptPath;
    if ( savePath.empty() ) {
        QPointer<QFileDialog> ptr = new QFileDialog(
            this
            , "Save .lua script"
            , QDir::homePath()
            , "Lua (*.lua)"
        );
        ptr->setViewMode( QFileDialog::Detail );
        ptr->setAcceptMode( QFileDialog::AcceptSave );
        if ( !ptr->exec() ) {
            return;
        }
        const QStringList fileNames = ptr->selectedFiles();
        assert( fileNames.size() == 1 );
        ptr.clear();
        savePath = fileNames[ 0 ].toStdString();
        isNewScript = true;
    }

    QTextEdit* editor = m_editorMapStack[ m_currentInfo->m_id ];
    assert( editor );
    QFile saveFile( savePath.c_str() );
    if ( !saveFile.open( QIODevice::WriteOnly ) ) {
        LOG( LOG_ERROR, "<font color=red>Failed to save file:</font> %s", savePath.c_str() );
        return;
    }

    const std::string str = editor->toPlainText().toStdString();
    saveFile.write( str.c_str(), str.size() );
    if ( !isNewScript ) {
        return;
    }

    assert( m_serverComm );
    m_serverComm->pushToServer( Message{ Message::Type::eUpdateScriptPath, m_currentInfo->m_id, str } );
}

void Gui::onRun()
{
    if ( !m_currentInfo ) {
        LOG( LOG_DEBUG, "<font color=orange>No selection, this btn should be disabled</font>" );
        return;
    }
    assert( m_currentInfo );
    assert( m_serverComm );
    m_serverComm->pushToServer( Message{ Message::Type::eRunScript, m_currentInfo->m_id, m_currentInfo->m_scriptPath } );
    m_serverComm->notifyServer();
}

void Gui::onStop()
{
    if ( !m_currentInfo ) {
        LOG( LOG_DEBUG, "<font color=orange>No selection, this btn should be disabled</font>" );
        return;
    }
    assert( m_currentInfo );
    assert( m_serverComm );
    m_serverComm->pushToServer( Message{ Message::Type::eStopScript, m_currentInfo->m_id } );
    m_serverComm->notifyServer();
}

void Gui::onUpdate()
{
}

void Gui::onOpen()
{
    if ( !m_currentInfo ) {
        LOG( LOG_DEBUG, "<font color=orange>No selection, this btn should be disabled</font>" );
        return;
    }
    QPointer<QFileDialog> ptr = new QFileDialog(
        this
        , "Open .lua script"
        , QDir::homePath()
        , "Lua (*.lua)"
    );
    ptr->setViewMode( QFileDialog::Detail );
    ptr->setAcceptMode( QFileDialog::AcceptOpen );
    if ( !ptr->exec() ) {
        return;
    }
    const QStringList fileNames = ptr->selectedFiles();
    assert( fileNames.size() == 1 );
    ptr.clear();

    m_currentInfo->m_scriptPath = fileNames[ 0 ].toStdString();
    QTextEdit* editor = m_editorMapStack[ m_currentInfo->m_id ];
    assert( editor );
    editor->setText( getFileContent( fileNames[ 0 ] ) );
}

static QTextEdit* createEditor( QWidget* parent, const std::filesystem::path& filePath )
{
    QTextEdit* editor = new QTextEdit( parent );
    new SyntaxHighlight( editor->document() );
    editor->setFont( QFontDatabase::systemFont( QFontDatabase::FixedFont ) );
    editor->setText( getFileContent( filePath.c_str() ) );
    return editor;
}

void Gui::selectionChanged( GuiControllerModel::GamepadInfo* gi )
{
    assert( gi );
    m_currentInfo = gi;
    QTextEdit* editor = m_editorMapStack[ gi->m_id ];
    if ( !editor ) {
        editor = createEditor( this, gi->m_scriptPath );
        assert( editor );
        m_editorStack.addWidget( editor );
        m_editorMapStack[ gi->m_id ] = editor;
    }
    m_editorStack.setCurrentWidget( editor );
}

void Gui::processServerMessages()
{
    assert( m_serverComm );
    while ( const std::optional<Message> msg = m_serverComm->popFromServer() ) {
        switch ( msg->m_type ) {
        case Message::Type::eGamepadConnected:
            m_model[ msg->m_id ] = GuiControllerModel::GamepadInfo{ msg->m_id, msg->toString(), {}, true };
            m_model.refresh();
            break;

        case Message::Type::eUpdateScriptPath:
            m_model[ msg->m_id ].m_scriptPath = msg->toPath();
            m_model.refresh();
            break;

        case Message::Type::eGamepadDisconnected:
            m_model[ msg->m_id ].m_isConnected = false;
            m_model.refresh();
            break;

        default:
            assert( !"unhandled enum" );
            break;
        }
    }
}
