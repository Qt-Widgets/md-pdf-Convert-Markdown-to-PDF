
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

// Qt include.
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QThread>
#include <QBuffer>

#include <QDebug>


class PdfRendererError {
public:
	explicit PdfRendererError( const QString & reason )
		:	m_what( reason )
	{
	}

	const QString & what() const noexcept
	{
		return m_what;
	}

private:
	QString m_what;
}; // class PdfRendererError


//
// PdfRenderer
//

PdfRenderer::PdfRenderer()
	:	m_terminate( false )
{
	connect( this, &PdfRenderer::start, this, &PdfRenderer::renderImpl,
		Qt::QueuedConnection );
}

void
PdfRenderer::render( const QString & fileName, QSharedPointer< MD::Document > doc,
	const RenderOpts & opts )
{
	m_fileName = fileName;
	m_doc = doc;
	m_opts = opts;

	emit start();
}

void
PdfRenderer::terminate()
{
	QMutexLocker lock( &m_mutex );

	m_terminate = true;
}

void
PdfRenderer::renderImpl()
{
	{
		const int itemsCount = m_doc->items().size();

		emit progress( 0 );

		PdfStreamedDocument document( m_fileName.toLocal8Bit().data() );

		PdfPainter painter;

		try {
			int itemIdx = 0;

			PdfAuxData pdfData;

			pdfData.doc = &document;
			pdfData.painter = &painter;
			createPage( pdfData );

			for( const auto & i : m_doc->items() )
			{
				++itemIdx;

				{
					QMutexLocker lock( &m_mutex );

					if( m_terminate )
						break;
				}

				switch( i->type() )
				{
					case MD::ItemType::Heading :
						drawHeading( pdfData, m_opts, static_cast< MD::Heading* > ( i.data() ),
							m_doc );
						break;

					case MD::ItemType::Paragraph :
						drawParagraph( pdfData, m_opts, static_cast< MD::Paragraph* > ( i.data() ),
							m_doc );
						break;

					case MD::ItemType::Code :
					case MD::ItemType::List :
					case MD::ItemType::Blockquote :
					case MD::ItemType::Table :
						break;

					case MD::ItemType::PageBreak :
					{
						if( itemIdx < itemsCount )
						{
							painter.FinishPage();

							createPage( pdfData );
						}
					}
						break;

					default :
						break;
				}

				emit progress( static_cast< int > ( static_cast< double > (itemIdx) /
					static_cast< double > (itemsCount) * 100.0 ) );
			}

			painter.FinishPage();

			document.Close();

			emit done( m_terminate );
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

			emit error( QString::fromLatin1( PdfError::ErrorMessage( e.GetError() ) ) );
		}
		catch( const PdfRendererError & e )
		{
			try {
				painter.FinishPage();
				document.Close();
			}
			catch( ... )
			{
			}

			emit error( e.what() );
		}
	}

	try {
		clean();
	}
	catch( const PdfError & e )
	{
		emit error( QString::fromLatin1( PdfError::ErrorMessage( e.GetError() ) ) );
	}

	deleteLater();
}

void
PdfRenderer::clean()
{
	PdfEncodingFactory::FreeGlobalEncodingInstances();
}

PdfFont *
PdfRenderer::createFont( const QString & name, bool bold, bool italic, float size,
	PdfStreamedDocument * doc )
{
	auto * font = doc->CreateFont( name.toLocal8Bit().data(), bold, italic , false,
		PdfEncodingFactory::GlobalIdentityEncodingInstance(),
		PdfFontCache::eFontCreationFlags_None );

	if( !font )
		throw PdfRendererError( tr( "Unable to create font: %1. Please choose another one." )
			.arg( name ) );

	font->SetFontSize( size );

	return font;
}

