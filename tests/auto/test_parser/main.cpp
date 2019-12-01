
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


TEST_CASE( "empty" )
{
	MD::Parser p;
	auto doc = p.parse( QLatin1String( "./test1.md" ) );
	REQUIRE( doc->isEmpty() == true );
}

TEST_CASE( "only text" )
{
	MD::Parser p;
	auto doc = p.parse( QLatin1String( "./test2.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 1 );
	REQUIRE( doc->items().first()->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().first().data() );

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
	REQUIRE( doc->items().size() == 2 );

	{
		REQUIRE( doc->items().first()->type() == MD::ItemType::Paragraph );

		auto dp = static_cast< MD::Paragraph* > ( doc->items().first().data() );

		REQUIRE( dp->items().size() == 1 );
		REQUIRE( dp->items().first()->type() == MD::ItemType::Text );

		auto dt = static_cast< MD::Text* > ( dp->items().first().data() );

		REQUIRE( dt->opts() == MD::TextOption::TextWithoutFormat );
		REQUIRE( dt->text() == QLatin1String( "Paragraph 1." ) );
	}

	{
		REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

		auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

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
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().first()->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().first().data() );

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
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().first()->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().first().data() );

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
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().first()->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().first().data() );

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
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().first()->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().first().data() );

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
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().first()->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().first().data() );

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
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().first()->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().first().data() );

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
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().first()->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().first().data() );

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
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().first()->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().first().data() );

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
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().first()->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().first().data() );

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
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().first()->type() == MD::ItemType::Paragraph );

	auto dp = static_cast< MD::Paragraph* > ( doc->items().first().data() );

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
		REQUIRE( doc->items().size() == 1 );

		REQUIRE( doc->items().first()->type() == MD::ItemType::Paragraph );

		auto dp = static_cast< MD::Paragraph* > ( doc->items().first().data() );

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
		REQUIRE( doc->items().size() == 3 );

		{
			REQUIRE( doc->items().at( 0 )->type() == MD::ItemType::Paragraph );

			auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 0 ).data() );

			REQUIRE( dp->items().size() == 1 );

			REQUIRE( dp->items().at( 0 )->type() == MD::ItemType::Text );

			auto t = static_cast< MD::Text* > ( dp->items().at( 0 ).data() );

			REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( t->text() == QLatin1String( "Line 1..." ) );
		}

		{
			REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

			auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

			REQUIRE( dp->items().size() == 1 );

			REQUIRE( dp->items().at( 0 )->type() == MD::ItemType::Text );

			auto t = static_cast< MD::Text* > ( dp->items().at( 0 ).data() );

			REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
			REQUIRE( t->text() == QLatin1String( "Line 2..." ) );
		}

		{
			REQUIRE( doc->items().at( 2 )->type() == MD::ItemType::Paragraph );

			auto dp = static_cast< MD::Paragraph* > ( doc->items().at( 2 ).data() );

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
		REQUIRE( doc->items().size() == 1 );

		REQUIRE( doc->items().first()->type() == MD::ItemType::Paragraph );

		auto dp = static_cast< MD::Paragraph* > ( doc->items().first().data() );

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
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().at( 0 )->type() == MD::ItemType::Blockquote );

	auto bq = static_cast< MD::Blockquote* > ( doc->items().at( 0 ).data() );

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
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().at( 0 )->type() == MD::ItemType::Blockquote );

	auto bq = static_cast< MD::Blockquote* > ( doc->items().at( 0 ).data() );

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
	REQUIRE( doc->items().size() == 2 );

	for( int i = 0; i < 2; ++i )
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
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().at( 0 )->type() == MD::ItemType::Code );

	auto c = static_cast< MD::Code* > ( doc->items().at( 0 ).data() );

	REQUIRE( c->inlined() == false );
	REQUIRE( c->text() ==
		QLatin1String( "if( a > b )\n  do_something();\nelse\n  dont_do_anything();" ) );
}

TEST_CASE( "indented code" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test21.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().at( 0 )->type() == MD::ItemType::Code );

	auto c = static_cast< MD::Code* > ( doc->items().at( 0 ).data() );

	REQUIRE( c->inlined() == false );
	REQUIRE( c->text() ==
		QLatin1String( "if( a > b )\n  do_something();\nelse\n  dont_do_anything();" ) );
}

TEST_CASE( "indented by tabs code" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test22.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().at( 0 )->type() == MD::ItemType::Code );

	auto c = static_cast< MD::Code* > ( doc->items().at( 0 ).data() );

	REQUIRE( c->inlined() == false );
	REQUIRE( c->text() ==
		QLatin1String( "if( a > b )\n  do_something();\nelse\n  dont_do_anything();" ) );
}

TEST_CASE( "simple unordered list" )
{
	MD::Parser parser;

	auto doc = parser.parse( QLatin1String( "./test23.md" ) );

	REQUIRE( doc->isEmpty() == false );
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().at( 0 )->type() == MD::ItemType::List );

	auto l = static_cast< MD::List* > ( doc->items().at( 0 ).data() );

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
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().at( 0 )->type() == MD::ItemType::List );

	auto l = static_cast< MD::List* > ( doc->items().at( 0 ).data() );

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
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().at( 0 )->type() == MD::ItemType::List );

	auto l = static_cast< MD::List* > ( doc->items().at( 0 ).data() );

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
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().at( 0 )->type() == MD::ItemType::List );

	auto l = static_cast< MD::List* > ( doc->items().at( 0 ).data() );

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
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().at( 0 )->type() == MD::ItemType::List );

	auto l = static_cast< MD::List* > ( doc->items().at( 0 ).data() );

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
	REQUIRE( doc->items().size() == 1 );

	REQUIRE( doc->items().at( 0 )->type() == MD::ItemType::List );

	auto l = static_cast< MD::List* > ( doc->items().at( 0 ).data() );

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
	REQUIRE( doc->items().size() == 2 );

	REQUIRE( doc->items().at( 0 )->type() == MD::ItemType::List );

	auto l = static_cast< MD::List* > ( doc->items().at( 0 ).data() );

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

	REQUIRE( doc->items().at( 1 )->type() == MD::ItemType::Paragraph );

	auto p = static_cast< MD::Paragraph* > ( doc->items().at( 1 ).data() );

	REQUIRE( p->items().size() == 1 );

	REQUIRE( p->items().at( 0 )->type() == MD::ItemType::Text );

	auto t = static_cast< MD::Text* > ( p->items().at( 0 ).data() );

	REQUIRE( t->opts() == MD::TextOption::TextWithoutFormat );
	REQUIRE( t->text() == QLatin1String( "Standalone paragraph" ) );
}
