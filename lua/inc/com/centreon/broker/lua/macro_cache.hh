/*
** Copyright 2018 Centreon
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

#ifndef CCB_LUA_MACRO_CACHE_HH
#  define CCB_LUA_MACRO_CACHE_HH

#  include <QHash>
#  include "com/centreon/broker/bam/dimension_ba_bv_relation_event.hh"
#  include "com/centreon/broker/bam/dimension_ba_event.hh"
#  include "com/centreon/broker/bam/dimension_bv_event.hh"
#  include "com/centreon/broker/bam/dimension_truncate_table_signal.hh"
#  include "com/centreon/broker/neb/host.hh"
#  include "com/centreon/broker/neb/host_group.hh"
#  include "com/centreon/broker/neb/host_group_member.hh"
#  include "com/centreon/broker/neb/instance.hh"
#  include "com/centreon/broker/neb/service.hh"
#  include "com/centreon/broker/neb/service_group.hh"
#  include "com/centreon/broker/neb/service_group_member.hh"
#  include "com/centreon/broker/persistent_cache.hh"
#  include "com/centreon/broker/storage/index_mapping.hh"
#  include "com/centreon/broker/storage/metric_mapping.hh"

CCB_BEGIN()

namespace         lua {
  /**
   *  @class macro_cache macro_cache.hh "com/centreon/broker/lua/macro_cache.hh"
   *  @brief Data cache for Lua macro.
   */
  class            macro_cache {
  public:
                   macro_cache(misc::shared_ptr<persistent_cache> const& cache);
                   ~macro_cache();

    void           write(misc::shared_ptr<io::data> const& data);

    storage::index_mapping const&
                   get_index_mapping(unsigned int index_id) const;
    storage::metric_mapping const&
                   get_metric_mapping(unsigned int metric_id) const;
    QString const& get_host_name(unsigned int host_id) const;
    QString const& get_host_group_name(unsigned int id) const;
    QHash<unsigned int, QHash<unsigned int, neb::host_group_member> > const&
                   get_host_group_members() const;
    QString const& get_service_description(
                     unsigned int host_id,
                     unsigned int service_id) const;
    QString const& get_service_group_name(unsigned int id) const;
    QHash<QPair<unsigned int, unsigned int>,
          QHash<unsigned int, neb::service_group_member> > const&
                   get_service_group_members() const;
    QString const& get_instance(unsigned int instance_id) const;

    QMultiHash<unsigned int, bam::dimension_ba_bv_relation_event> const&
                   get_dimension_ba_bv_relation_events() const;
    bam::dimension_ba_event const&
                   get_dimension_ba_event(unsigned int id) const;
    bam::dimension_bv_event const&
                   get_dimension_bv_event(unsigned int id) const;

  private:
                   macro_cache(macro_cache const& f);
    macro_cache&   operator=(macro_cache const& f);

    void           _process_instance(neb::instance const& in);
    void           _process_host(neb::host const& h);
    void           _process_host_group(neb::host_group const& hg);
    void           _process_host_group_member(neb::host_group_member const& hgm);
    void           _process_service(neb::service const& s);
    void           _process_service_group(neb::service_group const& sg);
    void           _process_service_group_member(neb::service_group_member const& sgm);
    void           _process_index_mapping(storage::index_mapping const& im);
    void           _process_metric_mapping(storage::metric_mapping const& mm);
    void           _process_dimension_ba_event(
                     bam::dimension_ba_event const& dbae);
    void           _process_dimension_ba_bv_relation_event(
                     bam::dimension_ba_bv_relation_event const& rel);
    void           _process_dimension_bv_event(
                     bam::dimension_bv_event const& dbve);
    void           _process_dimension_truncate_table_signal(
                     bam::dimension_truncate_table_signal const& trunc);

    void           _save_to_disk();

    misc::shared_ptr<persistent_cache>
                   _cache;
    QHash<unsigned int, neb::instance>
                   _instances;
    QHash<unsigned int, neb::host>
                   _hosts;
    QHash<unsigned int, neb::host_group>
                   _host_groups;
    QHash<unsigned int, QHash<unsigned int, neb::host_group_member> >
                   _host_group_members;
    QHash<QPair<unsigned int, unsigned int>, neb::service>
                   _services;
    QHash<unsigned int, neb::service_group>
                   _service_groups;
    QHash<QPair<unsigned int, unsigned int>,
          QHash<unsigned int, neb::service_group_member> >
                   _service_group_members;
    QHash<unsigned int, storage::index_mapping>
                   _index_mappings;
    QHash<unsigned int, storage::metric_mapping>
                   _metric_mappings;
    QHash<unsigned int, bam::dimension_ba_event>
                   _dimension_ba_events;
    QMultiHash<unsigned int, bam::dimension_ba_bv_relation_event>
                   _dimension_ba_bv_relation_events;
    QHash<unsigned int, bam::dimension_bv_event>
                   _dimension_bv_events;
  };
}

CCB_END()

#endif // !CCB_LUA_MACRO_CACHE_HH
