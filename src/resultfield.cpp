/**
 *  ResultField.cpp
 *
 *  A single field from a single row from
 *  a MySQL result set
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
 *
 *  @param  result  the result implementation
 *  @param  data    the field data
 */
ResultField::ResultField(std::shared_ptr<ResultImpl> result, const char *data) :
    _result(result),
    _data(data)
{}

/**
 *  Get whether the field is NULL
 */
bool ResultField::isNULL() const
{
    return _data == nullptr;
}

/**
 *  Cast to a string
 *
 *  Note that if the value is NULL, this will yield
 *  an empty string. To check for NULL values, use
 *  the isNULL function.
 */
ResultField::operator std::string() const
{
    // check for NULL value
    return isNULL() ? std::string() : std::string(_data);
}

/**
 *  Write the field to a stream
 *
 *  @param  stream  output stream to write to
 *  @param  field   the field to write
 */
std::ostream& operator<<(std::ostream& stream, const ResultField& field)
{
    // is this field NULL?
    if (field.isNULL()) return stream << "(NULL)";

    // write the string value
    else return stream << (std::string) field;
}

/**
 *  End namespace
 */
}}
