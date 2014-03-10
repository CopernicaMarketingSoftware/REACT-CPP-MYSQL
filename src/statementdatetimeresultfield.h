/**
 *  StatementDateTimeResultField.h
 *
 *  Class representing a result field in
 *  a result set from a prepared statement
 *  holding a date and/or time.
 *
 *  @copyright 2014 Copernica BV
 */

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

/**
 *  Result field class
 */
class StatementDateTimeResultField : public StatementResultField
{
private:
    /**
     *  The date information from MySQL
     */
    MYSQL_TIME _value;

    /**
     *  Get access to the data pointer for MySQL to fill
     */
    virtual void *getValue()
    {
        return static_cast<void*>(&_value);
    }
public:
    /**
     *  Constructor
     */
    StatementDateTimeResultField() : StatementResultField() {}

    /**
     *  Cast to a number
     */
    virtual operator int8_t()   const override { return 0; }
    virtual operator uint16_t() const override { return 0; }
    virtual operator int16_t()  const override { return 0; }
    virtual operator uint32_t() const override { return 0; }
    virtual operator int32_t()  const override { return 0; }
    virtual operator uint64_t() const override { return 0; }
    virtual operator int64_t()  const override { return 0; }
    virtual operator float()    const override { return 0; }
    virtual operator double()   const override { return 0; }

    /**
     *  Cast to a string
     */
    virtual operator std::string() const override { return ""; /* TODO */ }

    /**
     *  Cast to a time structure
     */
    virtual operator std::tm() const override {
        return std::tm { (int)_value.second, (int)_value.minute, (int)_value.hour, (int)_value.day, (int)_value.month - 1, (int)_value.year - 1900, 0, 0, -1 };
    }
};

/**
 *  End namespace
 */
}}
