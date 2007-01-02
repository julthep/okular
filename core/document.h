/***************************************************************************
 *   Copyright (C) 2004-2005 by Enrico Ros <eros.kde@email.it>             *
 *   Copyright (C) 2004-2005 by Albert Astals Cid <tsdgeos@terra.es>       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _OKULAR_DOCUMENT_H_
#define _OKULAR_DOCUMENT_H_

#include "okular_export.h"

#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QVector>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#include <kmimetype.h>

#include "area.h"

class KBookmark;
class KPrinter;
class KPrintDialogPage;
class KUrl;
class KActionCollection;
class QToolBox;

namespace Okular {

class Annotation;
class BookmarkManager;
class DocumentFonts;
class DocumentInfo;
class DocumentObserver;
class DocumentSynopsis;
class DocumentViewport;
class EmbeddedFile;
class ExportFormat;
class Generator;
class Link;
class Page;
class PixmapRequest;
class SourceReference;
class VisiblePageRect;

/** IDs for seaches. Globally defined here. **/
#define PART_SEARCH_ID 1
#define PAGEVIEW_SEARCH_ID 2
#define SW_SEARCH_ID 3


/**
 * @short The Document. Heart of everything. Actions take place here.
 *
 * The Document is the main object in Okular. All views query the Document to
 * get data/properties or even for accessing pages (in a 'const' way).
 *
 * It is designed to keep it detached from the document type (pdf, ps, you
 * name it..) so whenever you want to get some data, it asks its internals
 * generator to do the job and return results in a format-indepedent way.
 *
 * Apart from the generator (the currently running one) the document stores
 * all the Pages ('Page' class) of the current document in a vector and
 * notifies all the registered DocumentObservers when some content changes.
 *
 * For a better understanding of hieracies @see README.internals.png
 * @see DocumentObserver, Page
 */
class OKULAR_EXPORT Document : public QObject
{
    Q_OBJECT
    public:
        Document( QHash<QString, Generator*> * genList );
        ~Document();

        // communication with the part

        // document handling
        bool openDocument( const QString & docFile, const KUrl & url, const KMimeType::Ptr &mime );
        void closeDocument();

        // observer stuff
        void addObserver( DocumentObserver * pObserver );
        void removeObserver( DocumentObserver * pObserver );
        void reparseConfig();

        // enum definitions
        enum Permission { AllowModify = 1, AllowCopy = 2, AllowPrint = 4, AllowNotes = 8 };

        // query methods (const ones)
        bool isOpened() const;
        const DocumentInfo * documentInfo() const;
        const DocumentSynopsis * documentSynopsis() const;
        const DocumentFonts * documentFonts() const;
        const QList<EmbeddedFile*> *embeddedFiles() const;
        const Page * page( int page ) const;
        const DocumentViewport & viewport() const;
        const QVector< VisiblePageRect * > & visiblePageRects() const;
        void setVisiblePageRects( const QVector< VisiblePageRect * > & visiblePageRects, int excludeId = -1 );
        uint currentPage() const;
        uint pages() const;
        KUrl currentDocument() const;
        bool isAllowed( int /*Document::Permisison(s)*/ ) const;
        bool supportsSearching() const;
        bool supportsRotation()  const;
        bool supportsPaperSizes() const;
        QStringList paperSizes() const;
        bool canExportToText() const;
        bool exportToText( const QString& fileName ) const;
        QList<ExportFormat> exportFormats() const;
        bool exportTo( const QString& fileName, const ExportFormat& format ) const;
        bool historyAtBegin() const;
        bool historyAtEnd() const;
        QVariant getMetaData( const QString & key, const QVariant & option = QVariant() ) const;
        int rotation() const;
        QSizeF allPagesSize() const;
        QString pageSizeString(int page) const;

        // gui altering stuff
        QString xmlFile();
        void setupGui( KActionCollection *ac, QToolBox *tBox );

