#pragma once

#include <iostream>
#include <memory>
#include <vector>

// Use the same type names as C
#include <decimal/decimal>
typedef std::decimal::decimal32::__decfloat32   _Decimal32;
typedef std::decimal::decimal64::__decfloat64   _Decimal64;
typedef std::decimal::decimal128::__decfloat128 _Decimal128;

#include <smalltime/smalltime.h>
#include <cbe/cbe.h>



namespace enc
{

typedef enum
{
    ENCODE_TYPE_BOOLEAN,
    ENCODE_TYPE_INTEGER,
    ENCODE_TYPE_FLOAT,
    ENCODE_TYPE_DECIMAL,
    ENCODE_TYPE_TIME,
    ENCODE_TYPE_STRING,
    ENCODE_TYPE_BINARY,
    ENCODE_TYPE_LIST,
    ENCODE_TYPE_MAP,
    ENCODE_TYPE_CONTAINER_END,
    ENCODE_TYPE_EMPTY,
    ENCODE_TYPE_PADDING,
} major_type;

major_type get_major_type(bool value);
major_type get_major_type(int8_t value);
major_type get_major_type(int16_t value);
major_type get_major_type(int32_t value);
major_type get_major_type(int64_t value);
major_type get_major_type(__int128 value);
major_type get_major_type(float value);
major_type get_major_type(double value);
major_type get_major_type(__float128 value);
major_type get_major_type(_Decimal32 value);
major_type get_major_type(_Decimal64 value);
major_type get_major_type(_Decimal128 value);
major_type get_major_type(const std::string& value);
major_type get_major_type(const std::vector<uint8_t>& value);

unsigned int to_doy(int year, unsigned int month, unsigned int day);

std::string to_id_string(bool value);
std::string to_id_string(int8_t value);
std::string to_id_string(int16_t value);
std::string to_id_string(int32_t value);
std::string to_id_string(int64_t value);
std::string to_id_string(__int128 value);
std::string to_id_string(float value);
std::string to_id_string(double value);
std::string to_id_string(__float128 value);
std::string to_id_string(_Decimal32 value);
std::string to_id_string(_Decimal64 value);
std::string to_id_string(_Decimal128 value);
std::string to_id_string(const std::string& value); 
std::string to_id_string(const std::vector<uint8_t>& value);
std::string to_id_string_time(smalltime value);


namespace adl_helper
{
    template<class T> std::string as_string(T&& object)
    {
        return std::to_string(std::forward<T>(object));
    }
}
template<class T> std::string to_string(T&& object)
{
    return adl_helper::as_string(std::forward<T>(object));
}

class encoder;

class encoding
{
private:
    const enc::major_type _type;
    const size_t _size;
    const std::string _string_value;
    std::shared_ptr<encoding> _next;
    std::shared_ptr<encoding> _last;

public:
    encoding(const enc::major_type type, const size_t size, const std::string& string_value);

    std::shared_ptr<encoding> next();

    std::shared_ptr<encoding> set_next(std::shared_ptr<encoding> next);

    std::shared_ptr<encoding> list();
    std::shared_ptr<encoding> map();
    std::shared_ptr<encoding> end();
    std::shared_ptr<encoding> empty();
    std::shared_ptr<encoding> pad(int count);
    std::shared_ptr<encoding> smtime(smalltime value);
    std::shared_ptr<encoding> str(const std::string& value);
    std::shared_ptr<encoding> bin(const std::vector<uint8_t>& value);
    std::shared_ptr<encoding> bl(bool value);
    std::shared_ptr<encoding> i8(int8_t value);
    std::shared_ptr<encoding> i16(int16_t value);
    std::shared_ptr<encoding> i32(int32_t value);
    std::shared_ptr<encoding> i64(int64_t value);
    std::shared_ptr<encoding> i128(__int128 value);
    std::shared_ptr<encoding> i128(int64_t high, uint64_t low);
    std::shared_ptr<encoding> f32(float value);
    std::shared_ptr<encoding> f64(double value);
    std::shared_ptr<encoding> f128(__float128 value);
    std::shared_ptr<encoding> d32(_Decimal32 value);
    std::shared_ptr<encoding> d64(_Decimal64 value);
    std::shared_ptr<encoding> d128(_Decimal128 value);

