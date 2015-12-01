/*
** Copyright 2011-2013,2015 Centreon
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

#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <syslog.h>
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/defines.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::config;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
parser::parser() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
parser::parser(parser const& other) {
  (void)other;
}

/**
 *  Destructor.
 */
parser::~parser() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
parser& parser::operator=(parser const& other) {
  (void)other;
  return (*this);
}

/**
 *  Parse a configuration file.
 *
 *  @param[in]  file File to process.
 *  @param[out] s    Resulting configuration state.
 */
void parser::parse(QString const& file, state& s) {
  // Parse XML document.
  QFile f(file);
  if (!f.open(QIODevice::ReadOnly))
    throw (exceptions::msg() << "config parser: could not open file '"
             << file << "': " << f.errorString());
  QDomDocument d;
  {
    QString msg;
    int line;
    int col;
    if (!d.setContent(&f, false, &msg, &line, &col))
      throw (exceptions::msg() << "config parser: could not parse " \
                  "file '" << file << "': " << msg << " (line " << line
               << ", column " << col << ")");
  }

  // Clear state.
  s.clear();

  // Browse first-level elements.
  QDomElement root(d.documentElement());
  QDomNodeList level1(root.childNodes());
  for (int i = 0, len = level1.size(); i < len; ++i) {
    QDomElement elem(level1.item(i).toElement());
    // Process only element nodes.
    if (!elem.isNull()) {
      QString name(elem.tagName());
      if (name == "broker_id") {
	s.broker_id(elem.text().toUInt());
      }
      else if (name == "broker_name") {
	s.broker_name(elem.text().toStdString());
      }
      else if ((name == "poller_id")
	  || (name == "instance")) {
        s.poller_id(elem.text().toUInt());
      }
      else if ((name == "poller_name")
	       || (name == "instance_name")) {
        s.poller_name(elem.text().toStdString());
      }
      else if (name == "flush_logs") {
        QString val(elem.text());
        s.flush_logs(!((val == "no") || (val == "0")));
      }
      else if (name == "include") {
        QString included_file(elem.text());
        parse(included_file, s);
      }
      else if ((name == "endpoint") || (name == "output")) {
        endpoint out;
        out.read_filters.insert("all");
        out.write_filters.insert("all");
        _parse_endpoint(elem, out);
        s.endpoints().push_back(out);
      }
      else if (name == "input") {
        endpoint in;
        in.read_filters.insert("all");
        _parse_endpoint(elem, in);
        s.endpoints().push_back(in);
      }
      else if (name == "logger") {
        logger logr;
        _parse_logger(elem, logr);
        s.loggers().push_back(logr);
      }
      else if (name == "log_thread_id") {
        QString val(elem.text());
        s.log_thread_id((val == "yes") || val.toInt());
      }
      else if (name == "log_timestamp") {
        QString val(elem.text());
        s.log_timestamp((val == "yes") || val.toInt());
      }
      else if (name == "log_human_readable_timestamp") {
        QString val(elem.text());
        s.log_human_readable_timestamp((val == "yes") || val.toInt());
      }
      else if (name == "event_queue_max_size") {
        unsigned int val(elem.text().toUInt());
        s.event_queue_max_size(val);
      }
      else if (name == "cache_directory")
        s.cache_directory(elem.text().toStdString());
      else if (name == "command_file")
        s.command_file(elem.text().toStdString());
      else if (name == "command_protocol")
        s.command_protocol(elem.text().toStdString());
      else if (name == "module")
        s.module_list().push_back(elem.text().toStdString());
      else if (name == "module_directory")
        s.module_directory(elem.text().toStdString());
      else {
        QDomDocument subdoc;
        subdoc.appendChild(subdoc.importNode(elem, true));
        s.params()[name.toStdString()] = subdoc.toString().toStdString();
      }
    }
  }

  return ;
}

/**
 *  Parse a boolean value.
 *
 *  @param[in] value String representation of the boolean.
 */
