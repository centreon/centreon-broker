/*
**  Copyright 2010 MERETHIS
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

#include <memory>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/XMLString.hpp>
#include "config/globals.hh"
#include "config/parser.hh"
#include "exceptions/basic.hh"
#include "logging/logging.hh"

using namespace config;

/**************************************
*                                     *
*            Local Objects            *
*                                     *
**************************************/

/**
 *  Associations between tag name and its id.
 */
parser::_tag_id const parser::_interface_tag_to_id[] =
{
  {"db", _interface_db},
  {"failover", _interface_failover},
  {"filename", _interface_filename},
  {"host", _interface_host},
  {"name", _interface_name},
  {"net_iface", _interface_net_iface},
  {"password", _interface_password},
  {"port", _interface_port},
  {"protocol", _interface_protocol},
  {"socket", _interface_socket},
  {"type", _interface_type},
  {"user", _interface_user},
#ifdef USE_TLS
  {"ca", _interface_ca},
  {"cert", _interface_cert},
  {"compress", _interface_compress},
  {"key", _interface_key},
  {"tls", _interface_tls},
#endif /* !USE_TLS */
  {NULL, _unknown}
};
parser::_tag_id const parser::_logger_tag_to_id[] =
{
  {"config", _logger_config},
  {"debug", _logger_debug},
  {"error", _logger_error},
  {"info", _logger_info},
  {"level", _logger_level},
  {"name", _logger_name},
  {"type", _logger_type},
  {NULL, _unknown}
};

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Clear all parsing elements.
 */
void parser::_clear()
{
  while (!_current.empty())
    _current.pop();
  _current.push(_unknown);
  _inputs.clear();
  _loggers.clear();
  _outputs.clear();
  return ;
}

/**
 *  Copy internal data members to this object.
 *
 *  @param[in] p Object to copy from.
 */
void parser::_internal_copy(parser const& p)
{
  _current = p._current;
  _loggers = p._loggers;
  return ;
}

/**
 *  Parse properties of main tags.
 */
void parser::_parse_properties(XMLCh const* localname,
                               parser::_tag_id const tag_to_id[])
{
  _current_type prev;
  char* tag;

  tag = xercesc::XMLString::transcode(localname);
  if (!tag)
    throw (exceptions::basic() << "Empty token.");
  prev = _current.top();
  for (unsigned int i = 0; tag_to_id[i].tag; ++i)
    if (!strcmp(tag, tag_to_id[i].tag))
      {
        _current.push(tag_to_id[i].id);
        break ;
      }
  if (_current.top() == prev)
    {
      exceptions::basic e;

      e << "Invalid token \"" << tag << "\"";
      xercesc::XMLString::release(&tag);
      throw (e);
    }
  xercesc::XMLString::release(&tag);
  return ;
}

/**
 *  Receive notification of character data.
 *
 *  @param[in] chars  Characters.
 *  @param[in] length Length of chars.
 */
void parser::characters(XMLCh const* const chars,
#if XERCES_VERSION_MAJOR >= 3
                        XMLSize_t const length)
#else
                        unsigned int const length)
#endif /* XERCES_VERSION_MAJOR */
{
  char* data;

  data = xercesc::XMLString::transcode(chars);
  if (data)
    {
      _data.append(data, length);
      xercesc::XMLString::release(&data);
    }
  return ;
}

/**
 *  @brief End of XML element.
 *
 *  This callback is called by the SAX parser.
 *
 *  @param[in] uri       Unused.
 *  @param[in] localname Unused.
 *  @param[in] qname     Unused.
 */
