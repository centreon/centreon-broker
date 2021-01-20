/*
** Copyright 2014-2016 Centreon
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

#include "com/centreon/broker/bam/configuration/applier/state.hh"
#include <fmt/format.h>
#include "com/centreon/broker/bam/exp_builder.hh"
#include "com/centreon/broker/bam/exp_parser.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace com::centreon::broker::bam::configuration;

/**************************************
 *                                     *
 *            Static Objects           *
 *                                     *
 **************************************/

/**
 *  Get BA identifier for circular path search.
 *
 *  @return BA identifier for circular path search.
 */
static std::string ba_node_id(uint32_t ba_id) {
  return fmt::format("BA {}", ba_id);
}

/**
 *  Get boolean expression identifier for circular path search.
 *
 *  @return Boolean expression identifier for circular path search.
 */
static std::string boolexp_node_id(uint32_t boolexp_id) {
  return fmt::format("boolean expression {}", boolexp_id);
}

/**
 *  Get KPI identifier for circular path search.
 *
 *  @return KPI identifier for circular path search.
 */
static std::string kpi_node_id(uint32_t kpi_id) {
  return fmt::format("KPI {}", kpi_id);
}

/**
 *  Get meta-service identifier for circular path search.
 *
 *  @return Meta-service identifier for circular path search.
 */
static std::string meta_node_id(uint32_t meta_id) {
  return fmt::format("meta-service {}", meta_id);
}

/**
 *  Get service identifier for circular path search.
 *
 *  @return Service identifier for circular path search.
 */
