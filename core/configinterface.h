/***************************************************************************
 *   Copyright (C) 2007 by Pino Toscano <pino@kde.org>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _OKULAR_CONFIGINTERFACE_H_
#define _OKULAR_CONFIGINTERFACE_H_

#include "okular_export.h"

class KConfigDialog;

namespace Okular {

/**
 * @short Abstract interface for configuration control
 *
 * This interface defines a way to configure the Generator itself.
 */
class OKULAR_EXPORT ConfigInterface
{
    public:
        virtual ~ConfigInterface() {}

        /**
         * This method is called to tell the generator to re-parse its configuration.
         *
         * Returns true if something has changed.
         */
        virtual bool reparseConfig() = 0;

        /**
         * This method allows the generator to add custom configuration pages to the
         * config @p dialog of okular.
         */
        virtual void addPages( KConfigDialog *dialog ) = 0;
};

}

Q_DECLARE_INTERFACE( Okular::ConfigInterface, "org.kde.okular.ConfigInterface/0.1" )

#endif
