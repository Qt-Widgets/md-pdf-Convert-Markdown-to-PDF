
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

#include <md-pdf/md_parser.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
// doctest include.
#include <doctest/doctest.h>

#include <QFile>
#include <QDir>


TEST_CASE( "empty" )
{
	MD::Parser p;
	auto doc = p.parse( QLatin1String( "./test1.md" ) );
	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 1 );
	REQUIRE( doc->items().at( 0 )->type() == MD::ItemType::Anchor );
}

TEST_CASE( "only text" )
{
	MD::Parser p;
	auto doc = p.parse( QLatin1String( "./test2.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );
	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

	REQUIRE( dp->items().size() == 1 );
	REQUIRE( dp->items().first()->type() == MD::ItemType::Text );

	auto dt = static_cast< MD::Text* > ( dp->items().first().data() );

	REQUIRE( dt->opts() == MD::TextOption::TextWithoutFormat );
	REQUIRE( dt->text() == QLatin1String( "This is just a text!" ) );
}

TEST_CASE( "two paragraphs" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test3.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 3 );

	{
		REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

		auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

		REQUIRE( dp->items().size() == 1 );
		REQUIRE( dp->items().first()->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().first().data() );

		REQUIRE( dt->opts() == MD::TextOption::TextWithoutFormat );
		REQUIRE( dt->text() == QLatin1String( "Paragraph 1." ) );
	}

	{
		REQUIRE( doc->items().at( 2 )->type() == MD::ItemType::Paragraph );

		auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 2 ).data() );

		REQUIRE( dp->items().size() == 1 );
		REQUIRE( dp->items().first()->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().first().data() );

		REQUIRE( dt->opts() == MD::TextOption::TextWithoutFormat );
		REQUIRE( dt->text() == QLatin1String( "Paragraph 2." ) );
	}
}

TEST_CASE( "three lines" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test4.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

	REQUIRE( dp->items().size() == 3 );

	{
		REQUIRE( dp->items().at( 0 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 0 ).data() );

		REQUIRE( dt->opts() == MD::TextOption::TextWithoutFormat );
		REQUIRE( dt->text() == QLatin1String( "Line 1..." ) );
	}

	{
		REQUIRE( dp->items().at( 1 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 1 ).data() );

		REQUIRE( dt->opts() == MD::TextOption::TextWithoutFormat );
		REQUIRE( dt->text() == QLatin1String( "Line 2..." ) );
	}

	{
		REQUIRE( dp->items().at( 2 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 2 ).data() );

		REQUIRE( dt->opts() == MD::TextOption::TextWithoutFormat );
		REQUIRE( dt->text() == QLatin1String( "Line 3..." ) );
	}
}

TEST_CASE( "with linebreak" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test5.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

	REQUIRE( dp->items().size() == 4 );

	{
		REQUIRE( dp->items().at( 0 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 0 ).data() );

		REQUIRE( dt->opts() == MD::TextOption::TextWithoutFormat );
		REQUIRE( dt->text() == QLatin1String( "Line 1..." ) );
	}

	REQUIRE( dp->items().at( 1 )->type() == MD::ItemType::LineBreak );

	{
		REQUIRE( dp->items().at( 2 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 2 ).data() );

		REQUIRE( dt->opts() == MD::TextOption::TextWithoutFormat );
		REQUIRE( dt->text() == QLatin1String( "Line 2..." ) );
	}

	{
		REQUIRE( dp->items().at( 3 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 3 ).data() );

		REQUIRE( dt->opts() == MD::TextOption::TextWithoutFormat );
		REQUIRE( dt->text() == QLatin1String( "Line 3..." ) );
	}
}

TEST_CASE( "text formatting" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test6.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

	REQUIRE( dp->items().size() == 3 );

	{
		REQUIRE( dp->items().at( 0 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 0 ).data() );

		REQUIRE( dt->opts() == MD::TextOption::ItalicText );
		REQUIRE( dt->text() == QLatin1String( "Line 1..." ) );
	}

	{
		REQUIRE( dp->items().at( 1 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 1 ).data() );

		REQUIRE( dt->opts() == MD::TextOption::BoldText );
		REQUIRE( dt->text() == QLatin1String( "Line 2..." ) );
	}

	{
		REQUIRE( dp->items().at( 2 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 2 ).data() );

		REQUIRE( dt->opts() == MD::TextOption::StrikethroughText );
		REQUIRE( dt->text() == QLatin1String( "Line 3..." ) );
	}
}

TEST_CASE( "multiline formatting" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test7.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

	REQUIRE( dp->items().size() == 3 );

	{
		REQUIRE( dp->items().at( 0 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 0 ).data() );

		REQUIRE( dt->opts() == ( MD::TextOption::ItalicText | MD::TextOption::BoldText ) );
		REQUIRE( dt->text() == QLatin1String( "Line 1..." ) );
	}

	{
		REQUIRE( dp->items().at( 1 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 1 ).data() );

		REQUIRE( dt->opts() == ( MD::TextOption::ItalicText | MD::TextOption::BoldText ) );
		REQUIRE( dt->text() == QLatin1String( "Line 2..." ) );
	}

	{
		REQUIRE( dp->items().at( 2 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 2 ).data() );

		REQUIRE( dt->opts() == ( MD::TextOption::ItalicText | MD::TextOption::BoldText ) );
		REQUIRE( dt->text() == QLatin1String( "Line 3..." ) );
	}
}

TEST_CASE( "multiline multiformatting" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test8.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

	REQUIRE( dp->items().size() == 3 );

	{
		REQUIRE( dp->items().at( 0 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 0 ).data() );

		REQUIRE( dt->opts() == ( MD::TextOption::ItalicText | MD::TextOption::BoldText |
			MD::TextOption::StrikethroughText ) );
		REQUIRE( dt->text() == QLatin1String( "Line 1..." ) );
	}

	{
		REQUIRE( dp->items().at( 1 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 1 ).data() );

		REQUIRE( dt->opts() == ( MD::TextOption::ItalicText | MD::TextOption::BoldText |
			MD::TextOption::StrikethroughText ) );
		REQUIRE( dt->text() == QLatin1String( "Line 2..." ) );
	}

	{
		REQUIRE( dp->items().at( 2 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 2 ).data() );

		REQUIRE( dt->opts() == ( MD::TextOption::ItalicText | MD::TextOption::BoldText |
			MD::TextOption::StrikethroughText ) );
		REQUIRE( dt->text() == QLatin1String( "Line 3..." ) );
	}
}

TEST_CASE( "multiline multiformatting not continues" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test9.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

	REQUIRE( dp->items().size() == 3 );

	{
		REQUIRE( dp->items().at( 0 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 0 ).data() );

		REQUIRE( dt->opts() == ( MD::TextOption::ItalicText | MD::TextOption::BoldText |
			MD::TextOption::StrikethroughText ) );
		REQUIRE( dt->text() == QLatin1String( "Line 1..." ) );
	}

	{
		REQUIRE( dp->items().at( 1 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 1 ).data() );

		REQUIRE( dt->opts() == ( MD::TextOption::ItalicText | MD::TextOption::BoldText |
			MD::TextOption::StrikethroughText ) );
		REQUIRE( dt->text() == QLatin1String( "Line 2..." ) );
	}

	{
		REQUIRE( dp->items().at( 2 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 2 ).data() );

		REQUIRE( dt->opts() == ( MD::TextOption::ItalicText | MD::TextOption::BoldText ) );
		REQUIRE( dt->text() == QLatin1String( "Line 3..." ) );
	}
}

TEST_CASE( "it's not a formatting" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test10.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

	REQUIRE( dp->items().size() == 3 );

	{
		REQUIRE( dp->items().at( 0 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 0 ).data() );

		REQUIRE( dt->opts() == MD::TextOption::TextWithoutFormat );
		REQUIRE( dt->text() == QLatin1String( "~~__*Line 1..." ) );
	}

	{
		REQUIRE( dp->items().at( 1 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 1 ).data() );

		REQUIRE( dt->opts() == MD::TextOption::TextWithoutFormat );
		REQUIRE( dt->text() == QLatin1String( "Line 2...~~" ) );
	}

	{
		REQUIRE( dp->items().at( 2 )->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().at( 2 ).data() );

		REQUIRE( dt->opts() == MD::TextOption::TextWithoutFormat );
		REQUIRE( dt->text() == QLatin1String( "Line 3...*__" ) );
	}
}

