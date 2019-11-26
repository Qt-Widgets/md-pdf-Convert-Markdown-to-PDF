
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

			parse( s, doc, linksToParse, fi.filePath() + QDir::separator() );

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
Parser::parseFragment( const QStringList & fr, QSharedPointer< Block > parent,
	QStringList & linksToParse, const QString & workingPath )
{
	switch( whatIsTheLine( fr.first() ) )
	{
		case BlockType::Text :
			parseText( fr, parent, linksToParse, workingPath );
			break;

		case BlockType::Blockquote :
			parseBlockquote( fr, parent, linksToParse, workingPath );
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
			parseList( fr, parent, linksToParse, workingPath );
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
Parser::parseText( const QStringList & fr, QSharedPointer< Block > parent,
	QStringList & linksToParse, const QString & workingPath )
{

}

void
Parser::parseBlockquote( const QStringList & fr, QSharedPointer< Block > parent,
	QStringList & linksToParse, const QString & workingPath )
{
	QSharedPointer< Blockquote > bq( new Blockquote() );

	const int indent = fr.first().indexOf( QLatin1Char( '>' ) );

	QStringList data = fr;
	StringListStream stream( data );

	if( indent > -1 )
	{
		for( auto it = data.begin(), last = data.end(); it != last; ++it )
			*it = it->right( it->length() - indent - 1 );

		parse( stream, bq, linksToParse, workingPath );
	}
}

void
Parser::parseList( const QStringList & fr, QSharedPointer< Block > parent,
	QStringList & linksToParse, const QString & workingPath )
{
	QStringList data = fr;

	for( auto it = data.begin(), last  = data.end(); it != last; ++it )
		it->replace( QLatin1Char( '\t' ), QLatin1String( "    " ) );

	static const QRegExp space( QLatin1String( "[^\\s]+" ) );
	static const QRegExp item( QLatin1String( "^(\\*|\\-|\\+|(\\d+)\\.)\\s" ) );

	const int indent = space.indexIn( fr.first() );

	if( indent > -1 )
	{
		QSharedPointer< List > list( new List() );

		QStringList listItem;
		auto it = data.begin();

		*it = it->right( it->length() - indent );

		listItem.append( *it );

		++it;

		for( auto last = data.end(); it != last; ++it )
		{
			int s = space.indexIn( *it );
			s = ( s > indent ? indent : s );

			*it = it->right( it->length() - s );

			const int i = item.indexIn( *it );

			if( i == 0 )
			{
				parseListItem( listItem, list, linksToParse, workingPath );
				listItem.clear();
			}
			else
				listItem.append( *it );
		}

		if( !listItem.isEmpty() )
			parseListItem( listItem, list, linksToParse, workingPath );

		if( !list->isEmpty() )
			parent->appendItem( list );
	}
}

void
Parser::parseListItem( const QStringList & fr, QSharedPointer< Block > parent,
	QStringList & linksToParse, const QString & workingPath )
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

			parse( stream, item, linksToParse, workingPath );

			data.clear();

			parseList( fr.mid( pos ), item, linksToParse, workingPath );

			break;
		}
		else
			data.append( *it );
	}

	if( !data.isEmpty() )
	{
		StringListStream stream( data );

		parse( stream, item, linksToParse, workingPath );
	}

	if( !item->isEmpty() )
		parent->appendItem( item );
}

void
Parser::parseCode( const QStringList & fr, QSharedPointer< Block > parent, int indent )
{
	auto tmp = fr;
	tmp.removeFirst();
	tmp.removeLast();

	parseCodeIndentedBySpaces( tmp, parent, indent );
}

void
Parser::parseCodeIndentedBySpaces( const QStringList & fr, QSharedPointer< Block > parent,
	int indent )
{
	QString code;

	for( const auto & l : fr )
		code.append( ( indent > 0 ? l.right( l.length() - indent ) + QLatin1Char( '\n' ) :
			l + QLatin1Char( '\n' ) ) );

	parent->appendItem( QSharedPointer< Item > ( new Code( code ) ) );
}

} /* namespace MD */
