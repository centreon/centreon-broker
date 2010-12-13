/*
**  Copyright 2009-2011 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef INTERFACE_DB_DESTINATION_HH_
# define INTERFACE_DB_DESTINATION_HH_

# include <string>
# include <soci.h>
# include "events/events.h"
# include "interface/destination.h"

namespace          Interface {
  namespace        DB {
    /**
     *  @class destination destination.h "interface/db/destination.hh"
     *  @brief Insert/update events in a database.
     *
     *  Handle insertion of events in their generic form into a database.
     */
    class          destination : public Interface::Destination {
     public:
      enum         DB {
        UNKNOWN = 0,
        MYSQL,
        ORACLE,
        POSTGRESQL
      };

     private:
      static void                    (destination::* processing_table[])(Events::Event const&);
      Events::Acknowledgement        _acknowledgement;
      std::auto_ptr<soci::statement> _acknowledgement_stmt;
      Events::comment                _comment;
      std::auto_ptr<soci::statement> _comment_stmt;
      Events::custom_variable_status _custom_variable_status;
      std::auto_ptr<soci::statement> _custom_variable_status_stmt;
      Events::Downtime               _downtime;
      std::auto_ptr<soci::statement> _downtime_stmt;
      Events::event_handler          _event_handler;
      std::auto_ptr<soci::statement> _event_handler_stmt;
      Events::flapping_status        _flapping_status;
      std::auto_ptr<soci::statement> _flapping_status_stmt;
      Events::Host                   _host;
      std::auto_ptr<soci::statement> _host_stmt;
      Events::HostCheck              _host_check;
      std::auto_ptr<soci::statement> _host_check_stmt;
      Events::HostStatus             _host_status;
      std::auto_ptr<soci::statement> _host_status_stmt;
      Events::Issue                  _issue;
      std::auto_ptr<soci::statement> _issue_stmt;
      Events::notification           _notification;
      std::auto_ptr<soci::statement> _notification_stmt;
      Events::Program                _program;
      std::auto_ptr<soci::statement> _program_stmt;
      Events::ProgramStatus          _program_status;
      std::auto_ptr<soci::statement> _program_status_stmt;
      Events::Service                _service;
      std::auto_ptr<soci::statement> _service_insert_stmt;
      std::auto_ptr<soci::statement> _service_stmt;
      Events::ServiceCheck           _service_check;
      std::auto_ptr<soci::statement> _service_check_stmt;
      Events::ServiceStatus          _service_status;
      std::auto_ptr<soci::statement> _service_status_stmt;
      Events::state                  _state;
      std::auto_ptr<soci::statement> _state_stmt;
      std::auto_ptr<soci::session>   _conn; // Connection object is necessary after statements.
                   destination(destination const& dest);
      destination& operator=(destination const& dest);
      void         _clean_tables(int instance_id);
      template     <typename T>
      void         _insert(T const& t);
      template     <typename T>
      void         _prepare_insert(std::auto_ptr<soci::statement>& st,
                     T& t);
      template     <typename T>
      void         _prepare_update(std::auto_ptr<soci::statement>& st,
                     T& t,
                     std::vector<std::string> const& id);
      void         _process_acknowledgement(Events::Event const& event);
      void         _process_comment(Events::Event const& event);
      void         _process_custom_variable(Events::Event const& event);
      void         _process_custom_variable_status(Events::Event const& event);
      void         _process_downtime(const Events::Event& event);
      void         _process_event_handler(Events::Event const& event);
      void         _process_flapping_status(Events::Event const& event);
      void         _process_host(Events::Event const& event);
      void         _process_host_check(Events::Event const& event);
      void         _process_host_dependency(Events::Event const& event);
      void         _process_host_group(Events::Event const& event);
      void         _process_host_group_member(Events::Event const& event);
      void         _process_host_parent(Events::Event const& event);
      void         _process_host_status(Events::Event const& event);
      void         _process_issue(Events::Event const& event);
      void         _process_issue_parent(Events::Event const& event);
      void         _process_log(Events::Event const& event);
      void         _process_notification(Events::Event const& event);
      void         _process_program(Events::Event const& event);
      void         _process_program_status(Events::Event const& event);
      void         _process_service(Events::Event const& event);
      void         _process_service_check(Events::Event const& event);
      void         _process_service_dependency(Events::Event const& event);
      void         _process_service_group(Events::Event const& event);
      void         _process_service_group_member(Events::Event const& event);
      void         _process_service_status(Events::Event const& event);
      void         _process_state(Events::Event const& event);

     public:
                   destination();
                   ~destination();
      void         Close();
      void         connect(DB db_type,
                     std::string const& db,
                     std::string const& host,
                     std::string const& user,
                     std::string const& pass);
      void         Event(Events::Event* event);
    };
  }
}

#endif /* !INTERFACE_DB_DESTINATION_HH_ */
