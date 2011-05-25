/*
** Copyright 2009-2011 MERETHIS
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
**
** For more information: contact@centreon.com
*/

#ifndef IO_TLS_INTERNAL_HH_
# define IO_TLS_INTERNAL_HH_

# include <gnutls/gnutls.h>
# include <sys/types.h>

namespace                      io {
  namespace                    tls {
    // Data.
    extern unsigned char const dh_params_2048[];
    extern gnutls_dh_params_t  dh_params;

    // Code.
    void                       destroy();
    void                       initialize();
    ssize_t                    pull_helper(gnutls_transport_ptr_t ptr,
                                 void* data,
                                 size_t size);
    ssize_t                    push_helper(gnutls_transport_ptr_t ptr,
                                 void const* data,
                                 size_t size);
  }
}

#endif /* !IO_TLS_INTERNAL_HH_ */
