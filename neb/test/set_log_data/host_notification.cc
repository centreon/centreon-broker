/*
** Copyright 2014 Centreon
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

#include <cstdlib>
#include <iostream>
#include <list>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/neb/log_entry.hh"
#include "com/centreon/broker/neb/set_log_data.hh"

using namespace com::centreon::broker;

/**
 *  Check that a host notification log is properly parsed.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  try {
    // Host notification logs.
    struct {
      char const* str;
      struct {
        char const* host_name;
        short msg_type;
        char const* notification_contact;
        char const* notification_cmd;
        char const* output;
        short status;
      } expected;
    } const notification_logs[] = {
      // #1.
      {
        "HOST NOTIFICATION: root;myserver;DOWN;host-notify-by-email;PING CRITICAL - Packet loss = 100%",
        { "myserver", 3, "root", "host-notify-by-email", "PING CRITICAL - Packet loss = 100%", 1 }
      },
      // #2.
      {
        "HOST NOTIFICATION: wheel;centreon.com;CUSTOM (UP);host-notify-by-phone;PING ok - Packet loss = 0%, RTA = 0.80 ms;root;Some comment",
        { "centreon.com", 3, "wheel", "host-notify-by-phone", "PING ok - Packet loss = 0%, RTA = 0.80 ms", 0 }
      },
      // #3.
      {
        "HOST NOTIFICATION: merethis;merethis.com;ACKNOWLEDGEMENT (UNREACHABLE);host-notify-by-pager;merethis.com is unreachable;nobody;wrote anything",
        { "merethis.com", 3, "merethis", "host-notify-by-pager", "merethis.com is unreachable", 2 }
      },
      // #4.
      {
        "HOST NOTIFICATION: centreon;192.168.1.1;FLAPPINGSTART (DOWN);host-notify-by-whatever;Checks /tmp and /var at 10% and 5%, and / at 100MB and 50MB",
        { "192.168.1.1", 3, "centreon", "host-notify-by-whatever", "Checks /tmp and /var at 10% and 5%, and / at 100MB and 50MB", 1 }
      },
      // #5.
      {
        "HOST NOTIFICATION: admin;NS1;FLAPPINGSTOP (UP);host-notify-by-xmpp;Warning if not two processes with command name portsentry.",
        { "NS1", 3, "admin", "host-notify-by-xmpp", "Warning if not two processes with command name portsentry.", 0 }
      },
      // #6.
      {
        "HOST NOTIFICATION: foo;google.com;FLAPPINGDISABLED (UNREACHABLE);host-notify-by-mail;Alert if CPU of any processes over 10% or 20%",
        { "google.com", 3, "foo", "host-notify-by-mail", "Alert if CPU of any processes over 10% or 20%", 2 }
      },
      // #7.
      {
        "HOST NOTIFICATION: bar;127.0.0.1;DOWNTIMESTART (DOWN);host-notify-by-popup;Dummy check",
        { "127.0.0.1", 3, "bar", "host-notify-by-popup", "Dummy check", 1 }
      },
      // #8.
      {
        "HOST NOTIFICATION: baz;myverysecretserver;DOWNTIMEEND (UP);host-notify-by-xpl;08:46:26 up 15 min,  3 users,  load average: 0,10, 0,41, 0,51",
        { "myverysecretserver", 3, "baz", "host-notify-by-xpl", "08:46:26 up 15 min,  3 users,  load average: 0,10, 0,41, 0,51", 0 }
      },
      // #9.
      {
        "HOST NOTIFICATION: QUX;SomeRandomServer;DOWNTIMECANCELLED (UNREACHABLE);host-notify-by-sms;Could not reach server",
        { "SomeRandomServer", 3, "QUX", "host-notify-by-sms", "Could not reach server", 2 }
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
               << "invalid host notification at iteration " << i
               << ": got (host name '" << it->host_name
               << "', message type " << it->msg_type
               << ", notification contact '" << it->notification_contact
               << "', notification command '" << it->notification_cmd
               << "', output '" << it->output << "', state "
               << it->status << "), expected ('"
               << notification_logs[i].expected.host_name << "', "
               << notification_logs[i].expected.msg_type << ", '"
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
