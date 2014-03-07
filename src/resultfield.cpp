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
    _data(data),
    _field(nullptr)
{}

/**
 *  Constructor
 *
 *  @param  result  the result implementation
 *  @param  field   the field implementation
 */
ResultField::ResultField(std::shared_ptr<ResultImpl> result, StatementResultField *field) :
    _result(result),
    _data(nullptr),
    _field(field)
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
 *  Cast to a float
 *
 *  Note that if the value is NULL, this will yield
 *  an empty string. To check for NULL values, use
 *  the isNULL function.
 *
 *  @throws std::invalid_argument   if this field does not contain a number
 *  @throws std::out_of_range       if the value is too small or too big to fit in a float
 */
ResultField::operator float() const
{
    // check for NULL value
    if (isNULL()) return 0;

    // convert to float
    return std::stof(_data);
}

/**
 *  Cast to a double
 *
 *  Note that if the value is NULL, this will yield
 *  an empty string. To check for NULL values, use
 *  the isNULL function.
 *
 *  @throws std::invalid_argument   if this field does not contain a number
 *  @throws std::out_of_range       if the value is too small or too big to fit in a double
 */
ResultField::operator double() const
{
    // check for NULL value
    if (isNULL()) return 0;

    // convert to float
    return std::stod(_data);
}

/**
 *  Cast to a long double
 *
 *  Note that if the value is NULL, this will yield
 *  an empty string. To check for NULL values, use
 *  the isNULL function.
 *
 *  @throws std::invalid_argument   if this field does not contain a number
 *  @throws std::out_of_range       if the value is too small or too big to fit in a long double
 */
ResultField::operator long double() const
{
    // check for NULL value
    if (isNULL()) return 0;

    // convert to float
    return std::stold(_data);
}

/**
 *  Cast to an integer
 *
 *  Note that if the value is NULL, this will yield
 *  an empty string. To check for NULL values, use
 *  the isNULL function.
 *
 *  @throws std::invalid_argument   if this field does not contain a number
 *  @throws std::out_of_range       if the value is too small or too big to fit in an integer
 */
ResultField::operator int() const
{
    // check for NULL value
    if (isNULL()) return 0;

    // convert to integer
    return std::stoi(_data);
}

/**
 *  Cast to a long integer
 *
 *  Note that if the value is NULL, this will yield
 *  an empty string. To check for NULL values, use
 *  the isNULL function.
 *
 *  @throws std::invalid_argument   if this field does not contain a number
 *  @throws std::out_of_range       if the value is too small or too big to fit in a long integer
 */
ResultField::operator long() const
{
    // check for NULL value
    if (isNULL()) return 0;

    // convert to long integer
    return std::stol(_data);
}

/**
 *  Cast to an unsigned long integer
 *
 *  Note that if the value is NULL, this will yield
 *  an empty string. To check for NULL values, use
 *  the isNULL function.
 *
 *  @throws std::invalid_argument   if this field does not contain a number
 *  @throws std::out_of_range       if the value is too small or too big to fit in an unsigned long integer
 */
ResultField::operator unsigned long() const
{
    // check for null value
    if (isNULL()) return 0;

    // convert to long unsigned integer
    return std::stoul(_data);
}

/**
 *  Cast to a very long integer
 *
 *  Note that if the value is NULL, this will yield
 *  an empty string. To check for NULL values, use
 *  the isNULL function.
 *
 *  @throws std::invalid_argument   if this field does not contain a number
 *  @throws std::out_of_range       if the value is too small or too big to fit in a very long integer
 */
ResultField::operator long long() const
{
    // check for null value
    if (isNULL()) return 0;

    // convert to very long integer
    return std::stoll(_data);
}

/**
 *  Cast to a very long unsigned integer
 *
 *  Note that if the value is NULL, this will yield
 *  an empty string. To check for NULL values, use
 *  the isNULL function.
 *
 *  @throws std::invalid_argument   if this field does not contain a number
 *  @throws std::out_of_range       if the value is too small or too big to fit in a very long unsigned integer
 */
ResultField::operator unsigned long long() const
{
    // check for null value
    if (isNULL()) return 0;

    // convert to very long unsigned integer
    return std::stoull(_data);
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
