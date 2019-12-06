
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


//
// PdfRenderer
//

using namespace PoDoFo;


namespace /* anonymous */ {

	struct CoordsPageAttribs {
		double pageWidth = 0.0;
		double pageHeight = 0.0;
		double margin = 0.0;
		double x = 0.0;
		double y = 0.0;
	}; // struct CoordsPageAttribs

	struct Font {
		PdfFont * font = nullptr;
		PdfFont * bold = nullptr;
		PdfFont * italic = nullptr;
		PdfFont * boldItalic = nullptr;
	}; // struct Font

	struct Fonts {
		Font textFont;
		Font codeFont;
		Font h1Font;
		Font h2Font;
		Font h3Font;
		Font h4Font;
		Font h5Font;
		Font h6Font;
	}; // struct Fonts

	struct PdfAuxData {
		PdfStreamedDocument * doc = nullptr;
		PdfPainter * painter = nullptr;
		PdfPage * page = nullptr;
		CoordsPageAttribs coords;
		Fonts fonts;
	}; // struct PdfAuxData;

	enum FontType {
		TextFont,
		CodeFont,
		H1Font,
		H2Font,
		H3Font,
		H4Font,
		H5Font,
		H6Font
	}; // enum FontType

	PdfFont * createFont( PdfFont *& font, const QString & name, const QString & suffix,
		int size, PdfStreamedDocument * doc )
	{
		if( !font )
		{
			font = doc->CreateFont(
				( name + ( !suffix.isEmpty() ? QString::fromLatin1( " " ) + suffix :
					QString() ) ).toLocal8Bit().data() );

			if( !font )
				PODOFO_RAISE_ERROR( ePdfError_InvalidHandle )

			font->SetFontSize( size );
		}

		return font;
	}

	PdfFont * createFont( FontType type, const MD::TextOptions & textOpts,
		const RenderOpts & opts, PdfAuxData & pdfData )
	{
		switch( type )
		{
			case TextFont :
			{
				if( textOpts & MD::TextOption::BoldText && textOpts & MD::TextOption::ItalicText )
				{
					return createFont( pdfData.fonts.textFont.boldItalic,
						opts.m_textFont, QLatin1String( "BoldItalic" ),
						opts.m_textFontSize, pdfData.doc );
				}
				else if( textOpts & MD::TextOption::BoldText )
				{
					return createFont( pdfData.fonts.textFont.bold,
						opts.m_textFont, QLatin1String( "Bold" ),
						opts.m_textFontSize, pdfData.doc );
				}
				else if( textOpts & MD::TextOption::ItalicText )
				{
					return createFont( pdfData.fonts.textFont.italic,
						opts.m_textFont, QLatin1String( "Italic" ),
						opts.m_textFontSize, pdfData.doc );
				}
				else
				{
					return createFont( pdfData.fonts.textFont.font,
						opts.m_textFont, QString( "" ),
						opts.m_textFontSize, pdfData.doc );
				}
			}

			case CodeFont :
			{
				return createFont( pdfData.fonts.codeFont.font,
					opts.m_codeFont, QString( "" ),
					opts.m_codeFontSize, pdfData.doc );
			}

			case H1Font :
			{
				return createFont( pdfData.fonts.h1Font.font,
					opts.m_textFont, QString( "" ),
					opts.m_textFontSize + 10, pdfData.doc );
			}

			case H2Font :
			{
				return createFont( pdfData.fonts.h2Font.font,
					opts.m_textFont, QString( "" ),
					opts.m_textFontSize + 8, pdfData.doc );
			}

			case H3Font :
			{
				return createFont( pdfData.fonts.h3Font.font,
					opts.m_textFont, QString( "" ),
					opts.m_textFontSize + 6, pdfData.doc );
			}

			case H4Font :
			{
				return createFont( pdfData.fonts.h4Font.font,
					opts.m_textFont, QString( "" ),
					opts.m_textFontSize + 4, pdfData.doc );
			}

			case H5Font :
			{
				return createFont( pdfData.fonts.h5Font.font,
					opts.m_textFont, QString( "" ),
					opts.m_textFontSize + 2, pdfData.doc );
			}

			case H6Font :
			{
				return createFont( pdfData.fonts.h6Font.font,
					opts.m_textFont, QString( "" ),
					opts.m_textFontSize + 1, pdfData.doc );
			}
		}
	}

	static const double c_margin = 25.0;

	void createPage( PdfAuxData & pdfData )
	{
		pdfData.page = pdfData.doc->CreatePage(
			PdfPage::CreateStandardPageSize( ePdfPageSize_A4 ) );

		if( !pdfData.page )
			PODOFO_RAISE_ERROR( ePdfError_InvalidHandle )

		pdfData.painter->SetPage( pdfData.page );

		pdfData.coords = { pdfData.page->GetPageSize().GetWidth(),
			pdfData.page->GetPageSize().GetHeight(),
			c_margin, c_margin, pdfData.page->GetPageSize().GetHeight() - c_margin };
	}

	void drawHeading( PdfAuxData & pdfData, const RenderOpts & renderOpts,
		MD::Heading * item, QSharedPointer< MD::Document > doc )
	{
		FontType fontType = H1Font;

		switch( item->level() )
		{
			case 1 :
				fontType = H1Font;
				break;

			case 2 :
				fontType = H2Font;
				break;

			case 3 :
				fontType = H3Font;
				break;

			case 4 :
				fontType = H4Font;
				break;

			case 5 :
				fontType = H5Font;
				break;

			case 6 :
			default :
				fontType = H6Font;
				break;
		}

		PdfFont * font = createFont( fontType, MD::TextOption::TextWithoutFormat,
			renderOpts, pdfData );

		pdfData.painter->SetFont( font );
		pdfData.painter->SetColor( 0.0, 0.0, 0.0 );

		const double width = pdfData.coords.pageWidth - pdfData.coords.margin * 2;

		const auto lines = pdfData.painter->GetMultiLineTextAsLines(
			width, item->text().utf16() );

		const double height = lines.size() * font->GetFontMetrics()->GetLineSpacing();

		if( pdfData.coords.y - height > pdfData.coords.margin )
		{
			pdfData.painter->DrawMultiLineText( pdfData.coords.margin, pdfData.coords.y - height,
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
