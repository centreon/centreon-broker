/*
**  Copyright 2009 MERETHIS
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

#include <stdlib.h>
#include <string.h>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include "correlation/parser.hh"
#include "exceptions/basic.hh"
#include "logging/logging.hh"

using namespace Correlation;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Find a node with its host_id and optionally its service_id.
 *
 *  @param[in] host_id    String of the host_id.
 *  @param[in] service_id Potentially NULL string of the service_id.
 *
 *  @return Node matching host_id/service_id.
 */
Node* parser::_find_node(char const* host_id, char const* service_id)
{
  Node* node;

  node = NULL;
  if (!service_id)
    {
      std::map<int, Node>::iterator it;

      it = _hosts->find(strtol(host_id, NULL, 0));
      if (it != _hosts->end())
        node = &it->second;
    }
  else
    {
      std::map<std::pair<int, int>, Node>::iterator it;

      it = _services->find(std::make_pair(strtol(host_id, NULL, 0),
                                          strtol(service_id, NULL, 0)));
      if (it != _services->end())
        node = &it->second;
    }
  return (node);
}

void parser::startElement(XMLCh const* const uri,
                          XMLCh const* const localname,
                          XMLCh const* const qname,
                          xercesc::Attributes const& attrs)
{
  char* value;

  (void)qname;
  if (!_in_root)
    _in_root = true;
  else
    {
      value = xercesc::XMLString::transcode(localname);
      if (!strcmp(value, "dependency"))
        {
          XMLCh* attr1 = NULL;
          XMLCh* attr2 = NULL;
          XMLCh* attr3 = NULL;
          XMLCh* attr4 = NULL;
          XMLCh const* hi1;
          XMLCh const* hi2;
          char* host_id1 = NULL;
          char* host_id2 = NULL;
          XMLCh const* si1;
          XMLCh const* si2;
          char* service_id1 = NULL;
          char* service_id2 = NULL;
          exceptions::basic e;

          // Transform C strings to Xerces-C++ strings.
          attr1 = xercesc::XMLString::transcode("dependent_host_id");
          attr2 = xercesc::XMLString::transcode("host_id");
          attr3 = xercesc::XMLString::transcode("dependent_service_id");
          attr4 = xercesc::XMLString::transcode("service_id");
          if (!attr1 || !attr2 || !attr3 || !attr4)
            {
              e << "Could not convert attribute name of a 'dependency' "
                   "definition.";
              goto dependency_error;
            }

          // Fetch attributes of the XML node.
          hi1 = attrs.getValue(uri, attr1);
          hi2 = attrs.getValue(uri, attr2);
          si1 = attrs.getValue(uri, attr3);
          si2 = attrs.getValue(uri, attr4);
          xercesc::XMLString::release(&attr1);
          xercesc::XMLString::release(&attr2);
          xercesc::XMLString::release(&attr3);
          xercesc::XMLString::release(&attr4);
          if (!hi1 || !hi2)
            {
              e << "Missing an host id for an element of a dependency "
                   "definition.";
              goto dependency_error;
            }
          host_id1 = xercesc::XMLString::transcode(hi1);
          host_id2 = xercesc::XMLString::transcode(hi2);
          if (!host_id1 || !host_id2)
            {
              e << "Failed to convert host id attributes of a dependency "
                   "definition.";
              goto dependency_error;
            }
          if (si1)
            service_id1 = xercesc::XMLString::transcode(si1);
          if (si2)
            service_id2 = xercesc::XMLString::transcode(si2);

          // Process these attributes.
          Node* n1;
          Node* n2;

          n1 = _find_node(host_id1, service_id1);
          n2 = _find_node(host_id2, service_id2);
          if (n1 && n2)
            {
              n1->depends_on.push_back(n2);
              n2->depended_by.push_back(n1);
            }
          xercesc::XMLString::release(&host_id1);
          xercesc::XMLString::release(&host_id2);
          if (service_id1)
            xercesc::XMLString::release(&service_id1);
          if (service_id2)
            xercesc::XMLString::release(&service_id2);
          goto dependency_error_end;

         dependency_error:
          if (attr1)
            xercesc::XMLString::release(&attr1);
          if (attr2)
            xercesc::XMLString::release(&attr2);
          if (attr3)
            xercesc::XMLString::release(&attr3);
          if (attr4)
            xercesc::XMLString::release(&attr4);
          if (host_id1)
            xercesc::XMLString::release(&host_id1);
          if (host_id2)
            xercesc::XMLString::release(&host_id2);
          if (service_id1)
            xercesc::XMLString::release(&service_id1);
          if (service_id2)
            xercesc::XMLString::release(&service_id2);
          xercesc::XMLString::release(&value);
          throw (e);
         dependency_error_end:
          ;
        }
      else if (!strcmp(value, "host"))
        {
          XMLCh* i_ch = NULL;
          XMLCh const* i_attr;
          char* id = NULL;
          exceptions::basic e;
          Node node;

          // Convert C strings to Xerces-C++ strings.
          i_ch = xercesc::XMLString::transcode("id");
          if (!i_ch)
            {
              e << "Could not convert an 'id' attribute.";
              goto id_error;
            }

          // Get XML node attribute.
          i_attr = attrs.getValue(uri, i_ch);
          xercesc::XMLString::release(&i_ch);
          if (!i_attr)
            {
              e << "Could not find an 'id' attribute on a 'host' definition.";
              goto id_error;
            }
          id = xercesc::XMLString::transcode(i_attr);
          if (!id)
            {
              e << "Failed to convert an host 'id' attribute.";
              goto id_error;
            }

          // Process attribute.
          node.host_id = strtol(id, NULL, 0);
          xercesc::XMLString::release(&id);
          (*_hosts)[node.host_id] = node;
          goto id_error_end;

         id_error:
          if (id)
            xercesc::XMLString::release(&id);
          xercesc::XMLString::release(&value);
          throw (e);
         id_error_end:
          ;
        }
      else if (!strcmp(value, "parent"))
        {
          XMLCh* host_ch = NULL;
          XMLCh* parent_ch = NULL;
          XMLCh const* host_attr;
          XMLCh const* parent_attr;
          char* host = NULL;
          char* parent = NULL;
          exceptions::basic e;
          std::map<int, Node>::iterator it1;
          std::map<int, Node>::iterator it2;

          // Convert C strings to Xerces-C++ strings.
          host_ch = xercesc::XMLString::transcode("host");
          parent_ch = xercesc::XMLString::transcode("parent");
          if (!host_ch || !parent_ch)
            {
              e << "Could not convert attributes name.";
              goto parent_error;
            }

          // Get XML node attributes.
          host_attr = attrs.getValue(host_ch);
          parent_attr = attrs.getValue(parent_ch);
          xercesc::XMLString::release(&host_ch);
          xercesc::XMLString::release(&parent_ch);
          if (!host_attr || !parent_attr)
            {
              e << "Could not find 'host' or 'parent' attribute of a "
                   "parenting definition";
              goto parent_error;
            }
          host = xercesc::XMLString::transcode(host_attr);
          parent = xercesc::XMLString::transcode(parent_attr);
          if (!host || !parent)
            {
              e << "Failed to transcode attribute of a parenting definition.";
              goto parent_error;
            }

          // Process attributes.
          it1 = (*_hosts).find(strtol(host, NULL, 0));
          it2 = (*_hosts).find(strtol(parent, NULL, 0));
          if ((it1 != (*_hosts).end()) && (it2 != (*_hosts).end()))
            {
              it1->second.parents.push_back(&it2->second);
              it2->second.children.push_back(&it1->second);
            }
          xercesc::XMLString::release(&host);
          xercesc::XMLString::release(&parent);
          goto parent_error_end;

         parent_error:
          if (host_ch)
            xercesc::XMLString::release(&host_ch);
          if (parent_ch)
            xercesc::XMLString::release(&parent_ch);
          if (host)
            xercesc::XMLString::release(&host);
          if (parent)
            xercesc::XMLString::release(&parent);
          xercesc::XMLString::release(&value);
          throw (e);
         parent_error_end:
          ;
        }
      else if (!strcmp(value, "service"))
        {
          XMLCh* host_ch = NULL;
          XMLCh* id_ch = NULL;
          XMLCh const* host_attr;
          XMLCh const* id_attr;
          char* host = NULL;
          char* id = NULL;
          exceptions::basic e;
          Node node;

          // Convert C strings to Xerces-C++ strings.
          host_ch = xercesc::XMLString::transcode("host");
          id_ch = xercesc::XMLString::transcode("id");
          if (!host_ch || !id_ch)
            {
              e << "Could not convert attribute names of a 'service' definition.";
              goto service_error;
            }

          // Get XML node attributes.
          host_attr = attrs.getValue(host_ch);
          id_attr = attrs.getValue(id_ch);
          xercesc::XMLString::release(&host_ch);
          xercesc::XMLString::release(&id_ch);
          if (!host_attr || !id_attr)
            {
              e << "Could not find 'host' or 'id' attribute of a "
                   "'service' definition.";
              goto service_error;
            }
          host = xercesc::XMLString::transcode(host_attr);
          id = xercesc::XMLString::transcode(id_attr);
          if (!host || !id)
            {
              e << "Could not convert attribute of a 'service' definition.";
              throw (e);
            }

          // Process attributes.
          node.host_id = strtol(id, NULL, 0);
          node.service_id = strtol(id, NULL, 0);
          (*_services)[std::make_pair(node.host_id, node.service_id)] = node;
          xercesc::XMLString::release(&host);
          xercesc::XMLString::release(&id);
          goto service_error_end;

         service_error:
          if (host_ch)
            xercesc::XMLString::release(&host_ch);
          if (id_ch)
            xercesc::XMLString::release(&id_ch);
          if (host)
            xercesc::XMLString::release(&host);
          if (id)
            xercesc::XMLString::release(&id);
          xercesc::XMLString::release(&value);
          throw (e);
         service_error_end:
          ;
        }
      xercesc::XMLString::release(&value);
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
parser::parser() {}

/**
 *  Copy constructor.
 *
 *  @param[in] p Unused.
 */
parser::parser(parser const& p) : xercesc::DefaultHandler()
{
  (void)p;
}

/**
 *  Destructor.
 */
parser::~parser() {}

/**
 *  Assignment operator.
 *
 *  @param[in] p Unused.
 *
 *  @return *this
 */
parser& parser::operator=(parser const& p)
{
  (void)p;
  return (*this);
}

/**
 *  Parse a configuration file.
 *
 *  @param[in]  filename Path to the correlation file.
 *  @param[out] hosts    List of hosts.
 *  @param[out] services List of services.
 */
void parser::parse(char const* filename,
                   std::map<int, Node>& hosts,
                   std::map<std::pair<int, int>, Node>& services)
{
  std::auto_ptr<xercesc::SAX2XMLReader> reader;

  _hosts = &hosts;
  _services = &services;
  try
    {
      reader.reset(xercesc::XMLReaderFactory::createXMLReader());
      reader->setContentHandler(this);
      reader->setErrorHandler(this);
      reader->parse(filename);
    }
  catch (xercesc::XMLException const& e)
    {
      char* msg;

      msg = xercesc::XMLString::transcode(e.getMessage());
      logging::config << logging::HIGH << "Correlation parsing error on \""
                      << filename << "\" (line "
                      << (unsigned int)e.getSrcLine() << "): " << msg;
      xercesc::XMLString::release(&msg);
    }
  catch (xercesc::SAXParseException const& e)
    {
      char* msg;

      msg = xercesc::XMLString::transcode(e.getMessage());
      logging::config << logging::HIGH << "Correlation parsing error on \""
                      << filename << "\" (line "
                      << (unsigned int)e.getLineNumber() << ", character "
                      << (unsigned int)e.getColumnNumber() << "): " << msg;
      xercesc::XMLString::release(&msg);
    }
  catch (exceptions::basic const& e)
    {
      logging::config << logging::HIGH << "Error while parsing correlation file \""
                      << filename << "\": " << e.what();
    }
  return ;
}
