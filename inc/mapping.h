/*
** mapping.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/01/09 Matthieu Kermagoret
** Last update 06/01/09 Matthieu Kermagoret
*/

#ifndef MAPPING_H_
# define MAPPING_H_

# include "db/field_setter.hpp"
# include "db/mapping.hpp"

namespace CentreonBroker
{
  template <typename ObjectType,
    typename ReturnType,
    ReturnType (ObjectType::* method)() const>
    ReturnType MethodToFunction(const ObjectType& object)
      {
	return ((object.*method)());
      }
  
  template <typename ObjectType>
    struct MappedField
    {
      const char* field;
      const FieldSetter<ObjectType>& setter;
    };
}

#endif /* !MAPPING_H_ */
