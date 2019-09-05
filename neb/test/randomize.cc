/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */

#  include <cstdio>
#  include <cstdlib>
#  include <cstring>
#  include <list>
#  include <vector>
#  include "com/centreon/broker/exceptions/msg.hh"
#  include "com/centreon/broker/io/events.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/mapping/source.hh"
#  include "com/centreon/broker/mapping/property.hh"
#  include "com/centreon/broker/neb/events.hh"
#  include "com/centreon/broker/neb/internal.hh"
#  include "randomize.hh"

static std::list<char*> generated;

/**
 *  Randomize an object.
 *
 *  @param[out]    t        Base object.
 *  @param[out]    values   Generated values.
 */
namespace com {
namespace centreon {
namespace broker {
void randomize(io::data& t, std::vector<randval>* values) {
  using namespace com::centreon::broker;
  io::event_info const* info(io::events::instance().get_event_info(t.type()));
  if (!info)
    throw(exceptions::msg() << "cannot find mapping for type " << t.type());
  for (mapping::entry const* current_entry(info->get_mapping());
       !current_entry->is_null(); ++current_entry) {
    randval r;
    switch (current_entry->get_type()) {
      case mapping::source::BOOL: {
        r.b = ((rand() % 2) ? true : false);
        current_entry->set_bool(t, r.b);
      } break;
      case mapping::source::DOUBLE: {
        r.d = rand() + (rand() / 100000.0);
        current_entry->set_double(t, r.d);
      } break;
      case mapping::source::INT: {
        r.i = rand();
        current_entry->set_int(t, r.i);
      } break;
      case mapping::source::SHORT: {
        r.s = rand();
        current_entry->set_short(t, r.s);
      } break;
      case mapping::source::STRING: {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "%d", rand());
        r.S = new char[strlen(buffer) + 1];
        generated.push_back(r.S);
        strcpy(r.S, buffer);
        current_entry->set_string(t, r.S);
      } break;
      case mapping::source::TIME: {
        r.t = rand();
        current_entry->set_time(t, r.t);
      } break;
      case mapping::source::UINT: {
        r.u = rand();
        current_entry->set_uint(t, r.u);
      } break;
    }
    if (values)
      values->push_back(r);
  }
  return;
}

/**
 *  Initialize randomization engine.
 */
void randomize_init() {
  io::events::load();
  io::events& e(io::events::instance());
  e.register_category("neb", io::events::neb);
  e.register_event(
      io::events::neb, neb::de_acknowledgement,
      io::event_info("acknowledgement", &neb::acknowledgement::operations,
                     neb::acknowledgement::entries));
  e.register_event(
      io::events::neb, neb::de_custom_variable,
      io::event_info("custom_variable", &neb::custom_variable::operations,
                     neb::custom_variable::entries));
  e.register_event(io::events::neb, neb::de_custom_variable_status,
                   io::event_info("custom_variable_status",
                                  &neb::custom_variable_status::operations,
                                  neb::custom_variable_status::entries));
  e.register_event(io::events::neb, neb::de_downtime,
                   io::event_info("downtime", &neb::downtime::operations,
                                  neb::downtime::entries));
  e.register_event(
      io::events::neb, neb::de_event_handler,
      io::event_info("event_handler", &neb::event_handler::operations,
                     neb::event_handler::entries));
  e.register_event(
      io::events::neb, neb::de_flapping_status,
      io::event_info("flapping_status", &neb::flapping_status::operations,
                     neb::flapping_status::entries));
  e.register_event(io::events::neb, neb::de_host_check,
                   io::event_info("host_check", &neb::host_check::operations,
                                  neb::host_check::entries));
  e.register_event(
      io::events::neb, neb::de_host_dependency,
      io::event_info("host_dependency", &neb::host_dependency::operations,
                     neb::host_dependency::entries));
  e.register_event(
      io::events::neb, neb::de_host,
      io::event_info("host", &neb::host::operations, neb::host::entries));
  e.register_event(io::events::neb, neb::de_host_parent,
                   io::event_info("host_parent", &neb::host_parent::operations,
                                  neb::host_parent::entries));
  e.register_event(io::events::neb, neb::de_host_status,
                   io::event_info("host_status", &neb::host_status::operations,
                                  neb::host_status::entries));
  e.register_event(io::events::neb, neb::de_instance,
                   io::event_info("instance", &neb::instance::operations,
                                  neb::instance::entries));
  e.register_event(
      io::events::neb, neb::de_instance_status,
      io::event_info("instance_status", &neb::instance_status::operations,
                     neb::instance_status::entries));
  e.register_event(io::events::neb, neb::de_log_entry,
                   io::event_info("log_entry", &neb::log_entry::operations,
                                  neb::log_entry::entries));
  e.register_event(
      io::events::neb, neb::de_module,
      io::event_info("module", &neb::module::operations, neb::module::entries));
  e.register_event(
      io::events::neb, neb::de_service_check,
      io::event_info("service_check", &neb::service_check::operations,
                     neb::service_check::entries));
  e.register_event(
      io::events::neb, neb::de_service_dependency,
      io::event_info("service_dependency", &neb::service_dependency::operations,
                     neb::service_dependency::entries));
  e.register_event(io::events::neb, neb::de_service,
                   io::event_info("service", &neb::service::operations,
                                  neb::service::entries));
  e.register_event(
      io::events::neb, neb::de_service_status,
      io::event_info("service_status", &neb::service_status::operations,
                     neb::service_status::entries));
}

/**
 *  Delete memory used for generation.
 */
void randomize_cleanup() {
  for (std::list<char*>::iterator it(generated.begin()), end(generated.end());
       it != end; ++it)
    delete[] * it;
  generated.clear();
  io::events::unload();
}

} // Namespace broker
} // Namespace centreon
} // Namespace com
