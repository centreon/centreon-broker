/*
** Copyright 2013 Centreon
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

#ifndef CCB_STATS_PLAIN_TEXT_SERIALIZER_HH
#  define CCB_STATS_PLAIN_TEXT_SERIALIZER_HH

#  include <string>
#  include "com/centreon/broker/stats/serializer.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace         stats {
  /**
   *  @class plain_text_serializer plain_text_serializer.hh "com/centreon/broker/stats/plain_text_serializer.hh"
   *  @brief Serialize in plain text format.
   */
  class           plain_text_serializer : public serializer {
  public:
                  plain_text_serializer();
                  ~plain_text_serializer();

    virtual void  serialize(
                    std::string& buffer,
                    io::properties const& tree) const;

  private:
    void          _serialize(
                    std::string& buffer,
                    io::properties const& tree,
                    unsigned int indent) const;
  };
}

CCB_END()

#endif // !CCB_STATS_PLAIN_TEXT_SERIALIZER_HH
