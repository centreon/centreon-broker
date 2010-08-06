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

#ifndef CONFIG_PARSER_HH_
# define CONFIG_PARSER_HH_

# include <list>
# include <stack>
# include <xercesc/sax2/DefaultHandler.hpp>
# include "config/interface.hh"
# include "config/logger.hh"

namespace                     config
{
  /**
   *  @class parser parser.hh "config/parser.hh"
   *  @brief Parse configuration file.
   *
   *  Parse a configuration file and generate appropriate objects for further
   *  handling.
   */
  class                       parser : private xercesc::DefaultHandler
  {
   private:
    enum                      _current_type
    {
      _unknown = 0,
      _input,
      _interface_db,
      _interface_failover,
      _interface_filename,
      _interface_host,
      _interface_name,
      _interface_net_iface,
      _interface_password,
      _interface_port,
      _interface_protocol,
      _interface_socket,
      _interface_type,
      _interface_user,
# ifdef USE_TLS
      _interface_ca,
      _interface_cert,
      _interface_compress,
      _interface_key,
      _interface_tls,
# endif /* !USE_TLS */
      _logger,
      _logger_config,
      _logger_debug,
      _logger_error,
      _logger_info,
      _logger_level,
      _logger_name,
      _logger_type,
      _output
    };
    struct                    _tag_id
    {
      char const*             tag;
      _current_type           id;
    };
    std::stack<_current_type> _current;
    std::string               _data;
    std::list<interface>      _inputs;
    std::list<logger>         _loggers;
    std::list<interface>      _outputs;
    static _tag_id const      _interface_tag_to_id[];
    static _tag_id const      _logger_tag_to_id[];
    void                      _clear();
    void                      _internal_copy(parser const& p);
    void                      _parse_properties(XMLCh const* localname,
                                                _tag_id const tag_to_id[]);
    void                      characters(XMLCh const* const chars,
                                         XMLSize_t const length);
    void                      endElement(XMLCh const* const uri,
                                         XMLCh const* const localname,
                                         XMLCh const* const qname);
    void                      startElement(XMLCh const* const uri,
                                           XMLCh const* const localname,
                                           XMLCh const* const qname,
                                           xercesc::Attributes const& attrs);

   public:
                              parser();
                              parser(parser const& p);
                              ~parser();
    parser&                   operator=(parser const& p);
    std::list<interface>&     inputs();
    std::list<logger>&        loggers();
    std::list<interface>&     outputs();
    void                      parse(std::string const& file);
  };
}

#endif /* !CONFIG_PARSER_HH_ */
