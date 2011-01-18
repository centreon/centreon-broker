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

#ifndef INTERFACE_NDO_BASE_HH_
# define INTERFACE_NDO_BASE_HH_

# include "io/text.hh"

// Forward declaration
namespace      io {
  class        stream;
}

namespace      interface {
  namespace    ndo {
    /**
     *  @class base base.hh "interface/ndo/base.hh"
     *  @brief Base class of NDO protocol classes.
     *
     *  Holds the stream object so that destruction will only occur
     *  once.
     */
    class      base {
     private:
               base(base const& b);
      base&    operator=(base const& b);

     protected:
      io::text _stream;

     public:
               base(io::stream* s);
      virtual  ~base();
    };
  }
}

#endif /* !INTERFACE_NDO_BASE_HH_ */