TEST_CASE( "code" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test11.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

	REQUIRE( dp->items().size() == 1 );

	REQUIRE( dp->items().at( 0 )->type() == MD::ItemType::Code );

	auto c = static_cast< MD::Code* > ( dp->items().at( 0 ).data() );

	REQUIRE( c->inlined() == true );
	REQUIRE( c->text() == QLatin1String( "code" ) );
}

TEST_CASE( "code in text" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test12.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

	REQUIRE( dp->items().size() == 3 );

	REQUIRE( dp->items().at( 0 )->type() == MD::ItemType::Text );

	auto t1 = static_cast< MD::Text* > ( dp->items().at( 0 ).data() );

	REQUIRE( t1->text() == QLatin1String( "Code in the" ) );

	REQUIRE( dp->items().at( 1 )->type() == MD::ItemType::Code );

	auto c = static_cast< MD::Code* > ( dp->items().at( 1 ).data() );

	REQUIRE( c->inlined() == true );
	REQUIRE( c->text() == QLatin1String( "text" ) );

	REQUIRE( dp->items().at( 2 )->type() == MD::ItemType::Text );

	auto t2 = static_cast< MD::Text* > ( dp->items().at( 2 ).data() );

	REQUIRE( t2->text() == QLatin1String( "." ) );
}

TEST_CASE( "multilined inline code" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test13.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

	REQUIRE( dp->items().size() == 1 );

	REQUIRE( dp->items().at( 0 )->type() == MD::ItemType::Code );

	auto c = static_cast< MD::Code* > ( dp->items().at( 0 ).data() );

	REQUIRE( c->inlined() == true );
	REQUIRE( c->text() == QLatin1String( "Use this `code` in the code" ) );
}