void
PdfRenderer::createPage( PdfAuxData & pdfData )
{
	pdfData.page = pdfData.doc->CreatePage(
		PdfPage::CreateStandardPageSize( ePdfPageSize_A4 ) );

	if( !pdfData.page )
		throw PdfRendererError( QLatin1String( "Oops, can't create empty page in PDF." ) );

	pdfData.painter->SetPage( pdfData.page );

	pdfData.coords = { { c_margin, c_margin, c_margin, c_margin },
		pdfData.page->GetPageSize().GetWidth(),
		pdfData.page->GetPageSize().GetHeight(),
		c_margin, pdfData.page->GetPageSize().GetHeight() - c_margin };
}

PdfString
PdfRenderer::createPdfString( const QString & text )
{
	return PdfString( reinterpret_cast< pdf_utf8* > ( text.toUtf8().data() ) );
}

QString
PdfRenderer::createQString( const PdfString & str )
{
	return QString::fromUtf8( str.GetStringUtf8().c_str(),
		static_cast< int > ( str.GetCharacterLength() ) );
}

void
PdfRenderer::drawHeading( PdfAuxData & pdfData, const RenderOpts & renderOpts,
	MD::Heading * item, QSharedPointer< MD::Document > doc )
{
	{
		QMutexLocker lock( &m_mutex );

		if( m_terminate )
			return;
	}

	emit status( tr( "Drawing heading." ) );

	PdfFont * font = createFont( renderOpts.m_textFont.toLocal8Bit().data(),
		true, false, renderOpts.m_textFontSize + 16 - ( item->level() < 7 ? item->level() * 2 : 12 ),
		pdfData.doc );

	pdfData.painter->SetFont( font );
	pdfData.painter->SetColor( 0.0, 0.0, 0.0 );

	const double width = pdfData.coords.pageWidth - pdfData.coords.margins.left -
		pdfData.coords.margins.right;

	const auto lines = pdfData.painter->GetMultiLineTextAsLines(
		width, createPdfString( item->text() ) );

	const double height = lines.size() * font->GetFontMetrics()->GetLineSpacing();
	const double availableHeight = pdfData.coords.pageHeight - pdfData.coords.margins.top -
		pdfData.coords.margins.bottom;

	if( pdfData.coords.y - height > pdfData.coords.margins.bottom )
	{
		pdfData.painter->DrawMultiLineText( pdfData.coords.margins.left,
			pdfData.coords.y - height,
			width, height, createPdfString( item->text() ) );

		pdfData.coords.y -= height;
	}
	else if( height <= availableHeight )
	{
		pdfData.painter->FinishPage();
		createPage( pdfData );
		drawHeading( pdfData, renderOpts, item, doc );
	}
	else
	{
		std::vector< PdfString > tmp;
		double h = 0.0;
		std::size_t i = 0;
		double available = pdfData.coords.pageHeight - pdfData.coords.margins.top -
			pdfData.coords.margins.bottom;
		const double spacing = font->GetFontMetrics()->GetLineSpacing();

		while( available >= spacing )
		{
			tmp.push_back( lines.at( i ) );
			h += spacing;
			++i;
			available -= spacing;
		}

		QString text;

		for( const auto & s : tmp )
			text.append( createQString( s ) );

		QString toSave = item->text();
		toSave.remove( text );

		item->setText( toSave.simplified() );

		pdfData.painter->DrawMultiLineText( pdfData.coords.margins.left,
			pdfData.coords.y - h,
			width, h, createPdfString( text ) );

		pdfData.coords.y -= height;

		pdfData.painter->FinishPage();

		createPage( pdfData );

		drawHeading( pdfData, renderOpts, item, doc );
	}
}

void
PdfRenderer::drawText( PdfAuxData & pdfData, const RenderOpts & renderOpts,
	MD::Text * item, QSharedPointer< MD::Document > doc, bool & newLine, double offset,
	bool firstInParagraph )
{
	drawString( pdfData, renderOpts, item->text(),
		item->opts() & MD::TextOption::BoldText,
		item->opts() & MD::TextOption::ItalicText,
		item->opts() & MD::TextOption::StrikethroughText,
		doc, newLine, offset, firstInParagraph );
}

