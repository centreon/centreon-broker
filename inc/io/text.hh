/*
** Copyright 2009-2010 MERETHIS
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

#ifndef IO_TEXT_HH_
# define IO_TEXT_HH_

# include <memory>
# include "io/stream.hh"

namespace                 io {
  /**
   *  @class text text.hh "io/text.hh"
   *  @brief Handle a stream as a text stream.
   *
   *  The text class can be used when the input stream should be treated
   *  as text (for example text-based protocols). Text can extract data
   *  line by line.
   */
  class                   text : public stream {
   private:
    char                  _buffer[1024];
    bool                  _closed;
    size_t                _discard;
    size_t                _length;
    std::auto_ptr<stream> _stream;
                          text(text const& t);
    text&                 operator=(text const& t);

   public:
                          text(stream* s);
                          ~text();
    void                  close();
    char const*           line();
    unsigned int          receive(void* buffer, unsigned int size);
    void                  release();
    unsigned int          send(void const* buffer, unsigned int size);
  };
}

#endif /* !IO_TEXT_HH_ */
