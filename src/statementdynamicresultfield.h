/**
 *  StatementSignedCharResultField.h
 *
 *  Class representing a result field
 *  of varying length (i.e. a CHAR or
 *  a BLOB field).
 *
 *  @copyright 2014 Copernica BV
 */

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

/**
 *  Result field class for dynamic fields
 */
class StatementDynamicResultField : public StatementResultField
{
private:
    /**
     *  The field data
     */
    char *_value;

    /**
     *  Field size
     */
    unsigned long _size;

    /**
     *  Retrieve the value pointer
     */
    void *getValue() override
    {
        return static_cast<void*>(_value);
    }
protected:
    /**
     *  Is this a dynamically sized field?
     */
    virtual bool dynamic()
    {
        // we are!
        return true;
    }
public:
    /**
     *  Constructor
     */
    StatementDynamicResultField() :
        StatementResultField(),
        _value(nullptr),
        _size(0)
    {}

    /**
     *  Destructor
     */
    virtual ~StatementDynamicResultField()
    {
        std::free(_value);
    }

    /**
     *  Cast to a number
     */
    virtual operator int8_t()   const override { return isNULL() ? 0 : std::stoi(_value);  }
    virtual operator uint16_t() const override { return isNULL() ? 0 : std::stoi(_value);  }
    virtual operator int16_t()  const override { return isNULL() ? 0 : std::stoi(_value);  }
    virtual operator uint32_t() const override { return isNULL() ? 0 : std::stoul(_value);  }
    virtual operator int32_t()  const override { return isNULL() ? 0 : std::stoi(_value);  }
    virtual operator uint64_t() const override { return isNULL() ? 0 : std::stoull(_value); }
    virtual operator int64_t()  const override { return isNULL() ? 0 : std::stoll(_value);  }
    virtual operator float()    const override { return isNULL() ? 0 : std::stof(_value);  }
    virtual operator double()   const override { return isNULL() ? 0 : std::stod(_value);  }
    virtual operator uint128_t() const override
    {
        // Throw in case we are not the correct size
        if (_size != sizeof(uint128_t)) throw std::out_of_range("ResultField is the incorrect size, should be 16 bytes");

        // Declare our uint128_t output and copy the raw bytes from tmp into it
        uint128_t output;
        memcpy(&output, _value, sizeof(output));

        // Convert from network byte ordering to host byte ordering and return
        return ntohl128(output);
    }

    /**
     *  Cast to a string
     */
    virtual operator std::string() const override
    {
        return std::string(_value, _size);
    }

    /**
     *  Cast to a time structure
     */
    virtual operator std::tm() const override {
        return std::tm {};
    }

    // friends and family
    friend class StatementResultInfo;
};

/**
 *  End namespace
 */
}}
