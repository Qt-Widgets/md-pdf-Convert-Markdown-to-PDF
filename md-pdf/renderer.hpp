
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


//
// Renderer
//

//! Abstract renderer.
class Renderer
{
public:
	Renderer() = default;
	virtual ~Renderer() = default;

	virtual void render( const QString & fileName, QSharedPointer< MD::Document > doc ) = 0;
}; // class Renderer


//
// PdfRenderer
//

//! Renderer to PDF.
class PdfRenderer
	:	public Renderer
{
public:
	PdfRenderer() = default;
	~PdfRenderer() override = default;

	void render( const QString & fileName, QSharedPointer< MD::Document > doc ) override;
}; // class Renderer

#endif // MD_PDF_RENDERER_HPP_INCLUDED
