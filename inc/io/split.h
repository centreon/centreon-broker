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

#ifndef IO_SPLIT_H_
# define IO_SPLIT_H_

# include <fstream>
# include <stddef.h>
# include <string>
# include "io/stream.h"

namespace         IO
{
  /**
   *  \class Split split.h "io/split.h"
   *  \brief Split-file class.
   *
   *  This class is used to consider multiple files as a single input/output
   *  object.
   */
  class           Split : public Stream
  {
   private:
    std::string   basefile_;
    unsigned int  current_in_;
    unsigned int  current_out_;
    std::ifstream ifs_;
    unsigned int  max_file_size_;
    unsigned int  max_files_;
    std::ofstream ofs_;
    unsigned int  out_offset_;
    unsigned int  BrowseDir(unsigned int* min = NULL,
                            unsigned int* max = NULL);
    void          InternalCopy(const Split& split);
    bool          OpenNextInputFile();
    bool          OpenNextOutputFile();

   public:
                  Split();
                  Split(const Split& split);
                  ~Split();
    Split&        operator=(const Split& split);
    void          BaseFile(const std::string& basefile);
    void          Close();
    void          CloseInput();
    void          CloseOutput();
    void          MaxFileSize(unsigned int max_size);
    void          MaxFiles(unsigned int max_files);
    unsigned int  Receive(void* buffer, unsigned int size);
    unsigned int  Send(const void* buffer, unsigned int size);
  };
}

#endif /* !IO_SPLIT_H_ */