TEST_CASE( "three lines with \\r" )
{
	MD::Parser parser;

	QFile f( "./test14.md" );

	if( f.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
	{
		f.write( "Line 1...\rLine 2...\r\nLine 3...\n" );
		f.close();

		auto doc = parser.parse( QLatin1String( "./test14.md" ) );

		REQUIRE( doc->isEmpty() == false );
		REQUIRE( doc->items().size() == 2 );

		REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

		auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

		REQUIRE( dp->items().size() == 3 );

		{
			REQUIRE( dp->items().at( 0 )->type() == MD::ItemType::Text );

			auto dt = static_cast< MD::Text* > ( dp->items().at( 0 ).data() );

			REQUIRE( dt->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( dt->text() == QLatin1String( "Line 1..." ) );
		}

		{
			REQUIRE( dp->items().at( 1 )->type() == MD::ItemType::Text );

			auto dt = static_cast< MD::Text* > ( dp->items().at( 1 ).data() );

			REQUIRE( dt->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( dt->text() == QLatin1String( "Line 2..." ) );
		}

		{
			REQUIRE( dp->items().at( 2 )->type() == MD::ItemType::Text );

			auto dt = static_cast< MD::Text* > ( dp->items().at( 2 ).data() );

			REQUIRE( dt->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( dt->text() == QLatin1String( "Line 3..." ) );
		}
	}
	else
		REQUIRE( true == false );
}

TEST_CASE( "three paragraphs with \\r" )
{
	MD::Parser parser;

	QFile f( "./test15.md" );

	if( f.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
	{
		f.write( "Line 1...\r\rLine 2...\r\rLine 3...\r" );
		f.close();

		auto doc = parser.parse( QLatin1String( "./test15.md" ) );

		REQUIRE( doc->isEmpty() == false );
		REQUIRE( doc->items().size() == 4 );

		{
			REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

			auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

			REQUIRE( dp->items().size() == 1 );

			REQUIRE( dp->items().at( 0 )->type() == MD::ItemType::Text );

			auto t = static_cast< MD::Text* > ( dp->items().at( 0 ).data() );

			REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( t->text() == QLatin1String( "Line 1..." ) );
		}

		{
			REQUIRE( doc->items().at( 2 )->type() == MD::ItemType::Paragraph );

			auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 2 ).data() );

			REQUIRE( dp->items().size() == 1 );

			REQUIRE( dp->items().at( 0 )->type() == MD::ItemType::Text );

			auto t = static_cast< MD::Text* > ( dp->items().at( 0 ).data() );

			REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( t->text() == QLatin1String( "Line 2..." ) );
		}

		{
			REQUIRE( doc->items().at( 3 )->type() == MD::ItemType::Paragraph );

			auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 3 ).data() );

			REQUIRE( dp->items().size() == 1 );

			REQUIRE( dp->items().at( 0 )->type() == MD::ItemType::Text );

			auto t = static_cast< MD::Text* > ( dp->items().at( 0 ).data() );

			REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( t->text() == QLatin1String( "Line 3..." ) );
		}
	}
	else
		REQUIRE( true == false );
}

TEST_CASE( "and this is one paragraph" )
{
	MD::Parser parser;

	QFile f( "./test16.md" );

	if( f.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
	{
		f.write( "Line 1...\r\nLine 2...\r\nLine 3...\r\n" );
		f.close();

		auto doc = parser.parse( QLatin1String( "./test16.md" ) );

		REQUIRE( doc->isEmpty() == false );
		REQUIRE( doc->items().size() == 2 );

		REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

		auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

		REQUIRE( dp->items().size() == 3 );

		{
			REQUIRE( dp->items().at( 0 )->type() == MD::ItemType::Text );

			auto dt = static_cast< MD::Text* > ( dp->items().at( 0 ).data() );

			REQUIRE( dt->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( dt->text() == QLatin1String( "Line 1..." ) );
		}

		{
			REQUIRE( dp->items().at( 1 )->type() == MD::ItemType::Text );

			auto dt = static_cast< MD::Text* > ( dp->items().at( 1 ).data() );

			REQUIRE( dt->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( dt->text() == QLatin1String( "Line 2..." ) );
		}

		{
			REQUIRE( dp->items().at( 2 )->type() == MD::ItemType::Text );

			auto dt = static_cast< MD::Text* > ( dp->items().at( 2 ).data() );

			REQUIRE( dt->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( dt->text() == QLatin1String( "Line 3..." ) );
		}
	}
	else
		REQUIRE( true == false );
}

TEST_CASE( "quote" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test17.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Blockquote );

	auto bq = static_cast< MD::Blockquote* > ( doc->items().at( 1 ).data() );

	REQUIRE( !bq->isEmpty() );
	REQUIRE( bq->items().size() == 3 );

	{
		REQUIRE( bq->items().at( 0 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( bq->items().at( 0 ).data() );

		REQUIRE( !p->isEmpty() );
		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
		REQUIRE( t->text() == QLatin1String( "Quote paragraph 1." ) );
	}

	{
		REQUIRE( bq->items().at( 1 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( bq->items().at( 1 ).data() );

		REQUIRE( !p->isEmpty() );
		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
		REQUIRE( t->text() == QLatin1String( "Quote paragraph 2." ) );
	}

	REQUIRE( bq->items().at( 2 )->type() == MD::ItemType::Blockquote );

	auto nbq = static_cast< MD::Blockquote* > ( bq->items().at( 2 ).data() );

	REQUIRE( !nbq->isEmpty() );
	REQUIRE( nbq->items().size() == 1 );

	REQUIRE( nbq->items().at( 0 )->type() == MD::ItemType::Paragraph );

	auto p = static_cast< MD::Paragraph* > ( nbq->items().at( 0 ).data() );

	REQUIRE( !p->isEmpty() );
	REQUIRE( p->items().size() == 1 );

	REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

	auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

	REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
	REQUIRE( t->text() == QLatin1String( "Nested quote" ) );
}

TEST_CASE( "quote with spaces" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test18.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Blockquote );

	auto bq = static_cast< MD::Blockquote* > ( doc->items().at( 1 ).data() );

	REQUIRE( !bq->isEmpty() );
	REQUIRE( bq->items().size() == 3 );

	{
		REQUIRE( bq->items().at( 0 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( bq->items().at( 0 ).data() );

		REQUIRE( !p->isEmpty() );
		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
		REQUIRE( t->text() == QLatin1String( "Quote paragraph 1." ) );
	}

	{
		REQUIRE( bq->items().at( 1 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( bq->items().at( 1 ).data() );

		REQUIRE( !p->isEmpty() );
		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
		REQUIRE( t->text() == QLatin1String( "Quote paragraph 2." ) );
	}

	REQUIRE( bq->items().at( 2 )->type() == MD::ItemType::Blockquote );

	auto nbq = static_cast< MD::Blockquote* > ( bq->items().at( 2 ).data() );

	REQUIRE( !nbq->isEmpty() );
	REQUIRE( nbq->items().size() == 1 );

	REQUIRE( nbq->items().at( 0 )->type() == MD::ItemType::Paragraph );

	auto p = static_cast< MD::Paragraph* > ( nbq->items().at( 0 ).data() );

	REQUIRE( !p->isEmpty() );
	REQUIRE( p->items().size() == 1 );

	REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

	auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

	REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
	REQUIRE( t->text() == QLatin1String( "Nested quote" ) );
}

TEST_CASE( "two quotes" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test19.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 3 );

	for( int i = 1; i < 3; ++i )
	{
		REQUIRE( doc->items().at( i )->type() == MD::ItemType::Blockquote );

		auto bq = static_cast< MD::Blockquote* > ( doc->items().at( i ).data() );

		REQUIRE( !bq->isEmpty() );
		REQUIRE( bq->items().size() == 3 );

		{
			REQUIRE( bq->items().at( 0 )->type() == MD::ItemType::Paragraph );

			auto p = static_cast< MD::Paragraph* > ( bq->items().at( 0 ).data() );

			REQUIRE( !p->isEmpty() );
			REQUIRE( p->items().size() == 1 );

			REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

			auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

			REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( t->text() == QLatin1String( "Quote paragraph 1." ) );
		}

		{
			REQUIRE( bq->items().at( 1 )->type() == MD::ItemType::Paragraph );

			auto p = static_cast< MD::Paragraph* > ( bq->items().at( 1 ).data() );

			REQUIRE( !p->isEmpty() );
			REQUIRE( p->items().size() == 1 );

			REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

			auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

			REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( t->text() == QLatin1String( "Quote paragraph 2." ) );
		}

		REQUIRE( bq->items().at( 2 )->type() == MD::ItemType::Blockquote );

		auto nbq = static_cast< MD::Blockquote* > ( bq->items().at( 2 ).data() );

		REQUIRE( !nbq->isEmpty() );
		REQUIRE( nbq->items().size() == 1 );

		REQUIRE( nbq->items().at( 0 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( nbq->items().at( 0 ).data() );

		REQUIRE( !p->isEmpty() );
		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
		REQUIRE( t->text() == QLatin1String( "Nested quote" ) );
	}
}

TEST_CASE( "code" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test20.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Code );

	auto c = static_cast< MD::Code* > ( doc->items().at( 1 ).data() );

	REQUIRE( c->inlined() == false );
	REQUIRE( c->text() ==
		QLatin1String( "if( a > b )\n  do_something();\nelse\n  dont_do_anything();" ) );
}

TEST_CASE( "indented code" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test21.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Code );

	auto c = static_cast< MD::Code* > ( doc->items().at( 1 ).data() );

	REQUIRE( c->inlined() == false );
	REQUIRE( c->text() ==
		QLatin1String( "if( a > b )\n  do_something();\nelse\n  dont_do_anything();" ) );
}

TEST_CASE( "indented by tabs code" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test22.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Code );

	auto c = static_cast< MD::Code* > ( doc->items().at( 1 ).data() );

	REQUIRE( c->inlined() == false );
	REQUIRE( c->text() ==
		QLatin1String( "if( a > b )\n  do_something();\nelse\n  dont_do_anything();" ) );
}

TEST_CASE( "simple unordered list" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test23.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::List );

	auto l = static_cast< MD::List* > ( doc->items().at( 1 ).data() );

	REQUIRE( l->items().size() == 3 );

	for( int i = 0; i < 3; ++i )
	{
		REQUIRE( l->items().at( i )->type() == MD::ItemType::ListItem );

		auto item = static_cast< MD::ListItem* > ( l->items().at( i ).data() );

		REQUIRE( item->listType() == MD::ListItem::Unordered );

		REQUIRE( item->items().size() == 1 );

		REQUIRE( item->items().at( 0 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( item->items().at( 0 ).data() );

		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
		REQUIRE( t->text() == ( QString::fromLatin1( "Item " ) + QString::number( i + 1 ) ) );
	}
}

TEST_CASE( "nested unordered list" )
{
	auto checkItem = [] ( MD::ListItem * item, int i )
	{
		REQUIRE( item->items().at( 0 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( item->items().at( 0 ).data() );

		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
		REQUIRE( t->text() == ( QString::fromLatin1( "Item " ) + QString::number( i + 1 ) ) );
	};

	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test24.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::List );

	auto l = static_cast< MD::List* > ( doc->items().at( 1 ).data() );

	REQUIRE( l->items().size() == 3 );

	for( int i = 0; i < 3; ++i )
	{
		REQUIRE( l->items().at( i )->type() == MD::ItemType::ListItem );

		auto item = static_cast< MD::ListItem* > ( l->items().at( i ).data() );

		REQUIRE( item->listType() == MD::ListItem::Unordered );

		REQUIRE( item->items().size() == 2 );

		checkItem( item, i );

		REQUIRE( item->items().at( 1 )->type() == MD::ItemType::List );

		auto nl = static_cast< MD::List* > ( item->items().at( 1 ).data() );

		REQUIRE( nl->items().size() == 2 );

		for( int j = 0; j < 2; ++j )
		{
			REQUIRE( nl->items().at( j )->type() == MD::ItemType::ListItem );

			auto nitem = static_cast< MD::ListItem* > ( nl->items().at( j ).data() );

			checkItem( nitem, j );
		}
	}
}

TEST_CASE( "unordered list with paragraph" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test25.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::List );

	auto l = static_cast< MD::List* > ( doc->items().at( 1 ).data() );

	REQUIRE( l->items().size() == 3 );

	for( int i = 0; i < 3; ++i )
	{
		REQUIRE( l->items().at( i )->type() == MD::ItemType::ListItem );

		auto item = static_cast< MD::ListItem* > ( l->items().at( i ).data() );

		REQUIRE( item->listType() == MD::ListItem::Unordered );

		REQUIRE( item->items().size() == 2 );

		{
			REQUIRE( item->items().at( 0 )->type() == MD::ItemType::Paragraph );

			auto p = static_cast< MD::Paragraph* > ( item->items().at( 0 ).data() );

			REQUIRE( p->items().size() == 1 );

			REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

			auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

			REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( t->text() == ( QString::fromLatin1( "Item " ) + QString::number( i + 1 ) ) );
		}

		{
			REQUIRE( item->items().at( 1 )->type() == MD::ItemType::Paragraph );

			auto p = static_cast< MD::Paragraph* > ( item->items().at( 1 ).data() );

			REQUIRE( p->items().size() == 1 );

			REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

			auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

			REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( t->text() == ( QString::fromLatin1( "Paragraph in list" ) ) );
		}
	}
}

TEST_CASE( "nested unordered list with paragraph" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test26.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::List );

	auto l = static_cast< MD::List* > ( doc->items().at( 1 ).data() );

	REQUIRE( l->items().size() == 3 );

	for( int i = 0; i < 3; ++i )
	{
		REQUIRE( l->items().at( i )->type() == MD::ItemType::ListItem );

		auto item = static_cast< MD::ListItem* > ( l->items().at( i ).data() );

		REQUIRE( item->listType() == MD::ListItem::Unordered );

		REQUIRE( item->items().size() == 3 );

		{
			REQUIRE( item->items().at( 0 )->type() == MD::ItemType::Paragraph );

			auto p = static_cast< MD::Paragraph* > ( item->items().at( 0 ).data() );

			REQUIRE( p->items().size() == 1 );

			REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

			auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

			REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( t->text() == ( QString::fromLatin1( "Item " ) + QString::number( i + 1 ) ) );
		}

		{
			REQUIRE( item->items().at( 1 )->type() == MD::ItemType::Paragraph );

			auto p = static_cast< MD::Paragraph* > ( item->items().at( 1 ).data() );

			REQUIRE( p->items().size() == 1 );

			REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

			auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

			REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( t->text() == ( QString::fromLatin1( "Paragraph in list" ) ) );
		}

		{
			REQUIRE( item->items().at( 2 )->type() == MD::ItemType::List );

			auto nl = static_cast< MD::List* > ( item->items().at( 2 ).data() );

			REQUIRE( nl->items().at( 0 )->type() == MD::ItemType::ListItem );

			auto item = static_cast< MD::ListItem* > ( nl->items().at( 0 ).data() );

			REQUIRE( item->listType() == MD::ListItem::Unordered );

			REQUIRE( item->items().size() == 2 );

			{
				REQUIRE( item->items().at( 0 )->type() == MD::ItemType::Paragraph );

				auto p = static_cast< MD::Paragraph* > ( item->items().at( 0 ).data() );

				REQUIRE( p->items().size() == 1 );

				REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

				auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

				REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
				REQUIRE( t->text() == QLatin1String( "Nested" ) );
			}

			{
				REQUIRE( item->items().at( 1 )->type() == MD::ItemType::Paragraph );

				auto p = static_cast< MD::Paragraph* > ( item->items().at( 1 ).data() );

				REQUIRE( p->items().size() == 1 );

				REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

				auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

				REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
				REQUIRE( t->text() == ( QString::fromLatin1( "Paragraph in list" ) ) );
			}
		}
	}
}

TEST_CASE( "unordered list with code" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test27.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::List );

	auto l = static_cast< MD::List* > ( doc->items().at( 1 ).data() );

	REQUIRE( l->items().size() == 3 );

	for( int i = 0; i < 3; ++i )
	{
		REQUIRE( l->items().at( i )->type() == MD::ItemType::ListItem );

		auto item = static_cast< MD::ListItem* > ( l->items().at( i ).data() );

		REQUIRE( item->listType() == MD::ListItem::Unordered );

		REQUIRE( item->items().size() == 2 );

		{
			REQUIRE( item->items().at( 0 )->type() == MD::ItemType::Paragraph );

			auto p = static_cast< MD::Paragraph* > ( item->items().at( 0 ).data() );

			REQUIRE( p->items().size() == 1 );

			REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

			auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

			REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( t->text() == ( QString::fromLatin1( "Item " ) + QString::number( i + 1 ) ) );
		}

		{
			REQUIRE( item->items().at( 1 )->type() == MD::ItemType::Code );

			auto c = static_cast< MD::Code* > ( item->items().at( 1 ).data() );

			REQUIRE( c->inlined() == false );
			REQUIRE( c->text() == ( QLatin1String( "code" ) ) );
		}
	}
}

TEST_CASE( "unordered list with code 2" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test28.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::List );

	auto l = static_cast< MD::List* > ( doc->items().at( 1 ).data() );

	REQUIRE( l->items().size() == 3 );

	for( int i = 0; i < 3; ++i )
	{
		REQUIRE( l->items().at( i )->type() == MD::ItemType::ListItem );

		auto item = static_cast< MD::ListItem* > ( l->items().at( i ).data() );

		REQUIRE( item->listType() == MD::ListItem::Unordered );

		REQUIRE( item->items().size() == 2 );

		{
			REQUIRE( item->items().at( 0 )->type() == MD::ItemType::Paragraph );

			auto p = static_cast< MD::Paragraph* > ( item->items().at( 0 ).data() );

			REQUIRE( p->items().size() == 1 );

			REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

			auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

			REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( t->text() == ( QString::fromLatin1( "Item " ) + QString::number( i + 1 ) ) );
		}

		{
			REQUIRE( item->items().at( 1 )->type() == MD::ItemType::Code );

			auto c = static_cast< MD::Code* > ( item->items().at( 1 ).data() );

			REQUIRE( c->inlined() == false );
			REQUIRE( c->text() == ( QLatin1String( "code" ) ) );
		}
	}
}

TEST_CASE( "nested unordered list with paragraph and standalone paragraph" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test29.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 3 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::List );

	auto l = static_cast< MD::List* > ( doc->items().at( 1 ).data() );

	REQUIRE( l->items().size() == 3 );

	for( int i = 0; i < 3; ++i )
	{
		REQUIRE( l->items().at( i )->type() == MD::ItemType::ListItem );

		auto item = static_cast< MD::ListItem* > ( l->items().at( i ).data() );

		REQUIRE( item->listType() == MD::ListItem::Unordered );

		REQUIRE( item->items().size() == 3 );

		{
			REQUIRE( item->items().at( 0 )->type() == MD::ItemType::Paragraph );

			auto p = static_cast< MD::Paragraph* > ( item->items().at( 0 ).data() );

			REQUIRE( p->items().size() == 1 );

			REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

			auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

			REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( t->text() == ( QString::fromLatin1( "Item " ) + QString::number( i + 1 ) ) );
		}

		{
			REQUIRE( item->items().at( 1 )->type() == MD::ItemType::Paragraph );

			auto p = static_cast< MD::Paragraph* > ( item->items().at( 1 ).data() );

			REQUIRE( p->items().size() == 1 );

			REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

			auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

			REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( t->text() == ( QString::fromLatin1( "Paragraph in list" ) ) );
		}

		{
			REQUIRE( item->items().at( 2 )->type() == MD::ItemType::List );

			auto nl = static_cast< MD::List* > ( item->items().at( 2 ).data() );

			REQUIRE( nl->items().at( 0 )->type() == MD::ItemType::ListItem );

			auto item = static_cast< MD::ListItem* > ( nl->items().at( 0 ).data() );

			REQUIRE( item->listType() == MD::ListItem::Unordered );

			REQUIRE( item->items().size() == 2 );

			{
				REQUIRE( item->items().at( 0 )->type() == MD::ItemType::Paragraph );

				auto p = static_cast< MD::Paragraph* > ( item->items().at( 0 ).data() );

				REQUIRE( p->items().size() == 1 );

				REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

				auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

				REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
				REQUIRE( t->text() == QLatin1String( "Nested" ) );
			}

			{
				REQUIRE( item->items().at( 1 )->type() == MD::ItemType::Paragraph );

				auto p = static_cast< MD::Paragraph* > ( item->items().at( 1 ).data() );

				REQUIRE( p->items().size() == 1 );

				REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

				auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

				REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
				REQUIRE( t->text() == ( QString::fromLatin1( "Paragraph in list" ) ) );
			}
		}
	}

	REQUIRE( doc->items().at( 2 )->type() == MD::ItemType::Paragraph );

	auto p = static_cast< MD::Paragraph* > ( doc->items().at( 2 ).data() );

	REQUIRE( p->items().size() == 1 );

	REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

	auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

	REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
	REQUIRE( t->text() == QLatin1String( "Standalone paragraph" ) );
}

TEST_CASE( "three images" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test30.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

	auto p = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

	REQUIRE( p->items().size() == 6 );

	REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

	auto t1 = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

	REQUIRE( t1->text() == QLatin1String( "Text" ) );

	REQUIRE( p->items().at( 1 )->type() == MD::ItemType::Image );

	auto i1 = static_cast< MD::Image* > ( p->items().at( 1 ).data() );

	const QString wd = QDir().absolutePath() + QDir::separator();

	REQUIRE( i1->text() == QLatin1String( "Image 1" ) );
	REQUIRE( i1->url() == wd + QLatin1String( "a.jpg" ) );

	REQUIRE( p->items().at( 2 )->type() == MD::ItemType::Text );

	auto t2 = static_cast< MD::Text* > ( p->items().at( 2 ).data() );

	REQUIRE( t2->text() == QLatin1String( "continue" ) );

	REQUIRE( p->items().at( 3 )->type() == MD::ItemType::Image );

	auto i2 = static_cast< MD::Image* > ( p->items().at( 3 ).data() );

	REQUIRE( i2->text() == QLatin1String( "Image 2" ) );
	REQUIRE( i2->url() == wd + QLatin1String( "b.png" ) );

	REQUIRE( p->items().at( 4 )->type() == MD::ItemType::Text );

	auto t3 = static_cast< MD::Text* > ( p->items().at( 4 ).data() );

	REQUIRE( t3->text() == QLatin1String( "and" ) );

	REQUIRE( p->items().at( 5 )->type() == MD::ItemType::Image );

	auto i3 = static_cast< MD::Image* > ( p->items().at( 5 ).data() );

	REQUIRE( i3->text() == QLatin1String( "Image 3" ) );
	REQUIRE( i3->url() == QLatin1String( "http://www.where.com/c.jpeg" ) );
}

TEST_CASE( "links" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test31.md" ) );

	const QString wd = QDir().absolutePath() + QDir::separator();

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 9 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

	auto p = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

	REQUIRE( p->items().size() == 5 );

	REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Link );

	auto l0 = static_cast< MD::Link* > ( p->items().at( 0 ).data() );

	REQUIRE( l0->text() == QLatin1String( "link 0" ) );

	const QString wrong = QString::fromLatin1( "#wrong-label" ) + QDir::separator() +
		wd + QLatin1String( "test31.md" );

	REQUIRE( l0->url() == wrong );

	REQUIRE( p->items().at( 1 )->type() == MD::ItemType::Link );

	auto l1 = static_cast< MD::Link* > ( p->items().at( 1 ).data() );

	REQUIRE( l1->text() == QLatin1String( "link 1" ) );
	REQUIRE( l1->url() == ( wd + QLatin1String( "a.md" ) ) );

	REQUIRE( p->items().at( 2 )->type() == MD::ItemType::Link );

	auto l2 = static_cast< MD::Link* > ( p->items().at( 2 ).data() );

	REQUIRE( l2->text().isEmpty() );
	REQUIRE( l2->url() == wd + QLatin1String( "b.md" ) );
	REQUIRE( l2->textOptions() == MD::TextOption::TextWithoutFormat );

	REQUIRE( !l2->img().isNull() );
	REQUIRE( l2->img()->text() == QLatin1String( "image 1" ) );
	REQUIRE( l2->img()->url() == wd + QLatin1String( "a.png" ) );

	REQUIRE( p->items().at( 3 )->type() == MD::ItemType::Link );

	auto l3 = static_cast< MD::Link* > ( p->items().at( 3 ).data() );

	REQUIRE( l3->text() == QLatin1String( "link 3" ) );

	const QString label = QString::fromLatin1( "#label" ) + QDir::separator() +
		wd + QLatin1String( "test31.md" );

	REQUIRE( l3->url() == label );

	REQUIRE( p->items().at( 4 )->type() == MD::ItemType::FootnoteRef );

	auto f1 = static_cast< MD::FootnoteRef* > ( p->items().at( 4 ).data() );

	REQUIRE( f1->id() ==
		QString::fromLatin1( "ref" ) + QDir::separator() + wd + QLatin1String( "test31.md" ) );

	REQUIRE( !doc->labeledLinks().isEmpty() );
	REQUIRE( doc->labeledLinks().contains( label ) );
	REQUIRE( doc->labeledLinks()[ label ]->url() == QLatin1String( "http://www.where.com/a.md" ) );


	{
		REQUIRE( doc->items().at( 2 )->type() == MD::ItemType::Paragraph );

		p = static_cast< MD::Paragraph* > ( doc->items().at( 2 ).data() );

		REQUIRE( p->items().size() == 2 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::FootnoteRef );

		f1 = static_cast< MD::FootnoteRef* > ( p->items().at( 0 ).data() );

		REQUIRE( f1->id() ==
			QString::fromLatin1( "ref" ) + QDir::separator() + wd + QLatin1String( "test31.md" ) );

		auto t = static_cast< MD::Text* > ( p->items().at( 1 ).data() );

		REQUIRE( t->text() == QLatin1String( "text" ) );

		REQUIRE( doc->labeledLinks().size() == 2 );

		REQUIRE( doc->labeledLinks()[ QString::fromLatin1( "#1" ) +
			QDir::separator() + wd + QLatin1String( "test31.md" ) ]->url() == wd + QLatin1String( "a.md" ) );
	}

	{
		REQUIRE( doc->items().at( 3 )->type() == MD::ItemType::Paragraph );

		p = static_cast< MD::Paragraph* > ( doc->items().at( 3 ).data() );

		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Link );

		auto l = static_cast< MD::Link* > ( p->items().at( 0 ).data() );

		REQUIRE( l->url() == QString::fromLatin1( "#label" ) +
			QDir::separator() + wd + QLatin1String( "test31.md" ) );
	}

	REQUIRE( doc->items().at( 4 )->type() == MD::ItemType::PageBreak );
}

TEST_CASE( "code in blockquote" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test32.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Blockquote );

	auto q = static_cast< MD::Blockquote* > ( doc->items().at( 1 ).data() );

	REQUIRE( q->items().size() == 1 );

	REQUIRE( q->items().at( 0 )->type() == MD::ItemType::Code );

	auto c = static_cast< MD::Code* > ( q->items().at( 0 ).data() );

	REQUIRE( c->inlined() == false );
	REQUIRE( c->text() == QLatin1String( "if( a < b )\n  do_something();" ) );
}

TEST_CASE( "simple link" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test33.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

	auto p = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

	REQUIRE( p->items().size() == 1 );

	REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Link );

	auto l = static_cast< MD::Link* > ( p->items().at( 0 ).data() );

	REQUIRE( l->url() == QLatin1String( "www.google.com" ) );
	REQUIRE( l->text().isEmpty() );
}

TEST_CASE( "styled link" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test34.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

	auto p = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

	REQUIRE( p->items().size() == 1 );

	REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Link );

	auto l = static_cast< MD::Link* > ( p->items().at( 0 ).data() );

	REQUIRE( l->url() == QLatin1String( "https://www.google.com" ) );
	REQUIRE( l->text() == QLatin1String( "Google" ) );
	REQUIRE( l->textOptions() == MD::TextOption::BoldText );
}

TEST_CASE( "ordered list" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test35.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::List );

	auto l = static_cast< MD::List* > ( doc->items().at( 1 ).data() );

	REQUIRE( l->items().size() == 3 );

	{
		REQUIRE( l->items().at( 0 )->type() == MD::ItemType::ListItem );

		auto i1 = static_cast< MD::ListItem* > ( l->items().at( 0 ).data() );

		REQUIRE( i1->listType() == MD::ListItem::Ordered );
		REQUIRE( i1->orderedListPreState() == MD::ListItem::Start );
		REQUIRE( i1->items().size() == 1 );
		REQUIRE( i1->items().at( 0 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( i1->items().at( 0 ).data() );

		REQUIRE( p->items().size() == 1 );
		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );
		REQUIRE( static_cast< MD::Text* > ( p->items().at( 0 ).data() )->text() == QLatin1String( "1" ) );
	}

	REQUIRE( l->items().size() == 3 );

	{
		REQUIRE( l->items().at( 1 )->type() == MD::ItemType::ListItem );

		auto i1 = static_cast< MD::ListItem* > ( l->items().at( 1 ).data() );

		REQUIRE( i1->listType() == MD::ListItem::Ordered );
		REQUIRE( i1->orderedListPreState() == MD::ListItem::Continue );
		REQUIRE( i1->items().size() == 2 );
		REQUIRE( i1->items().at( 0 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( i1->items().at( 0 ).data() );

		REQUIRE( p->items().size() == 1 );
		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );
		REQUIRE( static_cast< MD::Text* > ( p->items().at( 0 ).data() )->text() == QLatin1String( "2" ) );

		REQUIRE( i1->items().at( 1 )->type() == MD::ItemType::List );

		auto nl = static_cast< MD::List* > ( i1->items().at( 1 ).data() );

		REQUIRE( nl->items().size() == 2 );

		{
			REQUIRE( nl->items().at( 0 )->type() == MD::ItemType::ListItem );

			auto i1 = static_cast< MD::ListItem* > ( nl->items().at( 0 ).data() );

			REQUIRE( i1->listType() == MD::ListItem::Ordered );
			REQUIRE( i1->orderedListPreState() == MD::ListItem::Start );
			REQUIRE( i1->items().size() == 1 );
			REQUIRE( i1->items().at( 0 )->type() == MD::ItemType::Paragraph );

			auto p = static_cast< MD::Paragraph* > ( i1->items().at( 0 ).data() );

			REQUIRE( p->items().size() == 1 );
			REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );
			REQUIRE( static_cast< MD::Text* > ( p->items().at( 0 ).data() )->text() == QLatin1String( "1" ) );
		}

		{
			REQUIRE( nl->items().at( 1 )->type() == MD::ItemType::ListItem );

			auto i1 = static_cast< MD::ListItem* > ( nl->items().at( 1 ).data() );

			REQUIRE( i1->listType() == MD::ListItem::Ordered );
			REQUIRE( i1->orderedListPreState() == MD::ListItem::Continue );
			REQUIRE( i1->items().size() == 1 );
			REQUIRE( i1->items().at( 0 )->type() == MD::ItemType::Paragraph );

			auto p = static_cast< MD::Paragraph* > ( i1->items().at( 0 ).data() );

			REQUIRE( p->items().size() == 1 );
			REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );
			REQUIRE( static_cast< MD::Text* > ( p->items().at( 0 ).data() )->text() == QLatin1String( "2" ) );
		}
	}

	{
		REQUIRE( l->items().at( 2 )->type() == MD::ItemType::ListItem );

		auto i1 = static_cast< MD::ListItem* > ( l->items().at( 2 ).data() );

		REQUIRE( i1->listType() == MD::ListItem::Ordered );
		REQUIRE( i1->orderedListPreState() == MD::ListItem::Continue );
		REQUIRE( i1->items().size() == 1 );
		REQUIRE( i1->items().at( 0 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( i1->items().at( 0 ).data() );

		REQUIRE( p->items().size() == 1 );
		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );
		REQUIRE( static_cast< MD::Text* > ( p->items().at( 0 ).data() )->text() == QLatin1String( "3" ) );
	}
}

TEST_CASE( "link with caption" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test36.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

	auto p = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

	REQUIRE( p->items().size() == 1 );

	REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Link );

	auto l = static_cast< MD::Link* > ( p->items().at( 0 ).data() );

	REQUIRE( l->url() == QLatin1String( "www.google.com" ) );
	REQUIRE( l->text() == QLatin1String( "Google" ) );
}