bool parser::parse_boolean(QString const& value) {
  bool conversion_ok;
  return (!value.compare("yes", Qt::CaseInsensitive)
          || !value.compare("enable", Qt::CaseInsensitive)
          || !value.compare("enabled", Qt::CaseInsensitive)
          || !value.compare("true", Qt::CaseInsensitive)
          || (value.toUInt(&conversion_ok) && conversion_ok));
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Parse the configuration of an endpoint.
 *
 *  @param[in]  elem XML element that have the endpoint configuration.
 *  @param[out] e    Element object.
 */
void parser::_parse_endpoint(QDomElement& elem, endpoint& e) {
  e.cfg = elem;
  QDomNodeList nlist(elem.childNodes());
  for (int i = 0, len = nlist.size(); i < len; ++i) {
    QDomElement entry(nlist.item(i).toElement());
    if (!entry.isNull()) {
      QString name(entry.tagName());
      if (name == "buffering_timeout")
        e.buffering_timeout
          = static_cast<time_t>(entry.text().toUInt());
      else if ((name == "failover") || (name == "secondary_failover"))
        e.failovers.push_back(entry.text().toStdString());
      else if (name == "name")
        e.name = entry.text().toStdString();
      else if (name == "read_timeout")
        e.read_timeout = static_cast<time_t>(entry.text().toInt());
      else if (name == "retry_interval")
        e.retry_interval = static_cast<time_t>(entry.text().toUInt());
      else if (name == "read_filters") {
        e.read_filters.clear();
        QDomNodeList nlist(entry.childNodes());
        for (int i(0), len(nlist.size()); i < len; ++i) {
          QDomElement entry(nlist.item(i).toElement());
          if (!entry.isNull()) {
            QString name(entry.tagName());
            if (name == "category")
              e.read_filters.insert(entry.text().toStdString());
            else if (name == "all")
              e.read_filters.insert("all");
          }
        }
      }
      else if ((name == "filters") || (name == "write_filters")) {
        e.write_filters.clear();
        QDomNodeList nlist(entry.childNodes());
        for (int i(0), len(nlist.size()); i < len; ++i) {
          QDomElement entry(nlist.item(i).toElement());
          if (!entry.isNull()) {
            QString name(entry.tagName());
            if (name == "category")
              e.write_filters.insert(entry.text().toStdString());
            else if (name == "all")
              e.write_filters.insert("all");
          }
        }
      }
      else if (name == "cache")
        e.cache_enabled = parse_boolean(entry.text());
      else if (name == "type")
        e.type = entry.text();
      e.params[name] = entry.text();
    }
  }
  return ;
}

/**
 *  Parse the configuration of a logging object.
 *
 *  @param[in]  elem XML element that have the logger configuration.
 *  @param[out] l    Logger object.
 */
void parser::_parse_logger(QDomElement& elem, logger& l) {
  QDomNodeList nlist(elem.childNodes());
  for (int i = 0, len = nlist.size(); i < len; ++i) {
    QDomElement entry(nlist.item(i).toElement());
    if (!entry.isNull()) {
      QString name(entry.tagName());
      if (name == "config")
        l.config(parse_boolean(entry.text()));
      else if (name == "debug")
        l.debug(parse_boolean(entry.text()));
      else if (name == "error")
        l.error(parse_boolean(entry.text()));
      else if (name == "info")
        l.info(parse_boolean(entry.text()));
      else if (name == "facility") {
        QString val(entry.text());
        if (!val.compare("kern", Qt::CaseInsensitive))
          l.facility(LOG_KERN);
        else if (!val.compare("user", Qt::CaseInsensitive))
          l.facility(LOG_USER);
        else if (!val.compare("mail", Qt::CaseInsensitive))
          l.facility(LOG_MAIL);
        else if (!val.compare("news", Qt::CaseInsensitive))
          l.facility(LOG_NEWS);
        else if (!val.compare("uucp", Qt::CaseInsensitive))
          l.facility(LOG_UUCP);
        else if (!val.compare("daemon", Qt::CaseInsensitive))
          l.facility(LOG_DAEMON);
        else if (!val.compare("auth", Qt::CaseInsensitive))
          l.facility(LOG_AUTH);
        else if (!val.compare("cron", Qt::CaseInsensitive))
          l.facility(LOG_CRON);
        else if (!val.compare("lpr", Qt::CaseInsensitive))
          l.facility(LOG_LPR);
        else if (!val.compare("local0", Qt::CaseInsensitive))
          l.facility(LOG_LOCAL0);
        else if (!val.compare("local1", Qt::CaseInsensitive))
          l.facility(LOG_LOCAL1);
        else if (!val.compare("local2", Qt::CaseInsensitive))
          l.facility(LOG_LOCAL2);
        else if (!val.compare("local3", Qt::CaseInsensitive))
          l.facility(LOG_LOCAL3);
        else if (!val.compare("local4", Qt::CaseInsensitive))
          l.facility(LOG_LOCAL4);
        else if (!val.compare("local5", Qt::CaseInsensitive))
          l.facility(LOG_LOCAL5);
        else if (!val.compare("local6", Qt::CaseInsensitive))
          l.facility(LOG_LOCAL6);
        else if (!val.compare("local7", Qt::CaseInsensitive))
          l.facility(LOG_LOCAL7);
        else
          l.facility(val.toUInt());
      }
      else if (name == "level") {
        QString val_str(entry.text());
        int val(val_str.toInt());
        if ((val == 3) || (val_str == "high"))
          l.level(com::centreon::broker::logging::low);
        else if ((val == 2) || (val_str == "medium"))
          l.level(com::centreon::broker::logging::medium);
        else if ((val == 1) || (val_str == "low"))
          l.level(com::centreon::broker::logging::high);
        else
          l.level(com::centreon::broker::logging::none);
      }
      else if (name == "max_size")
        l.max_size(entry.text().toUInt());
      else if (name == "name")
        l.name(entry.text());
      else if (name == "type") {
        QString val(entry.text());
        if (val == "file")
          l.type(logger::file);
        else if (val == "monitoring")
          l.type(logger::monitoring);
        else if (val == "standard")
          l.type(logger::standard);
        else if (val == "syslog")
          l.type(logger::syslog);
        else
          throw (exceptions::msg()
                 << "config parser: unknown logger type '"
                 << val << "'");
      }
    }
  }
  return ;
}