QVector< QPair< QRectF, PdfPage* > > normalizeRects(
	const QVector< QPair< QRectF, PdfPage* > > & rects )
{
	QVector< QPair< QRectF, PdfPage* > > ret;

	if( !rects.isEmpty() )
	{
		QPair< QRectF, PdfPage* > to( rects.first() );

		auto it = rects.cbegin();
		++it;

		for( auto last = rects.cend(); it != last; ++it )
		{
			if( qAbs( it->first.y() - to.first.y() ) < 0.001 )
				to.first.setWidth( to.first.width() + it->first.width() );
			else
			{
				ret.append( to );

				to = *it;
			}
		}

		ret.append( to );
	}

	return ret;
}

void
PdfRenderer::drawLink( PdfAuxData & pdfData, const RenderOpts & renderOpts,
	MD::Link * item, QSharedPointer< MD::Document > doc, bool & newLine, double offset,
	bool firstInParagraph )
{
	QVector< QPair< QRectF, PdfPage* > > rects;

	if( item->img()->isEmpty() )
	{
		pdfData.painter->Save();
		pdfData.painter->SetColor( renderOpts.m_linkColor.redF(),
			renderOpts.m_linkColor.greenF(),
			renderOpts.m_linkColor.blueF() );

		rects = normalizeRects( drawString( pdfData, renderOpts, item->text(),
			item->textOptions() & MD::TextOption::BoldText,
			item->textOptions() & MD::TextOption::ItalicText,
			item->textOptions() & MD::TextOption::StrikethroughText,
			doc, newLine, offset, firstInParagraph ) );

		pdfData.painter->Restore();
	}
	else
		rects.append( drawImage( pdfData, renderOpts, item->img().data(), doc, newLine, offset,
			firstInParagraph ) );

	if( !QUrl( item->url() ).isRelative() )
	{
		for( const auto & r : qAsConst( rects ) )
		{
			auto * annot = r.second->CreateAnnotation( ePdfAnnotation_Link,
				PdfRect( r.first.x(), r.first.y(), r.first.width(), r.first.height() ) );
			annot->SetBorderStyle( 0.0, 0.0, 0.0 );

			PdfAction action( ePdfAction_URI, pdfData.doc );
			action.SetURI( PdfString( item->url().toLatin1().data() ) );

			annot->SetAction( action );
			annot->SetFlags( ePdfAnnotationFlags_NoZoom );
		}
	}
}

