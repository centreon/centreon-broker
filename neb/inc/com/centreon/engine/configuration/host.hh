/*
** Copyright 2011-2013,2015-2017 Centreon
**
** This file is part of Centreon Engine.
**
** Centreon Engine is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Engine is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Engine. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCE_CONFIGURATION_HOST_HH
#define CCE_CONFIGURATION_HOST_HH

#include <list>
#include <memory>
#include <set>
#include "com/centreon/engine/common.hh"
#include "com/centreon/engine/configuration/group.hh"
#include "com/centreon/engine/configuration/object.hh"
#include "com/centreon/engine/configuration/point_2d.hh"
#include "com/centreon/engine/configuration/point_3d.hh"
#include "com/centreon/engine/customvariable.hh"
#include "com/centreon/engine/namespace.hh"
#include "com/centreon/engine/opt.hh"

CCE_BEGIN()

namespace configuration {
class hostextinfo;

class host : public object {
 public:
  enum action_on {
    none = 0,
    up = (1 << 0),
    down = (1 << 1),
    unreachable = (1 << 2),
    flapping = (1 << 3),
    downtime = (1 << 4)
  };
  typedef uint64_t key_type;

  host(key_type const& key = 0);
  host(host const& other);
  ~host() throw() override;
  host& operator=(host const& other);
  bool operator==(host const& other) const throw();
  bool operator!=(host const& other) const throw();
  bool operator<(host const& other) const throw();
  void check_validity() const override;
  key_type key() const throw();
  void merge(configuration::hostextinfo const& obj);
  void merge(object const& obj) override;
  bool parse(char const* key, char const* value) override;

  std::string const& action_url() const throw();
  std::string const& address() const throw();
  std::string const& alias() const throw();
  bool checks_active() const throw();
  bool checks_passive() const throw();
  std::string const& check_command() const throw();
  bool check_freshness() const throw();
  uint32_t check_interval() const throw();
  std::string const& check_period() const throw();
  set_string const& contactgroups() const throw();
  set_string const& contacts() const throw();
  point_2d const& coords_2d() const throw();
  point_3d const& coords_3d() const throw();
  map_customvar const& customvariables() const throw();
  map_customvar& customvariables() throw();
  std::string const& display_name() const throw();
  std::string const& event_handler() const throw();
  bool event_handler_enabled() const throw();
  uint32_t first_notification_delay() const throw();
  bool flap_detection_enabled() const throw();
  uint32_t flap_detection_options() const throw();
  uint32_t freshness_threshold() const throw();
  bool have_coords_2d() const throw();
  bool have_coords_3d() const throw();
  uint32_t high_flap_threshold() const throw();
  set_string& hostgroups() throw();
  set_string const& hostgroups() const throw();
  uint64_t host_id() const throw();
  std::string const& host_name() const throw();
  std::string const& icon_image() const throw();
  std::string const& icon_image_alt() const throw();
  uint32_t initial_state() const throw();
  uint32_t low_flap_threshold() const throw();
  uint32_t max_check_attempts() const throw();
  std::string const& notes() const throw();
  std::string const& notes_url() const throw();
  bool notifications_enabled() const throw();
  uint32_t notification_interval() const throw();
  uint32_t notification_options() const throw();
  std::string const& notification_period() const throw();
  bool obsess_over_host() const throw();
  set_string& parents() throw();
  set_string const& parents() const throw();
  bool process_perf_data() const throw();
  bool retain_nonstatus_information() const throw();
  bool retain_status_information() const throw();
  uint32_t retry_interval() const throw();
  uint32_t recovery_notification_delay() const throw();
  uint32_t stalking_options() const throw();
  std::string const& statusmap_image() const throw();
  std::string const& timezone() const throw();
  std::string const& vrml_image() const throw();
  int get_acknowledgement_timeout() const throw();
  bool set_acknowledgement_timeout(int value);

 private:
  typedef bool (*setter_func)(host&, char const*);

  bool _set_action_url(std::string const& value);
  bool _set_address(std::string const& value);
  bool _set_alias(std::string const& value);
  bool _set_checks_active(bool value);
  bool _set_checks_passive(bool value);
  bool _set_check_command(std::string const& value);
  bool _set_check_freshness(bool value);
  bool _set_check_interval(uint32_t value);
  bool _set_check_period(std::string const& value);
  bool _set_contactgroups(std::string const& value);
  bool _set_contacts(std::string const& value);
  bool _set_coords_2d(std::string const& value);
  bool _set_coords_3d(std::string const& value);
  bool _set_display_name(std::string const& value);
  bool _set_event_handler(std::string const& value);
  bool _set_event_handler_enabled(bool value);
  bool _set_failure_prediction_enabled(bool value);
  bool _set_failure_prediction_options(std::string const& value);
  bool _set_first_notification_delay(uint32_t value);
  bool _set_flap_detection_enabled(bool value);
  bool _set_flap_detection_options(std::string const& value);
  bool _set_freshness_threshold(uint32_t value);
  bool _set_high_flap_threshold(uint32_t value);
  bool _set_host_id(uint64_t value);
  bool _set_host_name(std::string const& value);
  bool _set_hostgroups(std::string const& value);
  bool _set_icon_image(std::string const& value);
  bool _set_icon_image_alt(std::string const& value);
  bool _set_initial_state(std::string const& value);
  bool _set_low_flap_threshold(uint32_t value);
  bool _set_max_check_attempts(uint32_t value);
  bool _set_notes(std::string const& value);
  bool _set_notes_url(std::string const& value);
  bool _set_notifications_enabled(bool value);
  bool _set_notification_interval(uint32_t value);
  bool _set_notification_options(std::string const& value);
  bool _set_notification_period(std::string const& value);
  bool _set_obsess_over_host(bool value);
  bool _set_parents(std::string const& value);
  bool _set_process_perf_data(bool value);
  bool _set_retain_nonstatus_information(bool value);
  bool _set_retain_status_information(bool value);
  bool _set_retry_interval(uint32_t value);
  bool _set_recovery_notification_delay(uint32_t value);
  bool _set_stalking_options(std::string const& value);
  bool _set_statusmap_image(std::string const& value);
  bool _set_timezone(std::string const& value);
  bool _set_vrml_image(std::string const& value);

  opt<int> _acknowledgement_timeout;
  std::string _action_url;
  std::string _address;
  std::string _alias;
  opt<bool> _checks_active;
  opt<bool> _checks_passive;
  std::string _check_command;
  opt<bool> _check_freshness;
  opt<uint32_t> _check_interval;
  std::string _check_period;
  group<set_string> _contactgroups;
  group<set_string> _contacts;
  opt<point_2d> _coords_2d;
  opt<point_3d> _coords_3d;
  map_customvar _customvariables;
  std::string _display_name;
  std::string _event_handler;
  opt<bool> _event_handler_enabled;
  opt<uint32_t> _first_notification_delay;
  opt<bool> _flap_detection_enabled;
  opt<uint32_t> _flap_detection_options;
  opt<uint32_t> _freshness_threshold;
  opt<uint32_t> _high_flap_threshold;
  group<set_string> _hostgroups;
  uint64_t _host_id;
  std::string _host_name;
  std::string _icon_image;
  std::string _icon_image_alt;
  opt<uint32_t> _initial_state;
  opt<uint32_t> _low_flap_threshold;
  opt<uint32_t> _max_check_attempts;
  std::string _notes;
  std::string _notes_url;
  opt<bool> _notifications_enabled;
  opt<uint32_t> _notification_interval;
  opt<uint32_t> _notification_options;
  std::string _notification_period;
  opt<bool> _obsess_over_host;
  group<set_string> _parents;
  opt<bool> _process_perf_data;
  opt<bool> _retain_nonstatus_information;
  opt<bool> _retain_status_information;
  opt<uint32_t> _retry_interval;
  opt<uint32_t> _recovery_notification_delay;
  static std::unordered_map<std::string, setter_func> const _setters;
  opt<uint32_t> _stalking_options;
  std::string _statusmap_image;
  opt<std::string> _timezone;
  std::string _vrml_image;
};

typedef std::shared_ptr<host> host_ptr;
typedef std::list<host> list_host;
typedef std::set<host> set_host;
}  // namespace configuration

CCE_END()

#endif  // !CCE_CONFIGURATION_HOST_HH
