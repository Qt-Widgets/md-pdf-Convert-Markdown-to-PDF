
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

// md-pdf include
#include "md_parser.hpp"

// Qt include.
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QRegExp>


namespace MD {

//
// Parser
//

Document
Parser::parse( const QString & fileName, bool recursive )
{
	Document doc;

	parseFile( fileName, recursive, doc );

	clearCache();

	return doc;
}

void
Parser::parseFile( const QString & fileName, bool recursive, Document & doc )
{
	QFileInfo fi( fileName );

	if( fi.exists() && fi.suffix().toLower() == QLatin1String( "md" ) )
	{
		QFile f( fileName );

		if( f.open( QIODevice::ReadOnly | QIODevice::Text ) )
		{
			QStringList linksToParse;

			QTextStream s( &f );

			parseStream( s, doc, linksToParse, fi.filePath() + QDir::separator() );

			f.close();

			m_parsedFiles.append( fi.absoluteFilePath() );

			if( recursive && !linksToParse.isEmpty() )
			{
				while( !linksToParse.isEmpty() )
				{
					auto nextFileName = linksToParse.first();
					linksToParse.removeFirst();

					if( !m_parsedFiles.contains( nextFileName ) )
						parseFile( nextFileName, recursive, doc );
				}
			}
		}
	}
}

void
Parser::parseStream( QTextStream & stream, Document & doc, QStringList & linksToParse,
	const QString & workingPath )
{
	QStringList fragment;

	BlockType type = BlockType::Unknown;
	bool emptyLineInList = false;

	auto pf = [&]()
		{
			parseFragment( fragment, doc, linksToParse, workingPath );
			fragment.clear();
			type = BlockType::Unknown;
		};


	while( !stream.atEnd() )
	{
		auto line = stream.readLine();
		auto simplified = line.simplified();

		BlockType lineType = whatIsTheLine( line, emptyLineInList );

		if( !simplified.isEmpty() && type == BlockType::Unknown )
		{
			type = lineType;

			fragment.append( line );

			continue;
		}

		if( simplified.isEmpty() )
		{
			switch( type )
			{
				case BlockType::Text :
				case BlockType::Blockquote :
				{
					pf();

					continue;
				}

				case BlockType::CodeIndentedBySpaces :
				{
					if( line.startsWith( QLatin1String( "    " ) ) ||
						line.startsWith( QLatin1Char( '\t' ) ) )
					{
						fragment.append( line );
					}
					else
						pf();

					continue;
				}

				case BlockType::Code :
				{
					fragment.append( line );

					continue;
				}

				case BlockType::List :
				{
					emptyLineInList = true;

					continue;
				}

				default :
					break;
			}
		}
		else if( emptyLineInList )
		{
			if( line.startsWith( QLatin1String( "    " ) ) ||
				line.startsWith( QLatin1Char( '\t' ) )  ||
				lineType == BlockType::List )
			{
				fragment.append( line );

				emptyLineInList = false;

				continue;
			}
			else
			{
				pf();

				continue;
			}
		}

		if( type != lineType && type != BlockType::Code )
		{
			pf();
			type = lineType;
			fragment.append( line );
		}
		else if( type == BlockType::Code && type == lineType )
		{
			fragment.append( line );

			pf();
		}
	}

	if( !fragment.isEmpty() )
		pf();
}

Parser::BlockType
Parser::whatIsTheLine( const QString & str, bool inList ) const
{
	const auto s = str.simplified();
	static const QRegExp olr( QLatin1String( "^\\d+\\..*" ) );

	if( inList )
	{
		if( s.startsWith( QLatin1Char( '-' ) ) ||
			s.startsWith( QLatin1Char( '+' ) ) ||
			s.startsWith( QLatin1Char( '*' ) ) ||
			olr.exactMatch( s ) )
		{
			return BlockType::List;
		}
		else if( str.startsWith( QLatin1String( "    " ) ) ||
			str.startsWith( QLatin1Char( '\t' ) ) )
		{
			if( str.startsWith( QLatin1String( "        " ) ) ||
				str.startsWith( QLatin1String( "\t\t" ) ) )
			{
				return BlockType::CodeIndentedBySpaces;
			}
			else if( s.startsWith( QLatin1Char( '>' ) ) )
				return BlockType::Blockquote;
			else if( s.startsWith( QLatin1String( "```" ) ) )
				return BlockType::Code;
			else if( s.isEmpty() )
				return BlockType::Unknown;
			else
				return BlockType::Text;
		}
	}
	else
	{
		if( s.startsWith( QLatin1Char( '-' ) ) ||
			s.startsWith( QLatin1Char( '+' ) ) ||
			s.startsWith( QLatin1Char( '*' ) ) ||
			olr.exactMatch( s ) )
		{
			return BlockType::List;
		}
		else if( str.startsWith( QLatin1String( "    " ) ) ||
			str.startsWith( QLatin1Char( '\t' ) ) )
		{
			return BlockType::CodeIndentedBySpaces;
		}
		else if( s.startsWith( QLatin1Char( '>' ) ) )
			return BlockType::Blockquote;
		else if( s.startsWith( QLatin1String( "```" ) ) )
			return BlockType::Code;
		else if( s.isEmpty() )
			return BlockType::Unknown;
		else
			return BlockType::Text;
	}
}

void
Parser::parseFragment( const QStringList & fr, Document & doc, QStringList & linksToParse,
	const QString & workingPath )
{
	switch( whatIsTheLine( fr.first() ) )
	{
		case BlockType::Text :
			parseText( fr, doc, linksToParse, workingPath );
			break;

		case BlockType::Blockquote :
			parseBlockquote( fr, doc, linksToParse, workingPath );
			break;

		case BlockType::Code :
			parseCode( fr, doc );
			break;

		case BlockType::CodeIndentedBySpaces :
			parseCodeIndentedBySpaces( fr, doc );
			break;

		case BlockType::List :
			parseList( fr, doc, linksToParse, workingPath );
			break;

		default :
			break;
	}
}

void
Parser::clearCache()
{
	m_parsedFiles.clear();
}

void
Parser::parseText( const QStringList & fr, Document & doc, QStringList & linksToParse,
	const QString & workingPath )
{

}

void
Parser::parseBlockquote( const QStringList & fr, Document & doc, QStringList & linksToParse,
	const QString & workingPath )
{

}

void
Parser::parseList( const QStringList & fr, Document & doc, QStringList & linksToParse,
	const QString & workingPath )
{

}

void
Parser::parseCode( const QStringList & fr, Document & doc )
{

}

void
Parser::parseCodeIndentedBySpaces( const QStringList & fr, Document & doc )
{

}

} /* namespace MD */