QVector< QPair< QRectF, PdfPage* > >
PdfRenderer::drawString( PdfAuxData & pdfData, const RenderOpts & renderOpts,
	const QString & str, bool bold, bool italic, bool strikethrough,
	QSharedPointer< MD::Document > doc, bool & newLine, double offset,
	bool firstInParagraph )
{
	Q_UNUSED( doc )

	QVector< QPair< QRectF, PdfPage * > > ret;

	{
		QMutexLocker lock( &m_mutex );

		if( m_terminate )
			return ret;
	}

	auto * font = createFont( renderOpts.m_textFont, false, false,
		renderOpts.m_textFontSize, pdfData.doc );

	const auto lineHeight = font->GetFontMetrics()->GetLineSpacing();

	static const QString charsWithoutSpaceBefore = QLatin1String( ".,;" );

	const auto words = str.split( QLatin1Char( ' ' ), QString::SkipEmptyParts );

	if( !firstInParagraph && !newLine && !words.isEmpty() &&
		!charsWithoutSpaceBefore.contains( words.first() ) )
	{
		pdfData.painter->SetFont( font );
		pdfData.painter->DrawText( pdfData.coords.x, pdfData.coords.y, " " );
		const auto w = font->GetFontMetrics()->StringWidth( " " );
		pdfData.coords.x += font->GetFontMetrics()->StringWidth( " " );
	}

	font = createFont( renderOpts.m_textFont, bold, italic,
		renderOpts.m_textFontSize, pdfData.doc );

	if( strikethrough )
		font->SetStrikeOut( true );

	pdfData.painter->SetFont( font );

	for( auto it = words.begin(), last = words.end(); it != last; ++it )
	{
		{
			QMutexLocker lock( &m_mutex );

			if( m_terminate )
				return ret;
		}

		const auto str = createPdfString( *it );

		const auto length = font->GetFontMetrics()->StringWidth( str );

		if( pdfData.coords.x + length <= pdfData.coords.pageWidth - pdfData.coords.margins.right )
		{
			newLine = false;

			pdfData.painter->DrawText( pdfData.coords.x, pdfData.coords.y, str );
			ret.append( qMakePair( QRectF( pdfData.coords.x, pdfData.coords.y,
				length, lineHeight ), pdfData.page ) );
			pdfData.coords.x += length;

			if( it + 1 != last )
			{
				const auto spaceWidth = font->GetFontMetrics()->StringWidth( " " );

				if( pdfData.coords.x + spaceWidth <= pdfData.coords.pageWidth -
					pdfData.coords.margins.right )
				{
					ret.append( qMakePair( QRectF( pdfData.coords.x, pdfData.coords.y,
						spaceWidth, lineHeight ), pdfData.page ) );
					pdfData.painter->DrawText( pdfData.coords.x, pdfData.coords.y, " " );
					pdfData.coords.x += spaceWidth;
				}
			}
		}
		else
		{
			if( pdfData.coords.margins.left + offset + length <=
				pdfData.coords.pageWidth - pdfData.coords.margins.right )
			{
				newLine = true;

				moveToNewLine( pdfData, offset, lineHeight, 1.0 );

				--it;
			}
			else
			{
				newLine = true;

				moveToNewLine( pdfData, offset, lineHeight, 1.0 );

				pdfData.painter->DrawText( pdfData.coords.x, pdfData.coords.y, str );
				ret.append( qMakePair( QRectF( pdfData.coords.x, pdfData.coords.y,
					font->GetFontMetrics()->StringWidth( str ), lineHeight ), pdfData.page ) );

				moveToNewLine( pdfData, offset, lineHeight, 1.0 );
			}
		}
	}

	return ret;
}

