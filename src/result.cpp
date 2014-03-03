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
 *  Iterator empty constructor
 */
Result::iterator::iterator() :
    _result(nullptr),
    _index(0)
{}

/**
 *  Constructor
 *
 *  @param  result  mysql result set
 *  @param  index   index to start from
 */
Result::iterator::iterator(std::shared_ptr<ResultImpl> result, size_t index) :
    _result(result),
    _index(index)
{}

/**
 *  Copy constructor
 *
 *  @param  that    iterator to copy
 */
Result::iterator::iterator(const Result::iterator& that) :
    _result(that._result),
    _index(that._index)
{}

/**
 *  Is this iterator pointing to a valid position?
 */
bool Result::iterator::valid() const
{
    // check for a valid result and if the index is within bounds
    return _result && _index < _result->size();
}

/**
 *  Assign another iterator
 */
Result::iterator& Result::iterator::operator=(const iterator& that)
{
    // copy result and index
    _result = that._result;
    _index = that._index;

    // allow chaining
    return *this;
}

/**
 *  Increment operator
 */
Result::iterator& Result::iterator::operator++()
{
    ++_index;
    return *this;
}

/**
 *  Increment operator (postfix)
 */
Result::iterator Result::iterator::operator++(int)
{
    // make a copy
    iterator copy(*this);

    // increment index
    ++_index;

    // return the copy
    return copy;
}

/**
 *  Compare with other iterator
 *
 *  @param  that    iterator to compare with
 */
bool Result::iterator::operator==(const iterator& that)
{
    // check whether the result matches
    if (_result != that._result) return false;

    // if we are both invalid, we match
    if (!valid() && !that.valid()) return true;

    // index should be identical
    return _index == that._index;
}

/**
 *  Compare with other iterator
 *
 *  @param  that    iterator to compare with
 */
bool Result::iterator::operator!=(const iterator& that)
{
    return !operator==(that);
}

/**
 *  Dereference
 */
ResultRow Result::iterator::operator*()
{
    // seek to current index
    _result->seek(_index);

    // and fetch the row
    return ResultRow(_result, _result->fetch());
}

/**
 *  Call method on dereferenced row
 */
std::unique_ptr<ResultRow> Result::iterator::operator->()
{
    // seek to current index
    _result->seek(_index);

    // fetch the row into a new pointer
    return std::unique_ptr<ResultRow>(new ResultRow(_result, _result->fetch()));
}

/**
 *  Is this a valid result
 */
bool Result::valid() const
{
    return _result.get() != nullptr;
}

/**
 *  Get the number of rows in this result
 */
size_t Result::size() const
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
 *  Retrieve iterator for first row
 */
Result::iterator Result::begin() const
{
    return iterator(_result, 0);
}

/**
 *  Retrieve iterator past the end
 */
Result::iterator Result::end() const
{
    return iterator(_result, _result->size());
}

/**
 *  End namespace
 */
}}
