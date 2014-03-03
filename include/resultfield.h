/**
 *  ResultField.h
 *
 *  A single field from a single row from
 *  a MySQL result set
 *
 *  @copyright 2014 Copernica BV
 */

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

// forward declaration
class ResultImpl;

/**
 *  Result field
 */
class ResultField
{
private:
    /**
     *  The result implementation
     */
    std::shared_ptr<ResultImpl> _result;

    /**
     *  The field data
     */
    const char *_data;
public:
    /**
     *  Constructor
     *
     *  @param  result  the result implementation
     *  @param  data    the field data
     */
    ResultField(std::shared_ptr<ResultImpl> result, const char *data);

    /**
     *  Get whether this field is NULL
     */
    bool isNULL() const;

    /**
     *  Cast to a string
     *
     *  Note that if the value is NULL, this will yield
     *  an empty string. To check for NULL values, use
     *  the isNULL function.
     */
    operator std::string() const;

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
    operator float() const;

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
    operator double() const;

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
    operator long double() const;

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
    operator int() const;

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
    operator long() const;

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
    operator unsigned long() const;

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
    operator long long() const;

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
    operator unsigned long long() const;
};

/**
 *  Write the field to a stream
 *
 *  @param  stream  output stream to write to
 *  @param  field   the field to write
 */
std::ostream& operator<<(std::ostream& stream, const ResultField& field);

/**
 *  End namespace
 */
}}
