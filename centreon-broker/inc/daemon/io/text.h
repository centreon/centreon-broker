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

#ifndef IO_TEXT_H_
# define IO_TEXT_H_

# include <memory>      // for auto_ptr
# include "io/stream.h"

namespace                 IO
{
  /**
   *  \class Text text.h "io/text.h"
   *  \brief Handle a stream as a text stream.
   *
   *  The Text class can be used when the input stream should be treated as
   *  text (for example text-based protocols). Text can extract data line by
   *  line.
   */
  class                   Text : public Stream
  {
   private:
    char                  buffer_[1024];
    bool                  closed_;
    size_t                discard_;
    size_t                length_;
    std::auto_ptr<Stream> stream_;
                          Text(const Text& text);
    Text&                 operator=(const Text& text);

   public:
                          Text(Stream* stream);
                          ~Text();
    const char*           Line();
    unsigned int          Receive(void* buffer, unsigned int size);
    void                  Release();
    unsigned int          Send(const void* buffer, unsigned int size);
  };
}

#endif /* !IO_TEXT_H_ */
