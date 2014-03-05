/*
** Copyright 2014 Merethis
**
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
*/

#ifndef CCB_CONFIGURATION_READER_EXCEPTION_HH
#  define CCB_CONFIGURATION_READER_EXCEPTION_HH

#  include "com/centreon/broker/exceptions/msg.hh"

CCB_BEGIN()

namespace               bam {
  namespace             configuration {
    /**
     *  @class reader_exception reader_exception.hh "com/centreon/broker/bam/configuration/reader_exception.hh"
     *  @brief Exception thrown when the reader fails to read from a database
     *
     *  Reader_exception.
     */
    class               reader_exception : public exceptions::msg {
    public:
                        reader_exception();
                        ~reader_exception() throw ();
      reader_exception& operator=(reader_exception const& other);
    };
  }
}

CCB_END()

#endif // !CCB_CONFIGURATION_READER_EXCEPTION_HH
