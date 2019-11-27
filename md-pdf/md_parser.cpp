
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

QSharedPointer< Document >
Parser::parse( const QString & fileName, bool recursive )
{
	QSharedPointer< Document > doc( new Document );

	parseFile( fileName, recursive, doc );

	clearCache();

	return doc;
}

void
Parser::parseFile( const QString & fileName, bool recursive, QSharedPointer< Block > doc )
{
	QFileInfo fi( fileName );

	if( fi.exists() && fi.suffix().toLower() == QLatin1String( "md" ) )
	{
		QFile f( fileName );

		if( f.open( QIODevice::ReadOnly | QIODevice::Text ) )
		{
			QStringList linksToParse;

			QTextStream s( &f );

			parse( s, doc, linksToParse, fi.filePath() + QDir::separator(), fi.fileName() );

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
			else if( s.startsWith( QLatin1String( "```" ) ) ||
				s.startsWith( QLatin1String( "~~~" ) ) )
			{
				return BlockType::Code;
			}
			else if( s.isEmpty() )
				return BlockType::Unknown;
			else
				return BlockType::Text;
		}
		else
			return BlockType::Text;
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
		else if( s.startsWith( QLatin1String( "```" ) ) ||
			s.startsWith( QLatin1String( "~~~" ) ) )
		{
			return BlockType::Code;
		}
		else if( s.isEmpty() )
			return BlockType::Unknown;
		else
			return BlockType::Text;
	}
}

