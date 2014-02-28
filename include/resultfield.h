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