void
PdfRenderer::drawInlinedCode( PdfAuxData & pdfData, const RenderOpts & renderOpts,
	MD::Code * item, QSharedPointer< MD::Document > doc, bool & newLine, double offset,
	bool firstInParagraph )
{
	Q_UNUSED( doc )

	{
		QMutexLocker lock( &m_mutex );

		if( m_terminate )
			return;
	}

	auto * font = createFont( renderOpts.m_codeFont, false, false,
		renderOpts.m_codeFontSize, pdfData.doc );
	auto * textFont = createFont( renderOpts.m_textFont, false, false,
		renderOpts.m_textFontSize, pdfData.doc );

	const auto lineHeight = font->GetFontMetrics()->GetLineSpacing();
	const auto spaceWidth = font->GetFontMetrics()->StringWidth( " " );
	auto rY = pdfData.coords.y + font->GetFontMetrics()->GetDescent();
	const auto rHeight = lineHeight;

	if( !firstInParagraph && !newLine )
	{
		pdfData.painter->SetFont( textFont );
		pdfData.painter->DrawText( pdfData.coords.x, pdfData.coords.y, " " );
		pdfData.coords.x += spaceWidth;
	}

	pdfData.painter->SetFont( font );

	const auto words = item->text().split( QLatin1Char( ' ' ), QString::SkipEmptyParts );

	for( auto it = words.begin(), last = words.end(); it != last; ++it )
	{
		{
			QMutexLocker lock( &m_mutex );

			if( m_terminate )
				return;
		}

		auto str = createPdfString( *it );

		const auto length = font->GetFontMetrics()->StringWidth( str );

		if( pdfData.coords.x + length <= pdfData.coords.pageWidth - pdfData.coords.margins.right )
		{
			newLine = false;

			pdfData.painter->Save();
			pdfData.painter->SetColor( renderOpts.m_codeBackground.redF(),
				renderOpts.m_codeBackground.greenF(),
				renderOpts.m_codeBackground.blueF() );
			pdfData.painter->SetStrokingColor( renderOpts.m_borderColor.redF(),
				renderOpts.m_borderColor.greenF(),
				renderOpts.m_borderColor.blueF() );
			pdfData.painter->Rectangle( pdfData.coords.x, rY, length, rHeight );
			pdfData.painter->Fill();
			pdfData.painter->Restore();

			pdfData.painter->DrawText( pdfData.coords.x, pdfData.coords.y, str );
			pdfData.coords.x += length;

			if( it + 1 != last )
			{
				if( font->GetFontMetrics()->StringWidth( createPdfString( *( it + 1 ) ) ) +
						pdfData.coords.x + spaceWidth <= pdfData.coords.pageWidth -
							pdfData.coords.margins.right )
				{
					if( pdfData.coords.x + spaceWidth <= pdfData.coords.pageWidth -
						pdfData.coords.margins.right )
					{
						pdfData.painter->Save();
						pdfData.painter->SetColor( renderOpts.m_codeBackground.redF(),
							renderOpts.m_codeBackground.greenF(),
							renderOpts.m_codeBackground.blueF() );
						pdfData.painter->SetStrokingColor( renderOpts.m_borderColor.redF(),
							renderOpts.m_borderColor.greenF(),
							renderOpts.m_borderColor.blueF() );
						pdfData.painter->Rectangle( pdfData.coords.x, rY,
							spaceWidth, rHeight );
						pdfData.painter->Fill();
						pdfData.painter->Restore();

						pdfData.painter->DrawText( pdfData.coords.x, pdfData.coords.y, " " );
						pdfData.coords.x += spaceWidth;
					}
				}
				else
				{
					newLine = true;

					moveToNewLine( pdfData, offset, textFont->GetFontMetrics()->GetLineSpacing(), 1.0 );

					rY = pdfData.coords.y + font->GetFontMetrics()->GetDescent();
				}
			}
		}
		else
		{
			newLine = true;

			if( pdfData.coords.margins.left + offset + length <=
				pdfData.coords.pageWidth - pdfData.coords.margins.right )
			{
				moveToNewLine( pdfData, offset, textFont->GetFontMetrics()->GetLineSpacing(), 1.0 );

				rY = pdfData.coords.y + font->GetFontMetrics()->GetDescent();

				--it;
			}
			else
			{
				moveToNewLine( pdfData, offset, textFont->GetFontMetrics()->GetLineSpacing(), 1.0 );

				rY = pdfData.coords.y + font->GetFontMetrics()->GetDescent();

				pdfData.painter->Save();
				pdfData.painter->SetColor( renderOpts.m_codeBackground.redF(),
					renderOpts.m_codeBackground.greenF(),
					renderOpts.m_codeBackground.blueF() );
				pdfData.painter->SetStrokingColor( renderOpts.m_borderColor.redF(),
					renderOpts.m_borderColor.greenF(),
					renderOpts.m_borderColor.blueF() );
				pdfData.painter->Rectangle( pdfData.coords.x, rY,
					pdfData.coords.pageWidth - pdfData.coords.margins.left -
						pdfData.coords.margins.right - offset, rHeight );
				pdfData.painter->Fill();
				pdfData.painter->Restore();

				pdfData.painter->DrawText( pdfData.coords.x, pdfData.coords.y, str );

				moveToNewLine( pdfData, offset, textFont->GetFontMetrics()->GetLineSpacing(), 1.0 );

				rY = pdfData.coords.y + font->GetFontMetrics()->GetDescent();
			}
		}
	}
}

