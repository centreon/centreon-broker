/*
** Copyright 2014-2015 Merethis
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

#ifndef CCB_BAM_CONFIGURATION_APPLIER_KPI_HH
#  define CCB_BAM_CONFIGURATION_APPLIER_KPI_HH

#  include <map>
#  include "com/centreon/broker/bam/kpi.hh"
#  include "com/centreon/broker/bam/configuration/kpi.hh"
#  include "com/centreon/broker/bam/configuration/state.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace     bam {
  // Forward declaration.
  class       service_book;

  namespace   configuration {
    namespace applier {
      // Forward declarations.
      class   ba;
      class   bool_expression;
      class   meta_service;

      /**
       *  @class kpi kpi.hh "com/centreon/broker/bam/configuration/applier/kpi.hh"
       *  @brief Apply KPI configuration.
       *
       *  Take the configuration of KPIs and apply it.
       */
      class   kpi {
      public:
              kpi();
              kpi(kpi const& other);
              ~kpi();
        kpi&  operator=(kpi const& other);
        void  apply(
                configuration::state::kpis const& my_kpis,
                hst_svc_mapping const& mapping,
                ba& my_bas,
                meta_service& my_metas,
                bool_expression& my_boolexps,
                service_book& book);
        void  visit(io::stream* visitor);

      private:
        struct applied {
          configuration::kpi         cfg;
          misc::shared_ptr<bam::kpi> obj;
        };

        void  _internal_copy(kpi const& other);
        misc::shared_ptr<bam::kpi>
              _new_kpi(configuration::kpi const& cfg);

        std::map<unsigned int, applied>
              _applied;
        ba*   _bas;
        service_book*
              _book;
        bool_expression*
              _boolexps;
        hst_svc_mapping const*
              _mapping;
        meta_service*
              _metas;
      };
    }
  }
}

CCB_END()

#endif // !CCB_BAM_CONFIGURATION_APPLIER_KPI_HH
