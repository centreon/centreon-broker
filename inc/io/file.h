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

#ifndef IO_FILE_H_
# define IO_FILE_H_

# include <string>
# include "io/fd.h"

namespace         CentreonBroker
{
  namespace       IO
  {
    /**
     *  \class FileStream file.h "io/file.h"
     *  \brief Wrapper of a file that can be treated as a byte stream.
     *
     *  The FileStream class can open a file and treat it as a byte stream.
     *
     *  \see FDStream
     */
    class         FileStream : public FDStream
    {
     public:
      enum        Mode
      {
	READ = 1,
	WRITE,
	READWRITE
      };
                  FileStream(const std::string& filename, Mode mode);
                  FileStream(const FileStream& file_stream);
                  ~FileStream() throw ();
      FileStream& operator=(const FileStream& file_stream);
    };
  }
}

#endif /* !IO_FILE_H_ */
