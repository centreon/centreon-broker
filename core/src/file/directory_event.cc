/*
** Copyright 2013 Centreon
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

#include "com/centreon/broker/file/directory_event.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::file;

/**
 *  Default constructor.
 */
directory_event::directory_event() {}

/**
 *  Constructor.
 *
 *  @param[in] path       The path of this directory event.
 *  @param[in] type       The type of this directory event.
 *  @param[in] file_type  The type of the file of this directory event.
 */
directory_event::directory_event(std::string const& path,
                                 type type,
                                 file_type ftp)
    : _path(path), _type(type), _file_type(ftp) {}

/**
 *  Copy constructor.
 *
 *  @param[in] o  The object to copy.
 */
directory_event::directory_event(directory_event const& o)
    : _path(o._path), _type(o._type), _file_type(o._file_type) {}

/**
 *  Assignment operator.
 *
 *  @param[in] o  The object to copy.
 *
 *  @return       A reference to this object.
 */
directory_event& directory_event::operator=(directory_event const& o) {
  if (this != &o) {
    _path = o._path;
    _type = o._type;
    _file_type = o._file_type;
  }
  return (*this);
}

/**
 *  Destructor.
 */
directory_event::~directory_event() {}

/**
 *  Get the fullpath of this event.
 *
 *  @return  The fullpath of this event.
 */
std::string const& directory_event::get_path() const {
  return (_path);
}

/**
 *  Get the type of this event.
 *
 *  @return  The type of this event.
 */
directory_event::type directory_event::get_type() const {
  return (_type);
}

/**
 *  Get the file type of this event.
 *
 *  @return  The file type of this event.
 */
directory_event::file_type directory_event::get_file_type() const {
  return (_file_type);
}
