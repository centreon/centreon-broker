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
#ifndef CCB_BAM_PARSE_EXCEPTION_HH
# define  CCB_BAM_PARSE_EXCEPTION_HH

# include "com/centreon/broker/exceptions/msg.hh"

CCB_BEGIN()

namespace               bam {

    /**
     *  @class reader_exception parser_exception.hh "com/centreon/broker/bam/configuration/parser_exception.hh"
     *  @brief Exception thrown when the parser fails to read a bam boolean expression
     *
     *  Reader_exception.
     */
    class                parse_exception : public exceptions::msg {
    public:
                         parse_exception();
                         parse_exception(const parse_exception&);
                         ~parse_exception() throw ();
      parse_exception&  operator=(parse_exception const& other);
    };

}

CCB_END()



#endif
