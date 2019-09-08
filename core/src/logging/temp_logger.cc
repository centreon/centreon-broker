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

#include "com/centreon/broker/logging/temp_logger.hh"
#include "com/centreon/broker/logging/manager.hh"

using namespace com::centreon::broker::logging;

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

temp_logger::redir const temp_logger::_redir_nothing = {
    &temp_logger::_nothing<bool>,
    &temp_logger::_nothing<double>,
    &temp_logger::_nothing<int>,
    &temp_logger::_nothing<long>,
    &temp_logger::_nothing<long long>,
    &temp_logger::_nothing<std::string const&>,
    &temp_logger::_nothing<unsigned int>,
    &temp_logger::_nothing<unsigned long>,
    &temp_logger::_nothing<unsigned long long>,
    &temp_logger::_nothing<char const*>,
    &temp_logger::_nothing<void const*>};
temp_logger::redir const temp_logger::_redir_stringifier = {
    &temp_logger::_to_stringifier<bool>,
    &temp_logger::_to_stringifier<double>,
    &temp_logger::_to_stringifier<int>,
    &temp_logger::_to_stringifier<long>,
    &temp_logger::_to_stringifier<long long>,
    &temp_logger::_to_stringifier<std::string const&>,
    &temp_logger::_to_stringifier<unsigned int>,
    &temp_logger::_to_stringifier<unsigned long>,
    &temp_logger::_to_stringifier<unsigned long long>,
    &temp_logger::_to_stringifier<char const*>,
    &temp_logger::_to_stringifier<void const*>};

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] t Object to copy from.
 */
void temp_logger::_internal_copy(temp_logger const& t) {
  _level = t._level;
  _redir = t._redir;
  t._redir = &_redir_nothing;
  _type = t._type;
  return;
}

/**
 *  Do nothing.
 *
 *  @param[in] t Unused.
 *
 *  @return This object.
 */
template <typename T>
temp_logger& temp_logger::_nothing(T t) throw() {
  (void)t;
  return (*this);
}

/**
 *  Redirect to stringifier.
 *
 *  @param[in] t Parameter to forward.
 *
 *  @return This object.
 */
template <typename T>
temp_logger& temp_logger::_to_stringifier(T t) throw() {
  misc::stringifier::operator<<(t);
  return (*this);
}

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
temp_logger::temp_logger(type log_type, level l, bool enable)
    : _level(l),
      _redir(enable ? &_redir_stringifier : &_redir_nothing),
      _type(log_type) {}

/**
 *  Copy constructor.
 *
 *  @param[in] t Object to copy.
 */
temp_logger::temp_logger(temp_logger const& t) : misc::stringifier(t) {
  _internal_copy(t);
}

/**
 *  Destructor.
 */
temp_logger::~temp_logger() {
  if (_redir != &_redir_nothing) {
    operator<<("\n");
    manager::instance().log_msg(_buffer, _current, _type, _level);
  }
}

/**
 *  Assignment operator.
 *
 *  @param[in] t Object to copy.
 *
 *  @return This object.
 */
temp_logger& temp_logger::operator=(temp_logger const& t) {
  misc::stringifier::operator=(t);
  _internal_copy(t);
  return (*this);
}

/**
 *  Boolean redirection.
 *
 *  @param[in] b Boolean.
 *
 *  @return This object.
 */
temp_logger& temp_logger::operator<<(bool b) throw() {
  return ((this->*(_redir->redirect_bool))(b));
}

/**
 *  Double redirection.
 *
 *  @param[in] d Double.
 *
 *  @return This object.
 */
temp_logger& temp_logger::operator<<(double d) throw() {
  return ((this->*(_redir->redirect_double))(d));
}

/**
 *  Integer redirection.
 *
 *  @param[in] i Integer.
 *
 *  @return This object.
 */
temp_logger& temp_logger::operator<<(int i) throw() {
  return ((this->*(_redir->redirect_int))(i));
}

/**
 *  Long redirection.
 *
 *  @param[in] l Long integer.
 *
 *  @return This object.
 */
temp_logger& temp_logger::operator<<(long l) throw() {
  return ((this->*(_redir->redirect_long))(l));
}

/**
 *  Long long redirection.
 *
 *  @param[in] ll Long long integer.
 *
 *  @return This object.
 */
temp_logger& temp_logger::operator<<(long long ll) throw() {
  return ((this->*(_redir->redirect_long_long))(ll));
}

/**
 *  std::string redirection.
 *
 *  @param[in] q std::string.
 *
 *  @return This object.
 */
temp_logger& temp_logger::operator<<(std::string const& q) throw() {
  return ((this->*(_redir->redirect_std_string))(q));
}

/**
 *  Unsigned integer redirection.
 *
 *  @param[in] u Unsigned integer.
 *
 *  @return This object.
 */
temp_logger& temp_logger::operator<<(unsigned int u) throw() {
  return ((this->*(_redir->redirect_unsigned_int))(u));
}

/**
 *  Unsigned long integer redirection.
 *
 *  @param[in] ul Unsigned long integer.
 *
 *  @return This object.
 */
temp_logger& temp_logger::operator<<(unsigned long ul) throw() {
  return ((this->*(_redir->redirect_unsigned_long))(ul));
}

/**
 *  Unsigned long long integer redirection.
 *
 *  @param[in] ull Unsigned long long integer.
 *
 *  @return This object.
 */
temp_logger& temp_logger::operator<<(unsigned long long ull) throw() {
  return ((this->*(_redir->redirect_unsigned_long_long))(ull));
}

/**
 *  String redirection.
 *
 *  @param[in] str String.
 *
 *  @return This object.
 */
temp_logger& temp_logger::operator<<(char const* str) throw() {
  return ((this->*(_redir->redirect_string))(str));
}

/**
 *  Pointer redirection.
 *
 *  @param[in] ptr Pointer.
 *
 *  @return This object.
 */
temp_logger& temp_logger::operator<<(void const* ptr) throw() {
  return ((this->*(_redir->redirect_pointer))(ptr));
}