void
Parser::parseFragment( QStringList & fr, QSharedPointer< Block > parent,
	QStringList & linksToParse, const QString & workingPath, const QString & fileName )
{
	switch( whatIsTheLine( fr.first() ) )
	{
		case BlockType::Text :
			parseText( fr, parent, linksToParse, workingPath, fileName );
			break;

		case BlockType::Blockquote :
			parseBlockquote( fr, parent, linksToParse, workingPath, fileName );
			break;

		case BlockType::Code :
			parseCode( fr, parent );
			break;

		case BlockType::CodeIndentedBySpaces :
		{
			int indent = 1;

			if( fr.first().startsWith( QLatin1String( "    " ) ) )
				indent = 4;

			parseCodeIndentedBySpaces( fr, parent, indent );
		}
			break;

		case BlockType::List :
			parseList( fr, parent, linksToParse, workingPath, fileName );
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
Parser::parseText( QStringList & fr, QSharedPointer< Block > parent,
	QStringList & linksToParse, const QString & workingPath, const QString & fileName )
{
	static const QRegExp hr( QLatin1String( "^\\s*#+\\s+.*" ) );
	static const QRegExp fnr( QLatin1String( "\\s*\\[\\^[^\\s]*\\]:.*" ) );
	static const QRegExp thr( QLatin1String( "\\s\\|\\s" ) );
	static const QRegExp tcr( QLatin1String(
		"^\\s*\\|?(\\s*:?-{3,}:?\\s*\\|)*\\s*:?-{3,}:?\\s*\\|?\\s*$" ) );

	if( hr.exactMatch( fr.first() ) )
		parseHeading( fr, parent, linksToParse, workingPath, fileName );
	else if( fnr.exactMatch( fr.first() ) )
		parseFootnote( fr, parent, linksToParse, workingPath, fileName );
	else if( thr.indexIn( fr.first() ) > -1 && fr.size() > 1 && tcr.exactMatch( fr.at( 1 ) ) )
		parseTable( fr, parent, linksToParse, workingPath, fileName );
	else
		parseParagraph( fr, parent, linksToParse, workingPath, fileName );
}

void
Parser::parseHeading( QStringList & fr, QSharedPointer< Block > parent,
	QStringList & linksToParse, const QString & workingPath, const QString & fileName )
{

}

void
Parser::parseFootnote( QStringList & fr, QSharedPointer< Block > parent,
	QStringList & linksToParse, const QString & workingPath, const QString & fileName )
{

}

void
Parser::parseTable( QStringList & fr, QSharedPointer< Block > parent,
	QStringList & linksToParse, const QString & workingPath, const QString & fileName )
{

}

void
Parser::parseParagraph( QStringList & fr, QSharedPointer< Block > parent,
	QStringList & linksToParse, const QString & workingPath, const QString & fileName )
{
	static const QRegExp h1r( QLatin1String( "^\\s*===*\\s*$" ) );
	static const QRegExp h2r( QLatin1String( "^\\s*---*\\s*$" ) );

	int i = 0;

	auto ph = [&]( const QString & label )
	{
		QStringList tmp = fr.mid( 0, i );
		tmp.first().prepend( label );

		parseHeading( tmp, parent, linksToParse, workingPath, fileName );

		for( int idx = 0; idx <= i; ++idx )
			fr.removeFirst();

		parseParagraph( fr, parent, linksToParse, workingPath, fileName );
	};
	\
	// Check for alternative syntax of H1 and H2 headings.
	for( const auto & l : qAsConst( fr ) )
	{
		if( h1r.exactMatch( l ) && i > 0 )
		{
			ph( QLatin1String( "# " ) );

			return;
		}
		else if( h2r.exactMatch( l ) && i > 0 )
		{
			ph( QLatin1String( "## " ) );

			return;
		}

		++i;
	}

	parseFormattedTextLinksImages( fr, parent, linksToParse, workingPath, fileName );
}

void
Parser::parseFormattedTextLinksImages( QStringList & fr, QSharedPointer< Block > parent,
	QStringList & linksToParse, const QString & workingPath, const QString & fileName )

{
	static const QString specialChars( QLatin1String( "\\`*_{}[]()#+-.!|" ) );

	QSharedPointer< Paragraph > p( new Paragraph() );

	bool hasBreakLine = false;

	enum class Lex {
		Bold,
		Italic,
		BoldAndItalic,
		Strikethrough,
		Text,
		Link,
		Image,
		ImageInLink,
		Code,
		FootnoteRef,
		BreakLine
	}; // enum class Lex

	struct PreparsedData {
		QVector< Lex > lexems;
		QVector< QSharedPointer< Text > > txt;
		QVector< QSharedPointer< Link > > lnk;
		QVector< QSharedPointer< Code > > code;
		QVector< QSharedPointer< FootnoteRef > > fnref;
		QVector< QSharedPointer< Image > > img;
	}; // struct PreparsedData

	PreparsedData data;

	auto parseImg = [&]( int i, QString & line ) -> int
	{
		return 0;
	};

	auto parseLnk = [&]( int i, QString & line ) -> int
	{
		return 0;
	};

	auto parseCode = [&]( int i, QString & line ) -> int
	{
		return 0;
	};

	auto parseUrl = [&]( int i, QString & line ) -> int
	{
		return 0;
	};

	auto createTextObj = [&]( QString & text )
	{
		if( !text.isEmpty() )
		{
			QSharedPointer< Text > t( new Text() );
			t->appendText( Text::TextWithOptions( text, TextWithoutFormat) );
			data.txt.append( t );
			text.clear();
		}
	};

	auto parseLine = [&]( QString & line )
	{
		hasBreakLine = line.endsWith( QLatin1String( "  " ) );

		line = line.simplified();

		static const QRegExp horRule( QLatin1String( "^(\\*{3,}|\\-{3,}|_{3,})$" ) );

		// Will skip horizontal rules, for now at least...
		if( !horRule.exactMatch( line ) )
		{
			QString text;

			for( int i = 0, length = line.length(); i < length; ++i )
			{
				if( line[ i ] == QLatin1Char( '\\' ) && i + 1 < length &&
					specialChars.contains( line[ i + 1] ) )
				{
					++i;

					text.append( line[ i ] );
				}
				else if( line[ i ] == QLatin1Char( '!' ) && i + 1 < length &&
					line[ i + 1 ] == QLatin1Char( '[' ) )
				{
					createTextObj( text );
					i = parseImg( i, line );
				}
				else if( line[ i ] == QLatin1Char( '[' ) )
				{
					createTextObj( text );
					i = parseLnk( i, line );
				}
				else if( line[ i ] == QLatin1Char( '`' ) )
				{
					createTextObj( text );
					i = parseCode( i, line );
				}
				else if( line[ i ] == QLatin1Char( '<' ) )
				{
					createTextObj( text );
					i = parseUrl( i, line );
				}
				else if( line[ i ] == QLatin1Char( '*' ) || line[ i ] == QLatin1Char( '_' ) )
				{
					QString style;

					while( i < length &&
						( line[ i ] == QLatin1Char( '*' ) || line[ i ] == QLatin1Char( '_' ) ) )
					{
						style.append( line[ i ] );
						++i;
					}

					if( style == QLatin1String( "*" ) || style == QLatin1String( "_" ) )
					{
						createTextObj( text );
						data.lexems.append( Lex::Italic );
					}
					else if( style == QLatin1String( "**" ) || style == QLatin1String( "__" ) )
					{
						createTextObj( text );
						data.lexems.append( Lex::Bold );
					}
					else if( style == QLatin1String( "***" ) || style == QLatin1String( "___" ) ||
						style == QLatin1String( "__*" ) || style == QLatin1String( "**_" ) ||
						style == QLatin1String( "*__" ) || style == QLatin1String( "__*" ) )
					{
						createTextObj( text );
						data.lexems.append( Lex::BoldAndItalic );
					}
					else
						text.append( style );
				}
				else if( line[ i ] == QLatin1Char( '~' ) && i + 1 < length &&
					line[ i + 1 ] == QLatin1Char( '~' ) )
				{
					++i;
					createTextObj( text );
					data.lexems.append( Lex::Strikethrough );
				}
			}

			createTextObj( text );
		}
	};

	for( auto it = fr.begin(), last = fr.end(); it != last; ++it )
		parseLine( *it );

	// Add here processing of parsed lexems.

	if( !p->isEmpty() )
		parent->appendItem( p );
}

void
Parser::parseBlockquote( QStringList & fr, QSharedPointer< Block > parent,
	QStringList & linksToParse, const QString & workingPath, const QString & fileName )
{
	QSharedPointer< Blockquote > bq( new Blockquote() );

	const int indent = fr.first().indexOf( QLatin1Char( '>' ) );

	StringListStream stream( fr );

	if( indent > -1 )
	{
		for( auto it = fr.begin(), last = fr.end(); it != last; ++it )
			*it = it->right( it->length() - indent - 1 );

		parse( stream, bq, linksToParse, workingPath, fileName );
	}

	if( !bq->isEmpty() )
		parent->appendItem( bq );
}

void
Parser::parseList( QStringList & fr, QSharedPointer< Block > parent,
	QStringList & linksToParse, const QString & workingPath, const QString & fileName )
{
	for( auto it = fr.begin(), last  = fr.end(); it != last; ++it )
		it->replace( QLatin1Char( '\t' ), QLatin1String( "    " ) );

	static const QRegExp space( QLatin1String( "[^\\s]+" ) );
	static const QRegExp item( QLatin1String( "^(\\*|\\-|\\+|(\\d+)\\.)\\s" ) );

	const int indent = space.indexIn( fr.first() );

	if( indent > -1 )
	{
		QSharedPointer< List > list( new List() );

		QStringList listItem;
		auto it = fr.begin();

		*it = it->right( it->length() - indent );

		listItem.append( *it );

		++it;

		for( auto last = fr.end(); it != last; ++it )
		{
			int s = space.indexIn( *it );
			s = ( s > indent ? indent : s );

			*it = it->right( it->length() - s );

			const int i = item.indexIn( *it );

			if( i == 0 )
			{
				parseListItem( listItem, list, linksToParse, workingPath, fileName );
				listItem.clear();
			}
			else
				listItem.append( *it );
		}

		if( !listItem.isEmpty() )
			parseListItem( listItem, list, linksToParse, workingPath, fileName );

		if( !list->isEmpty() )
			parent->appendItem( list );
	}
}

void
Parser::parseListItem( QStringList & fr, QSharedPointer< Block > parent,
	QStringList & linksToParse, const QString & workingPath, const QString & fileName )
{
	static const QRegExp unorderedRegExp( QLatin1String( "^[\\*|\\-|\\+]\\s+.*" ) );
	static const QRegExp orderedRegExp( QLatin1String( "^(\\d+)\\.\\s+.*" ) );
	static const QRegExp itemRegExp( QLatin1String( "^\\s*(\\*|\\-|\\+|(\\d+)\\.)\\s+" ) );

	QSharedPointer< ListItem > item( new ListItem() );

	if( unorderedRegExp.exactMatch( fr.first() ) )
		item->setListType( ListItem::Unordered );
	else
		item->setListType( ListItem::Ordered );

	int i = 0;

	if( item->listType() == ListItem::Ordered )
	{
		if( orderedRegExp.indexIn( fr.first() ) > -1 )
			i = orderedRegExp.cap( 1 ).toInt();

		item->setOrderedListPreState( i == 1 ? ListItem::Start : ListItem::Continue );
	}

	QStringList data;

	auto it = fr.begin();
	++it;

	int pos = 1;

	itemRegExp.indexIn( fr.first() );

	data.append( fr.first().right( fr.first().length() - itemRegExp.matchedLength() ) );

	for( auto last = fr.end(); it != last; ++it, ++pos )
	{
		const int i = itemRegExp.indexIn( *it );

		if( i > -1 )
		{
			StringListStream stream( data );

			parse( stream, item, linksToParse, workingPath, fileName, true );

			data.clear();

			QStringList nestedList = fr.mid( pos );

			parseList( nestedList, item, linksToParse, workingPath, fileName );

			break;
		}
		else
			data.append( *it );
	}

	if( !data.isEmpty() )
	{
		StringListStream stream( data );

		parse( stream, item, linksToParse, workingPath, fileName );
	}

	if( !item->isEmpty() )
		parent->appendItem( item );
}

void
Parser::parseCode( QStringList & fr, QSharedPointer< Block > parent, int indent )
{
	fr.removeFirst();
	fr.removeLast();

	parseCodeIndentedBySpaces( fr, parent, indent );
}

void
Parser::parseCodeIndentedBySpaces( QStringList & fr, QSharedPointer< Block > parent,
	int indent )
{
	QString code;

	for( const auto & l : qAsConst( fr ) )
		code.append( ( indent > 0 ? l.right( l.length() - indent ) + QLatin1Char( '\n' ) :
			l + QLatin1Char( '\n' ) ) );

	parent->appendItem( QSharedPointer< Item > ( new Code( code ) ) );
}

} /* namespace MD */
