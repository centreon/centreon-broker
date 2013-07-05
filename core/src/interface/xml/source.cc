/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#include <cstdlib>
#include "events/events.hh"
#include "exceptions/basic.hh"
#include "interface/xml/internal.hh"
#include "interface/xml/source.hh"

using namespace interface::xml;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Fetch data from the stream and give it to the XML parser.
 */
void source::fetchData() {
  QByteArray data;
  data.resize(2048);
  unsigned int rb(_stream->receive(data.data(), data.size()));
  data.resize(rb);
  setData(data);
  return ;
}

/**
 *  Extract data from the XML attributes.
 *
 *  @param[in] attrs Attributes of the node.
 */
template <typename T>
void source::handle_event(QXmlAttributes const& attrs) {
  std::auto_ptr<T> t(new T);
  for (typename umap<std::string, getter_setter<T> >::iterator
         it = xml_mapped_type<T>::map.begin(),
         end = xml_mapped_type<T>::map.end();
       it != end;
       ++it) {
    (*(it->second.setter))(*t,
      *(it->second.member),
      attrs.value(it->first.c_str()).toStdString().c_str());
  }
  return ;
}

/**
 *  Beginning of XML node.
 *
 *  @param[in] nspace    Namespace.
 *  @param[in] localname Local name.
 *  @param[in] qname     QName.
 *  @param[in] attrs     XML attributes of the node.
 *
 *  @return true on success.
 */
bool source::startElement(QString const& nspace,
                          QString const& localname,
                          QString const& qname,
                          QXmlAttributes const& attrs) {
  (void)nspace;
  (void)qname;
  if (localname.toStdString() == "acknowledgement")
    handle_event<events::acknowledgement>(attrs);
  else if (localname.toStdString() == "comment")
    handle_event<events::comment>(attrs);
  else if (localname.toStdString() == "custom_variable")
    handle_event<events::custom_variable>(attrs);
  else if (localname.toStdString() == "custom_variable_status")
    handle_event<events::custom_variable_status>(attrs);
  else if (localname.toStdString() == "downtime")
    handle_event<events::downtime>(attrs);
  else if (localname.toStdString() == "event_handler")
    handle_event<events::event_handler>(attrs);
  else if (localname.toStdString() == "host")
    handle_event<events::host>(attrs);
  else if (localname.toStdString() == "host_dependency")
    handle_event<events::host_dependency>(attrs);
  else if (localname.toStdString() == "host_group")
    handle_event<events::host_group>(attrs);
  else if (localname.toStdString() == "host_group_member")
    handle_event<events::host_group_member>(attrs);
  else if (localname.toStdString() == "host_parent")
    handle_event<events::host_parent>(attrs);
  else if (localname.toStdString() == "host_state")
    handle_event<events::host_state>(attrs);
  else if (localname.toStdString() == "host_status")
    handle_event<events::host_status>(attrs);
  else if (localname.toStdString() == "instance")
    handle_event<events::instance>(attrs);
  else if (localname.toStdString() == "instance_status")
    handle_event<events::instance_status>(attrs);
  else if (localname.toStdString() == "log_entry")
    handle_event<events::log_entry>(attrs);
  else if (localname.toStdString() == "module")
    handle_event<events::module>(attrs);
  else if (localname.toStdString() == "notification")
    handle_event<events::notification>(attrs);
  else if (localname.toStdString() == "service")
    handle_event<events::service>(attrs);
  else if (localname.toStdString() == "service_dependency")
    handle_event<events::service_dependency>(attrs);
  else if (localname.toStdString() == "service_group")
    handle_event<events::service_group>(attrs);
  else if (localname.toStdString() == "service_group_member")
    handle_event<events::service_group_member>(attrs);
  else if (localname.toStdString() == "service_state")
    handle_event<events::service_state>(attrs);
  else if (localname.toStdString() == "service_status")
    handle_event<events::service_status>(attrs);
  else
    throw (exceptions::basic() << "unknown XML node: "
                               << localname.toStdString().c_str());

  return (true);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] s Stream on which data will be sent.
 */
source::source(io::stream* s) : _stream(s) {
  _parser.setContentHandler(this);
  _parser.parse(this, true);
}

/**
 *  Destructor.
 */
source::~source() {}

/**
 *  Close the underlying stream.
 */
void source::close() {
  _stream->close();
  return ;
}

/**
 *  @brief Get the next available event.
 *
 *  Extract the next available event on the input stream, NULL if the
 *  stream is closed.
 *
 *  @return Next available event, NULL if stream is closed.
 */
events::event* source::event() {
  // Parse data as long as no event is fully parsed and the source
  // stream is open.
  while (_events.empty() && _parser.parseContinue())
    ;

  // Send resulting event.
  std::auto_ptr<events::event> e;
  if (!_events.empty()) {
    e.reset(_events.front());
    _events.pop();
  }

  return (e.release());
}
