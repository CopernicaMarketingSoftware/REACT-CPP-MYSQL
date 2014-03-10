/**
 *  ResultRow.cpp
 *
 *  Class with result data for a single MySQL row.
 *
 *  @copyright 2014 Copernica BV
 */

#include "includes.h"

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

/**
 *  Get the number of fields in the row
 *  @return size_t
 */
size_t ResultRow::size() const
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
const ResultField ResultRow::operator [] (size_t index) const
{
    // check for out of bounds
    if (index >= size()) throw Exception("Index out of bounds");

    // construct a result field, we also pass a result object to ensure
    // that the result will not be destructed for as long as the ResultField
    // objects is kept in scope by the user code
    return ResultField(_result, _fields[index].get());
}

/**
 *  Retrieve a field by name
 *
 *  This function throws an exception if no field
 *  exists under the given key.
 *
 *  @param  key     field name
 */
const ResultField ResultRow::operator [] (const std::string &key) const
{
    // check if field exist
    auto iter = _result->fields().find(key);
    if (iter == _result->fields().end()) throw Exception("Field key does not exist");

    // construct a result field, we also pass a result object to ensure
    // that the result will not be destructed for as long as the ResultField
    // objects is kept in scope by the user code
    return ResultField(_result, _fields[iter->second].get());
}

/**
 *  Get iterator for first field
 *  @return const_iterator
 */
ResultRow::iterator ResultRow::begin() const
{
    return iterator(_result->fields().cbegin(), this);
}

/**
 *  Get iterator for field by the given field name
 *  @param  name
 */
ResultRow::iterator ResultRow::find(const std::string& key) const
{
    return iterator(_result->fields().find(key), this);
}

/**
 *  Get the iterator past the end
 */
ResultRow::iterator ResultRow::end() const
{
    return iterator(_result->fields().cend(), this);
}

/**
 *  End namespace
 */
}}
