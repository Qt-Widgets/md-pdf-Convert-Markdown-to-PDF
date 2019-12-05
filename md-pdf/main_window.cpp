
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
#include "main_window.hpp"
#include "md_parser.hpp"
#include "renderer.hpp"

// Qt include.
#include <QFontDatabase>
#include <QToolButton>
#include <QColorDialog>
#include <QFileDialog>
#include <QPushButton>
#include <QMessageBox>

// podofo include.
#include <podofo/podofo.h>


//
// MainWindow
//

MainWindow::MainWindow()
	:	m_ui( new Ui::MainWindow() )
{
	m_ui->setupUi( this );

	m_ui->m_linkColor->setColor( Qt::blue );
	m_ui->m_borderColor->setColor( Qt::black );
	m_ui->m_codeBackground->setColor( Qt::lightGray );

	QFontDatabase fd;
	const auto fonts = fd.families();

	m_ui->m_textFont->addItems( fonts );
	m_ui->m_textFont->setCurrentText( QApplication::font().family() );

	m_ui->m_codeFont->addItems( fonts );
	m_ui->m_codeFont->setCurrentText(
		QFontDatabase::systemFont( QFontDatabase::FixedFont ).family() );

	connect( m_ui->m_linkColorBtn, &QToolButton::clicked, this, &MainWindow::changeLinkColor );
	connect( m_ui->m_borderColorBtn, &QToolButton::clicked, this, &MainWindow::changeBorderColor );
	connect( m_ui->m_codeBackgroundBtn, &QToolButton::clicked,
		this, &MainWindow::changeCodeBackground );
	connect( m_ui->m_fileNameBtn, &QToolButton::clicked,
		this, &MainWindow::selectMarkdown );
	connect( m_ui->m_startBtn, &QPushButton::clicked,
		this, &MainWindow::process );
}

void
MainWindow::changeLinkColor()
{
	QColorDialog dlg( m_ui->m_linkColor->color(), this );

	if( QDialog::Accepted == dlg.exec() )
		m_ui->m_linkColor->setColor( dlg.currentColor() );
}

void
MainWindow::changeBorderColor()
{
	QColorDialog dlg( m_ui->m_borderColor->color(), this );

	if( QDialog::Accepted == dlg.exec() )
		m_ui->m_borderColor->setColor( dlg.currentColor() );
}

void
MainWindow::changeCodeBackground()
{
	QColorDialog dlg( m_ui->m_codeBackground->color(), this );

	if( QDialog::Accepted == dlg.exec() )
		m_ui->m_codeBackground->setColor( dlg.currentColor() );
}

void
MainWindow::selectMarkdown()
{
	const auto fileName = QFileDialog::getOpenFileName( this, tr( "Select Markdown" ),
		QDir::homePath(),
		tr( "Markdown (*.md *.markdown)" ) );

	if( !fileName.isEmpty() )
	{
		m_ui->m_fileName->setText( fileName );
		m_ui->m_startBtn->setEnabled( true );
	}
}

void
MainWindow::process()
{
	auto fileName = QFileDialog::getSaveFileName( this, tr( "Save as" ),
		QDir::homePath(),
		tr( "PDF (*.pdf)" ) );

	if( !fileName.isEmpty() )
	{
		if( !fileName.endsWith( QLatin1String( ".pdf" ), Qt::CaseInsensitive ) )
			fileName.append( QLatin1String( ".pdf" ) );

		MD::Parser parser;

		auto doc = parser.parse( m_ui->m_fileName->text(), m_ui->m_recursive->isChecked() );

		if( !doc->isEmpty() )
		{
			PdfRenderer pdf;

			RenderOpts opts;

			opts.m_textFont = m_ui->m_textFont->currentText();
			opts.m_textFontSize = m_ui->m_textFontSize->value();
			opts.m_codeFont = m_ui->m_codeFont->currentText();
			opts.m_codeFontSize = m_ui->m_codeFontSize->value();
			opts.m_linkColor = m_ui->m_linkColor->color();
			opts.m_borderColor = m_ui->m_borderColor->color();
			opts.m_codeBackground = m_ui->m_codeBackground->color();

			try {
				pdf.render( fileName, doc, opts );

				QMessageBox::information( this, tr( "Markdown processed..." ),
					tr( "PDF generated. Have a look at the result. Thank you." ) );
			}
			catch( const PoDoFo::PdfError & e )
			{
				QMessageBox::critical( this, tr( "Error during rendering PDF..." ),
					tr( "%1\n\nNothing saved. Sorry." ).arg( QString::fromLatin1( e.what() ) ) );
			}
		}
		else
			QMessageBox::warning( this, tr( "Markdown is empty..." ),
				tr( "Input Markdown file is empty. Nothing saved." ) );
	}
}