void
PdfRenderer::moveToNewLine( PdfAuxData & pdfData, double xOffset, double yOffset,
	double yOffsetMultiplier )
{
	pdfData.coords.x = pdfData.coords.margins.left + xOffset;
	pdfData.coords.y -= yOffset * yOffsetMultiplier;

	if( pdfData.coords.y - yOffset < pdfData.coords.margins.bottom )
	{
		pdfData.painter->FinishPage();
		createPage( pdfData );
		pdfData.coords.x = pdfData.coords.margins.left + xOffset;
	}
}

void
PdfRenderer::drawParagraph( PdfAuxData & pdfData, const RenderOpts & renderOpts,
	MD::Paragraph * item, QSharedPointer< MD::Document > doc, double offset )
{
	{
		QMutexLocker lock( &m_mutex );

		if( m_terminate )
			return;
	}

	emit status( tr( "Drawing paragraph." ) );

	auto * font = createFont( renderOpts.m_textFont, false, false,
		renderOpts.m_textFontSize, pdfData.doc );

	const auto lineHeight = font->GetFontMetrics()->GetLineSpacing();

	pdfData.coords.y -= lineHeight;
	pdfData.coords.x = pdfData.coords.margins.left + offset;

	if( pdfData.coords.y < pdfData.coords.margins.bottom )
	{
		pdfData.painter->FinishPage();
		createPage( pdfData );
		pdfData.coords.x = pdfData.coords.margins.left + offset;
	}

	bool newLine = false;

	for( auto it = item->items().begin(), last = item->items().end(); it != last; ++it )
	{
		{
			QMutexLocker lock( &m_mutex );

			if( m_terminate )
				return;
		}

		switch( (*it)->type() )
		{
			case MD::ItemType::Text :
				drawText( pdfData, renderOpts, static_cast< MD::Text* > ( it->data() ),
					doc, newLine, offset, it == item->items().begin() );
				break;

			case MD::ItemType::Code :
				drawInlinedCode( pdfData, renderOpts, static_cast< MD::Code* > ( it->data() ),
					doc, newLine, offset, it == item->items().begin() );
				break;

			case MD::ItemType::Link :
				drawLink( pdfData, renderOpts, static_cast< MD::Link* > ( it->data() ),
					doc, newLine, offset, it == item->items().begin() );
				break;

			case MD::ItemType::Image :
				drawImage( pdfData, renderOpts, static_cast< MD::Image* > ( it->data() ),
					doc, newLine, offset, it == item->items().begin() );
				break;

			case MD::ItemType::LineBreak :
				moveToNewLine( pdfData, offset, lineHeight );
				break;

			default :
				break;
		}
	}

	moveToNewLine( pdfData, 0.0, lineHeight, 1.0 );
}

