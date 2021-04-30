/*
** Copyright 2014-2015 Centreon
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

#ifndef CCB_BAM_CONFIGURATION_STATE_HH
#define CCB_BAM_CONFIGURATION_STATE_HH

#include <deque>
#include <map>
#include "com/centreon/broker/bam/ba_svc_mapping.hh"
#include "com/centreon/broker/bam/configuration/ba.hh"
#include "com/centreon/broker/bam/configuration/bool_expression.hh"
#include "com/centreon/broker/bam/configuration/kpi.hh"
#include "com/centreon/broker/bam/hst_svc_mapping.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
namespace configuration {
/**
 *  @class state state.hh "com/centreon/broker/bam/configuration/state.hh"
 *  @brief Configuration state.
 *
 *  Holds the entire configuration of the BAM module.
 */
class state {
 public:
  /* Typedefs */
  typedef std::map<uint32_t, ba> bas;
  typedef std::map<uint32_t, kpi> kpis;
  typedef std::map<uint32_t, bool_expression> bool_exps;

  state();
  state(state const& other);
  ~state();
  state& operator=(state const& other);
  void clear();

  bas const& get_bas() const;
  kpis const& get_kpis() const;
  bool_exps const& get_bool_exps() const;
  hst_svc_mapping const& get_hst_svc_mapping() const;
  ba_svc_mapping const& get_ba_svc_mapping() const;
  ba_svc_mapping const& get_meta_svc_mapping() const;

  bas& get_bas();
  kpis& get_kpis();
  bool_exps& get_bool_exps();
  hst_svc_mapping& get_hst_svc_mapping();
  ba_svc_mapping& get_ba_svc_mapping();
  ba_svc_mapping& get_meta_svc_mapping();

 private:
  ba_svc_mapping _ba_svc_mapping;
  bas _bas;
  kpis _kpis;
  bool_exps _bool_expressions;
  hst_svc_mapping _hst_svc_mapping;
  ba_svc_mapping _meta_svc_mapping;
};
}  // namespace configuration
}  // namespace bam

CCB_END()

#endif  // ! CCB_BAM_CONFIGURATION_STATE_HH
