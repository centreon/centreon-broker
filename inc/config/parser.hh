/*
** This file is part of Centreon Dhana.
**
** Centreon Dhana is free software: you can redistribute it and/or modify it
** under the terms of the GNU Affero General Public License as published by the
** Free Software Foundation, either version 3 of the License, or (at your
** option) any later version.
**
** Centreon Dhana is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
** or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public
** License for more details.
**
** You should have received a copy of the GNU Affero General Public License
** along with Centreon Dhana. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CONFIG_PARSER_HH_
# define CONFIG_PARSER_HH_

# include <list>
# include <xercesc/sax2/DefaultHandler.hpp>
# include "config/logger.hh"

namespace                config
{
  /**
   *  @class parser parser.hh "config/parser.hh"
   *  @brief Parse configuration file.
   *
   *  Parse a configuration file and generate appropriate objects for further
   *  handling.
   */
  class                  parser : private xercesc::DefaultHandler
  {
   private:
    enum                 _current_type
    {
      _unknown = 0,
      _logger,
      _logger_config,
      _logger_debug,
      _logger_error,
      _logger_info,
      _logger_level,
      _logger_name,
      _logger_type
    };
    struct               _tag_id
    {
      char const*        tag;
      _current_type      id;
    };
    _current_type        _current;
    std::string          _data;
    std::list<logger>    _loggers;
    static _tag_id const _tag_to_id[];
    void                 _clear();
    void                 _internal_copy(parser const& p);
    void                 characters(XMLCh const* const chars,
                                    XMLSize_t const length);
    void                 endElement(XMLCh const* const uri,
                                    XMLCh const* const localname,
                                    XMLCh const* const qname);
    void                 startElement(XMLCh const* const uri,
                                      XMLCh const* const localname,
                                      XMLCh const* const qname,
                                      xercesc::Attributes const& attrs);

   public:
                         parser();
                         parser(parser const& p);
                         ~parser();
    parser&              operator=(parser const& p);
    std::list<logger>&   loggers();
    void                 parse(std::string const& file);
  };
}

#endif /* !CONFIG_PARSER_HH_ */
