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

#include <cstdlib>
#include <iostream>
#include <list>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/neb/log_entry.hh"
#include "com/centreon/broker/neb/set_log_data.hh"

using namespace com::centreon::broker;

/**
 *  Check that a service notification log is properly parsed.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  try {
    // Service notification logs.
    struct {
      char const* str;
      struct {
        char const* host_name;
        char const* service_description;
        short msg_type;
        char const* notification_contact;
        char const* notification_cmd;
        char const* output;
        short status;
      } expected;
    } const notification_logs[] = {
      // #1.
      {
        "SERVICE NOTIFICATION: admin;myserver;myservice;CRITICAL;notify-service-by-email;Connection refused",
        { "myserver", "myservice", 2, "admin", "notify-service-by-email", "Connection refused", 2 }
      },
      // #2.
      {
        "SERVICE NOTIFICATION: wheel;centreon.com;www;CUSTOM (WARNING);notify-service-by-phone;PING ok - Packet loss = 0%, RTA = 0.80 ms;nobody;wrote anything",
        { "centreon.com", "www", 2, "wheel", "notify-service-by-phone", "PING ok - Packet loss = 0%, RTA = 0.80 ms", 1 }
      },
      // #3.
      {
        "SERVICE NOTIFICATION: merethis;merethis.com;apache;ACKNOWLEDGEMENT (OK);notify-service-by-pager;merethis.com is OK;root;Some comment",
        { "merethis.com", "apache", 2, "merethis", "notify-service-by-pager", "merethis.com is OK", 0 }
      },
      // #4.
      {
        "SERVICE NOTIFICATION: centreon;192.168.1.1;/;FLAPPINGSTART (UNKNOWN);notify-service-by-whatever;Checks /tmp and /var at 10% and 5%, and / at 100MB and 50MB",
        { "192.168.1.1", "/", 2, "centreon", "notify-service-by-whatever", "Checks /tmp and /var at 10% and 5%, and / at 100MB and 50MB", 3 }
      },
      // #5.
      {
        "SERVICE NOTIFICATION: admin;NS1;nslookup;FLAPPINGSTOP (CRITICAL);notify-service-by-xmpp;Warning if not two processes with command name portsentry.",
        { "NS1", "nslookup", 2, "admin", "notify-service-by-xmpp", "Warning if not two processes with command name portsentry.", 2 }
      },
      // #6.
      {
        "SERVICE NOTIFICATION: foo;google.com;Web site;FLAPPINGDISABLED (WARNING);notify-service-by-mail;Alert if CPU of any processes over 10% or 20%",
        { "google.com", "Web site", 2, "foo", "notify-service-by-mail", "Alert if CPU of any processes over 10% or 20%", 1 }
      },
      // #7.
      {
        "SERVICE NOTIFICATION: bar;127.0.0.1;localservice;DOWNTIMESTART (OK);notify-service-by-popup;Dummy check",
        { "127.0.0.1", "localservice", 2, "bar", "notify-service-by-popup", "Dummy check", 0 }
      },
      // #8.
      {
        "SERVICE NOTIFICATION: baz;myverysecretserver;myverysecretservice;DOWNTIMEEND (UNKNOWN);notify-service-by-xpl;08:46:26 up 15 min,  3 users,  load average: 0,10, 0,41, 0,51",
        { "myverysecretserver", "myverysecretservice", 2, "baz", "notify-service-by-xpl", "08:46:26 up 15 min,  3 users,  load average: 0,10, 0,41, 0,51", 3 }
      },
      // #9.
      {
        "SERVICE NOTIFICATION: QUX;SomeRandomServer;someRandomService;DOWNTIMECANCELLED (CRITICAL);notify-service-by-sms;Could not reach service",
        { "SomeRandomServer", "someRandomService", 2, "QUX", "notify-service-by-sms", "Could not reach service", 2 }
      }
    };

    // Parse log entries.
    std::list<neb::log_entry> parsed;
    size_t i;
    for (i = 0;
         i < sizeof(notification_logs) / sizeof(*notification_logs);
         ++i) {
      neb::log_entry le;
      neb::set_log_data(le, notification_logs[i].str);
      parsed.push_back(le);
    }

    // Compare parsed and expected.
    i = 0;
    for (std::list<neb::log_entry>::const_iterator
           it(parsed.begin()),
           end(parsed.end());
         it != end;
         ++it, ++i) {
      if ((notification_logs[i].expected.host_name
           != it->host_name)
          || (notification_logs[i].expected.service_description
              != it->service_description)
          || (notification_logs[i].expected.msg_type
              != it->msg_type)
          || (notification_logs[i].expected.notification_contact
              != it->notification_contact)
          || (notification_logs[i].expected.notification_cmd
              != it->notification_cmd)
          || (notification_logs[i].expected.output
              != it->output)
          || (notification_logs[i].expected.status
              != it->status))
        throw (exceptions::msg()
               << "invalid service notification at iteration " << i
               << ": got (host name '" << it->host_name
               << "', service description '" << it->service_description
               << "', message type " << it->msg_type
               << ", notification contact '" << it->notification_contact
               << "', notification command '" << it->notification_cmd
               << "', output '" << it->output << "', state "
               << it->status << "), expected ('"
               << notification_logs[i].expected.host_name << "', '"
               << notification_logs[i].expected.service_description
               << "', " << notification_logs[i].expected.msg_type
               << ", '"
               << notification_logs[i].expected.notification_contact
               << "', '"
               << notification_logs[i].expected.notification_cmd
               << "', '" << notification_logs[i].expected.output
               << "', " << notification_logs[i].expected.status << ")");
    }

    // Success.
    error = false;
  }
  catch (std::exception const& e) {
    std::cerr << e.what() << "\n";
  }

  // Check that parsing worked properly.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