QPair< QRectF, PdfPage* >
PdfRenderer::drawImage( PdfAuxData & pdfData, const RenderOpts & renderOpts,
	MD::Image * item, QSharedPointer< MD::Document > doc, bool & newLine, double offset,
	bool firstInParagraph )
{
	Q_UNUSED( doc )

	emit status( tr( "Loading image." ) );

	const auto img = loadImage( item );

	if( !img.isNull() )
	{
		QByteArray data;
		QBuffer buf( &data );

		img.save( &buf, "jpg" );

		PdfImage pdfImg( pdfData.doc );
		pdfImg.LoadFromData( reinterpret_cast< unsigned char * >( data.data() ), data.size() );

		newLine = true;

		auto * font = createFont( renderOpts.m_textFont, false, false,
			renderOpts.m_textFontSize, pdfData.doc );

		const auto lineHeight = font->GetFontMetrics()->GetLineSpacing();

		if( !firstInParagraph )
			moveToNewLine( pdfData, offset, lineHeight, 1.0 );
		else
			pdfData.coords.x += offset;

		double x = 0.0;
		double scale = 1.0;
		const double availableWidth = pdfData.coords.pageWidth - pdfData.coords.margins.left -
			pdfData.coords.margins.right - offset;
		double availableHeight = pdfData.coords.y - pdfData.coords.margins.bottom;

		if( pdfImg.GetWidth() > availableWidth )
			scale = availableWidth / pdfImg.GetWidth();

		const double pageHeight = pdfData.coords.pageHeight - pdfData.coords.margins.top -
			pdfData.coords.margins.bottom;

		if( pdfImg.GetHeight() * scale > pageHeight )
		{
			scale = pageHeight / ( pdfImg.GetHeight() * scale );

			pdfData.painter->FinishPage();

			createPage( pdfData );

			availableHeight = pdfData.coords.y - pdfData.coords.margins.bottom;

			pdfData.coords.x += offset;
		}
		else if( pdfImg.GetHeight() * scale > availableHeight )
		{
			pdfData.painter->FinishPage();

			createPage( pdfData );

			availableHeight = pdfData.coords.y - pdfData.coords.margins.bottom;

			pdfData.coords.x += offset;
		}

		if( pdfImg.GetWidth() * scale < availableWidth )
			x = ( availableWidth - pdfImg.GetWidth() * scale ) / 2.0;

		pdfData.painter->DrawImage( pdfData.coords.x + x,
			pdfData.coords.y - pdfImg.GetHeight() * scale,
			&pdfImg, scale, scale );

		pdfData.coords.y -= pdfImg.GetHeight() * scale;

		QRectF r( pdfData.coords.x + x, pdfData.coords.y,
			pdfImg.GetWidth() * scale, pdfImg.GetHeight() * scale );

		auto * page = pdfData.page;

		moveToNewLine( pdfData, offset, lineHeight, 1.0 );

		return qMakePair( r, page );
	}
	else
		throw PdfRendererError( tr( "Unable to load image: %1" ).arg( item->url() ) );
}

//
// LoadImageFromNetwork
//

LoadImageFromNetwork::LoadImageFromNetwork( const QUrl & url, QThread * thread )
	:	m_thread( thread )
	,	m_reply( nullptr )
	,	m_url( url )
{
	connect( this, &LoadImageFromNetwork::start, this, &LoadImageFromNetwork::loadImpl,
		Qt::QueuedConnection );
}

const QImage &
LoadImageFromNetwork::image() const
{
	return m_img;
}

void
LoadImageFromNetwork::load()
{
	emit start();
}

void
LoadImageFromNetwork::loadImpl()
{
	QNetworkAccessManager * m = new QNetworkAccessManager( this );
	m_reply = m->get( QNetworkRequest( m_url ) );

	connect( m_reply, &QNetworkReply::finished, this, &LoadImageFromNetwork::loadFinished );
	connect( m_reply,
		static_cast< void(QNetworkReply::*)(QNetworkReply::NetworkError) >( &QNetworkReply::error ),
		this, &LoadImageFromNetwork::loadError );
}

void
LoadImageFromNetwork::loadFinished()
{
	m_img.loadFromData( m_reply->readAll() );

	m_thread->quit();
}

void
LoadImageFromNetwork::loadError( QNetworkReply::NetworkError )
{
	m_thread->quit();
}

QImage
PdfRenderer::loadImage( MD::Image * item )
{
	if( QFileInfo::exists( item->url() ) )
		return QImage( item->url() );
	else if( !QUrl( item->url() ).isRelative() )
	{
		QThread thread;

		LoadImageFromNetwork load( QUrl( item->url() ), &thread );

		load.moveToThread( &thread );
		thread.start();
		load.start();
		thread.wait();

		return load.image();
	}
	else
		throw PdfRendererError(
			tr( "Hmm, I don't know how to load this image: %1" ).arg( item->url() ) );
}