        // perform actions on document / pages
        void setViewportPage( int page, int excludeId = -1, bool smoothMove = false );
        void setViewport( const DocumentViewport & viewport, int excludeId = -1, bool smoothMove = false );
        void setPrevViewport();
        void setNextViewport();
        void setNextDocumentViewport( const DocumentViewport & viewport );
        void requestPixmaps( const QLinkedList< PixmapRequest * > & requests );
        void requestTextPage( uint page );
        void addPageAnnotation( int page, Annotation * annotation );
        void modifyPageAnnotation( int page, Annotation * newannotation );
        void removePageAnnotation( int page, Annotation * annotation );
        void removePageAnnotations( int page, QList< Annotation * > annotations );

        void setPageTextSelection( int page, RegularAreaRect * rect, const QColor & color );

        enum SearchType { NextMatch, PrevMatch, AllDoc, GoogleAll, GoogleAny };
        bool searchText( int searchID, const QString & text, bool fromStart, Qt::CaseSensitivity caseSensitivity,
                         SearchType type, bool moveViewport, const QColor & color, bool noDialogs = false );
        bool continueSearch( int searchID );
        void resetSearch( int searchID );
        bool continueLastSearch();

        void addBookmark( int page );
        void addBookmark( const KUrl& referurl, const Okular::DocumentViewport& vp, const QString& title = QString() );
        bool isBookmarked( int page ) const;
        void removeBookmark( const KUrl& referurl, const KBookmark& bm );
        const BookmarkManager * bookmarkManager() const;

        void processLink( const Link * link );
        void processSourceReference( const SourceReference * ref );
        bool canConfigurePrinter() const;
        bool print( KPrinter &printer );
        KPrintDialogPage* configurationWidget() const;
        // notifications sent by generator
        void requestDone( PixmapRequest * request );
//         inline pagesVector() { return pages_vector; };

    public Q_SLOTS:
        void slotRotation( int rotation );
        void slotPaperSizes( int );

    Q_SIGNALS:
        void close();
        void quit();
        void linkFind();
        void linkGoToPage();
        void linkPresentation();
        void linkEndPresentation();
        void openUrl(const KUrl &url);
        void error(const QString & string, int duration);
        void warning(const QString & string, int duration);
        void notice(const QString & string, int duration);

    private:
        class Private;
        Private* const d;

        Q_DISABLE_COPY( Document )

        Q_PRIVATE_SLOT( d, void saveDocumentInfo() const )
        Q_PRIVATE_SLOT( d, void slotTimedMemoryCheck() )
        Q_PRIVATE_SLOT( d, void sendGeneratorRequest() )
        Q_PRIVATE_SLOT( d, void rotationFinished( int page ) )
};


/**
 * @short A view on the document.
 *
 * The Viewport structure is the 'current view' over the document. Contained
 * data is broadcasted between observers to syncronize their viewports to get
 * the 'I scroll one view and others scroll too' views.
 */
class OKULAR_EXPORT DocumentViewport
{
    public:
        /**
         * Creates a new viewport for the given page @p number.
         */
        DocumentViewport( int number = -1 );

        /**
         * Creates a new viewport from the given xml @p description.
         */
        DocumentViewport( const QString &description );

        /**
         * Returns the viewport as xml description.
         */
        QString toString() const;

        /**
         * Returns whether the viewport is valid.
         */
        bool isValid() const;

        /**
         * @internal
         */
        bool operator==( const DocumentViewport &other ) const;

        /**
         * The number of the page nearest the center of the viewport.
         */
        int pageNumber;

        /**
         * Describes the relative position of the viewport.
         */
        enum Position
        {
            Center = 1,  ///< Relative to the center of the page.
            TopLeft = 2  ///< Relative to the top left corner of the page.
        };

        /**
         * If 'rePos.enabled == true' then this structure contains the
         * viewport center.
         */
        struct {
            bool enabled;
            double normalizedX;
            double normalizedY;
            Position pos;
        } rePos;

