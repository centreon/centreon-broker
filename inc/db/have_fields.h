/*
** have_fields.h for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/09/09 Matthieu Kermagoret
** Last update 06/09/09 Matthieu Kermagoret
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
