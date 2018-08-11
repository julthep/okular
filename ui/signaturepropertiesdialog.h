/***************************************************************************
 *   Copyright (C) 2018 by Chinmoy Ranjan Pradhan <chinmoyrp65@gmail.com>  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef OKULAR_SIGNATUREPROPERTIESDIALOG_H
#define OKULAR_SIGNATUREPROPERTIESDIALOG_H

#include <QDialog>

namespace Okular {
    class Document;
    class FormFieldSignature;
    class SignatureInfo;
}

class SignaturePropertiesDialog : public QDialog
{
    Q_OBJECT

    public:
        SignaturePropertiesDialog( Okular::Document *doc, Okular::FormFieldSignature *form, QWidget *parent = nullptr );

    public Q_SLOTS:
        void viewSignedVersion();
        void viewCertificateProperties();

    private:
        Okular::Document *m_doc;
        Okular::FormFieldSignature *m_signatureForm;
        Okular::SignatureInfo *m_signatureInfo;
};

#endif