static std::string service_node_id(uint32_t host_id, uint32_t service_id) {
  return fmt::format("service ({}, {})", host_id, service_id);
}

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
applier::state::state() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
applier::state::state(applier::state const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
applier::state::~state() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
applier::state& applier::state::operator=(applier::state const& other) {
  if (this != &other)
    _internal_copy(other);
  return *this;
}

/**
 *  Apply configuration.
 *
 *  @param[in] my_state  Configuration state.
 */
void applier::state::apply(bam::configuration::state const& my_state) {
  // Search for circular path in object graph.
  _circular_check(my_state);

  // Really apply objects.
  _ba_applier.apply(my_state.get_bas(), _book_service);
  _meta_service_applier.apply(my_state.get_meta_services(), _book_metric);
  _bool_exp_applier.apply(my_state.get_bool_exps(),
                          my_state.get_hst_svc_mapping(), _book_service,
                          _book_metric);
  _kpi_applier.apply(my_state.get_kpis(), my_state.get_hst_svc_mapping(),
                     _ba_applier, _meta_service_applier, _bool_exp_applier,
                     _book_service);
}

/**
 *  Get the book of metric listeners.
 *
 *  @return Book of metric listeners.
 */
bam::metric_book& applier::state::book_metric() {
  return _book_metric;
}

/**
 *  Get the book of service listeners.
 *
 *  @return Book of service listeners.
 */
bam::service_book& applier::state::book_service() {
  return _book_service;
}

/**
 *  @brief Visit applied state.
 *
 *  This method is used to generate default status.
 *
 *  @param[out] visitor  Visitor.
 */
void applier::state::visit(io::stream* visitor) {
  _ba_applier.visit(visitor);
  _kpi_applier.visit(visitor);
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Circular check node constructor.
 */
applier::state::circular_check_node::circular_check_node()
    : in_visit(false), visited(false) {}

/**
 *  Check BA computation graph for circular paths.
 *
 *  @param[in] my_state  Configuration state.
 */
void applier::state::_circular_check(configuration::state const& my_state) {
  // In this method, nodes are referenced by an internal ID named after
  // object type and ID.

  //
  // Populate graph with all objects.
  //
  _nodes.clear();

  // Add BAs.
  for (configuration::state::bas::const_iterator it(my_state.get_bas().begin()),
       end(my_state.get_bas().end());
       it != end; ++it) {
    circular_check_node& n(_nodes[ba_node_id(it->first)]);
    n.targets.insert(
        service_node_id(it->second.get_host_id(), it->second.get_service_id()));
  }
  // Add meta-services.
  for (configuration::state::meta_services::const_iterator
           it(my_state.get_meta_services().begin()),
       end(my_state.get_meta_services().end());
       it != end; ++it) {
    std::string meta_id(meta_node_id(it->first));
    circular_check_node& n(_nodes[meta_id]);
    n.targets.insert(
        service_node_id(it->second.get_host_id(), it->second.get_service_id()));
    for (configuration::meta_service::service_container::const_iterator
             it_svc(it->second.get_services().begin()),
         end_svc(it->second.get_services().end());
         it_svc != end_svc; ++it)
      _nodes[service_node_id(it_svc->first, it_svc->second)].targets.insert(
          meta_id);
  }
  // Add boolean expressions.
  for (configuration::state::bool_exps::const_iterator
           it(my_state.get_bool_exps().begin()),
       end(my_state.get_bool_exps().end());
       it != end; ++it) {
    std::string bool_id(boolexp_node_id(it->first));
    _nodes[bool_id];
    try {
      exp_parser parsr(it->second.get_expression());
      exp_builder buildr(parsr.get_postfix(), my_state.get_hst_svc_mapping());
      for (std::list<bool_service::ptr>::const_iterator
               it_svc(buildr.get_services().begin()),
           end_svc(buildr.get_services().end());
           it_svc != end_svc; ++it_svc)
        _nodes[service_node_id((*it_svc)->get_host_id(),
                               (*it_svc)->get_service_id())]
            .targets.insert(bool_id);
    }
    // Silently ignore parsing errors.
    catch (std::exception const& e) {
      (void)e;
    }
  }
  // Add KPIs.
  for (configuration::state::kpis::const_iterator
           it(my_state.get_kpis().begin()),
       end(my_state.get_kpis().end());
       it != end; ++it) {
    std::string kpi_id(kpi_node_id(it->first));
    circular_check_node& n(_nodes[kpi_id]);
    n.targets.insert(ba_node_id(it->second.get_ba_id()));
    std::string node_id;
    if (it->second.is_ba())
      node_id = ba_node_id(it->second.get_indicator_ba_id());
    else if (it->second.is_meta())
      node_id = meta_node_id(it->second.get_meta_id());
    else if (it->second.is_boolexp())
      node_id = boolexp_node_id(it->second.get_boolexp_id());
    else if (it->second.is_service())
      node_id = service_node_id(it->second.get_host_id(),
                                it->second.get_service_id());
    else
      continue;
    _nodes[node_id].targets.insert(kpi_id);
  }

  // Process all nodes.
  for (std::unordered_map<std::string, circular_check_node>::iterator
           it(_nodes.begin()),
       end(_nodes.end());
       it != end; ++it)
    if (!it->second.visited)
      _circular_check(it->second);
  _nodes.clear();
}

/**
 *  Check a node for circular path.
 *
 *  @param[in,out] n      Target node.
 */
void applier::state::_circular_check(applier::state::circular_check_node& n) {
  if (n.in_visit)
    throw(msg_fmt("BAM: loop found in BA graph"));
  if (!n.visited) {
    n.in_visit = true;
    for (std::set<std::string>::const_iterator it(n.targets.begin()),
         end(n.targets.end());
         it != end; ++it) {
      std::unordered_map<std::string, circular_check_node>::iterator it_node(
          _nodes.find(*it));
      if (it_node != _nodes.end())
        _circular_check(it_node->second);
    }
    n.visited = true;
    n.in_visit = false;
  }
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void applier::state::_internal_copy(applier::state const& other) {
  _ba_applier = other._ba_applier;
  _book_metric = other._book_metric;
  _book_service = other._book_service;
  _kpi_applier = other._kpi_applier;
  _bool_exp_applier = other._bool_exp_applier;
  _meta_service_applier = other._meta_service_applier;
}

/**
 *  Save the state to the cache.
 *
 *  @param[in] cache  The cache.
 */
void applier::state::save_to_cache(persistent_cache& cache) {
  _ba_applier.save_to_cache(cache);
}

/**
 *  Load the state from the cache.
 *
 *  @param[in] cache  the cache.
 */
void applier::state::load_from_cache(persistent_cache& cache) {
  _ba_applier.load_from_cache(cache);
}
