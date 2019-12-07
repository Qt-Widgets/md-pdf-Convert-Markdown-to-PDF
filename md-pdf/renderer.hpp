
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

#ifndef MD_PDF_RENDERER_HPP_INCLUDED
#define MD_PDF_RENDERER_HPP_INCLUDED

// md-pdf include.
#include "md_doc.hpp"

// Qt include.
#include <QColor>
#include <QObject>
#include <QMutex>


//
// RenderOpts
//

//! Options for rendering.
struct RenderOpts
{
	QString m_textFont;
	int m_textFontSize;
	QString m_codeFont;
	int m_codeFontSize;
	QColor m_linkColor;
	QColor m_borderColor;
	QColor m_codeBackground;
}; // struct RenderOpts


//
// Renderer
//

//! Abstract renderer.
class Renderer
	:	public QObject
{
	Q_OBJECT

signals:
	void progress( int percent );
	void error( const QString & msg );
	void done( bool terminated );

public:
	Renderer() = default;
	~Renderer() override = default;

	virtual void render( const QString & fileName, QSharedPointer< MD::Document > doc,
		const RenderOpts & opts ) = 0;
	virtual void clean() = 0;
}; // class Renderer


//
// PdfRenderer
//

//! Renderer to PDF.
class PdfRenderer
	:	public Renderer
{
	Q_OBJECT

signals:
	void start();

public:
	PdfRenderer();
	~PdfRenderer() override = default;

	void render( const QString & fileName, QSharedPointer< MD::Document > doc,
		const RenderOpts & opts ) override;
	void terminate();

private slots:
	void renderImpl();
	void clean() override;

private:
	QString m_fileName;
	QSharedPointer< MD::Document > m_doc;
	RenderOpts m_opts;
	QMutex m_mutex;
	bool m_terminate;
}; // class Renderer

#endif // MD_PDF_RENDERER_HPP_INCLUDED
