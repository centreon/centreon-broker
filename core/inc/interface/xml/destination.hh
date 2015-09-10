/*
** Copyright 2009-2011 Centreon
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

#ifndef INTERFACE_XML_DESTINATION_HH_
# define INTERFACE_XML_DESTINATION_HH_

# include <memory>
# include "interface/destination.hh"

// Forward declaration.
namespace          io {
  class            stream;
}

namespace          interface {
  namespace        xml {
    /**
     *  @class destination destination.hh "interface/xml/destination.hh"
     *  @brief XML destination.
     *
     *  The interface::xml::destination class converts events in their
     *  generic representation into an XML stream.
     */
    class          destination : public interface::destination {
     private:
      std::auto_ptr<io::stream>
                   _stream;
                   destination(destination const& dest);
      destination& operator=(destination const& dest);

     public:
                   destination(io::stream* s);
      virtual      ~destination();
      virtual void close();
      virtual void event(events::event* e);
    };
  }
}

#endif /* !INTERFACE_XML_DESTINATION_HH_ */
