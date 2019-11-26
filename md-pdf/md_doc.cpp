
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
#include "md_doc.hpp"


namespace MD {


//
// Item
//

ItemType
Item::type() const
{
	return ItemType::Unknown;
}


//
// Heading
//

Heading::Heading( const QString & text, int l )
	:	m_text( text )
	,	m_level( l )
{
}

ItemType
Heading::type() const
{
	return ItemType::Heading;
}

const QString &
Heading::text() const
{
	return m_text;
}

void
Heading::setText( const QString & t )
{
	m_text = t;
}

int
Heading::level() const
{
	return m_level;
}

void
Heading::setLevel( int l )
{
	m_level = l;
}


//
// Text
//

ItemType
Text::type() const
{
	return ItemType::Text;
}

const Text::TextData &
Text::data() const
{
	return m_data;
}

void
Text::setData( const TextData & d )
{
	m_data = d;
}

void
Text::appendText( const TextWithOptions & t )
{
	m_data.append( t );
}


//
// LineBreak
//

ItemType
LineBreak::type() const
{
	return ItemType::LineBreak;
}


//
// Block
//

const Block::Items &
Block::items() const
{
	return m_items;
}

void
Block::setItems( const Items & i )
{
	m_items = i;
}

void
Block::appendItem( QSharedPointer< Item > i )
{
	m_items.append( i );
}


//
// Paragraph
//

ItemType
Paragraph::type() const
{
	return ItemType::Paragraph;
}


//
// Blockquote
//

ItemType
Blockquote::type() const
{
	return ItemType::Blockquote;
}


//
// ListItem
//

ItemType
ListItem::type() const
{
	return ItemType::ListItem;
}


//
// OrderedList
//

ItemType
OrderedList::type() const
{
	return ItemType::OrderedList;
}


//
// UnorderedList
//

ItemType
UnorderedList::type() const
{
	return ItemType::UnorderedList;
}


//
// Link
//

Link::Link( const QUrl & u, const QString & t )
	:	m_url( u )
	,	m_text( t )
{
}

ItemType
Link::type() const
{
	return ItemType::Link;
}

const QUrl &
Link::url() const
{
	return m_url;
}

void
Link::setUrl( const QUrl & u )
{
	m_url = u;
}

const QString &
Link::text() const
{
	return m_text;
}

void
Link::setText( const QString & t )
{
	m_text = t;
}


//
// Image
//

Image::Image( const QUrl & u, const QString & t )
	:	Link( u, t )
{
}

ItemType
Image::type() const
{
	return ItemType::Image;
}


//
// Code
//

Code::Code( const QString & t, bool inl )
	:	m_text( t )
	,	m_inlined( inl )
{
}

ItemType
Code::type() const
{
	return ItemType::Code;
}

const QString &
Code::text() const
{
	return m_text;
}

void
Code::setText( const QString & t )
{
	m_text = t;
}

bool
Code::inlined() const
{
	return m_inlined;
}

void
Code::setInlined( bool on )
{
	m_inlined = on;
}


//
// TableCell
//

TableCell::TableCell( const QString & t )
	:	m_text( t )
{
}

ItemType
TableCell::type() const
{
	return ItemType::TableCell;
}

const QString &
TableCell::text() const
{
	return m_text;
}

void
TableCell::setText( const QString & t )
{
	m_text = t;
}


//
// TableRow
//

ItemType
TableRow::type() const
{
	return ItemType::TableRow;
}

const TableRow::Cells &
TableRow::cells() const
{
	return m_cells;
}

void
TableRow::setCells( const Cells & c )
{
	m_cells = c;
}

void
TableRow::appendCell( QSharedPointer< TableCell > c )
{
	m_cells.append( c );
}


//
// Table
//

ItemType
Table::type() const
{
	return ItemType::Table;
}

const Table::Rows &
Table::rows() const
{
	return m_rows;
}

void
Table::setRows( const Rows & r )
{
	m_rows = r;
}

void
Table::appendRow( QSharedPointer< TableRow > r )
{
	m_rows.append( r );
}


//
// FootnoteRef
//

FootnoteRef::FootnoteRef( const QString & i )
	:	m_id( i )
{
}

ItemType
FootnoteRef::type() const
{
	return ItemType::FootnoteRef;
}

const QString &
FootnoteRef::id() const
{
	return m_id;
}

void
FootnoteRef::setId( const QString & i )
{
	m_id = i;
}


//
// Footnote
//

ItemType
Footnote::type() const
{
	return ItemType::Footnote;
}


//
// Document
//

const Document::Footnotes &
Document::footnotesMap() const
{
	return m_footnotes;
}

void
Document::insertFootnote( const QString & id, QSharedPointer< Footnote > fn )
{
	m_footnotes.insert( id, fn );
}

} /* namespace MD */
