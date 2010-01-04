/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef IO_TLS_INTERNAL_H_
# define IO_TLS_INTERNAL_H_

# include <gnutls/gnutls.h> // for gnutls_dh_params_t, gnutls_transport_ptr_t
# include <sys/types.h>     // for size_t and ssize_t

namespace                      IO
{
  namespace                    TLS
  {
    // Data.
    extern const unsigned char dh_params_2048[];
    extern gnutls_dh_params_t  dh_params;

    // Code.
    void                       Destroy();
    void                       Initialize();
    ssize_t                    PullHelper(gnutls_transport_ptr_t ptr,
                                          void* data,
                                          size_t size);
    ssize_t                    PushHelper(gnutls_transport_ptr_t ptr,
                                          const void* data,
                                          size_t size);
  }
}

#endif /* !IO_TLS_INTERNAL_H_ */