    enc::major_type get_type() const;

    virtual bool is_equal(const encoding& rhs) const = 0;

    bool is_equal_in_type(const encoding& rhs) const;

    bool is_equal_in_type_and_size(const encoding& rhs) const;

    bool operator ==(const encoding& rhs) const;

    const std::string as_string() const;

    virtual bool has_value(bool value) const;
    virtual bool has_value(int8_t value) const;
    virtual bool has_value(int16_t value) const;
    virtual bool has_value(int32_t value) const;
    virtual bool has_value(int64_t value) const;
    virtual bool has_value(__int128 value) const;
    virtual bool has_value(float value) const;
    virtual bool has_value(double value) const;
    virtual bool has_value(__float128 value) const;
    virtual bool has_value(_Decimal32 value) const;
    virtual bool has_value(_Decimal64 value) const;
    virtual bool has_value(_Decimal128 value) const;
    virtual bool has_value(const std::string& value) const;
    virtual bool has_value(const std::vector<uint8_t>& value) const;

    virtual cbe_encode_status encode(encoder& encoder) = 0;

    friend std::string to_string(encoding const& self) {return self.as_string();}
};

std::ostream& operator << (std::ostream& os, const encoding& rhs);


class no_value_encoding: public encoding
{
public:
    no_value_encoding(enc::major_type type, const std::string& type_string): encoding(type, 0, type_string) {}
    bool is_equal(const encoding& rhs) const { (void) rhs; return true; }
};

class list_encoding: public no_value_encoding
{
public:
    list_encoding(): no_value_encoding(enc::ENCODE_TYPE_LIST, "List") {}
    cbe_encode_status encode(encoder& encoder);
};

class map_encoding: public no_value_encoding
{
public:
    map_encoding(): no_value_encoding(enc::ENCODE_TYPE_MAP, "Map") {}
    cbe_encode_status encode(encoder& encoder);
};

class end_container_encoding: public no_value_encoding
{
public:
    end_container_encoding(): no_value_encoding(enc::ENCODE_TYPE_CONTAINER_END, "End") {}
    cbe_encode_status encode(encoder& encoder);
};

class empty_encoding: public no_value_encoding
{
public:
    empty_encoding(): no_value_encoding(enc::ENCODE_TYPE_EMPTY, "Empty") {}
    cbe_encode_status encode(encoder& encoder);
};

class padding_encoding: public encoding
{
private:
    const int _byte_count;
public:
    padding_encoding(int byte_count): encoding(enc::ENCODE_TYPE_PADDING, byte_count, "Padding"), _byte_count(byte_count) {}
    cbe_encode_status encode(encoder& encoder);
    bool is_equal(const encoding& rhs) const { return get_type() == rhs.get_type() && rhs.has_value(_byte_count); }
    int byte_count() {return _byte_count;}
    bool has_value(int value) const { return _byte_count == value; }
};

class time_encoding: public encoding
{
private:
    const smalltime _value;
public:
    time_encoding(smalltime value): encoding(enc::ENCODE_TYPE_TIME, sizeof(value), enc::to_id_string_time(value)), _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    bool is_equal(const encoding& rhs) const { return get_type() == rhs.get_type() && rhs.has_value(_value); }
    bool has_value(smalltime value) const { return _value == value; }
    smalltime value() {return _value;}
};

class boolean_encoding: public encoding
{
private:
    const bool _value;
public:
    boolean_encoding(bool value): encoding(enc::get_major_type(value), sizeof(value), enc::to_id_string(value)), _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    bool is_equal(const encoding& rhs) const { return get_type() == rhs.get_type() && rhs.has_value(_value); }
    bool has_value(bool value) const           { return _value == value; }
    bool value() {return _value;}
};

// FIXME: number_encoding, int128_encoding, and dfp_encoding are split because
// conversions between decimal floating point and 128-bit integers cause the
// compiler to reference __bid_floattisd and __bid_fixddti from libdfp, which
// is only present on PPC and s390 in Ubuntu :/

template<typename T> class number_encoding: public encoding
{
private:
    const T _value;
public:
    number_encoding(T value): encoding(enc::get_major_type(value), sizeof(value), enc::to_id_string(value)), _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    bool is_equal(const encoding& rhs) const       { return rhs.has_value(_value); }
    bool has_value(int8_t value) const             { return (int8_t)_value == value; }
    bool has_value(int16_t value) const            { return (int16_t)_value == value; }
    bool has_value(int32_t value) const            { return (int32_t)_value == value; }
    bool has_value(int64_t value) const            { return (int64_t)_value == value; }
    bool has_value(__int128 value) const           { return (__int128)_value == value; }
    bool has_value(float value) const              { return (float)_value == value; }
    bool has_value(double value) const             { return (double)_value == value; }
    bool has_value(__float128 value) const         { return (__float128)_value == value; }
    bool has_value(_Decimal32 value) const         { return (_Decimal32)_value == value; }
    bool has_value(_Decimal64 value) const         { return (_Decimal64)_value == value; }
    bool has_value(_Decimal128 value) const        { return (_Decimal128)_value == value; }
    T value() {return _value;}
};

class int128_encoding: public encoding
{
private:
    const __int128 _value;
public:
    int128_encoding(__int128 value): encoding(enc::get_major_type(value), sizeof(value), enc::to_id_string(value)), _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    bool is_equal(const encoding& rhs) const       { return rhs.has_value(_value); }
    bool has_value(int8_t value) const             { return (int8_t)_value == value; }
    bool has_value(int16_t value) const            { return (int16_t)_value == value; }
    bool has_value(int32_t value) const            { return (int32_t)_value == value; }
    bool has_value(int64_t value) const            { return (int64_t)_value == value; }
    bool has_value(__int128 value) const           { return (__int128)_value == value; }
    bool has_value(float value) const              { return (float)_value == value; }
    bool has_value(double value) const             { return (double)_value == value; }
    bool has_value(__float128 value) const         { return (__float128)_value == value; }
    // FIXME: Casting to float instead of decimal to avoid linker error due to
    //        missing libdfp: undefined reference to `__bid_floattisd'
    bool has_value(_Decimal32 value) const         { return (float)_value == value; }
    bool has_value(_Decimal64 value) const         { return (double)_value == value; }
    bool has_value(_Decimal128 value) const        { return (__float128)_value == value; }
    __int128 value() {return _value;}
};

template<typename T> class dfp_encoding: public encoding
{
private:
    const T _value;
public:
    dfp_encoding(T value): encoding(enc::get_major_type(value), sizeof(value), enc::to_id_string(value)), _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    bool is_equal(const encoding& rhs) const       { return rhs.has_value(_value); }
    bool has_value(int8_t value) const             { return (int8_t)_value == value; }
    bool has_value(int16_t value) const            { return (int16_t)_value == value; }
    bool has_value(int32_t value) const            { return (int32_t)_value == value; }
    bool has_value(int64_t value) const            { return (int64_t)_value == value; }
    // FIXME: Casting to __float128 instead of __int128 to avoid linker error due to
    //        missing libdfp: undefined reference to `__bid_fixddti'
    bool has_value(__int128 value) const           { return (__float128)_value == value; }
    bool has_value(float value) const              { return (float)_value == value; }
    bool has_value(double value) const             { return (double)_value == value; }
    bool has_value(__float128 value) const         { return (__float128)_value == value; }
    bool has_value(_Decimal32 value) const         { return (_Decimal32)_value == value; }
    bool has_value(_Decimal64 value) const         { return (_Decimal64)_value == value; }
    bool has_value(_Decimal128 value) const        { return (_Decimal128)_value == value; }
    T value() {return _value;}
};

class string_encoding: public encoding
{
private:
    const std::string _value;
public:
    string_encoding(const std::string& value): encoding(enc::get_major_type(value), 1, enc::to_id_string(value)), _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    bool is_equal(const encoding& rhs) const { return get_type() == rhs.get_type() && rhs.has_value(_value); }
    bool has_value(const std::string& value) const       { return _value == value; }
    std::string value() {return _value;}
};

class binary_encoding: public encoding
{
private:
    const std::vector<uint8_t> _value;
public:
    binary_encoding(const std::vector<uint8_t>& value): encoding(enc::get_major_type(value), 1, enc::to_id_string(value)), _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    bool is_equal(const encoding& rhs) const    { return get_type() == rhs.get_type() && rhs.has_value(_value); }
    bool has_value(const std::vector<uint8_t>& value) const { return _value == value; }
    std::vector<uint8_t> value() {return _value;}
};

class encoder
{
public:
    virtual cbe_encode_status encode(list_encoding& encoding) = 0;
    virtual cbe_encode_status encode(map_encoding& encoding) = 0;
    virtual cbe_encode_status encode(end_container_encoding& encoding) = 0;
    virtual cbe_encode_status encode(empty_encoding& encoding) = 0;
    virtual cbe_encode_status encode(padding_encoding& encoding) = 0;
    virtual cbe_encode_status encode(time_encoding& encoding) = 0;
    virtual cbe_encode_status encode(string_encoding& encoding) = 0;
    virtual cbe_encode_status encode(binary_encoding& encoding) = 0;
    virtual cbe_encode_status encode(boolean_encoding& encoding) = 0;
    virtual cbe_encode_status encode(number_encoding<int8_t>& encoding) = 0;
    virtual cbe_encode_status encode(number_encoding<int16_t>& encoding) = 0;
    virtual cbe_encode_status encode(number_encoding<int32_t>& encoding) = 0;
    virtual cbe_encode_status encode(number_encoding<int64_t>& encoding) = 0;
    virtual cbe_encode_status encode(int128_encoding& encoding) = 0;
    virtual cbe_encode_status encode(number_encoding<float>& encoding) = 0;
    virtual cbe_encode_status encode(number_encoding<double>& encoding) = 0;
    virtual cbe_encode_status encode(number_encoding<__float128>& encoding) = 0;
    virtual cbe_encode_status encode(dfp_encoding<_Decimal32>& encoding) = 0;
    virtual cbe_encode_status encode(dfp_encoding<_Decimal64>& encoding) = 0;
    virtual cbe_encode_status encode(dfp_encoding<_Decimal128>& encoding) = 0;
};


std::shared_ptr<list_encoding>                 list();
std::shared_ptr<map_encoding>                  map();
std::shared_ptr<end_container_encoding>        end();
std::shared_ptr<empty_encoding>                empty();
std::shared_ptr<time_encoding>                 smtime(smalltime value);
std::shared_ptr<time_encoding>                 smtime(int year, int month, int day, int hour, int minute, int second, int usec);
std::shared_ptr<string_encoding>               str(const std::string& value);
std::shared_ptr<binary_encoding>               bin(const std::vector<uint8_t>& value);
std::shared_ptr<boolean_encoding>              bl(bool value);
std::shared_ptr<number_encoding<int8_t>>       i8(int8_t value);
std::shared_ptr<number_encoding<int16_t>>      i16(int16_t value);
std::shared_ptr<number_encoding<int32_t>>      i32(int32_t value);
std::shared_ptr<number_encoding<int64_t>>      i64(int64_t value);
std::shared_ptr<int128_encoding>               i128(__int128 value);
std::shared_ptr<int128_encoding>               i128(int64_t high, uint64_t low);
std::shared_ptr<number_encoding<float>>        f32(float value);
std::shared_ptr<number_encoding<double>>       f64(float value);
std::shared_ptr<number_encoding<__float128>>   f128(float value);
std::shared_ptr<dfp_encoding<_Decimal32>>      d32(_Decimal32 value);
std::shared_ptr<dfp_encoding<_Decimal64>>      d64(_Decimal64 value);
std::shared_ptr<dfp_encoding<_Decimal128>>     d128(_Decimal32 value);
std::shared_ptr<padding_encoding>              pad(int byte_count);

} // namespace enc
