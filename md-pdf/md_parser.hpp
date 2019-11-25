
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2019 Igor Mironchik

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MD_PDF_MD_PARSER_HPP_INCLUDED
#define MD_PDF_MD_PARSER_HPP_INCLUDED

// md-pdf include.
#include "md_doc.hpp"

// Qt include.
#include <QTextStream>


namespace MD {

//
// Parser
//

//! MD parser.
class Parser final
{
public:
	Parser() = default;
	~Parser() = default;

	Document parse( const QString & fileName, bool recursive = true );

private:
	void parseFile( const QString & fileName, bool recursive, Document & doc );
	void parseStream( QTextStream & stream, Document & doc, QStringList & linksToParse,
		const QString & workingPath );
	void clearCache();

	enum class BlockType {
		Unknown,
		Text,
		List,
		CodeIndentedBySpaces,
		Code,
		Blockquote
	}; // enum BlockType

	BlockType whatIsTheLine( const QString & str, bool inList = false ) const;
	void parseFragment( const QStringList & fr, Document & doc, QStringList & linksToParse,
		const QString & workingPath );
	void parseText( const QStringList & fr, Document & doc, QStringList & linksToParse,
		const QString & workingPath );
	void parseBlockquote( const QStringList & fr, Document & doc, QStringList & linksToParse,
		const QString & workingPath );
	void parseList( const QStringList & fr, Document & doc, QStringList & linksToParse,
		const QString & workingPath );
	void parseCode( const QStringList & fr, Document & doc );
	void parseCodeIndentedBySpaces( const QStringList & fr, Document & doc );

private:
	QStringList m_parsedFiles;

	Q_DISABLE_COPY( Parser )
}; // class Parser

} /* namespace MD */

#endif // MD_PDF_MD_PARSER_HPP_INCLUDED