void parser::endElement(XMLCh const* const uri,
                        XMLCh const* const localname,
                        XMLCh const* const qname)
{
  _current_type prev;

  (void)uri;
  (void)localname;
  (void)qname;
  prev = _current.top();
  _current.pop();
  switch (prev)
    {
     case _conf:
     case _input:
     case _output:
     case _logger:
      break ;
     case _correlation:
      globals::correlation = strtol(_data.c_str(), NULL, 0);
      break ;
     case _correlation_file:
      globals::correlation_file = _data;
      break ;
     case _instance:
      globals::instance = strtol(_data.c_str(), NULL, 0);
      break ;
     case _instance_name:
      globals::instance_name = _data;
      break ;
     case _interface_db:
      if (_current.top() == _input)
        _inputs.back().db = _data;
      else
        _outputs.back().db = _data;
      break ;
     case _interface_failover:
      if (_current.top() == _input)
        _inputs.back().failover_name = _data;
      else
        _outputs.back().failover_name = _data;
      break ;
     case _interface_filename:
      if (_current.top() == _input)
        _inputs.back().filename = _data;
      else
        _outputs.back().filename = _data;
      break ;
     case _interface_host:
      {
        interface* i;

        if (_current.top() == _input)
          i = &_inputs.back();
        else
          i = &_outputs.back();
        i->host = _data;
        if (i->type == interface::ipv4_server)
          i->type = interface::ipv4_client;
        else if (i->type == interface::ipv6_server)
          i->type = interface::ipv6_client;
      }
      break ;
     case _interface_name:
      if (_current.top() == _input)
        _inputs.back().name = _data;
      else
        _outputs.back().name = _data;
      break ;
     case _interface_net_iface:
      if (_current.top() == _input)
        _inputs.back().net_iface = _data;
      else
        _outputs.back().net_iface = _data;
      break ;
     case _interface_password:
      if (_current.top() == _input)
        _inputs.back().password = _data;
      else
        _outputs.back().password = _data;
      break ;
     case _interface_port:
      if (_current.top() == _input)
        _inputs.back().port = strtol(_data.c_str(), NULL, 0);
      else
        _outputs.back().port = strtol(_data.c_str(), NULL, 0);
      break ;
     case _interface_protocol:
      {
        interface::protocol_type p;
        if (!strcasecmp(_data.c_str(), "ndo"))
          p = interface::ndo;
        else if (!strcasecmp(_data.c_str(), "xml"))
          p = interface::xml;
        else
          p = interface::unknown_proto;
        if (_current.top() == _input)
          _inputs.back().protocol = p;
        else
          _outputs.back().protocol = p;
      }
      break ;
     case _interface_socket:
     case _interface_type:
      {
        interface* i;
        char const* t;

        if (_current.top() == _input)
          i = &_inputs.back();
        else
          i = &_outputs.back();
        t = _data.c_str();
        if (!strcasecmp(t, "file"))
          i->type = interface::file;
        else if (!strcasecmp(t, "ipv4"))
          i->type = (i->host.empty() ? interface::ipv4_server
                                     : interface::ipv4_client);
        else if (!strcasecmp(t, "ipv6"))
          i->type = (i->host.empty() ? interface::ipv6_server
                                     : interface::ipv6_client);
#ifdef USE_MYSQL
        else if (!strcasecmp(t, "mysql"))
          i->type = interface::mysql;
#endif /* USE_MYSQL */
#ifdef USE_ORACLE
        else if (!strcasecmp(t, "oracle"))
          i->type = interface::oracle;
#endif /* USE_ORACLE */
#ifdef USE_POSTGRESQL
        else if (!strcasecmp(t, "postgresql"))
          i->type = interface::postgresql;
#endif /* USE_POSTGRESQL */
        else if (!strcasecmp(t, "unix_client"))
          i->type = interface::unix_client;
        else if (!strcasecmp(t, "unix_server"))
          i->type = interface::unix_server;
        else
          throw (exceptions::basic() << "Unknown interface type \""
                                     << t << "\"");
      }
      break ;
     case _interface_user:
      if (_current.top() == _input)
        _inputs.back().user = _data;
      else
        _outputs.back().user = _data;
      break ;
#ifdef USE_TLS
     case _interface_ca:
      if (_current.top() == _input)
        _inputs.back().ca = _data;
      else
        _outputs.back().ca = _data;
      break ;
     case _interface_cert:
      if (_current.top() == _input)
        _inputs.back().cert = _data;
      else
        _outputs.back().cert = _data;
      break ;
     case _interface_compress:
      if (_current.top() == _input)
        _inputs.back().compress = strtol(_data.c_str(), NULL, 0);
      else
        _outputs.back().compress = strtol(_data.c_str(), NULL, 0);
      break ;
     case _interface_key:
      if (_current.top() == _input)
        _inputs.back().key = _data;
      else
        _outputs.back().key = _data;
      break ;
     case _interface_tls:
      if (_current.top() == _input)
        _inputs.back().tls = strtol(_data.c_str(), NULL, 0);
      else
        _outputs.back().tls = strtol(_data.c_str(), NULL, 0);
      break ;
#endif /* USE_TLS */
     case _logger_config:
      _loggers.back().config(strtol(_data.c_str(), NULL, 0));
      break ;
     case _logger_debug:
      _loggers.back().debug(strtol(_data.c_str(), NULL, 0));
      break ;
     case _logger_error:
      _loggers.back().error(strtol(_data.c_str(), NULL, 0));
      break ;
     case _logger_info:
      _loggers.back().info(strtol(_data.c_str(), NULL, 0));
      break ;
     case _logger_level:
      {
        int val;
        logging::level lvl;

        val = strtol(_data.c_str(), NULL, 0);
        if (logging::NONE == val)
          lvl = logging::NONE;
        else if (logging::HIGH == val)
          lvl = logging::HIGH;
        else if (logging::MEDIUM == val)
          lvl = logging::MEDIUM;
        else
          lvl = logging::LOW;
        _loggers.back().level(lvl);
      }
      break ;
     case _logger_name:
      _loggers.back().name(_data);
      break ;
     case _logger_type:
      {
        logger::logger_type t;

        if (!strcasecmp(_data.c_str(), "file"))
          t = logger::file;
        else if (!strcasecmp(_data.c_str(), "standard"))
          t = logger::standard;
        else if (!strcasecmp(_data.c_str(), "syslog"))
          t = logger::syslog;
        else
          throw (exceptions::basic() << "Invalid logger type \""
                                     << _data.c_str() << "\"");
        _loggers.back().type(t);
      }
      break ;
     default:
      throw (exceptions::basic() << "Bug in the config parsing engine.");
    }
  return ;
}

