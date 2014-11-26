/*
** Copyright 2011-2014 Merethis
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

#ifndef CCB_NOTIFICATION_MACRO_GETTERS_HH
#  define CCB_NOTIFICATION_MACRO_GETTERS_HH

#  include <QString>
#  include <QHash>
#  include <QList>
#  include <string>
#  include <sstream>
#  include <iomanip>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"
#  include "com/centreon/broker/notification/state.hh"
#  include "com/centreon/broker/notification/objects/contact.hh"
#  include "com/centreon/broker/notification/node_cache.hh"
#  include "com/centreon/broker/notification/utilities/get_datetime_string.hh"

CCB_BEGIN()

namespace        notification {

  /**
   *  Set the precision of a stringstream.
   *
   *  @tparam precision  The precision.
   *
   *  @param[out] ost    The stringstream to modify.
   *
   *  @return  The value of the macro.
   */
  template <int precision>
  void set_precision(std::ostringstream& ost) {
     ost << std::fixed << std::setprecision(precision);
  }

  /**
   *  @brief Does not set the precision of a stringstream.
   *
   *  Specialization of set_precision for precision = 0;
   *
   *  @param[out] ost    The stringstream to modify.
   *
   *  @return  The value of the macro.
   */
  template <> void inline set_precision<0>(std::ostringstream& ost) {}

  /**
   *  Write a type into string.
   *
   *  @tparam T           The type.
   *  @tparam precision   The precision one wants, 0 for none.
   *
   *  @param[in] value    The value.
   *
   *  @return  The value of the macro.
   */
  template <typename T, int precision>
  std::string to_string(T const& value) {
    std::ostringstream ost;
    set_precision<precision>(ost);
    ost << value;
    return (ost.str());
  }

  /**
   *  @brief Write a type into a string.
   *
   *  Specialization for std::string and 0 precision.
   *
   *  @param[in] value  The value of the string.
   *
   *  @return  The value of the macro.
   */
  template <> std::string inline to_string <std::string, 0>(
                                            std::string const& value) {
    return (value);
  }

  /**
   *  @brief Write a type into a string.
   *
   *  Specialization for QString and 0 precision.
   *
   *  @param[in] value  The value of the string.
   *
   *  @return  The value of the macro.
   */
  template <> std::string inline to_string <QString, 0>(
                                            QString const& value) {
    return (value.toStdString());
  }

  /**
   *  Get the member of a host as a string.
   *
   *  @tparam T         The type of the host object.
   *  @tparam U         The type of the member.
   *  @tparam Member    The member.
   *  @tparam precision The precision of the string, 0 for none.
   *
   *  @param[in] id     The id of the host.
   *  @param[in] st     The state of the conf.
   *  @param[in] cache  A node cache.
   *
   *  @return  The value of the macro.
   */
  template <typename T, typename U, U (T::* member), int precision>
    std::string get_host_member_as_string(
                  objects::node_id id,
                  state const& st,
                  node_cache const& cache) {
    return (to_string<U, precision>(
              cache.get_host(id.get_host_id()).get_node().*member));
  }

  /**
   *  Get the member of a service as a string.
   *
   *  @tparam T         The type of the service object.
   *  @tparam U         The type of the member.
   *  @tparam Member    The member.
   *  @tparam precision The precision of the string, 0 for none.
   *
   *  @param[in] id     The id of the service.
   *  @param[in] st     The state of the conf.
   *  @param[in] cache  A node cache.
   *
   *  @return  The value of the macro.
   */
  template <typename T, typename U, U (T::* member), int precision>
    std::string get_service_member_as_string(
                  objects::node_id id,
                  state const& st,
                  node_cache const& cache) {
    return (to_string<U, precision>(
              cache.get_service(id.get_service_id()).get_node().*member));
  }

  /**
   *  Get the member of a host status as a string.
   *
   *  @tparam T         The type of the host status object.
   *  @tparam U         The type of the member.
   *  @tparam Member    The member.
   *  @tparam precision The precision of the string, 0 for none.
   *
   *  @param[in] id     The id of the host.
   *  @param[in] st     The state of the conf.
   *  @param[in] cache  A node cache.
   *
   *  @return  The value of the macro.
   */
  template <typename T, typename U, U (T::* member), int precision>
    std::string get_host_status_member_as_string(
                  objects::node_id id,
                  state const& st,
                  node_cache const& cache) {
      return (to_string<U, precision>(
                cache.get_host(id.get_host_id()).get_status().*member));
  }

  /**
   *  Get the member of a service status as a string.
   *
   *  @tparam T         The type of the service status object.
   *  @tparam U         The type of the member.
   *  @tparam Member    The member.
   *  @tparam precision The precision of the string, 0 for none.
   *
   *  @param[in] id     The id of the host.
   *  @param[in] st     The state of the conf.
   *  @param[in] cache  A node cache.
   *
   *  @return  The value of the macro.
   */
  template <typename T, typename U, U (T::* member), int precision>
    std::string get_service_status_member_as_string(
                  objects::node_id id,
                  state const& st,
                  node_cache const& cache) {
      return (to_string<U, precision>(
                cache.get_service(id.get_service_id()).get_status().*member));
  }

  /**
   *  Get the member of a previous host status as a string.
   *
   *  @tparam T         The type of the previous host status object.
   *  @tparam U         The type of the member.
   *  @tparam Member    The member.
   *  @tparam precision The precision of the string, 0 for none.
   *
   *  @param[in] id     The id of the host.
   *  @param[in] st     The state of the conf.
   *  @param[in] cache  A node cache.
   *
   *  @return  The value of the macro.
   */
  template <typename T, typename U, U (T::* member), int precision>
    std::string get_host_prevstatus_member_as_string(
                  objects::node_id id,
                  state const& st,
                  node_cache const& cache) {
      return (to_string<U, precision>(
                cache.get_host(id.get_host_id()).get_prev_status().*member));
  }

  /**
   *  Get the member of a previous service status as a string.
   *
   *  @tparam T         The type of the previous service status object.
   *  @tparam U         The type of the member.
   *  @tparam Member    The member.
   *  @tparam precision The precision of the string, 0 for none.
   *
   *  @param[in] id     The id of the service.
   *  @param[in] st     The state of the conf.
   *  @param[in] cache  A node cache.
   *
   *  @return  The value of the macro.
   */
  template <typename T, typename U, U (T::* member), int precision>
    std::string get_service_prevstatus_member_as_string(
                  objects::node_id id,
                  state const& st,
                  node_cache const& cache) {
      return (to_string<U, precision>(
                cache.get_service(id.get_service_id()).get_prev_status().*member));
  }

  /**
   *  Get the groups of a host.
   *
   *  @tparam get_all  True if we want all the groups, false if only one.
   *
   *  @param[in] id     The id of the host.
   *  @param[in] st     The state of the conf.
   *  @param[in] cache  A node cache.
   *
   *  @return  The value of the macro.
   */
  template <bool get_all>
  std::string get_host_groups(
                 objects::node_id id,
                 state const& st,
                 node_cache const& cache) {}

  template <> std::string get_host_groups<true>(
                            objects::node_id id,
                            state const& st,
                            node_cache const& cache);

  template <> std::string get_host_groups<false>(
                            objects::node_id id,
                            state const& st,
                            node_cache const& cache);

  /**
   *  Get the groups of a service.
   *
   *  @tparam get_all  True if we want all the groups, false if only one.
   *
   *  @param[in] id     The id of the service.
   *  @param[in] st     The state of the conf.
   *  @param[in] cache  A node cache.
   *
   *  @return  The value of the macro.
   */
  template <bool get_all>
  std::string get_service_groups(
                 objects::node_id id,
                 state const& st,
                 node_cache const& cache) {}

  template <> std::string get_service_groups<true>(
                            objects::node_id id,
                            state const& st,
                            node_cache const& cache);

  template <> std::string get_service_groups<false>(
                            objects::node_id id,
                            state const& st,
                            node_cache const& cache);

  /**
   *  Get the output of a host.
   *
   *  @tparam get_long_output  True if we want the long output, false if we want the short one.
   *
   *  @param[in] id            The id of the host.
   *  @param[in] st            The state of the conf.
   *  @param[in] cache         A node cache.
   *
   *  @return  The value of the macro.
   */
  template <bool get_long_output>
  std::string get_host_output(
                objects::node_id id,
                state const& st,
                node_cache const& cache) {}

  template <> std::string get_host_output<false>(
                            objects::node_id id,
                            state const& st,
                            node_cache const& cache);

  template <> std::string get_host_output<true>(
                            objects::node_id id,
                            state const& st,
                            node_cache const& cache);

  /**
   *  Get the output of a service.
   *
   *  @tparam get_long_output  True if we want the long output, false if we want the short one.
   *
   *  @param[in] id            The id of the service.
   *  @param[in] st            The state of the conf.
   *  @param[in] cache         A node cache.
   *
   *  @return  The value of the macro.
   */
  template <bool get_long_output>
  std::string get_service_output(
                objects::node_id id,
                state const& st,
                node_cache const& cache) {}

  template <> std::string get_service_output<false>(
                            objects::node_id id,
                            state const& st,
                            node_cache const& cache);

  template <> std::string get_service_output<true>(
                            objects::node_id id,
                            state const& st,
                            node_cache const& cache);

  /**
   *  Get the date or time string.
   *
   *  @tparam date_type        The type of the date.
   *
   *  @param[in] id            The id of the host.
   *  @param[in] st            The state of the conf.
   *  @param[in] cache         A node cache.
   *
   *  @return  The value of the macro.
   */
  template <int date_type>
  std::string get_datetime_string(
                objects::node_id id,
                state const& st,
                node_cache const& cache) {
    (void)id; (void)cache;
    utilities::get_datetime_string(
                 ::time(NULL),
                 48,
                 date_type,
                 st.get_date_format());
  }

  /**
   *  Get the total number of services associated with the host.
   *
   *  @tparam service_state    The state of the services we want to count: -1 for all.
   *
   *  @param[in] id            The id of the host.
   *  @param[in] st            The state of the conf.
   *  @param[in] cache         A node cache.
   *
   *  @return  The value of the macro.
   */
  template <int service_state>
  std::string get_total_host_services(
                objects::node_id id,
                state const& st,
                node_cache const& cache) {
    QList<objects::node::ptr> services = st.get_all_services_of_host(id);
    if (service_state != -1) {
      size_t count = 0;
      for (QList<objects::node::ptr>::iterator it(services.begin()),
                                               end(services.end());
           it != end;++it)
        if ((*it)->get_hard_state() == objects::node_state(service_state))
          ++count;
      return (to_string<size_t, 0>(count));
    }
    else
      return (to_string<int, 0>(services.count()));
  }

  /**
   *  Get the total number of hosts with a given state.
   *
   *  @tparam host_state       The state of the hosts we want to count. -1 for all that are not up.
   *
   *  @param[in] id            The id of the host.
   *  @param[in] st            The state of the conf.
   *  @param[in] cache         A node cache.
   *
   *  @return  The value of the macro.
   */
  template <int host_state>
  std::string get_total_hosts(
                objects::node_id id,
                state const& st,
                node_cache const& cache) {
    return (to_string<int, 0>(st.get_all_hosts_in_state(host_state).size()));
  }

  /**
   *  Get the total number of services with a given state.
   *
   *  @tparam service_state       The state of the services we want to count. -1 for all that are not ok.
   *
   *  @param[in] id            The id of the service.
   *  @param[in] st            The state of the conf.
   *  @param[in] cache         A node cache.
   *
   *  @return  The value of the macro.
   */
  template <int service_state>
  std::string get_total_services(
                objects::node_id id,
                state const& st,
                node_cache const& cache) {
    return (to_string<int, 0>(st.get_all_services_in_state(service_state).size()));
  }

  /**
   *  Get the total number of hosts unhandled with a given state.
   *
   *  @tparam host_state       The state of the hosts we want to count. -1 for all that are not up.
   *
   *  @param[in] id            The id of the host.
   *  @param[in] st            The state of the conf.
   *  @param[in] cache         A node cache.
   *
   *  @return  The value of the macro.
   */
  template <int host_state>
  std::string get_total_hosts_unhandled(
                objects::node_id id,
                state const& st,
                node_cache const& cache) {
    size_t count = 0;
    QList<objects::node::ptr> list = st.get_all_hosts_in_state(host_state);
    for (QList<objects::node::ptr>::const_iterator it = list.begin(),
                                                   end = list.end();
         it != end;
         ++it) {
      if (!st.has_node_been_acknowledged((*it)->get_node_id()) &&
            !st.is_node_in_downtime((*it)->get_node_id()))
        ++count;
    }
    return (to_string<size_t, 0>(count));
  }

  /**
   *  Get the total number of services unhandled with a given state.
   *
   *  @tparam service_state       The state of the services we want to count. -1 for all that are not ok.
   *
   *  @param[in] id            The id of the service.
   *  @param[in] st            The state of the conf.
   *  @param[in] cache         A node cache.
   *
   *  @return  The value of the macro.
   */
  template <int service_state>
  std::string get_total_services_unhandled(
                objects::node_id id,
                state const& st,
                node_cache const& cache) {
    size_t count = 0;
    QList<objects::node::ptr> list = st.get_all_services_in_state(service_state);
    for (QList<objects::node::ptr>::const_iterator it = list.begin(),
                                                   end = list.end();
         it != end;
         ++it) {
      if (!st.has_node_been_acknowledged((*it)->get_node_id()) &&
            !st.is_node_in_downtime((*it)->get_node_id()))
        ++count;
    }
    return (to_string<size_t, 0>(count));
  }

  /**
   *  Get the group alias of a node.
   *
   *  @tparam is_host          Is the node an host ?
   *
   *  @param[in] id            The id of the node.
   *  @param[in] st            The state of the conf.
   *  @param[in] cache         A node cache.
   *
   *  @return  The value of the macro.
   */
  /*template <bool is_host>
  std::string get_group_alias(
                objects::node_id id,
                state const& st,
                node_cache const& cache) {
    /*objects::node
    if (is_host) {

    }
    else {

    }
  }*/

  // Static, non template getters.
  std::string   get_host_state(
                  objects::node_id id,
                  state const& st,
                  node_cache const& cache);

  std::string  get_service_state(
                 objects::node_id id,
                 state const& st,
                 node_cache const& cache);

  std::string   get_last_host_state(
                  objects::node_id id,
                  state const& st,
                  node_cache const& cache);

  std::string   get_last_service_state(
                  objects::node_id id,
                  state const& st,
                  node_cache const& cache);

  std::string   get_host_state_type(
                  objects::node_id id,
                  state const& st,
                  node_cache const& cache);

  std::string  get_service_state_type(
                 objects::node_id id,
                 state const& st,
                 node_cache const& cache);

  std::string   null_getter(objects::node_id id,
                            state const& st,
                            node_cache const& cache);

  std::string   get_host_duration(
                  objects::node_id id,
                  state const& st,
                  node_cache const& cache);

  std::string  get_service_duration(
                 objects::node_id id,
                 state const& st,
                 node_cache const& cache);

  std::string   get_host_duration_sec(
                  objects::node_id id,
                  state const& st,
                  node_cache const& cache);

  std::string  get_service_duration_sec(
                 objects::node_id id,
                 state const& st,
                 node_cache const& cache);

  std::string  get_timet_string(
                 objects::node_id id,
                 state const& st,
                 node_cache const& cache);
}

CCB_END()

#endif // !CCB_NOTIFICATION_MACRO_GETTERS_HH
