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

#ifndef CALLBACKS_H_
# define CALLBACKS_H_

namespace CentreonBroker
{
  int CallbackAcknowledgement(int callback_type, void* data) throw ();
  int CallbackComment(int callback_type, void* data) throw ();
  int CallbackDowntime(int callback_type, void* data) throw ();
  int CallbackHostStatus(int callback_type, void* data) throw ();
  int CallbackLog(int callback_type, void* data) throw ();
  int CallbackProgramStatus(int callback_type, void* data) throw ();
  int CallbackServiceStatus(int callback_type, void* data) throw ();
}

#endif /* !CALLBACKS_H_ */
