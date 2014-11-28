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

#ifndef CCB_BAM_CONFIGURATION_META_SERVICE_HH
#  define CCB_BAM_CONFIGURATION_META_SERVICE_HH

#  include <list>
#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                bam {
  namespace              configuration {
    /**
     *  @class meta_service meta_service.hh "com/centreon/broker/bam/configuration/meta_service.hh"
     *  @brief Meta-service configuration state.
     *
     *  The meta_service class is used to store configuration read from
     *  the database.
     */
    class                meta_service {
    public:
      typedef std::list<unsigned int> metric_container;

                         meta_service(
                           unsigned int id = 0,
                           std::string const& name = "",
                           std::string const& computation = "",
                           double warning_level = 0.0,
                           double critical_level = 0.0,
                           std::string const& filter = "",
                           std::string const& metric = "");
                         meta_service(meta_service const& other);
                         ~meta_service();
      meta_service&      operator=(meta_service const& other);
      bool               operator==(meta_service const& other) const;
      bool               operator!=(meta_service const& other) const;

      std::string const& get_computation() const;
      unsigned int       get_id() const;
      double             get_level_critical() const;
      double             get_level_warning() const;
      std::string const& get_metric_name() const;
      metric_container const&
                         get_metrics() const;
      std::string const& get_name() const;
      std::string const& get_service_filter() const;

      void               add_metric(unsigned int metric_id);
      void               set_computation(std::string const& function);
      void               set_id(unsigned int id);
      void               set_level_critical(double level);
      void               set_level_warning(double level);
      void               set_metric_name(std::string const& metric);
      void               set_name(std::string const& name);
      void               set_service_filter(std::string const& filter);

    private:
      void               _internal_copy(meta_service const& other);

      std::string        _computation;
      unsigned int       _id;
      double             _level_critical;
      double             _level_warning;
      std::string        _metric_name;
      metric_container   _metrics;
      std::string        _name;
      std::string        _service_filter;
    };
  }
}

CCB_END()

#endif // !CCB_BAM_CONFIGURATION_META_SERVICE_HH
