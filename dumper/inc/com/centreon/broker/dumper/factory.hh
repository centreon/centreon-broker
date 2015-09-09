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

#ifndef CCB_DUMPER_FACTORY_HH
#  define CCB_DUMPER_FACTORY_HH

#  include "com/centreon/broker/io/factory.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace         dumper {
  /**
   *  @class factory factory.hh "com/centreon/broker/dumper/factory.hh"
   *  @brief Dumper layer factory.
   *
   *  Build dumper objects.
   */
  class           factory : public io::factory {
  public:
                  factory();
                  factory(factory const& f);
                  ~factory();
    factory&      operator=(factory const& f);
    io::factory*  clone() const;
    bool          has_endpoint(
                    config::endpoint& cfg,
                    bool is_input,
                    bool is_output) const;
    io::endpoint* new_endpoint(
                    config::endpoint& cfg,
                    bool is_input,
                    bool is_output,
                    bool& is_acceptor) const;
  };
}

CCB_END()

#endif // !CCB_DUMPER_FACTORY_HH
