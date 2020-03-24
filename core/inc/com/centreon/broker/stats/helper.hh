//
// Created by syl on 3/24/20.
//

#ifndef CENTREON_BROKER_CORE_INC_COM_CENTREON_STATS_HELPER_HH_
#define CENTREON_BROKER_CORE_INC_COM_CENTREON_STATS_HELPER_HH_

#include "com/centreon/broker/namespace.hh"

CCB_BEGIN();

namespace stats {

void get_generic_stats(json11::Json::object& object) noexcept;
void get_mysql_stats(json11::Json::object& object) noexcept;
void get_loaded_module_stats(std::vector<json11::Json::object>& object) noexcept;
void get_endpoint_stats(std::vector<json11::Json::object>& object) noexcept;

};

CCB_END();

#endif  // CENTREON_BROKER_CORE_INC_COM_CENTREON_STATS_HELPER_HH_
