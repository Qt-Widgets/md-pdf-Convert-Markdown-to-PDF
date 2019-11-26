
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

#ifndef MD_PDF_MD_DOC_HPP_INCLUDED
#define MD_PDF_MD_DOC_HPP_INCLUDED

// Qt include.
#include <QObject>
#include <QString>
#include <QFlags>
#include <QSharedPointer>
#include <QUrl>
#include <QMap>


namespace MD {


//
// ItemType
//

//! Enumeration of item types.
enum class ItemType {
	//! Unknown.
	Unknown,
	//! Heading.
	Heading,
	//! Text.
	Text,
	//! Paragraph.
	Paragraph,
	//! Line break.
	LineBreak,
	//! Blockquote.
	Blockquote,
	//! List item.
	ListItem,
	//! List.
	List,
	//! Link.
	Link,
	//! Image.
	Image,
	//! Code.
	Code,
	//! Table cell.
	TableCell,
	//! Table row.
	TableRow,
	//! Table.
	Table,
	//! Footnote ref.
	FootnoteRef,
	//! Footnote.
	Footnote,
	//! Document.
	Document
}; // enum class ItemType


//
// Item
//

//! Base class for item in Markdown document.
class Item {
protected:
	Item() = default;

public:
	virtual ~Item() = default;

