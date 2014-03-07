/**
 *  Parameter.h
 *
 *  Input parameter for MySQL prepared statement
 *
 *  @copyright 2014 Copernica BV
 */

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

/**
 *  Input parameter class
 */
class Parameter : public MYSQL_BIND
{
private:
    /**
     *  Templated base constructor
     */
    template <typename T>
    Parameter(enum_field_types type, T value)
    {
        // empty the struct (MySQL dictates it)
        memset(this, 0, sizeof(*this));

        // set the buffer type
        buffer_type = type;

        // determine whether the value is unsigned
        is_unsigned = std::is_unsigned<T>::value;

        // allocate memory for the value
        T *data = static_cast<T*>(std::malloc(sizeof(T)));
        *data = value;

        // store the value
        buffer = data;
    }
public:
    /**
     *  Integral constructors
     *
     *  @param  value   parameter value
     */
    Parameter(int8_t value)      : Parameter(MYSQL_TYPE_TINY,        value) {}
    Parameter(uint16_t value)    : Parameter(MYSQL_TYPE_SHORT,       value) {}
    Parameter(int16_t value)     : Parameter(MYSQL_TYPE_SHORT,       value) {}
    Parameter(uint32_t value)    : Parameter(MYSQL_TYPE_LONG,        value) {}
    Parameter(int32_t value)     : Parameter(MYSQL_TYPE_LONG,        value) {}
    Parameter(uint64_t value)    : Parameter(MYSQL_TYPE_LONGLONG,    value) {}
    Parameter(int64_t value)     : Parameter(MYSQL_TYPE_LONGLONG,    value) {}
    Parameter(float value)       : Parameter(MYSQL_TYPE_FLOAT,       value) {}
    Parameter(double value)      : Parameter(MYSQL_TYPE_DOUBLE,      value) {}

    /**
     *  String constructor
     *
     *  @param  value   parameter value
     */
    Parameter(const std::string& value)
    {
        // empty the struct (MySQL dictates it)
        memset(this, 0, sizeof(*this));

        // set the buffer type
        buffer_type = MYSQL_TYPE_STRING;

        // allocate memory for the value
        char *data = static_cast<char*>(std::malloc(value.size()));
        std::memcpy(data, value.c_str(), value.size());

        // store the value and length
        buffer = data;
        buffer_length = value.size();
    }

    /**
     *  Binary constructor
     */
    Parameter(const std::vector<char>& value)
    {
        // empty the struct (MySQL dictates it)
        memset(this, 0, sizeof(*this));

        // set the buffer type
        buffer_type = MYSQL_TYPE_BLOB;

        // allocate memory for the value
        char *data = static_cast<char*>(std::malloc(value.size()));
        std::memcpy(data, value.data(), value.size());

        // store the value and length
        buffer = data;
        buffer_length = value.size();
    }

    /**
     *  NULL constructor
     */
    Parameter(std::nullptr_t value)
    {
        // empty the struct (MySQL dictates it)
        memset(this, 0, sizeof(*this));

        // set the buffer type
        buffer_type = MYSQL_TYPE_NULL;
    }

    /**
     *  Destructor
     *
     *  Do *NOT* make this function virtual.
     *
     *  Making this virtual will create a vtable,
     *  changing the size of the object which will
     *  make MySQL very very unhappy.
     */
    ~Parameter()
    {
        // if we have a buffer, free it
        std::free(buffer);
    }
};

/**
 *  End namespace
 */
}}
