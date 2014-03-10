/**
 *  StatementIntegralResultField.h
 *
 *  Class representing a result field
 *  holding an integral value.
 *
 *  @copyright 2014 Copernica BV
 */

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

/**
 *  Result field class for integral types
 */
template <typename T>
class StatementIntegralResultField : public StatementResultField
{
private:
    /**
     *  The field data
     */
    T _value;

    /**
     *  Retrieve the value pointer
     */
    void *getValue() override
    {
        return static_cast<void*>(&_value);
    }
public:
    /**
     *  Constructor
     */
    StatementIntegralResultField() : StatementResultField()
    {}

    /**
     *  Cast to a number
     */
    virtual operator int8_t()   const override { return isNULL() ? 0 : _value; }
    virtual operator uint16_t() const override { return isNULL() ? 0 : _value; }
    virtual operator int16_t()  const override { return isNULL() ? 0 : _value; }
    virtual operator uint32_t() const override { return isNULL() ? 0 : _value; }
    virtual operator int32_t()  const override { return isNULL() ? 0 : _value; }
    virtual operator uint64_t() const override { return isNULL() ? 0 : _value; }
    virtual operator int64_t()  const override { return isNULL() ? 0 : _value; }
    virtual operator float()    const override { return isNULL() ? 0 : _value; }
    virtual operator double()   const override { return isNULL() ? 0 : _value; }

    /**
     *  Cast to a string
     */
    virtual operator std::string() const override
    {
        return std::to_string(_value);
    }

    /**
     *  Cast to a time structure
     */
    virtual operator std::tm() const override
    {
        return std::tm{};
    }
};

// the different integral fields we support
using StatementSignedCharResultField        =   StatementIntegralResultField<int8_t>;
using StatementUnsignedShortResultField     =   StatementIntegralResultField<uint16_t>;
using StatementSignedShortResultField       =   StatementIntegralResultField<int16_t>;
using StatementUnsignedLongResultField      =   StatementIntegralResultField<uint32_t>;
using StatementSignedLongResultField        =   StatementIntegralResultField<int32_t>;
using StatementUnsignedLongLongResultField  =   StatementIntegralResultField<uint64_t>;
using StatementSignedLongLongResultField    =   StatementIntegralResultField<int64_t>;
using StatementFloatResultField             =   StatementIntegralResultField<float>;
using StatementDoubleResultField            =   StatementIntegralResultField<double>;

/**
 *  End namespace
 */
}}
