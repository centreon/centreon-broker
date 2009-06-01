/*
** field_setter.hpp for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/29/09 Matthieu Kermagoret
** Last update 06/01/09 Matthieu Kermagoret
*/

#ifndef DB_FIELD_SETTER_HPP_
# define DB_FIELD_SETTER_HPP_

# include <sys/types.h>
# include "db/query.hpp"

namespace CentreonBroker
{
  // Forward declarations (see db_query.hpp).
  template <typename ObjectType> class Query;


  /****************************************************************************
  *                                                                           *
  *                                                                           *
  *                               FieldSetter                                 *
  *                                                                           *
  *                                                                           *
  ****************************************************************************/

  /**
   *  This is the base setter class. To extract data from any object, the
   *  mapping will use this object's virtual method operator().
   */
  template <typename ObjectType>
  class          FieldSetter
  {
   private:
    // FieldSetter copy constructor.
                 FieldSetter(const FieldSetter& field_setter)
    {
      (void)field_setter;
    }
    // FieldSetter operator= overload.
    FieldSetter& operator=(const FieldSetter& field_setter)
    {
      (void)field_setter;
      return (*this);
    }

   public:
    // FieldSetter default constructor.
                 FieldSetter() {}
    // FieldSetter virtual destructor.
    virtual      ~FieldSetter() {}
    // This virtual method will be called by mapped queries to extract data
    // from the mapped object.
    virtual void operator()(Query<ObjectType>* query,
                            const ObjectType& object) const = 0;
  };


  /****************************************************************************
  *                                                                           *
  *                                                                           *
  *                          DynamicFieldSetter                               *
  *                                                                           *
  *                                                                           *
  ****************************************************************************/

  /**
   *  This template class is used for setters which use getters that are built
   *  at runtime (like boost::bind ones). The default template is not used.
   */
  template <typename ObjectType,
            typename FieldType,
            typename GetterType = FieldType (*)(const ObjectType&)>
  class   DynamicFieldSetter
  {
  };

  /**
   *  Template specialization on the double type.
   */
  template <typename ObjectType, typename GetterType>
  class                 DynamicFieldSetter<ObjectType, double, GetterType>
    : public FieldSetter<ObjectType>
  {
   private:
    const GetterType& getter_;
    // DynamicFieldSetter copy constructor.
                        DynamicFieldSetter(const DynamicFieldSetter& dfs)
    {
      (void)dfs;
    }
    // DynamicFieldSetter operator= overload.
    DynamicFieldSetter& operator=(const DynamicFieldSetter& dfs)
    {
      (void)dfs;
      return (*this);
    }

   public:
    // DynamicFieldSetter default constructor.
                        DynamicFieldSetter(const GetterType& getter)
      : getter_(getter) {}
    // DynamicFieldSetter destructor.
                        ~DynamicFieldSetter() {}
    // FieldSetter's operator= override.
    void                operator()(Query<ObjectType>* query,
                                   const ObjectType& object) const
    {
      query->SetDouble(this->getter_(object));
      return ;
    }
  };

  /**
   *  Template specialization on the int type.
   */
  template <typename ObjectType, typename GetterType>
  class                 DynamicFieldSetter<ObjectType, int, GetterType>
    : public FieldSetter<ObjectType>
  {
   private:
    const GetterType& getter_;
    // DynamicFieldSetter copy constructor.
                        DynamicFieldSetter(const DynamicFieldSetter& dfs)
    {
      (void)dfs;
    }
    // DynamicFieldSetter operator= overload.
    DynamicFieldSetter& operator=(const DynamicFieldSetter& dfs)
    {
      (void)dfs;
      return (*this);
    }

   public:
    // DynamicFieldSetter default constructor.
                        DynamicFieldSetter(const GetterType& getter)
      : getter_(getter) {}
    // DynamicFieldSetter destructor.
                        ~DynamicFieldSetter() {}
    // FieldSetter's operator= override.
    void                operator()(Query<ObjectType>* query,
                                   const ObjectType& object) const
    {
      query->SetInt(this->getter_(object));
      return ;
    }
  };

  /**
   *  Template specialization on the short type.
   */
  template <typename ObjectType, typename GetterType>
  class                 DynamicFieldSetter<ObjectType, short, GetterType>
    : public FieldSetter<ObjectType>
  {
   private:
    const GetterType& getter_;
    // DynamicFieldSetter copy constructor.
                        DynamicFieldSetter(const DynamicFieldSetter& dfs)
    {
      (void)dfs;
    }
    // DynamicFieldSetter operator= overload.
    DynamicFieldSetter& operator=(const DynamicFieldSetter& dfs)
    {
      (void)dfs;
      return (*this);
    }

   public:
    // DynamicFieldSetter default constructor.
                        DynamicFieldSetter(const GetterType& getter)
      : getter_(getter) {}
    // DynamicFieldSetter destructor.
                        ~DynamicFieldSetter() {}
    // FieldSetter's operator= override.
    void                operator()(Query<ObjectType>* query,
                                   const ObjectType& object) const
    {
      query->SetShort(this->getter_(object));
      return ;
    }
  };

  /**
   *  Template specialization on the string type.
   */
  template <typename ObjectType, typename GetterType>
  class                 DynamicFieldSetter<ObjectType,
                                           const std::string&,
                                           GetterType>
    : public FieldSetter<ObjectType>
  {
   private:
    const GetterType& getter_;
    // DynamicFieldSetter copy constructor.
                        DynamicFieldSetter(const DynamicFieldSetter& dfs)
    {
      (void)dfs;
    }
    // DynamicFieldSetter operator= overload.
    DynamicFieldSetter& operator=(const DynamicFieldSetter& dfs)
    {
      (void)dfs;
      return (*this);
    }

   public:
    // DynamicFieldSetter default constructor.
                        DynamicFieldSetter(const GetterType& getter)
      : getter_(getter) {}
    // DynamicFieldSetter destructor.
                        ~DynamicFieldSetter() {}
    // FieldSetter's operator= override.
    void                operator()(Query<ObjectType>* query,
                                   const ObjectType& object) const
    {
      query->SetString(this->getter_(object));
      return ;
    }
  };

  /**
   *  Template specialization on the time_t type.
   */
  template <typename ObjectType, typename GetterType>
  class                 DynamicFieldSetter<ObjectType,
                                           time_t,
                                           GetterType>
    : public FieldSetter<ObjectType>
  {
   private:
    const GetterType& getter_;
    // DynamicFieldSetter copy constructor.
                        DynamicFieldSetter(const DynamicFieldSetter& dfs)
    {
      (void)dfs;
    }
    // DynamicFieldSetter operator= overload.
    DynamicFieldSetter& operator=(const DynamicFieldSetter& dfs)
    {
      (void)dfs;
      return (*this);
    }

   public:
    // DynamicFieldSetter default constructor.
                        DynamicFieldSetter(const GetterType& getter)
      : getter_(getter) {}
    // DynamicFieldSetter destructor.
                        ~DynamicFieldSetter() {}
    // FieldSetter's operator= override.
    void                operator()(Query<ObjectType>* query,
                                   const ObjectType& object) const
    {
      query->SetTimeT(this->getter_(object));
      return ;
    }
  };
}

#endif /* !DB_FIELD_SETTER_HPP_ */
