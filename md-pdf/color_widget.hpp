
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

#ifndef MD_PDF_COLOR_WIDGET_HPP_INCLUDED
#define MD_PDF_COLOR_WIDGET_HPP_INCLUDED

// Qt include.
#include <QFrame>


//
// ColorWidget
//

//! Color widget.
class ColorWidget final
	:	public QFrame
{
	Q_OBJECT

public:
	ColorWidget( QWidget * parent );
	~ColorWidget() override = default;

	const QColor & color() const;
	void setColor( const QColor & c );

protected:
	void paintEvent( QPaintEvent * ) override;

private:
	QColor m_color;

	Q_DISABLE_COPY( ColorWidget )
}; // class ColorWidget

#endif // MD_PDF_COLOR_WIDGET_HPP_INCLUDED
