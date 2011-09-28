/*
** Copyright 2011 Merethis
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

#ifndef CCB_NAMESPACE_HH_
# define CCB_NAMESPACE_HH_

# ifdef CCB_BEGIN
#  undef CCB_BEGIN
# endif /* CCB_BEGIN */
# define CCB_BEGIN() namespace com { \
                       namespace centreon { \
                         namespace broker {

# ifdef CCB_END
#  undef CCB_END
# endif /* CCB_END */
# define CCB_END() } } }

#endif /* !CCB_NAMESPACE_HH_ */
