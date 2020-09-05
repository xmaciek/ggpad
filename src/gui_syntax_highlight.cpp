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

#include "gui_syntax_highlight.hpp"

#define PRE_NUM_TOKENS "[ \\t\\+\\-\\*\\/\\%\\(\\)]"

#define WHOLE( X ) "\\b" X "\\b"
constexpr static const char* LUA_KEYWORDS[] = {
    WHOLE( "and" )
    , WHOLE( "break" )
    , WHOLE( "do" )
    , WHOLE( "else" )
    , WHOLE( "elseif" )
    , WHOLE( "end" )
    , WHOLE( "false" )
    , WHOLE( "for" )
    , WHOLE( "function" )
    , WHOLE( "goto" )
    , WHOLE( "if" )
    , WHOLE( "in" )
    , WHOLE( "local" )
    , WHOLE( "nil" )
    , WHOLE( "not" )
    , WHOLE( "or" )
    , WHOLE( "repeat" )
    , WHOLE( "require" ) // not a Lua keyword, but used as such
    , WHOLE( "return" )
    , WHOLE( "then" )
    , WHOLE( "true" )
    , WHOLE( "until" )
    , WHOLE( "while" )
};
#undef WHOLE

constexpr static const char* GGPAD_KEYWORDS[] = {
    "GGPAD_event"
    , "GGPAD_nativeEvent"
    , "GGPAD_update"
    , "GGPAD_keyboardSet"
    , "GGPAD_mouseMove"
    , "GGPAD_mouseSet"
#define MAKE_ENUM( NAME ) , "Gamepad." #NAME
#include "button_enum.def"
#undef MAKE_ENUM
#define MAKE_ENUM( NAME, VALUE ) , "Keyboard." #NAME
#include "key_enum.def"
#undef MAKE_ENUM
#define MAKE_ENUM( NAME, VALUE ) , "Mouse." #NAME
#define MAKE_ENUM2( NAME, VALUE ) , "Mouse." #NAME
#include "mouse_enum.def"
#undef MAKE_ENUM2
#undef MAKE_ENUM
};

constexpr static const char* OBSOLETE_KEWORDS[] = {
    "WOLOLO"
};

SyntaxHighlight::SyntaxHighlight( QTextDocument* parent )
: QSyntaxHighlighter( parent )
{
    QTextCharFormat formatGGPAD;
    formatGGPAD.setForeground( QColor( "#FDBC4B" ) );
    formatGGPAD.setFontWeight( QFont::Bold );
    for ( const char* it : GGPAD_KEYWORDS ) {
        m_matchPairs.push_back( { QRegularExpression( it ), formatGGPAD, 0 } );
    }

    QTextCharFormat formatLuaKeyword;
    formatLuaKeyword.setForeground( QColor( "#2980B9" ) );
    formatLuaKeyword.setFontWeight( QFont::Bold );
    for ( const char* it : LUA_KEYWORDS ) {
        m_matchPairs.push_back( { QRegularExpression( it ), formatLuaKeyword, 0 } );
    }

    QTextCharFormat formatNumber;
    formatNumber.setForeground( QColor( "#F67400" ) );
    m_matchPairs.push_back( { QRegularExpression( PRE_NUM_TOKENS "[0-9]+" ), formatNumber, 1 } ); // decimal
    m_matchPairs.push_back( { QRegularExpression( PRE_NUM_TOKENS "-[0-9]+" ), formatNumber, 1 } ); // decimal negative
    m_matchPairs.push_back( { QRegularExpression( PRE_NUM_TOKENS "0x[0-9a-fA-F]+" ), formatNumber, 1 } ); // hexadecimal
    m_matchPairs.push_back( { QRegularExpression( PRE_NUM_TOKENS "[0-9]+[.][0-9]+" ), formatNumber, 1 } ); // float

    QTextCharFormat formatString;
    formatString.setForeground( QColor( "#FF55FF" ) );
    m_matchPairs.push_back( { QRegularExpression( "\"[^\"]*\"" ), formatString, 0 } );

    QTextCharFormat formatObsolete;
    formatObsolete.setForeground( QColor( "#FF0004" ) );
    formatObsolete.setFontUnderline( true );
    for ( const char* it : OBSOLETE_KEWORDS ) {
        m_matchPairs.push_back( { QRegularExpression( it ), formatObsolete, 0 } );
    }
}

void SyntaxHighlight::highlightBlock( const QString& text )
{
    for ( const Pair& it : m_matchPairs ) {
        QRegularExpressionMatchIterator matchIterator = it.m_pattern.globalMatch( text );
        while ( matchIterator.hasNext() ) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat( match.capturedStart() + it.m_offset, match.capturedLength() - it.m_offset, it.m_format );
        }
    }
}
