
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


namespace MD {


//
// ItemType
//

//! Enumeration of item types.
enum class ItemType {
	//! Unknown.
	Unknown = 0,
	//! Heading.
	Heading = 1,
	//! Text.
	Text = 2,
	//! Paragraph.
	Paragraph = 3,
	//! Line break.
	LineBreak = 4,
	//! Blockquote.
	Blockquote = 5,
	//! List item.
	ListItem = 6,
	//! Ordered list.
	OrderedList = 7,
	//! Unordered list.
	UnorderedList = 8,
	//! Link.
	Link = 9,
	//! Image.
	Image = 10,
	//! Code.
	Code = 11
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
		ItalicText = 2
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

private:
	Q_DISABLE_COPY( ListItem )
}; // class ListItem


//
// OrderedList
//

//! Ordered list.
class OrderedList final
	:	public Block
{
public:
	OrderedList() = default;
	~OrderedList() override = default;

	ItemType type() const override;

private:
	Q_DISABLE_COPY( OrderedList )
}; // class OrderedList


//
// UnorderedList
//

//! Unordered list.
class UnorderedList final
	:	public Block
{
public:
	UnorderedList() = default;
	~UnorderedList() override = default;

	ItemType type() const override;

private:
	Q_DISABLE_COPY( UnorderedList )
}; // class UnorderedList


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
// Document
//

//! Document.
class Document final
{
public:
	Document() = default;
	~Document() = default;

	typedef QVector< QSharedPointer< Item > > Items;

	const Items & items() const;
	void setItems( const Items & i );
	void appendItem( QSharedPointer< Item > i );

private:
	Items m_items;
}; // class Document;

} /* namespace MD */

#endif // MD_PDF_MD_DOC_HPP_INCLUDED