/**
 *  @brief Beginning of an XML element.
 *
 *  This callback is called by the SAX parser.
 *
 *  @param[in] uri       
 *  @param[in] localname Name of the attribute.
 *  @param[in] qname     
 *  @param[in] attrs     
 */
void parser::startElement(XMLCh const* const uri,
                          XMLCh const* const localname,
                          XMLCh const* const qname,
                          xercesc::Attributes const& attrs)
{
  (void)uri;
  (void)qname;
  (void)attrs;
  _data.clear();
  switch (_current.top())
    {
     case _conf:
      {
        char* name;

        name = xercesc::XMLString::transcode(localname);
        if (!name)
          throw (exceptions::basic() << "Empty token.");
        if (!strcmp(name, "correlation"))
          _current.push(_correlation);
        else if (!strcmp(name, "correlation_file"))
          _current.push(_correlation_file);
        else if (!strcmp(name, "input"))
          {
            _current.push(_input);
            _inputs.push_back(interface());
          }
        else if (!strcmp(name, "instance"))
          _current.push(_instance);
        else if (!strcmp(name, "instance_name"))
          _current.push(_instance_name);
        else if (!strcmp(name, "logger"))
          {
            _current.push(_logger);
            _loggers.push_back(logger());
          }
        else if (!strcmp(name, "output"))
          {
            _current.push(_output);
            _outputs.push_back(interface());
          }
        else
          {
            exceptions::basic e;

            e << "Invalid token \"" << name << "\"";
            xercesc::XMLString::release(&name);
            throw (e);
          }
        xercesc::XMLString::release(&name);
      }
      break ;
     case _input:
     case _output:
      _parse_properties(localname, _interface_tag_to_id);
      break ;
     case _logger:
      _parse_properties(localname, _logger_tag_to_id);
      break ;
     case _unknown:
      _current.push(_conf);
      break ;
     default:
      {
        exceptions::basic e;
        char* tag;

        tag = xercesc::XMLString::transcode(localname);
        e << "Invalid tag \"" << (tag ? tag : "unknown tag") << "\"";
        xercesc::XMLString::release(&tag);
        throw (e);
      }
    }
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
parser::parser()
{
  _current.push(_unknown);
}

/**
 *  Copy constructor.
 *
 *  @param[in] p Object to build from.
 */
parser::parser(parser const& p) : xercesc::DefaultHandler()
{
  _internal_copy(p);
}

/**
 *  Destructor.
 */
parser::~parser() {}

/**
 *  Assignment operator overload.
 *
 *  @param[in] p Object to copy from.
 *
 *  @return This object.
 */
parser& parser::operator=(parser const& p)
{
  _internal_copy(p);
  return (*this);
}

/**
 *  Return the list of parsed input objects.
 *
 *  @return List of parsed input objects.
 */
std::list<interface>& parser::inputs()
{
  return (_inputs);
}

/**
 *  Return the list of parsed logger objects.
 *
 *  @return List of parsed logger objects.
 */
std::list<logger>& parser::loggers()
{
  return (_loggers);
}

/**
 *  Return the list of parsed output objects.
 *
 *  @return List of parsed output objects.
 */
std::list<interface>& parser::outputs()
{
  return (_outputs);
}

/**
 *  Parse a configuration file.
 *
 *  @param[in] file XML configuration file.
 */
void parser::parse(std::string const& file)
{
  _clear();
  std::auto_ptr<xercesc::SAX2XMLReader> parser;
  parser.reset(xercesc::XMLReaderFactory::createXMLReader());
  parser->setContentHandler(this);
  parser->setErrorHandler(this);
  try
    {
      parser->parse(file.c_str());
    }
  catch (xercesc::XMLException const& e)
    {
      char* msg;

      msg = xercesc::XMLString::transcode(e.getMessage());
      logging::config << logging::HIGH << "Configuration parsing error on \""
                      << file.c_str() << "\" (line "
                      << (unsigned int)e.getSrcLine() << "): " << msg;
      xercesc::XMLString::release(&msg);
      _clear();
    }
  catch (xercesc::SAXParseException const& e)
    {
      char* msg;

      msg = xercesc::XMLString::transcode(e.getMessage());
      logging::config << logging::HIGH << "Configuration parsing error on \""
                      << file.c_str() << "\" (line "
                      << (unsigned int)e.getLineNumber() << ", character "
                      << (unsigned int)e.getColumnNumber() << "): " << msg;
      xercesc::XMLString::release(&msg);
      _clear();
    }
  catch (exceptions::basic const& e)
    {
      logging::config << logging::HIGH << "Error while parsing config file \""
                      << file.c_str() << "\": " << e.what();
      _clear();
    }
  return ;
}