        /**
         * If 'autoFit.enabled == true' then the page must be autofitted in the viewport.
         */
        struct {
            bool enabled;
            bool width;
            bool height;
        } autoFit;
};

/**
 * @short A DOM tree containing information about the document.
 *
 * The DocumentInfo structure can be filled in by generators to display
 * metadata about the currently opened file.
 */
class OKULAR_EXPORT DocumentInfo : public QDomDocument
{
    public:
        /**
         * Creates a new document info.
         */
        DocumentInfo();

        /**
         * Sets a value for a special key. The title should be an i18n'ed
         * string, since it's used in the document information dialog.
         */
        void set( const QString &key, const QString &value,
                  const QString &title = QString() );

        /**
         * Returns the value for a given key or an empty string when the
         * key doesn't exist.
         */
        QString get( const QString &key ) const;
};

/**
 * @short A DOM tree that describes the Table of Contents.
 *
 * The Synopsis (TOC or Table Of Contents for friends) is represented via
 * a dom tree where each node has an internal name (displayed in the listview)
 * and one or more attributes.
 *
 * In the tree the tag name is the 'screen' name of the entry. A tag can have
 * attributes. Here follows the list of tag attributes with meaning:
 * - Icon: An icon to be set in the Lisview for the node
 * - Viewport: A string description of the referred viewport
 * - ViewportName: A 'named reference' to the viewport that must be converted
 *      using getMetaData( "NamedViewport", *viewport_name* )
 */
class OKULAR_EXPORT DocumentSynopsis : public QDomDocument
{
    public:
        /**
         * Creates a new document synopsis object.
         */
        DocumentSynopsis();

        /**
         * Creates a new document synopsis object with the given
         * @p document as parent node.
         */
        DocumentSynopsis( const QDomDocument &document );
};

/**
 * @short A DOM thee describing fonts used in document.
 *
 * Root's childrend (if any) are font nodes with the following attributes:
 * - Name
 * - Type
 * - Embedded (if font is shipped inside the document)
 * - File (system's file that provides this font
 */
class OKULAR_EXPORT DocumentFonts : public QDomDocument
{
    public:
        /**
         * Creates a new document fonts object.
         */
        DocumentFonts();
};

/**
 * @short An embedded file into the document.
 *
 * This class represents a sort of interface of an embedded file in a document.
 *
 * Generators \b must re-implement its members to give the all the information
 * about an embedded file, like its name, its description, the date of creation
 * and modification, and the real data of the file.
 */
class OKULAR_EXPORT EmbeddedFile
{
    public:
        /**
         * Creates a new embedded file.
         */
        EmbeddedFile();

        /**
         * Destroys the embedded file.
         */
        virtual ~EmbeddedFile();

        /**
         * Returns the name of the file
         */
        virtual QString name() const = 0;

        /**
         * Returns the description of the file, or an empty string if not
         * available
         */
        virtual QString description() const = 0;

        /**
         * Returns the real data representing the file contents
         */
        virtual QByteArray data() const = 0;

        /**
         * Returns the modification date of the file, or an invalid date
         * if not available
         */
        virtual QDateTime modificationDate() const = 0;

        /**
         * Returns the creation date of the file, or an invalid date
         * if not available
         */
        virtual QDateTime creationDate() const = 0;
};

/**
 * @short An area of a specified page
 */
class OKULAR_EXPORT VisiblePageRect
{
    public:
        /**
         * Creates a new visible page rectangle.
         *
         * @param pageNumber The page number where the rectangle is located.
         * @param rectangle The rectangle in normalized coordinates.
         */
        VisiblePageRect( int pageNumber = -1, const NormalizedRect &rectangle = NormalizedRect() );

        /**
         * The page number where the rectangle is located.
         */
        int pageNumber;

        /**
         * The rectangle in normalized coordinates.
         */
        NormalizedRect rect;
};

}

#endif
