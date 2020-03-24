#include "com/centreon/broker/stats/helper.hh"
#include <json11.hpp>
#include <asio.hpp>
#include "com/centreon/broker/config/applier/modules.hh"
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/misc/filesystem.hh"
#include "com/centreon/broker/mysql_manager.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::stats;

void get_generic_stats(json11::Json::object& object) noexcept {
  object["version"] = CENTREON_BROKER_VERSION;
  object["pid"] = getpid();
  object["now"] = std::to_string(::time(nullptr));

  std::string asio_version{std::to_string(ASIO_VERSION / 100000)};
  asio_version.append(".")
      .append(std::to_string(ASIO_VERSION / 100 % 1000))
      .append(".")
      .append(std::to_string(ASIO_VERSION % 100));
  object["asio_version"] = asio_version;
}

void get_mysql_stats(json11::Json::object& object) noexcept {
  std::map<std::string, std::string> stats(
      mysql_manager::instance().get_stats());
  json11::Json::object subtree;
  for (std::pair<std::string, std::string> const& p : stats)
    subtree[p.first] = p.second;
  object["mysql manager"] = subtree;
}

void get_loaded_module_stats(std::vector<json11::Json::object>& object) noexcept {
  config::applier::modules& mod_applier(config::applier::modules::instance());

  std::lock_guard<std::mutex> lock(mod_applier.module_mutex());

  for (config::applier::modules::iterator it(mod_applier.begin()),
           end(mod_applier.end());
       it != end; ++it) {
    json11::Json::object subtree;
    subtree["name"] = it->first;
    subtree["state"] = "loaded";
    subtree["size"] =
        std::to_string(misc::filesystem::file_size(it->first)) + "B";
    object.emplace_back(subtree);
  }
}