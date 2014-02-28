/**
 *  ResultRow.h
 *
 *  Class with result data for a single MySQL row
 *
 *  @copyright 2014 Copernica BV
 */

#include "includes.h"

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

/**
 *  Construct the row
 *
 *  @param  result  the result object with all the rows
 *  @param  fields  the fields for this row
 */
ResultRow::ResultRow(std::shared_ptr<ResultImpl> result, MYSQL_ROW fields) :
    _result(result),
    _fields(fields)
{}

/**
 *  Get the number of fields in the row
 */
size_t ResultRow::size()
{
    return _result->fields().size();
}

/**
 *  Retrieve a field by index
 *
 *  This function throws an exception if no field
 *  exists under the given index (i.e. index
 *  is not smaller than size()).
 *
 *  @param  index   field index
 */
std::string ResultRow::operator [] (size_t index) const
{
    // check whether the given field exists
    if (index >= size()) throw Exception("Invalid field index");

    // construct string and return
    return std::string(_fields[index]);
}

/**
 *  Retrieve a field by name
 *
 *  This function throws an exception if no field
 *  exists under the given key.
 *
 *  @param  key     field name
 */
std::string ResultRow::operator [] (const std::string& key) const
{
    // find field in the list
    auto iter = _result->fields().find(key);

    // check whether it exists
    if (iter == _result->fields().end()) throw Exception("Invalid field name");

    // now retrieve the field by the corresponding index
    return operator [] (iter->second);
}

/**
 *  End namespace
 */
}}
