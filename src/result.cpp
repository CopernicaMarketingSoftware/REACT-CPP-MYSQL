/**
 *  Result.cpp
 *
 *  Class representing a MySQL result set
 *
 *  @copyright 2014 Copernica BV
 */

#include "includes.h"

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

/**
 *  Constructor
 */
Result::Result(MYSQL_RES *result) :
    _result(std::make_shared<ResultImpl>(result))
{}

/**
 *  Invalid constructor
 */
Result::Result(std::nullptr_t result) :
    _result(nullptr)
{}

/**
 *  Move constructor
 */
Result::Result(Result&& that) :
    _result(std::move(that._result))
{}

/**
 *  Destructor
 */
Result::~Result()
{}

/**
 *  Is this a valid result
 */
bool Result::valid()
{
    return _result.get() != nullptr;
}

/**
 *  Get the number of rows in this result
 */
size_t Result::size()
{
    return valid() ? _result->size() : 0;
}

/**
 *  Retrieve a row at the given offset
 *
 *  This function will throw an exception if no
 *  row exists at the given index (i.e.: index
 *  is greater than size()) or if the object is
 *  in an invalid state.
 *
 *  @param  index   row index in the result
 *  @throws Exception
 */
ResultRow Result::operator [] (size_t index)
{
    // check whether we are valid
    if (!valid()) throw Exception("Invalid result object");

    // seek to requested index
    _result->seek(index);

    // and fetch the row
    return ResultRow(_result, _result->fetch());
}

/**
 *  End namespace
 */
}}
