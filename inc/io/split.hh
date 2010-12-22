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

#ifndef IO_SPLIT_HH_
# define IO_SPLIT_HH_

# include <fstream>
# include <stddef.h>
# include <string>
# include "io/stream.hh"

namespace         io {
  /**
   *  @class split split.hh "io/split.hh"
   *  @brief Split-file class.
   *
   *  This class is used to consider multiple files as a single
   *  input/output object.
   */
  class           split : public stream {
   private:
    std::string   _basefile;
    unsigned int  _current_in;
    unsigned int  _current_out;
    std::ifstream _ifs;
    unsigned int  _max_file_size;
    unsigned int  _max_files;
    std::ofstream _ofs;
    unsigned int  _out_offset;
    unsigned int  _browse_dir(unsigned int* min = NULL,
                    unsigned int* max = NULL);
    void          _internal_copy(split const& s);
    bool          _open_next_input_file();
    bool          _open_next_output_file();

   public:
                  split();
                  split(split const& s);
                  ~split();
    split&        operator=(split const& s);
    void          base_file(std::string const& basefile);
    void          close();
    void          close_input();
    void          close_output();
    void          max_file_size(unsigned int max_size);
    void          max_files(unsigned int max_files);
    unsigned int  receive(void* buffer, unsigned int size);
    unsigned int  send(void const* buffer, unsigned int size);
  };
}

#endif /* !IO_SPLIT_HH_ */
