
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

	if( fi.exists() && ( fi.suffix().toLower() == QLatin1String( "md" ) ||
		fi.suffix().toLower() == QLatin1String( "markdown" ) ) )
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

	QSharedPointer< Paragraph > p( new Paragraph() );

	parseFormattedTextLinksImages( fr, p, linksToParse, workingPath, fileName );

	if( !p->isEmpty() )
		parent->appendItem( p );
}

void
Parser::parseFormattedTextLinksImages( QStringList & fr, QSharedPointer< Block > parent,
	QStringList & linksToParse, const QString & workingPath, const QString & fileName )

{
	static const QString specialChars( QLatin1String( "\\`*_{}[]()#+-.!|" ) );

	enum class Lex {
		Bold,
		Italic,
		BoldAndItalic,
		Strikethrough,
		Text,
		Link,
		Image,
		ImageInLink,
		StartOfCode,
		StartOfQuotedCode,
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

		int processedText = 0;
		int processedLnk = 0;
		int processedCode = 0;
		int processedFnRef = 0;
		int processedImg = 0;
	}; // struct PreparsedData

	PreparsedData data;

	// Skip spaces in line from pos \a i.
	auto skipSpaces = []( int i, const QString & line ) -> int
	{
		const int length = line.length();

		while( i < length || !line[ i ].isSpace() )
			++i;

		return i;
	}; // skipSpaces

	// Read text of the link. I.e. in [...]
	auto readLinkText = []( int & i, const QString & line ) -> QString
	{
		const int length = line.length();
		QString t;

		while( i < length ||
			( line[ i ] != QLatin1Char( ']' ) && line[ i - 1 ] != QLatin1Char( '\\' ) ) )
		{
			if( line[ i ] != QLatin1Char( '\\' ) )
				t.append( line[ i ] );

			++i;
		}

		return t;
	}; // readLinkText

	// Read URL.
	auto readLnk = [&]( int & i, const QString & line ) -> QString
	{
		++i;
		i = skipSpaces( i, line );
		const int length = line.length();

		if( i < length )
		{
			QString lnk;

			while( i < length && !line[ i ].isSpace() &&
				( line[ i ] != QLatin1Char( ')' ) && line[ i - 1 ] != QLatin1Char( '\\' ) )
				&& line[ i ] != QLatin1Char( ']' ) )
			{
				lnk.append( line[ i ] );
				++i;
			}

			return lnk;
		}
		else
			return QString();
	}; // readLnk

	// Skip link's caption.
	auto skipLnkCaption = [&]( int & i, const QString & line ) -> bool
	{
		bool quoted = false;

		if( line[ i ] == QLatin1Char( '"') )
		{
			quoted = true;
			++i;
		}

		const int length = line.length();

		while( i < length &&
			( quoted ?
				( line[ i ] != QLatin1Char( '"' ) && line[ i - 1 ] != QLatin1Char( '\\' ) ) :
				( line[ i ] != QLatin1Char( ')' ) ) ) )
		{
			++i;
		}

		if( quoted )
		{
			i = skipSpaces( i, line );

			if( line[ i ] == QLatin1Char( ')' ) )
			{
				++i;

				return true;
			}
		}
		else if( line[ i ] == QLatin1Char( ')' ) )
			return true;

		return false;
	}; // skipLnkCaption

	// Read image.
	auto parseImg = [&]( int i, const QString & line, QString & text, bool * ok = nullptr ) -> int
	{
		const int start = i;
		i += 2;
		const int length = line.length();

		QString t = readLinkText( i, line );

		if( !t.isEmpty() )
		{
			i = skipSpaces( i, line );

			if( i < length && line[ i ] == QLatin1Char( '(' ) )
			{
				QString lnk = readLnk( i, line );

				if( !lnk.isEmpty() && i < length )
				{
					i = skipSpaces( i, line );

					if( i < length )
					{
						if( skipLnkCaption( i, line ) )
						{
							QSharedPointer< Image > img( new Image() );
							img->setText( t );
							img->setUrl( lnk );
							data.img.append( img );
							data.lexems.append( Lex::Image );

							if( ok )
								*ok = true;

							return i;
						}
					}
				}
			}
		}

		text.append( line.mid( start, i - start ) );

		return i;
	}; // parseImg

	// Read link.
	auto parseLnk = [&]( int i, const QString & line, QString & text ) -> int
	{
		const int startPos = i;
		bool withImage = false;

		++i;

		i = skipSpaces( i, line );

		const int length = line.length();
		QString lnkText, url;

		if( i < length )
		{
			if( i + 1 < length && line[ i ] == QLatin1Char( '!' ) &&
				line[ i + 1 ] == QLatin1Char( '[' ) )
			{
				bool ok = false;

				QString tmp;

				i = parseImg( i, line, tmp, &ok );

				if( !ok )
				{
					text.append( line.mid( startPos, length - i ) );

					return i;
				}
				else
					withImage = true;
			}
			else if( line[ i ] == QLatin1Char( '^' ) )
			{
				++i;
				auto lnk = readLnk( i, line );

				i = skipSpaces( i, line );

				if( i < length && line[ i ] == QLatin1Char( ']' ) )
				{
					if( i + 1 < length && line[ i + 1 ] != QLatin1Char( ':' ) )
					{
						QSharedPointer< FootnoteRef > fnr(
							new FootnoteRef( lnk + QDir::separator() + workingPath + fileName ) );

						data.fnref.append( fnr );
						data.lexems.append( Lex::FootnoteRef );

						return i + 1;
					}
				}
				else
				{
					text.append( line.mid( startPos, length - i ) );

					return i;
				}
			}
			else
			{
				i = skipSpaces( i, line );

				if( i < length )
				{
					lnkText = readLinkText( i, line ).simplified();

					if( lnkText.isEmpty() )
					{
						text.append( line.mid( startPos, length - i ) );

						return i;
					}
				}
				else
				{
					text.append( line.mid( startPos, length - i ) );

					return i;
				}
			}
		}

		i = skipSpaces( i, line );

		if( i < length )
		{
			if( line[ i ] == QLatin1Char( ':' ) )
			{
				i = skipSpaces( i, line );

				if( i < length )
				{
					url = readLnk( i, line );

					if( !url.isEmpty() )
					{
						if( QUrl( url ).isLocalFile() )
						{
							QFileInfo fi( url );

							if( fi.isRelative() )
								url = workingPath + url;

							linksToParse.append( url );
						}

						if( parent->type() == ItemType::Document )
						{
							QSharedPointer< Link > lnk( new Link() );
							lnk->setUrl( url );

							static_cast< Document* > ( parent.data() )->insertLabeledLink(
								QString::fromLatin1( "#" ) + lnkText +
								QDir::separator() + workingPath + fileName, lnk );
						}

						return length;
					}
					else
					{
						text.append( line.mid( startPos, length - i ) );

						return i;
					}
				}
				else
				{
					text.append( line.mid( startPos, length - i ) );

					return i;
				}
			}
			else if( line[ i ] == QLatin1Char( '(' ) )
			{
				i = skipSpaces( i, line );

				if( i < length )
				{
					url = readLnk( i, line );

					if( !url.isEmpty() && i < length )
					{
						if( !url.startsWith( QLatin1Char( '#' ) ) )
						{
							i = skipSpaces( i, line );

							if( i < length )
							{
								if( skipLnkCaption( i, line ) )
								{
									if( QUrl( url ).isLocalFile() )
									{
										QFileInfo fi( url );

										if( fi.isRelative() )
											url = workingPath + url;

										linksToParse.append( url );
									}
								}
							}
						}
						else
							url = url + QDir::separator() + workingPath + fileName;
					}
					else
					{
						text.append( line.mid( startPos, length - i ) );

						return i;
					}
				}
				else
				{
					text.append( line.mid( startPos, length - i ) );

					return i;
				}
			}
			else if( line[ i ] == QLatin1Char( '[' ) )
			{
				i = skipSpaces( i, line );

				if( i < length )
				{
					url = readLnk( i, line );

					if( !url.isEmpty() )
					{
						i = skipSpaces( i, line );

						if( i < length && line[ i ] == QLatin1Char( ']' ) )
						{
							url = QString::fromLatin1( "#" ) + url +
								QDir::separator() + workingPath + fileName;

							++i;
						}
						else
						{
							text.append( line.mid( startPos, length - i ) );

							return i;
						}
					}
				}
				else
				{
					text.append( line.mid( startPos, length - i ) );

					return i;
				}
			}
			else
			{
				text.append( line.mid( startPos, length - i ) );

				return i;
			}
		}
		else
		{
			text.append( line.mid( startPos, length - i ) );

			if( withImage )
				data.img.removeLast();
		}

		QSharedPointer< Link > lnk( new Link() );
		lnk->setUrl( url );
		lnk->setText( lnkText );
		data.lnk.append( lnk );

		if( withImage )
			data.lexems.append( Lex::ImageInLink );
		else
			data.lexems.append( Lex::Link );

		return i;
	}; // parseLnk

	enum class LineParsingState {
		Finished,
		UnfinishedCode,
		UnfinishedQuotedCode
	}; // enum class LineParsingState

	// Create text object.
	auto createTextObj = [&]( const QString & text )
	{
		if( !text.isEmpty() )
		{
			QSharedPointer< Text > t( new Text() );
			t->setText( text );
			t->setOpts( TextWithoutFormat );
			data.txt.append( t );
			data.lexems.append( Lex::Text );
		}
	}; // createTextObject

	// Read code.
	auto parseCode = [&]( int i, const QString & line, LineParsingState & prevAndNext ) -> int
	{
		const int length = line.length();

		bool quoted = false;

		if( prevAndNext != LineParsingState::Finished )
			quoted = ( prevAndNext == LineParsingState::UnfinishedQuotedCode );
		else
		{
			if( i + 1 < length && line[ i + 1 ] == QLatin1Char( '`' ) )
			{
				quoted = true;

				data.lexems.append( Lex::StartOfQuotedCode );

				i += 2;
			}
			else
			{
				data.lexems.append( Lex::StartOfCode );

				++i;
			}
		}

		QString code;
		bool finished = false;

		while( i < length )
		{
			if( line[ i ] == QLatin1Char( '`' ) )
			{
				if( !quoted )
				{
					finished = true;

					break;
				}
				else if( i + 1 < length && line[ i + 1 ] == QLatin1Char( '`' ) )
				{
					finished = true;

					i += 2;

					break;
				}
			}

			code.append( line[ i ] );

			++i;
		}

		createTextObj( code );

		if( finished )
			data.lexems.append( quoted ? Lex::StartOfQuotedCode : Lex::StartOfCode );

		return i;
	}; // parseCode

	// Read URL in <...>
	auto parseUrl = [&]( int i, const QString & line, QString & text ) -> int
	{
		++i;

		const int length = line.length();

		bool done = false;
		QString url;

		while( i < length )
		{
			if( line[ i ] != QLatin1Char( '>' ) )
				url.append( line[ i ] );
			else
			{
				done = true;
				++i;
				break;
			}

			++i;
		}

		if( done )
		{
			QSharedPointer< Link > lnk( new Link() );
			lnk->setUrl( url.simplified() );
			data.lnk.append( lnk );
			data.lexems.append( Lex::Link );
		}
		else
			text.append( QString::fromLatin1( "<" ) + url );

		return i;
	}; // parseUrl

	// Parse one line in paragraph.
	auto parseLine = [&]( QString & line, LineParsingState prev ) -> LineParsingState
	{
		bool hasBreakLine = line.endsWith( QLatin1String( "  " ) );

		int pos = 0;

		if( prev != LineParsingState::Finished )
		{
			pos = parseCode( 0, line, prev );

			if( prev != LineParsingState::Finished )
				return prev;
		}

		static const QRegExp nonSpace( QLatin1String( "[^\\s]" ) );

		const int ns = nonSpace.indexIn( line );

		if( ns > 0 )
			line = line.right( line.length() - ns );

		static const QRegExp horRule( QLatin1String( "^(\\*{3,}|\\-{3,}|_{3,})$" ) );

		// Will skip horizontal rules, for now at least...
		if( !horRule.exactMatch( line ) )
		{
			QString text;

			for( int i = pos, length = line.length(); i < length; ++i )
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
					createTextObj( text.simplified() );
					text.clear();
					i = parseImg( i, line, text );
				}
				else if( line[ i ] == QLatin1Char( '[' ) )
				{
					createTextObj( text.simplified() );
					text.clear();
					i = parseLnk( i, line, text );
				}
				else if( line[ i ] == QLatin1Char( '`' ) )
				{
					createTextObj( text.simplified() );
					text.clear();
					i = parseCode( i, line, prev );

					if( prev != LineParsingState::Finished )
						return prev;
				}
				else if( line[ i ] == QLatin1Char( '<' ) )
				{
					createTextObj( text.simplified() );
					text.clear();
					i = parseUrl( i, line, text );
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
						createTextObj( text.simplified() );
						text.clear();
						data.lexems.append( Lex::Italic );
					}
					else if( style == QLatin1String( "**" ) || style == QLatin1String( "__" ) )
					{
						createTextObj( text.simplified() );
						text.clear();
						data.lexems.append( Lex::Bold );
					}
					else if( style == QLatin1String( "***" ) || style == QLatin1String( "___" ) ||
						style == QLatin1String( "__*" ) || style == QLatin1String( "**_" ) ||
						style == QLatin1String( "*__" ) || style == QLatin1String( "__*" ) )
					{
						createTextObj( text.simplified() );
						text.clear();
						data.lexems.append( Lex::BoldAndItalic );
					}
					else
						text.append( style );
				}
				else if( line[ i ] == QLatin1Char( '~' ) && i + 1 < length &&
					line[ i + 1 ] == QLatin1Char( '~' ) )
				{
					++i;
					createTextObj( text.simplified() );
					text.clear();
					data.lexems.append( Lex::Strikethrough );
				}
				else
					text.append( line[ i ] );
			}

			createTextObj( text.simplified() );
			text.clear();

			if( hasBreakLine )
				data.lexems.append( Lex::BreakLine );
		}

		return LineParsingState::Finished;
	}; // parseLine

	LineParsingState state = LineParsingState::Finished;

	for( auto it = fr.begin(), last = fr.end(); it != last; ++it )
		state = parseLine( *it, state );

	// Set flags for all nested items.
	auto setFlags = [&]( Lex lex, QVector< Lex >::iterator it )
	{
		static const auto lexToFormat = []( Lex lex ) -> TextOptions
		{
			switch( lex )
			{
				case Lex::Bold :
					return BoldText;

				case Lex::Italic :
					return ItalicText;

				case Lex::BoldAndItalic :
					return ( BoldText | ItalicText );

				case Lex::Strikethrough :
					return StrikethroughText;

				default :
					return TextWithoutFormat;
			}
		};

		auto close = std::find( it + 1, data.lexems.end(), lex );

		if( close != data.lexems.end() )
		{
			int processedText = data.processedText;
			int processedLnk = data.processedLnk;

			for( auto i = it + 1; i != close; ++i )
			{
				switch( *i )
				{
					case Lex::Text :
					{
						data.txt[ processedText ]->setOpts( data.txt[ processedText ]->opts() |
							lexToFormat( lex ) );
						++processedText;
					}
						break;

					case Lex::Link :
					{
						data.lnk[ processedLnk ]->setTextOptions( data.lnk[ processedLnk ]->textOptions() |
							lexToFormat( lex ) );
						++processedLnk;
					}
						break;

					default :
						break;
				}
			}
		}
	}; // setFlags

	// Add real items to paragraph  after pre-parsing. Handle code.
	for( auto it = data.lexems.begin(), last = data.lexems.end(); it != last; ++it )
	{
		switch( *it )
		{
			case Lex::Bold :
			case Lex::Italic :
			case Lex::BoldAndItalic :
			case Lex::Strikethrough :
			{
				setFlags( *it, it );
			}
				break;

			case Lex::StartOfCode :
			case Lex::StartOfQuotedCode :
			{
				auto end = std::find( it + 1, data.lexems.end(), *it );

				if( end != data.lexems.end() )
				{
					++it;

					QSharedPointer< Code > c( new Code( QString(), true ) );

					for( ; it != end; ++it )
					{
						c->setText( c->text() + data.txt[ data.processedText ]->text() +
							QLatin1Char( ' ' ) );

						++data.processedText;
					}

					++it;

					parent->appendItem( c );
				}
				else
				{
					auto text = data.txt[ data.processedText ]->text();
					text.prepend( *it == Lex::StartOfCode ?
						QLatin1String( "`" ) : QLatin1String( "``" ) );
					data.txt[ data.processedText ]->setText( text );
				}
			}
				break;

			case Lex::Text :
			{
				parent->appendItem( data.txt[ data.processedText ] );
				++data.processedText;
			}
				break;

			case Lex::Link :
			{
				parent->appendItem( data.lnk[ data.processedLnk ] );
				++data.processedLnk;
			}
				break;

			case Lex::Image :
			{
				parent->appendItem( data.img[ data.processedImg ] );
				++data.processedImg;
			}
				break;

			case Lex::BreakLine :
			{
				parent->appendItem( QSharedPointer< Item > ( new LineBreak() ) );
			}
				break;

			case Lex::ImageInLink :
			{
				data.lnk[ data.processedLnk ]->setImg( data.img[ data.processedImg ] );
				++data.processedImg;
				parent->appendItem( data.lnk[ data.processedLnk ] );
				++data.processedLnk;
			}
				break;

			case Lex::Code :
			{
				parent->appendItem( data.code[ data.processedCode ] );
				++data.processedCode;
			}
				break;

			case Lex::FootnoteRef :
			{
				parent->appendItem( data.fnref[ data.processedFnRef ] );
				++data.processedFnRef;
			}
				break;
		}
	}
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
