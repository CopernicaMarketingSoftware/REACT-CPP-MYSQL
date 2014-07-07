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
class ResultFieldImpl;

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
     *  The field implementation
     */
    ResultFieldImpl *_field;
public:
    /**
     *  Constructor
     *
     *  @param  result  the result implementation
     *  @param  field   the field implementation
     */
    ResultField(std::shared_ptr<ResultImpl> result, ResultFieldImpl *field);

    /**
     *  Get whether this field is NULL
     */
    bool isNULL() const;

    /**
     *  Cast to a number
     *
     *  Note that if the value is NULL, this will yield 0.
     *  To check for NULL values, use the isNULL function.
     *
     *  @throws std::invalid_argument   if this field does not contain a number
     *  @throws std::out_of_range       if the value is too small or too big to fit in the requested type
     */
    operator int8_t()   const;
    operator uint16_t() const;
    operator int16_t()  const;
    operator uint32_t() const;
    operator int32_t()  const;
    operator uint64_t() const;
    operator int64_t()  const;
    operator float()    const;
    operator double()   const;

    /**
     *  Cast to a uint128_t, this assumes a binary(16) field network byte ordering
     *
     *  @throws std::out_of_range      if the value is not the correct size (16 bytes)
     */
    operator uint128_t() const;

    /**
     *  Cast to a string
     *
     *  Note that if the value is NULL, this will yield
     *  an empty string. To check for NULL values, use
     *  the isNULL function.
     */
    operator std::string() const;

    /**
     *  Cast to a time structure
     *
     *  Note that if the value is NULL, or if it is not
     *  a date type, this function will return an std::tm
     *  structure set at its epoch (1900-01-01 00:00:00).
     */
    operator std::tm() const;
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
