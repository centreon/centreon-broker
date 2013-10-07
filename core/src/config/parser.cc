/*
** Copyright 2011-2012 Merethis
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

#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <syslog.h>
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/defines.hh"

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
 *  @param[in] p Object to copy.
 */
parser::parser(parser const& p) {
  (void)p;
}

/**
 *  Destructor.
 */
parser::~parser() {}

/**
 *  Assignment operator.
 *
 *  @param[in] p Object to copy.
 *
 *  @return This object.
 */
parser& parser::operator=(parser const& p) {
  (void)p;
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

  // Check if temporary object is already define.
  bool has_temprorary(false);

  // Check if instance id is already define.
  bool has_instance_id(false);

  // Browse first-level elements.
  QDomElement root(d.documentElement());
  QDomNodeList level1(root.childNodes());
  for (int i = 0, len = level1.size(); i < len; ++i) {
    QDomElement elem(level1.item(i).toElement());
    // Process only element nodes.
    if (!elem.isNull()) {
      QString name(elem.tagName());
      if (name == "instance") {
        s.instance_id(elem.text().toUInt());
        has_instance_id = true;
      }
      else if (name == "instance_name") {
        s.instance_name(elem.text());
      }
      else if (name == "flush_logs") {
        QString val(elem.text());
        s.flush_logs(!((val == "no") || (val == "0")));
      }
      else if (name == "include") {
        QString included_file(elem.text());
        parse(included_file, s);
      }
      else if (name == "input") {
        endpoint in;
        _parse_endpoint(elem, in);
        s.inputs().push_back(in);
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
      else if (name == "event_queue_max_size") {
        unsigned int val(elem.text().toUInt());
        s.event_queue_max_size(val);
      }
      else if (name == "module")
        s.module_list().push_back(elem.text());
      else if (name == "module_directory")
        s.module_directory(elem.text());
      else if (name == "output") {
        endpoint out;
        _parse_endpoint(elem, out);
        s.outputs().push_back(out);
      }
      else if (name == "temporary") {
        if (has_temprorary)
          throw (exceptions::msg() << "config parser: one temporary "
                 "object is already define");
        if (!has_instance_id)
          throw (exceptions::msg()
                 << "config parser: missing instance before temporary");
        _parse_endpoint(elem, s.temporary());
        if (s.temporary().type != "file")
          throw (exceptions::msg()
                 << "config parser: bad temporary type");
        QMap<QString, QString>::iterator
          it(s.temporary().params.find("path"));
        if (it == s.temporary().params.end())
          throw (exceptions::msg()
                 << "config params: bad temporary path");
        it.value() += "-" + QString().setNum(s.instance_id());
        has_temprorary = true;
      }
      else {
        QDomDocument subdoc;
        subdoc.appendChild(subdoc.importNode(elem, true));
        s.params()[name] = subdoc.toString();
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
  QDomNodeList nlist(elem.childNodes());
  for (int i = 0, len = nlist.size(); i < len; ++i) {
    QDomElement entry(nlist.item(i).toElement());
    if (!entry.isNull()) {
      QString name(entry.tagName());
      if (name == "buffering_timeout")
        e.buffering_timeout
          = static_cast<time_t>(entry.text().toUInt());
      else if (name == "failover")
        e.failover = entry.text();
      else if (name == "name")
        e.name = entry.text();
      else if (name == "read_timeout")
        e.read_timeout = static_cast<time_t>(entry.text().toInt());
      else if (name == "retry_interval")
        e.retry_interval = static_cast<time_t>(entry.text().toUInt());
      else if (name == "filters") {
        QDomNodeList nlist(entry.childNodes());
        for (int i(0), len(nlist.size()); i < len; ++i) {
          QDomElement entry(nlist.item(i).toElement());
          if (!entry.isNull()) {
            QString name(entry.tagName());
            if (name == "category") {
              // entry.text();
              // e.filters.insert();
            }
          }
        }
      }
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
