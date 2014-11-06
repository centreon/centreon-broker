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

#ifndef CCB_BAM_BA_SVC_MAPPING_HH
#  define CCB_BAM_BA_SVC_MAPPING_HH

#  include <map>
#  include <string>
#  include <utility>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace           bam {
  /**
   *  @class ba_svc_mapping ba_svc_mapping.hh "com/centreon/broker/bam/ba_svc_mapping.hh"
   *  @brief Link BA ID to host name and service description.
   *
   *  Allow users to get a virtual BA host name and service description
   *  by a BA ID.
   */
  class             ba_svc_mapping {
  public:
                    ba_svc_mapping();
                    ba_svc_mapping(ba_svc_mapping const& other);
                    ~ba_svc_mapping();
    ba_svc_mapping& operator=(ba_svc_mapping const& other);
    std::pair<std::string, std::string>
                    get_service(unsigned int ba_id);
    void            set(
                      unsigned int ba_id,
                      std::string const& hst,
                      std::string const& svc);

  private:
    void            _internal_copy(ba_svc_mapping const& other);

    std::map<unsigned int, std::pair<std::string, std::string> >
                    _mapping;
  };
}

CCB_END()

#endif // !CCB_BAM_BA_SVC_MAPPING_HH
