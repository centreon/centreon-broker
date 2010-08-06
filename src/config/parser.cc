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
 *  Association between tag name and its id.
 */
parser::_tag_id const parser::_tag_to_id[] =
{
  {"config", _logger_config},
  {"debug", _logger_debug},
  {"error", _logger_error},
  {"info", _logger_info},
  {"level", _logger_level},
  {"name", _logger_name},
  {"type", _logger_type}
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
  _current = _unknown;
  _loggers.clear();
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
 *  Receive notification of character data.
 *
 *  @param[in] chars  Characters.
 *  @param[in] length Length of chars.
 */
void parser::characters(XMLCh const* const chars,
                        XMLSize_t const length)
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
  (void)uri;
  (void)localname;
  (void)qname;
  switch (_current)
    {
     case _logger:
      _current = _unknown;
      break ;
     case _logger_config:
      _loggers.back().config(strtol(_data.c_str(), NULL, 0));
      _current = _logger;
      break ;
     case _logger_debug:
      _loggers.back().debug(strtol(_data.c_str(), NULL, 0));
      _current = _logger;
      break ;
     case _logger_error:
      _loggers.back().error(strtol(_data.c_str(), NULL, 0));
      _current = _logger;
      break ;
     case _logger_info:
      _loggers.back().info(strtol(_data.c_str(), NULL, 0));
      _current = _logger;
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
      }
      _current = _logger;
      break ;
     case _logger_name:
      _loggers.back().name(_data);
      _current = _logger;
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
      _current = _logger;
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
  switch (_current)
    {
     case _unknown:
      {
        char* name;

        name = xercesc::XMLString::transcode(localname);
        if (!name)
          throw (exceptions::basic() << "Empty token.");
        if (!strcmp(name, "logger"))
          {
            _current = _logger;
            _loggers.push_back(logger());
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
     case _logger:
      {
        char* tag;

        tag = xercesc::XMLString::transcode(localname);
        if (!tag)
          throw (exceptions::basic() << "Empty token.");
        for (unsigned int i = 0;
             i < sizeof(_tag_to_id) / sizeof(*_tag_to_id);
             ++i)
          if (!strcmp(tag, _tag_to_id[i].tag))
            {
              _current = _tag_to_id[i].id;
              break ;
            }
        if (_current == _logger)
          {
            exceptions::basic e;

            e << "Invalid token \"" << tag << "\"";
            xercesc::XMLString::release(&tag);
            throw (e);
          }
        xercesc::XMLString::release(&tag);
      }
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
parser::parser() : _current(parser::_unknown) {}

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
 *  Return the list of parsed logger objects.
 *
 *  @return List of parsed logger objects.
 */
std::list<logger>& parser::loggers()
{
  return (_loggers);
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
