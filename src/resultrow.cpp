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
ResultRow::ResultRow(std::shared_ptr<ResultImpl> result, MYSQL_ROW fields)
{
    // initialize all fields
    for (auto iter = result->fields().begin(); iter != result->fields().end(); iter++)
    {
        // add to named fields and indexed fields
        _namedFields.insert(std::make_pair(iter->first, ResultField(result, fields[iter->second])));
        _indexedFields.push_back(ResultField(result, fields[iter->second]));
    }
}

/**
 *  Get the number of fields in the row
 */
size_t ResultRow::size() const
{
    return _indexedFields.size();
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
const ResultField& ResultRow::operator [] (size_t index) const
{
    // check whether the given field exists
    if (index >= size()) throw Exception("Invalid field index");

    // construct string and return
    return _indexedFields[index];
}

/**
 *  Retrieve a field by name
 *
 *  This function throws an exception if no field
 *  exists under the given key.
 *
 *  @param  key     field name
 */
const ResultField& ResultRow::operator [] (const std::string& key) const
{
    // find field in the list
    auto iter = _namedFields.find(key);

    // check whether it exists
    if (iter == _namedFields.end()) throw Exception("Invalid field name");

    // return the field
    return iter->second;
}

/**
 *  Get iterator for first field
 */
std::map<std::string, ResultField>::const_iterator ResultRow::begin() const
{
    return _namedFields.begin();
}

/**
 *  Get iterator for field by the given name
 */
std::map<std::string, ResultField>::const_iterator ResultRow::find(const std::string& key) const
{
    return _namedFields.find(key);
}

/**
 *  Get the iterator past the end
 */
std::map<std::string, ResultField>::const_iterator ResultRow::end() const
{
    return _namedFields.end();
}

/**
 *  End namespace
 */
}}
