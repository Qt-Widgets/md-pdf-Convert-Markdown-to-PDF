
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
