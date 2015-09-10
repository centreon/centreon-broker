/*
** Copyright 2011-2013 Centreon
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

#ifndef CCB_EXCEPTIONS_WITH_POINTER_HH
#  define CCB_EXCEPTIONS_WITH_POINTER_HH

#  include <memory>
#  include "com/centreon/broker/exceptions/msg.hh"
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                      exceptions {
  /**
   *  @class with_pointer with_pointer.hh "com/centreon/broker/exceptions/with_pointer.hh"
   *  @brief Exception with data pointer attached.
   *
   *  This class is used to provide exception chaining when it is
   *  necessary to attach a data pointer to the exception.
   */
  class                        with_pointer : public msg {
  public:
                               with_pointer(
                                 msg const& base,
                                 misc::shared_ptr<io::data> ptr);
                               with_pointer(with_pointer const& wp);
    virtual                    ~with_pointer() throw ();
    with_pointer&              operator=(with_pointer const& wp);
    virtual msg*               clone() const;
    misc::shared_ptr<io::data> ptr() const;
    virtual void               rethrow() const;
    char const*                what() const throw ();

  private:
    mutable std::auto_ptr<msg> _base;
    mutable misc::shared_ptr<io::data>
                               _ptr;
  };
}

CCB_END()

#endif // !CCB_EXCEPTIONS_WITH_POINTER_HH
