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

#ifndef CCB_BAM_META_SERVICE_STATUS_HH
#  define CCB_BAM_META_SERVICE_STATUS_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                bam {
  /**
   *  @class meta_service_status meta_service_status.hh "com/centreon/broker/bam/meta_service_status.hh"
   *  @brief Update status of a meta-service.
   *
   *  Update the status of a meta-service. This will update the
   *  meta_service table.
   */
  class                  meta_service_status : public io::data {
  public:
                         meta_service_status();
                         meta_service_status(
                           meta_service_status const& other);
                         ~meta_service_status();
    meta_service_status& operator=(meta_service_status const& other);
    unsigned int         type() const;

    unsigned int         meta_service_id;
    double               value;

  private:
    void                 _internal_copy(
                           meta_service_status const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_META_SERVICE_STATUS_HH
