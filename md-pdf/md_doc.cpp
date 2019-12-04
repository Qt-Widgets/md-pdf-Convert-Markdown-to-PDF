
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
// PageBreak
//

ItemType
PageBreak::type() const
{
	return ItemType::PageBreak;
}


//
// Heading
//

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

bool
Heading::isLabeled() const
{
	return !m_label.isEmpty();
}

const QString &
Heading::label() const
{
	return m_label;
}

void
Heading::setLabel( const QString & l )
{
	m_label = l;
}


//
// Text
//

ItemType
Text::type() const
{
	return ItemType::Text;
}

const QString &
Text::text() const
{
	return m_text;
}

void
Text::setText( const QString & t )
{
	m_text = t;
}

const TextOptions &
Text::opts() const
{
	return m_opts;
}

void
Text::setOpts( const TextOptions & o )
{
	m_opts = o;
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

bool
Block::isEmpty() const
{
	return m_items.isEmpty();
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

ListItem::ListType
ListItem::listType() const
{
	return m_listType;
}

void
ListItem::setListType( ListType t )
{
	m_listType = t;
}

ListItem::OrderedListPreState
ListItem::orderedListPreState() const
{
	return m_orderedListState;
}

void
ListItem::setOrderedListPreState( OrderedListPreState s )
{
	m_orderedListState = s;
}


//
// List
//

ItemType
List::type() const
{
	return ItemType::List;
}


//
// Image
//

ItemType
Image::type() const
{
	return ItemType::Image;
}

const QString &
Image::url() const
{
	return m_url;
}

void
Image::setUrl( const QString & u )
{
	m_url = u;
}

const QString &
Image::text() const
{
	return m_text;
}

void
Image::setText( const QString & t )
{
	m_text = t;
}

bool
Image::isEmpty() const
{
	return ( m_url.isEmpty() );
}


//
// Link
//

Link::Link()
	:	m_img( new Image() )
{

}

ItemType
Link::type() const
{
	return ItemType::Link;
}

const QString &
Link::url() const
{
	return m_url;
}

void
Link::setUrl( const QString & u )
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

TextOptions
Link::textOptions() const
{
	return m_opts;
}

void
Link::setTextOptions( const TextOptions & o )
{
	m_opts = o;
}

QSharedPointer< Image >
Link::img() const
{
	return m_img;
}

void
Link::setImg( QSharedPointer< Image > i )
{
	m_img = i;
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

ItemType
TableCell::type() const
{
	return ItemType::TableCell;
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

bool
TableRow::isEmpty() const
{
	return m_cells.isEmpty();
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

Table::Alignment
Table::columnAlignment( int idx ) const
{
	return m_aligns.at( idx );
}

void
Table::setColumnAlignment( int idx, Alignment a )
{
	if( idx + 1 > columnsCount() )
		m_aligns.append( a );
	else
		m_aligns[ idx ] = a;
}

int
Table::columnsCount() const
{
	return m_aligns.size();
}

bool
Table::isEmpty() const
{
	return ( m_aligns.isEmpty() || m_rows.isEmpty() );
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

ItemType
Document::type() const
{
	return ItemType::Document;
}

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

const Document::LabeledLinks &
Document::labeledLinks() const
{
	return m_labeledLinks;
}

void
Document::insertLabeledLink( const QString & label, QSharedPointer< Link > lnk )
{
	m_labeledLinks.insert( label, lnk );
}

const Document::LabeledHeadings &
Document::labeledHeadings() const
{
	return m_labeledHeadings;
}

void
Document::insertLabeledHeading( const QString & label, QSharedPointer< Heading > h )
{
	m_labeledHeadings.insert( label, h );
}

} /* namespace MD */
