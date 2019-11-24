
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
	Paragraph = 3
}; // enum class ItemType


//
// Item
//

//! Base class for item in Markdown document.
class Item {
protected:
	Item();

public:
	virtual ~Item();

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
	~Heading() override;

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
	Text();
	~Text() override;

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
// Paragraph
//

//! Paragraph.
class Paragraph final
	:	public Item
{
public:
	Paragraph();
	~Paragraph() override;

	ItemType type() const override;

	typedef QVector< QSharedPointer< Item > > Items;

	const Items & items() const;
	void setItems( const Items & i );
	void appendItem( QSharedPointer< Item > i );

private:
	Items m_items;

	Q_DISABLE_COPY( Paragraph )
}; // class Paragraph


#endif // MD_PDF_MD_DOC_HPP_INCLUDED
