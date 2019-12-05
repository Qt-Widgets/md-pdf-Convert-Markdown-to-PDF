
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

	void cleanPodofo()
	{
		PdfEncodingFactory::FreeGlobalEncodingInstances();
	}

} /* namespace anonymous */

void
PdfRenderer::render( const QString & fileName, QSharedPointer< MD::Document > doc,
	const RenderOpts & opts )
{
	PdfStreamedDocument document( fileName.toLocal8Bit().data() );

	PdfPainter painter;

	PdfPage * page = nullptr;

	try {
		page = document.CreatePage( PdfPage::CreateStandardPageSize( ePdfPageSize_A4 ) );

		if( !page )
			PODOFO_RAISE_ERROR( ePdfError_InvalidHandle )

		painter.SetPage( page );

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

		cleanPodofo();

		throw e;
	}

	cleanPodofo();
}
