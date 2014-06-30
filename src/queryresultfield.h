/**
 *  QueryResultField.h
 *
 *  Class representing a result field
 *  from a regular query
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
class QueryResultField : public ResultFieldImpl
{
    /**
     *  Field data
     */
    const char *_data;
public:
    /**
     *  Constructor
     */
    QueryResultField(const char *data) : _data(data) {}

    /**
     *  Is this a NULL field?
     */
    bool isNULL() const override
    {
        return _data == nullptr;
    }

    /**
     *  Cast to a number
     */
    virtual operator int8_t()   const override { return isNULL() ? 0 : std::stoi(_data);  }
    virtual operator uint16_t() const override { return isNULL() ? 0 : std::stoi(_data);  }
    virtual operator int16_t()  const override { return isNULL() ? 0 : std::stoi(_data);  }
    virtual operator uint32_t() const override { return isNULL() ? 0 : std::stoul(_data);  }
    virtual operator int32_t()  const override { return isNULL() ? 0 : std::stoi(_data);  }
    virtual operator uint64_t() const override { return isNULL() ? 0 : std::stoull(_data); }
    virtual operator int64_t()  const override { return isNULL() ? 0 : std::stoll(_data);  }
    virtual operator float()    const override { return isNULL() ? 0 : std::stof(_data);  }
    virtual operator double()   const override { return isNULL() ? 0 : std::stod(_data);  }

    /**
     *  Cast to a string
     */
    virtual operator std::string() const
    {
        return isNULL() ? "" : _data;
    }

    /**
     *  Cast to a time structure
     */
    virtual operator std::tm() const override {
        return std::tm{};
    }
};

/**
 *  End namespace
 */
}}
