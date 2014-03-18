/**
 *  LocalParameter.h
 *
 *  A parameter in a local prepared-statement-like query.
 *
 *  @copyright 2014 Copernica BV
 */

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

/**
 *  Local input parameter
 */
class LocalParameter
{
private:
    /**
     *  The parameter in string representation
     */
    std::string _value;

    /**
     *  Are we representing an integral value?
     */
    bool _integral;

    /**
     *  Buffer for value quoting
     */
    char *_buffer;
public:
    /**
     *  Integral constructors
     *
     *  @param  value   parameter value
     */
    LocalParameter(uint8_t value)     : _value(std::to_string(value)), _integral(true), _buffer(nullptr) {}
    LocalParameter(int8_t value)      : _value(std::to_string(value)), _integral(true), _buffer(nullptr) {}
    LocalParameter(uint16_t value)    : _value(std::to_string(value)), _integral(true), _buffer(nullptr) {}
    LocalParameter(int16_t value)     : _value(std::to_string(value)), _integral(true), _buffer(nullptr) {}
    LocalParameter(uint32_t value)    : _value(std::to_string(value)), _integral(true), _buffer(nullptr) {}
    LocalParameter(int32_t value)     : _value(std::to_string(value)), _integral(true), _buffer(nullptr) {}
    LocalParameter(uint64_t value)    : _value(std::to_string(value)), _integral(true), _buffer(nullptr) {}
    LocalParameter(int64_t value)     : _value(std::to_string(value)), _integral(true), _buffer(nullptr) {}
    LocalParameter(float value)       : _value(std::to_string(value)), _integral(true), _buffer(nullptr) {}
    LocalParameter(double value)      : _value(std::to_string(value)), _integral(true), _buffer(nullptr) {}

    /**
     *  String constructor
     *
     *  @param  value   parameter value
     */
    LocalParameter(const std::string& value) :
        _value(value),
        _integral(false),
        _buffer(static_cast<char*>(std::malloc(size())))
    {}

    /**
     *  Character array constructor
     *
     *  @param  value   parameter value
     */
    LocalParameter(const char *value) :
        _value(value),
        _integral(false),
        _buffer(static_cast<char*>(std::malloc(size())))
    {}

    /**
     *  NULL constructor
     *
     *  @param  value
     */
    LocalParameter(std::nullptr_t value) :
        _value("NULL"),
        _integral(true),
        _buffer(nullptr)
    {}

    /**
     *  Destructor
     */
    virtual ~LocalParameter()
    {
        std::free(_buffer);
    }

    /**
     *  The maximum size we could need to store
     *  this parameter inside a query string.
     */
    size_t size() const
    {
        // integral types need not be quoted or escaped
        // so their size is as-is, no extra space needed
        if (_integral) return _value.size();

        // otherwise, we might need quotes and each character
        // could be escaped to two, so to be safe we return
        // double the size plus that of two quote characters
        else return _value.size() * 2 + 2;
    }

    /**
     *  Escape the parameter, but do not quote it
     *
     *  @param  connection  mysql connection to use for escaping
     */
    const std::string escape(MYSQL *connection)
    {
        // integral values don't need escaping
        if (_integral) return _value;

        // escape the value
        auto length = mysql_real_escape_string(connection, _buffer, _value.c_str(), _value.size());

        // and wrap it in a string
        return std::string(_buffer, length);
    }

    /**
     *  Escape the parameter, and quote it if necessary
     *
     *  @param  connection  mysql connection to use for escaping
     */
    const std::string quote(MYSQL *connection)
    {
        // integral values need escaping nor quoting
        if (_integral) return _value;

        // write the opening quote to the buffer
        _buffer[0] = '\'';

        // escape the value and retrieve the size
        auto length = mysql_real_escape_string(connection, _buffer + 1, _value.c_str(), _value.size());

        // add the closing quote and terminating null character
        _buffer[length + 1] = '\'';
        _buffer[length + 2] = '\0';

        // and wrap it in a string
        return std::string(_buffer, length + 2);
    }
};

/**
 *  End namespace
 */
}}
