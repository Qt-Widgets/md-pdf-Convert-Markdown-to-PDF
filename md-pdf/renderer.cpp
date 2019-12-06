
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

// md-pdf include.
#include "renderer.hpp"

// podofo include.
#include <podofo/podofo.h>

#include <QDebug>


//
// PdfRenderer
//

using namespace PoDoFo;


namespace /* anonymous */ {

static const double c_margin = 25.0;

struct PageMargins {
	double left = c_margin;
	double right = c_margin;
	double top = c_margin;
	double bottom = c_margin;
}; // struct PageMargins

struct CoordsPageAttribs {
	PageMargins margins;
	double pageWidth = 0.0;
	double pageHeight = 0.0;
	double x = 0.0;
	double y = 0.0;
}; // struct CoordsPageAttribs

struct PdfAuxData {
	PdfStreamedDocument * doc = nullptr;
	PdfPainter * painter = nullptr;
	PdfPage * page = nullptr;
	CoordsPageAttribs coords;
}; // struct PdfAuxData;

PdfFont * createFont( const QString & name, bool bold, bool italic, float size,
	PdfStreamedDocument * doc )
{
	auto * font = doc->CreateFont( name.toLocal8Bit().data(), bold, italic );

	if( !font )
		PODOFO_RAISE_ERROR( ePdfError_InvalidHandle )

	font->SetFontSize( size );

	return font;
}

void createPage( PdfAuxData & pdfData )
{
	pdfData.page = pdfData.doc->CreatePage(
		PdfPage::CreateStandardPageSize( ePdfPageSize_A4 ) );

	if( !pdfData.page )
		PODOFO_RAISE_ERROR( ePdfError_InvalidHandle )

	pdfData.painter->SetPage( pdfData.page );

	pdfData.coords = { { c_margin, c_margin, c_margin, c_margin },
		pdfData.page->GetPageSize().GetWidth(),
		pdfData.page->GetPageSize().GetHeight(),
		c_margin, pdfData.page->GetPageSize().GetHeight() - c_margin };
}

void drawHeading( PdfAuxData & pdfData, const RenderOpts & renderOpts,
	MD::Heading * item, QSharedPointer< MD::Document > doc )
{
	PdfFont * font = createFont( renderOpts.m_textFont.toLocal8Bit().data(),
		true, false, renderOpts.m_textFontSize + 16 - ( item->level() < 7 ? item->level() * 2 : 12 ),
		pdfData.doc );

	pdfData.painter->SetFont( font );
	pdfData.painter->SetColor( 0.0, 0.0, 0.0 );

	const double width = pdfData.coords.pageWidth - pdfData.coords.margins.left -
		pdfData.coords.margins.right;

	const auto lines = pdfData.painter->GetMultiLineTextAsLines(
		width, item->text().utf16() );

	const double height = lines.size() * font->GetFontMetrics()->GetLineSpacing();

	if( pdfData.coords.y - height > pdfData.coords.margins.bottom )
	{
		pdfData.painter->DrawMultiLineText( pdfData.coords.margins.left,
			pdfData.coords.y - height,
			width, height, item->text().toUtf8().data() );

		pdfData.coords.y -= height;
	}
	else
	{
		pdfData.painter->FinishPage();
		createPage( pdfData );
		drawHeading( pdfData, renderOpts, item, doc );
	}
}

} /* namespace anonymous */

void
PdfRenderer::render( const QString & fileName, QSharedPointer< MD::Document > doc,
	const RenderOpts & opts )
{
	PdfStreamedDocument document( fileName.toLocal8Bit().data() );

	PdfPainter painter;

	try {
		int pageIdx = 0;

		PdfAuxData pdfData;

		pdfData.doc = &document;
		pdfData.painter = &painter;
		createPage( pdfData );

		for( const auto & i : doc->items() )
		{
			++pageIdx;

			switch( i->type() )
			{
				case MD::ItemType::Heading :
					drawHeading( pdfData, opts, static_cast< MD::Heading* > ( i.data() ), doc );
					break;

				case MD::ItemType::Paragraph :
				case MD::ItemType::Code :
				case MD::ItemType::List :
				case MD::ItemType::Blockquote :
				case MD::ItemType::Table :
					break;

				case MD::ItemType::PageBreak :
				{
					if( pageIdx < doc->items().size() )
					{
						painter.FinishPage();

						createPage( pdfData );
					}
				}
					break;

				default :
					break;
			}
		}

		painter.FinishPage();

		document.Close();
	}
	catch( const PdfError & e )
	{
		try {
			painter.FinishPage();
			document.Close();
		}
		catch( ... )
		{
		}

		throw e;
	}
}

void
PdfRenderer::clean()
{
	PdfEncodingFactory::FreeGlobalEncodingInstances();
}
