/*
** Copyright 2011-2012 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_FILE_OPENER_HH
#  define CCB_FILE_OPENER_HH

#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                        file {
  /**
   *  @class opener opener.hh "com/centreon/broker/file/opener.hh"
   *  @brief Open a file stream.
   *
   *  Open a file stream.
   */
  class                          opener : public io::endpoint {
  public:
                                 opener(bool is_in, bool is_out);
                                 opener(opener const& o);
                                 ~opener();
    opener&                      operator=(opener const& o);
    io::endpoint*                clone() const;
    void                         close();
    misc::shared_ptr<io::stream> open();
    misc::shared_ptr<io::stream> open(QString const& id);
    void                         set_filename(QString const& filename);
    void                         set_max_size(unsigned long long max);

   private:
    QString                      _filename;
    bool                         _is_in;
    bool                         _is_out;
    unsigned long long           _max_size;
  };
}

CCB_END()

#endif /* !CCB_FILE_OPENER_HH_ */
