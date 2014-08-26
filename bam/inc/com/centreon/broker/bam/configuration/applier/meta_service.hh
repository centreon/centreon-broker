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
** <http:/www.gnu.org/licenses/>.
*/

#ifndef CCB_BAM_CONFIGURATION_APPLIER_META_SERVICE_HH
#  define CCB_BAM_CONFIGURATION_APPLIER_META_SERVICE_HH

#  include <map>
#  include "com/centreon/broker/bam/configuration/meta_service.hh"
#  include "com/centreon/broker/bam/configuration/state.hh"
#  include "com/centreon/broker/bam/meta_service.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace             bam {
  // Forward declaration.
  class               metric_book;

  namespace           configuration {
    namespace         applier {
      /**
       *  @class meta_service meta_service.hh "com/centreon/broker/bam/configuration/applier/meta_service.hh"
       *  @brief Apply meta-services.
       *
       *  Create, update and delete meta-services.
       */
      class           meta_service {
      public:
                      meta_service();
                      meta_service(meta_service const& other);
                      ~meta_service();
        meta_service& operator=(meta_service const& other);
        void          apply(
                        configuration::state::meta_services const& my_meta,
                        metric_book& book);

      private:
        struct applied {
          configuration::meta_service         cfg;
          misc::shared_ptr<bam::meta_service> obj;
        };

        void          _internal_copy(meta_service const& other);
        void          _modify_meta(
                        bam::meta_service& obj,
                        metric_book& book,
                        configuration::meta_service const& old_cfg,
                        configuration::meta_service const& new_cfg);
        misc::shared_ptr<bam::meta_service>
                      _new_meta(
                        configuration::meta_service const& cfg,
                        metric_book& book);

        std::map<unsigned int, applied>
                      _applied;
      };
    }
  }
}

CCB_END()

#endif // !CCB_BAM_CONFIGURATION_APPLIER_META_SERVICE_HH