	virtual ItemType type() const;

private:
	Q_DISABLE_COPY( Item )
}; // class Item


//
// Heading
//

//! Heading.
class Heading final
	:	public Item
{
public:
	Heading( const QString & text, int level );
	~Heading() override = default;

	ItemType type() const override;

	const QString & text() const;
	void setText( const QString & t );

	int level() const;
	void setLevel( int l );

private:
	QString m_text;
	int m_level;

	Q_DISABLE_COPY( Heading )
}; // class Heading


//
// Text
//

//! Text.
class Text final
	:	public Item
{
public:
	Text() = default;
	~Text() override = default;

	ItemType type() const override;

	//! Text option.
	enum TextOption {
		//! No format.
		NoFormat = 0,
		//! Bold text.
		BoldText = 1,
		//! Italic text.
		ItalicText = 2,
		//! Strikethrough.
		StrikethroughText = 4
	}; // enum TextOption

	Q_DECLARE_FLAGS( TextOptions, TextOption )

	typedef QPair< QString, TextOptions > TextWithOptions;
	typedef QVector< TextWithOptions > TextData;

	const TextData & data() const;
	void setData( const TextData & d );
	void appendText( const TextWithOptions & t );

private:
	TextData m_data;

	Q_DISABLE_COPY( Text )
}; // class Text

Q_DECLARE_OPERATORS_FOR_FLAGS( Text::TextOptions )


//
// LineBreak
//

//! Line break.
class LineBreak final
	:	public Item
{
public:
	LineBreak() = default;
	~LineBreak() override = default;

	ItemType type() const override;

private:
	Q_DISABLE_COPY( LineBreak )
}; // class LineBreak


//
// Block
//

//! Abstract block.
class Block
	:	public Item
{
protected:
	Block() = default;

public:
	~Block() override = default;

	typedef QVector< QSharedPointer< Item > > Items;

	const Items & items() const;
	void setItems( const Items & i );
	void appendItem( QSharedPointer< Item > i );

	bool isEmpty() const;

private:
	Items m_items;

	Q_DISABLE_COPY( Block )
}; // class Block


//
// Paragraph
//

//! Paragraph.
class Paragraph final
	:	public Block
{
public:
	Paragraph() = default;
	~Paragraph() override = default;

	ItemType type() const override;

private:
	Q_DISABLE_COPY( Paragraph )
}; // class Paragraph


//
// Blockquote
//

//! Blockquote.
class Blockquote final
	:	public Block
{
public:
	Blockquote() = default;
	~Blockquote() override = default;

	ItemType type() const override;

private:
	Q_DISABLE_COPY( Blockquote )
}; // class Blockquote


//
// ListItem
//

//! List item.
class ListItem final
	:	public Block
{
public:
	ListItem() = default;
	~ListItem() override = default;

	ItemType type() const override;

	//! Type of the list.
	enum ListType {
		Ordered,
		Unordered
	}; // enum ListType

	//! Preliminary state of the ordered list.
	enum OrderedListPreState {
		Start,
		Continue
	}; // enum OrderedListPreState

	ListType listType() const;
	void setListType( ListType t );

	OrderedListPreState orderedListPreState() const;
	void setOrderedListPreState( OrderedListPreState s );

private:
	ListType m_listType = Unordered;
	OrderedListPreState m_orderedListState = Start;

	Q_DISABLE_COPY( ListItem )
}; // class ListItem


//
// List
//

//! List.
class List final
	:	public Block
{
public:
	List() = default;
	~List() override = default;

	ItemType type() const override;

private:
	Q_DISABLE_COPY( List )
}; // class List


//
// Link
//

//! Link.
class Link
	:	public Item
{
public:
	Link( const QUrl & u, const QString & t );
	~Link() override = default;

	ItemType type() const override;

	const QUrl & url() const;
	void setUrl( const QUrl & u );

	const QString & text() const;
	void setText( const QString & t );

private:
	QUrl m_url;
	QString m_text;

	Q_DISABLE_COPY( Link )
}; // class Link


//
// Image
//

//! Image.
class Image final
	:	public Link
{
public:
	Image( const QUrl & u, const QString & t );
	~Image() override = default;

	ItemType type() const override;

private:
	Q_DISABLE_COPY( Image )
}; // class Image


//
// Code
//

//! Code.
class Code final
	:	public Item
{
public:
	explicit Code( const QString & t, bool inl = false );
	~Code() override = default;

	ItemType type() const override;

	const QString & text() const;
	void setText( const QString & t );

	bool inlined() const;
	void setInlined( bool on = true );

private:
	QString m_text;
	bool m_inlined;

	Q_DISABLE_COPY( Code )
}; // class Code


//
// TableCell
//

//! Table cell.
class TableCell final
	:	public Item
{
public:
	explicit TableCell( const QString & t );
	~TableCell() override = default;

	ItemType type() const override;

	const QString & text() const;
	void setText( const QString & t );

private:
	QString m_text;

	Q_DISABLE_COPY( TableCell )
}; // class TableCell


//
// TableRow
//

//! Table row.
class TableRow final
	:	public Item
{
public:
	TableRow() = default;
	~TableRow() override = default;

	ItemType type() const override;

	typedef QVector< QSharedPointer< TableCell > > Cells;

	const Cells & cells() const;
	void setCells( const Cells & c );
	void appendCell( QSharedPointer< TableCell > c );

private:
	Cells m_cells;

	Q_DISABLE_COPY( TableRow )
}; // class TableRow


//
// Table
//

//! Table.
class Table final
	:	public Item
{
public:
	Table() = default;
	~Table() override = default;

	ItemType type() const override;

	typedef QVector< QSharedPointer< TableRow > > Rows;

	const Rows & rows() const;
	void setRows( const Rows & r );
	void appendRow( QSharedPointer< TableRow > r );

private:
	Rows m_rows;

	Q_DISABLE_COPY( Table )
}; // class Table


//
// FootnoteRef
//

//! Footnote ref.
class FootnoteRef final
	:	public Item
{
public:
	explicit FootnoteRef( const QString & i );
	~FootnoteRef() override = default;

	ItemType type() const override;

	const QString & id() const;
	void setId( const QString & i );

private:
	QString m_id;

	Q_DISABLE_COPY( FootnoteRef )
}; // class FootnoteRef


//
// Footnote
//

//! Footnote.
class Footnote final
	:	public Block
{
public:
	Footnote() = default;
	~Footnote() override = default;

	ItemType type() const override;

private:
	Q_DISABLE_COPY( Footnote )
}; // class Footnote


//
// Document
//

//! Document.
class Document final
	:	public Block
{
public:
	Document() = default;
	~Document() override = default;

	ItemType type() const override;

	typedef QMap< QString, QSharedPointer< Footnote > > Footnotes;

	const Footnotes & footnotesMap() const;
	void insertFootnote( const QString & id, QSharedPointer< Footnote > fn );

private:
	Footnotes m_footnotes;

	Q_DISABLE_COPY( Document )
}; // class Document;

} /* namespace MD */

#endif // MD_PDF_MD_DOC_HPP_INCLUDED
