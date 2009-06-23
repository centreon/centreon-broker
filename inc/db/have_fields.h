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

#ifndef DB_HAVE_FIELDS_H_
# define DB_HAVE_FIELDS_H_

# include <string>

namespace          CentreonBroker
{
  namespace        DB
  {
    /**
     *  Some queries can descriminate a table against its fields. This class
     *  represents such queries.
     */
    class          HaveFields
    {
     private:
                   HaveFields(const HaveFields& hf) throw ();
      HaveFields&  operator=(const HaveFields& hf) throw ();

     public:
                   HaveFields() throw ();
      virtual      ~HaveFields();
      /**
       *  The following functions will set the next argument in the query.
       */
      virtual void SetDouble(double value) = 0;
      virtual void SetInt(int value) = 0;
      virtual void SetShort(short value) = 0;
      virtual void SetString(const std::string& value) = 0;
      virtual void SetTime(time_t value) = 0;
    };
  }
}

#endif /* !DB_HAVE_FIELDS_H_ */