TEST_CASE( "wrong links" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test37.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 17 );

	{
		REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->text() == QLatin1String( "[Google] ( www.google.com Google Shmoogle..." ) );
	}

	{
		REQUIRE( doc->items().at( 2 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( doc->items().at( 2 ).data() );

		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->text() == QLatin1String( "[Google] (" ) );
	}

	{
		REQUIRE( doc->items().at( 3 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( doc->items().at( 3 ).data() );

		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->text() == QLatin1String( "[Google" ) );
	}

	{
		REQUIRE( doc->items().at( 4 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( doc->items().at( 4 ).data() );

		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->text() == QLatin1String( "[Google]" ) );
	}

	{
		REQUIRE( doc->items().at( 5 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( doc->items().at( 5 ).data() );

		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->text() == QLatin1String( "[![Google](" ) );
	}

	{
		REQUIRE( doc->items().at( 6 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( doc->items().at( 6 ).data() );

		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->text() == QLatin1String( "![Google](" ) );
	}

	{
		REQUIRE( doc->items().at( 7 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( doc->items().at( 7 ).data() );

		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->text() == QLatin1String( "[Google] ( www.google.com \"Google Shmoogle...\"" ) );
	}

	{
		REQUIRE( doc->items().at( 8 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( doc->items().at( 8 ).data() );

		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->text() == QLatin1String( "[![Google](https://www.google.com/logo.png)" ) );
	}

	{
		REQUIRE( doc->items().at( 9 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( doc->items().at( 9 ).data() );

		REQUIRE( p->items().size() == 2 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->text() == QLatin1String( "text" ) );

		REQUIRE( p->items().at( 1 )->type() == MD::ItemType::Text );

		t = static_cast< MD::Text* > ( p->items().at( 1 ).data() );

		REQUIRE( t->text() == QLatin1String( "[^ref]:" ) );
	}

	{
		REQUIRE( doc->items().at( 10 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( doc->items().at( 10 ).data() );

		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->text() == QLatin1String( "[^ref" ) );
	}

	{
		REQUIRE( doc->items().at( 11 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( doc->items().at( 11 ).data() );

		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->text() == QLatin1String( "[" ) );
	}

	{
		REQUIRE( doc->items().at( 12 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( doc->items().at( 12 ).data() );

		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->text() == QLatin1String( "[1]:" ) );
	}

	{
		REQUIRE( doc->items().at( 13 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( doc->items().at( 13 ).data() );

		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->text() == QLatin1String( "[1]:" ) );
	}

	{
		REQUIRE( doc->items().at( 14 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( doc->items().at( 14 ).data() );

		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->text() == QLatin1String( "[text][link" ) );
	}

	{
		REQUIRE( doc->items().at( 15 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( doc->items().at( 15 ).data() );

		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->text() == QLatin1String( "[text]#" ) );
	}

	{
		REQUIRE( doc->items().at( 16 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( doc->items().at( 16 ).data() );

		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->text() == QLatin1String( "<www.google.com" ) );
	}
}

TEST_CASE( "ordered list with tabs" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test38.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::List );

	auto l = static_cast< MD::List* > ( doc->items().at( 1 ).data() );

	REQUIRE( l->items().size() == 3 );

	{
		REQUIRE( l->items().at( 0 )->type() == MD::ItemType::ListItem );

		auto i1 = static_cast< MD::ListItem* > ( l->items().at( 0 ).data() );

		REQUIRE( i1->listType() == MD::ListItem::Ordered );
		REQUIRE( i1->orderedListPreState() == MD::ListItem::Start );
		REQUIRE( i1->items().size() == 1 );
		REQUIRE( i1->items().at( 0 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( i1->items().at( 0 ).data() );

		REQUIRE( p->items().size() == 1 );
		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );
		REQUIRE( static_cast< MD::Text* > ( p->items().at( 0 ).data() )->text() == QLatin1String( "1" ) );
	}

	REQUIRE( l->items().size() == 3 );

	{
		REQUIRE( l->items().at( 1 )->type() == MD::ItemType::ListItem );

		auto i1 = static_cast< MD::ListItem* > ( l->items().at( 1 ).data() );

		REQUIRE( i1->listType() == MD::ListItem::Ordered );
		REQUIRE( i1->orderedListPreState() == MD::ListItem::Continue );
		REQUIRE( i1->items().size() == 2 );
		REQUIRE( i1->items().at( 0 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( i1->items().at( 0 ).data() );

		REQUIRE( p->items().size() == 1 );
		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );
		REQUIRE( static_cast< MD::Text* > ( p->items().at( 0 ).data() )->text() == QLatin1String( "2" ) );

		REQUIRE( i1->items().at( 1 )->type() == MD::ItemType::List );

		auto nl = static_cast< MD::List* > ( i1->items().at( 1 ).data() );

		REQUIRE( nl->items().size() == 2 );

		{
			REQUIRE( nl->items().at( 0 )->type() == MD::ItemType::ListItem );

			auto i1 = static_cast< MD::ListItem* > ( nl->items().at( 0 ).data() );

			REQUIRE( i1->listType() == MD::ListItem::Ordered );
			REQUIRE( i1->orderedListPreState() == MD::ListItem::Start );
			REQUIRE( i1->items().size() == 1 );
			REQUIRE( i1->items().at( 0 )->type() == MD::ItemType::Paragraph );

			auto p = static_cast< MD::Paragraph* > ( i1->items().at( 0 ).data() );

			REQUIRE( p->items().size() == 1 );
			REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );
			REQUIRE( static_cast< MD::Text* > ( p->items().at( 0 ).data() )->text() == QLatin1String( "1" ) );
		}

		{
			REQUIRE( nl->items().at( 1 )->type() == MD::ItemType::ListItem );

			auto i1 = static_cast< MD::ListItem* > ( nl->items().at( 1 ).data() );

			REQUIRE( i1->listType() == MD::ListItem::Ordered );
			REQUIRE( i1->orderedListPreState() == MD::ListItem::Continue );
			REQUIRE( i1->items().size() == 1 );
			REQUIRE( i1->items().at( 0 )->type() == MD::ItemType::Paragraph );

			auto p = static_cast< MD::Paragraph* > ( i1->items().at( 0 ).data() );

			REQUIRE( p->items().size() == 1 );
			REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );
			REQUIRE( static_cast< MD::Text* > ( p->items().at( 0 ).data() )->text() == QLatin1String( "2" ) );
		}
	}

	{
		REQUIRE( l->items().at( 2 )->type() == MD::ItemType::ListItem );

		auto i1 = static_cast< MD::ListItem* > ( l->items().at( 2 ).data() );

		REQUIRE( i1->listType() == MD::ListItem::Ordered );
		REQUIRE( i1->orderedListPreState() == MD::ListItem::Continue );
		REQUIRE( i1->items().size() == 1 );
		REQUIRE( i1->items().at( 0 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( i1->items().at( 0 ).data() );

		REQUIRE( p->items().size() == 1 );
		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );
		REQUIRE( static_cast< MD::Text* > ( p->items().at( 0 ).data() )->text() == QLatin1String( "3" ) );
	}
}

TEST_CASE( "wrong style" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test39.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

	auto p = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

	REQUIRE( p->items().size() == 1 );
	REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

	auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

	REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
	REQUIRE( t->text() == QLatin1String( "****text****" ) );
}

TEST_CASE( "unfinished code" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test40.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

	REQUIRE( dp->items().size() == 2 );

	REQUIRE( dp->items().at( 0 )->type() == MD::ItemType::Text );

	auto t = static_cast< MD::Text* > ( dp->items().at( 0 ).data() );

	REQUIRE( t->text() == QLatin1String( "``Use this `code`" ) );

	REQUIRE( dp->items().at( 1 )->type() == MD::ItemType::Text );

	t = static_cast< MD::Text* > ( dp->items().at( 1 ).data() );

	REQUIRE( t->text() == QLatin1String( "in the code" ) );
}

TEST_CASE( "unordered list with paragraph with tabs" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test41.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::List );

	auto l = static_cast< MD::List* > ( doc->items().at( 1 ).data() );

	REQUIRE( l->items().size() == 3 );

	for( int i = 0; i < 3; ++i )
	{
		REQUIRE( l->items().at( i )->type() == MD::ItemType::ListItem );

		auto item = static_cast< MD::ListItem* > ( l->items().at( i ).data() );

		REQUIRE( item->listType() == MD::ListItem::Unordered );

		REQUIRE( item->items().size() == 2 );

		{
			REQUIRE( item->items().at( 0 )->type() == MD::ItemType::Paragraph );

			auto p = static_cast< MD::Paragraph* > ( item->items().at( 0 ).data() );

			REQUIRE( p->items().size() == 1 );

			REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

			auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

			REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( t->text() == ( QString::fromLatin1( "Item " ) + QString::number( i + 1 ) ) );
		}

		{
			REQUIRE( item->items().at( 1 )->type() == MD::ItemType::Paragraph );

			auto p = static_cast< MD::Paragraph* > ( item->items().at( 1 ).data() );

			REQUIRE( p->items().size() == 1 );

			REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

			auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

			REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( t->text() == ( QString::fromLatin1( "Paragraph in list" ) ) );
		}
	}
}

TEST_CASE( "linked md" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test42.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 5 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::List );

	auto l = static_cast< MD::List* > ( doc->items().at( 1 ).data() );

	REQUIRE( l->items().size() == 2 );

	const QString wd = QDir().absolutePath() + QDir::separator();

	for( int i = 0; i < 2; ++i )
	{
		REQUIRE( l->items().at( i )->type() == MD::ItemType::ListItem );

		auto li = static_cast< MD::ListItem* > ( l->items().at( i ).data() );

		REQUIRE( li->items().size() == 1 );
		REQUIRE( li->items().at( 0 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( li->items().at( 0 ).data() );

		REQUIRE( p->items().size() == 1 );
		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Link );

		auto lnk = static_cast< MD::Link* > ( p->items().at( 0 ).data() );

		REQUIRE( lnk->text() == QLatin1String( "Chapter 1" ) );
		REQUIRE( lnk->url() == wd + QLatin1String( "test42-1.md" ) );
	}

	REQUIRE( doc->items().at( 2 )->type() == MD::ItemType::PageBreak );

	REQUIRE( doc->items().at( 3 )->type() == MD::ItemType::Anchor );

	REQUIRE( static_cast< MD::Anchor* > ( doc->items().at( 3 ).data() )->label() ==
		wd + QLatin1String( "test42-1.md" ) );

	REQUIRE( doc->items().at( 4 )->type() == MD::ItemType::Paragraph );

	auto p = static_cast< MD::Paragraph* > ( doc->items().at( 4 ).data() );

	REQUIRE( p->items().size() == 1 );
	REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

	auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

	REQUIRE( t->text() == QLatin1String( "Paragraph 1" ) );
}

TEST_CASE( "linked md (not recursive)" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test42.md" ), false );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::List );

	auto l = static_cast< MD::List* > ( doc->items().at( 1 ).data() );

	REQUIRE( l->items().size() == 2 );

	const QString wd = QDir().absolutePath() + QDir::separator();

	for( int i = 0; i < 2; ++i )
	{
		REQUIRE( l->items().at( i )->type() == MD::ItemType::ListItem );

		auto li = static_cast< MD::ListItem* > ( l->items().at( i ).data() );

		REQUIRE( li->items().size() == 1 );
		REQUIRE( li->items().at( 0 )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( li->items().at( 0 ).data() );

		REQUIRE( p->items().size() == 1 );
		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Link );

		auto lnk = static_cast< MD::Link* > ( p->items().at( 0 ).data() );

		REQUIRE( lnk->text() == QLatin1String( "Chapter 1" ) );
		REQUIRE( lnk->url() == wd + QLatin1String( "test42-1.md" ) );
	}
}

TEST_CASE( "blockquote in list" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test44.md" ), false );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::List );

	auto l = static_cast< MD::List* > ( doc->items().at( 1 ).data() );

	REQUIRE( l->items().size() == 1 );

	REQUIRE( l->items().at( 0 )->type() == MD::ItemType::ListItem );

	auto li = static_cast< MD::ListItem* > ( l->items().at( 0 ).data() );

	REQUIRE( li->items().size() == 2 );
	REQUIRE( li->items().at( 0 )->type() == MD::ItemType::Paragraph );

	auto p = static_cast< MD::Paragraph* > ( li->items().at( 0 ).data() );

	REQUIRE( p->items().size() == 1 );
	REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

	auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

	REQUIRE( t->text() == QLatin1String( "Item" ) );

	REQUIRE( li->items().at( 1 )->type() == MD::ItemType::Blockquote );

	auto bq = static_cast< MD::Blockquote* > ( li->items().at( 1 ).data() );

	REQUIRE( bq->items().size() == 1 );

	REQUIRE( bq->items().at( 0 )->type() == MD::ItemType::Paragraph );

	p = static_cast< MD::Paragraph* > ( bq->items().at( 0 ).data() );

	REQUIRE( p->items().size() == 1 );
	REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

	t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

	REQUIRE( t->text() == QLatin1String( "Quote" ) );
}

TEST_CASE( "footnote" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test45.md" ), false );

	REQUIRE( !doc->isEmpty() );
	REQUIRE( doc->items().size() == 1 );
	REQUIRE( doc->items().at( 0 )->type() == MD::ItemType::Anchor );

	REQUIRE( doc->footnotesMap().size() == 1 );

	const QString wd = QDir().absolutePath() + QDir::separator();

	const QString label = QString::fromLatin1( "#footnote" ) + QDir::separator() + wd +
		QLatin1String( "test45.md" );

	REQUIRE( doc->footnotesMap().contains( label ) );

	auto f = doc->footnotesMap()[ label ];

	REQUIRE( f->items().size() == 3 );

	for( int i = 0; i < 3; ++i )
	{
		REQUIRE( f->items().at( i )->type() == MD::ItemType::Paragraph );

		auto p = static_cast< MD::Paragraph* > ( f->items().at( i ).data() );

		REQUIRE( p->items().size() == 1 );

		REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

		auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

		REQUIRE( t->text() == QLatin1String( "Paragraph in footnote" ) );
	}
}

TEST_CASE( "headings" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test46.md" ) );

	REQUIRE( !doc->isEmpty() );
	REQUIRE( doc->items().size() == 14 );

	int idx = 1;

	for( int i = 1; i < 4; ++i )
	{
		for( int j = 1; j < 3; ++j )
		{
			REQUIRE( doc->items().at( idx )->type() == MD::ItemType::Heading );

			auto h = static_cast< MD::Heading* > ( doc->items().at( idx ).data() );

			REQUIRE( h->level() == j );
			REQUIRE( h->text() == QString::fromLatin1( "Heading " ) + QString::number( j ) );
			REQUIRE( !h->isLabeled() );

			++idx;

			REQUIRE( doc->items().at( idx )->type() == MD::ItemType::Paragraph );

			auto p = static_cast< MD::Paragraph* > ( doc->items().at( idx ).data() );

			REQUIRE( p->items().size() == 1 );
			REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );
			REQUIRE( static_cast< MD::Text* > ( p->items().at( 0 ).data() )->text() ==
				QString::fromLatin1( "Paragraph " ) + QString::number( j ) );

			++idx;
		}
	}

	REQUIRE( doc->items().at( idx )->type() == MD::ItemType::Heading );

	auto h = static_cast< MD::Heading* > ( doc->items().at( idx ).data() );

	REQUIRE( h->level() == 3 );
	REQUIRE( h->text() == QLatin1String( "Heading 3" ) );
	REQUIRE( h->isLabeled() );

	const QString wd = QDir().absolutePath() + QDir::separator();
	const QString label = QString::fromLatin1( "#heading-3" ) + QDir::separator() +
		wd + QLatin1String( "test46.md" );

	REQUIRE( h->label() == label );

	REQUIRE( doc->labeledHeadings().size() == 1 );
	REQUIRE( doc->labeledHeadings().contains( label ) );
	REQUIRE( doc->labeledHeadings()[ label ].data() == h );
}

TEST_CASE( "tables" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test47.md" ) );

	REQUIRE( !doc->isEmpty() );
	REQUIRE( doc->items().size() == 3 );

	for( int i = 1; i < 3; ++i )
	{
		REQUIRE( doc->items().at( i )->type() == MD::ItemType::Table );

		auto t = static_cast< MD::Table* > ( doc->items().at( i ).data() );

		REQUIRE( t->columnsCount() == 2 );
		REQUIRE( t->rows().size() == 2 );

		auto r0 = t->rows().at( 0 );

		REQUIRE( r0->cells().size() == 2 );

		{
			auto c0 = static_cast< MD::TableCell* > ( r0->cells().at( 0 ).data() );

			REQUIRE( c0->items().size() == 1 );
			REQUIRE( c0->items().at( 0 )->type() == MD::ItemType::Text );

			auto t0 = static_cast< MD::Text* > ( c0->items().at( 0 ).data() );

			REQUIRE( t0->text() == QLatin1String( "Column 1" ) );
		}

		{
			auto c1 = static_cast< MD::TableCell* > ( r0->cells().at( 1 ).data() );

			REQUIRE( c1->items().size() == 1 );
			REQUIRE( c1->items().at( 0 )->type() == MD::ItemType::Text );

			auto t1 = static_cast< MD::Text* > ( c1->items().at( 0 ).data() );

			REQUIRE( t1->text() == QLatin1String( "Column 2" ) );
		}

		auto r1 = t->rows().at( 1 );

		REQUIRE( r1->cells().size() == 2 );

		{
			auto c0 = static_cast< MD::TableCell* > ( r1->cells().at( 0 ).data() );

			REQUIRE( c0->items().size() == 1 );
			REQUIRE( c0->items().at( 0 )->type() == MD::ItemType::Text );

			auto t0 = static_cast< MD::Text* > ( c0->items().at( 0 ).data() );

			REQUIRE( t0->text() == QLatin1String( "Cell 1" ) );
		}

		{
			auto c1 = static_cast< MD::TableCell* > ( r1->cells().at( 1 ).data() );

			REQUIRE( c1->items().size() == 1 );
			REQUIRE( c1->items().at( 0 )->type() == MD::ItemType::Text );

			auto t1 = static_cast< MD::Text* > ( c1->items().at( 0 ).data() );

			REQUIRE( t1->text() == QLatin1String( "Cell 2" ) );
		}
	}

	auto table = static_cast< MD::Table* > ( doc->items().at( 1 ).data() );

	REQUIRE( table->columnAlignment( 0 ) == MD::Table::AlignLeft );
	REQUIRE( table->columnAlignment( 1 ) == MD::Table::AlignLeft );

	table = static_cast< MD::Table* > ( doc->items().at( 2 ).data() );

	REQUIRE( table->columnAlignment( 0 ) == MD::Table::AlignCenter );
	REQUIRE( table->columnAlignment( 1 ) == MD::Table::AlignRight );
}

TEST_CASE( "without spaces" )
{
	MD::Parser parser;
	auto doc = parser.parse( QLatin1String( "./test48.md" ) );
	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 4 );
	REQUIRE( doc->items().at( 0 )->type() == MD::ItemType::Anchor );

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Heading );
	REQUIRE( static_cast< MD::Heading* > ( doc->items().at( 1 ).data() )->text() ==
		QLatin1String( "Heading" ) );

	REQUIRE( doc->items().at( 2 )->type() == MD::ItemType::Paragraph );
	auto * p = static_cast< MD::Paragraph* > ( doc->items().at( 2 ).data() );
	REQUIRE( p->items().size() == 1 );
	REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );
	REQUIRE( static_cast< MD::Text* > ( p->items().at( 0 ).data() )->text() ==
		QLatin1String( "Paragraph" ) );

	REQUIRE( doc->items().at( 3 )->type() == MD::ItemType::Heading );
	REQUIRE( static_cast< MD::Heading* > ( doc->items().at( 3 ).data() )->text() ==
		QLatin1String( "Heading" ) );
}

TEST_CASE( "comments" )
{
	MD::Parser parser;
	auto doc = parser.parse( QLatin1String( "./test49.md" ) );
	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 5 );
	REQUIRE( doc->items().at( 0 )->type() == MD::ItemType::Anchor );

	for( int i = 1; i < 4; ++i )
	{
		REQUIRE( doc->items().at( i )->type() == MD::ItemType::Heading );
		auto * h = static_cast< MD::Heading* > ( doc->items().at( i ).data() );
		REQUIRE( h->text() == QLatin1String( "Heading 1" ) );
		REQUIRE( h->level() == 1 );
	}

	REQUIRE( doc->items().at( 4 )->type() == MD::ItemType::Paragraph );
	auto * p = static_cast< MD::Paragraph* > ( doc->items().at( 4 ).data() );
	REQUIRE( p->items().size() == 1 );
	REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );
	auto * t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );
	REQUIRE( t->text() == QLatin1String( "--> # Heading 1" ) );
}
