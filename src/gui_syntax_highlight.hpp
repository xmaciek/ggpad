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

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

#include <vector>

class SyntaxHighlight : public QSyntaxHighlighter {
    Q_OBJECT

private:
    struct Pair {
        QRegularExpression m_pattern;
        QTextCharFormat m_format;
        std::size_t m_offset = 0;
    };
    std::vector<Pair> m_matchPairs;

public:
    SyntaxHighlight( QTextDocument* parent );
    void highlightBlock( const QString& ) override;
};
