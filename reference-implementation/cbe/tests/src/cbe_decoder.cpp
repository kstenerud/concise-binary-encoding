#include "cbe_decoder.h"

// #define KSLogger_LocalLevel TRACE
#include "kslogger.h"
#include "test_utils.h"

#define MAX_CONTAINER_DEPTH 500

static cbe_decoder* get_decoder(struct cbe_decode_process* process)
{
    return (cbe_decoder*)cbe_decode_get_user_context(process);
}

static bool on_nil(struct cbe_decode_process* process)
{
    return get_decoder(process)->set_next(enc::nil());
}

static bool on_boolean(struct cbe_decode_process* process, bool value)
{
    return get_decoder(process)->set_next(enc::bl(value));
}

static bool on_int_8(struct cbe_decode_process* process, int8_t value)
{
    return get_decoder(process)->set_next(enc::i8(value));
}

static bool on_int_16(struct cbe_decode_process* process, int16_t value)
{
    return get_decoder(process)->set_next(enc::i16(value));
}

static bool on_int_32(struct cbe_decode_process* process, int32_t value)
{
    return get_decoder(process)->set_next(enc::i32(value));
}

static bool on_int_64(struct cbe_decode_process* process, int64_t value)
{
    return get_decoder(process)->set_next(enc::i64(value));
}

static bool on_int_128(struct cbe_decode_process* process, __int128 value)
{
    return get_decoder(process)->set_next(enc::i128(value));
}

static bool on_float_32(struct cbe_decode_process* process, float value)
{
    return get_decoder(process)->set_next(enc::f32(value));
}

static bool on_float_64(struct cbe_decode_process* process, double value)
{
    return get_decoder(process)->set_next(enc::f64(value));
}

static bool on_float_128(struct cbe_decode_process* process, __float128 value)
{
    return get_decoder(process)->set_next(enc::f128(value));
}

static bool on_decimal_32(struct cbe_decode_process* process, _Decimal32 value)
{
    return get_decoder(process)->set_next(enc::d32(value));
}

static bool on_decimal_64(struct cbe_decode_process* process, _Decimal64 value)
{
    return get_decoder(process)->set_next(enc::d64(value));
}

static bool on_decimal_128(struct cbe_decode_process* process, _Decimal128 value)
{
    return get_decoder(process)->set_next(enc::d128(value));
}

static bool on_time(struct cbe_decode_process* process, smalltime value)
{
    return get_decoder(process)->set_next(enc::smtime(value));
}

static bool on_list_begin(struct cbe_decode_process* process)
{
    return get_decoder(process)->set_next(enc::list());
}

static bool on_list_end(struct cbe_decode_process* process)
{
    return get_decoder(process)->set_next(enc::end());
}

static bool on_map_begin(struct cbe_decode_process* process)
{
    return get_decoder(process)->set_next(enc::map());
}

static bool on_map_end(struct cbe_decode_process* process)
{
    return get_decoder(process)->set_next(enc::end());
}

static bool on_string_begin(struct cbe_decode_process* process, int64_t byte_count)
{
    return get_decoder(process)->string_begin(byte_count);
}

static bool on_binary_begin(struct cbe_decode_process* process, int64_t byte_count)
{
    return get_decoder(process)->binary_begin(byte_count);
}

static bool on_data(struct cbe_decode_process* process,
                     const uint8_t* start,
                     int64_t byte_count)
{
    return get_decoder(process)->add_data(std::vector<uint8_t>(start, start + byte_count));
}


static const cbe_decode_callbacks g_callbacks =
{
    on_nil: on_nil,
    on_boolean: on_boolean,
    on_int_8: on_int_8,
    on_int_16: on_int_16,
    on_int_32: on_int_32,
    on_int_64: on_int_64,
    on_int_128: on_int_128,
    on_float_32: on_float_32,
    on_float_64: on_float_64,
    on_float_128: on_float_128,
    on_decimal_32: on_decimal_32,
    on_decimal_64: on_decimal_64,
    on_decimal_128: on_decimal_128,
    on_time: on_time,
    on_list_begin: on_list_begin,
    on_list_end: on_list_end,
    on_map_begin: on_map_begin,
    on_map_end: on_map_end,
    on_string_begin: on_string_begin,
    on_binary_begin: on_binary_begin,
    on_data: on_data,
};

cbe_decoder::cbe_decoder()
: _process_backing_store(cbe_decode_process_size(MAX_CONTAINER_DEPTH))
, _process((cbe_decode_process*)_process_backing_store.data())
{
}

std::vector<uint8_t>& cbe_decoder::received_data()
{
    return _received_data;
}

std::shared_ptr<enc::encoding> cbe_decoder::decoded()
{
    return _decoded;
}

cbe_decode_status cbe_decoder::feed(std::vector<uint8_t>& data)
{
    KSLOG_DEBUG("Feeding %d bytes", data.size());
    KSLOG_TRACE("Feeding %s", as_string(data).c_str());
    _received_data.insert(_received_data.begin(), data.begin(), data.end());
    cbe_decode_status status = cbe_decode_feed(_process, data.data(), data.size());
    _read_offset += cbe_decode_get_buffer_offset(_process);
    return status;
}

cbe_decode_status cbe_decoder::begin()
{
    return cbe_decode_begin(_process, &g_callbacks, MAX_CONTAINER_DEPTH, (void*)this);
}

cbe_decode_status cbe_decoder::end()
{
    if(!_process_is_valid)
    {
        KSLOG_ERROR("Called end() too many times");
        return (cbe_decode_status)9999999;
    }
    _process_is_valid = false;
    return cbe_decode_end(_process);
}

cbe_decode_status cbe_decoder::decode(std::vector<uint8_t>& document)
{
    return cbe_decode(&g_callbacks, (void*)this, MAX_CONTAINER_DEPTH, document.data(), document.size());
}

bool cbe_decoder::set_next(std::shared_ptr<enc::encoding> encoding)
{
    if(_currently_decoding_type != CBE_DECODING_OTHER)
    {
        KSLOG_ERROR("Expected _currently_decoding_type OTHER, not %d", _currently_decoding_type);
        return false;
    }
    if(_decoded)
    {
        _decoded->set_next(encoding);
    }
    else
    {
        _decoded = encoding;
    }
    return true;
}

bool cbe_decoder::string_begin(int64_t byte_count)
{
    if(_currently_decoding_type != CBE_DECODING_OTHER)
    {
        KSLOG_ERROR("Expected _currently_decoding_type OTHER, not %d", _currently_decoding_type);
        return false;
    }
    _currently_decoding_type = CBE_DECODING_STRING;
    _currently_decoding_length = byte_count;
    _currently_decoding_offset = 0;
    _currently_decoding_data.clear();
    return true;
}

bool cbe_decoder::binary_begin(int64_t byte_count)
{
    if(_currently_decoding_type != CBE_DECODING_OTHER)
    {
        KSLOG_ERROR("Expected _currently_decoding_type OTHER, not %d", _currently_decoding_type);
        return false;
    }
    _currently_decoding_type = CBE_DECODING_BINARY;
    _currently_decoding_length = byte_count;
    _currently_decoding_offset = 0;
    _currently_decoding_data.clear();
    return true;
}

bool cbe_decoder::add_data(const std::vector<uint8_t>& data)
{
    KSLOG_DEBUG("Add data %d bytes", data.size());
    if(_currently_decoding_type == CBE_DECODING_OTHER)
    {
        KSLOG_ERROR("Expecting _currently_decoding_type DATA, not OTHER");
        return false;
    }

    if(_currently_decoding_offset + (int64_t)data.size() > _currently_decoding_length)
    {
        KSLOG_ERROR("_currently_decoding_offset + data.size() (%d) > _currently_decoding_length (%d)",
            _currently_decoding_offset + (int64_t)data.size(), _currently_decoding_length);
        return false;
    }

    _currently_decoding_data.insert(_currently_decoding_data.end(), data.begin(), data.end());
    _currently_decoding_offset += data.size();

    if(_currently_decoding_offset == _currently_decoding_length)
    {
        cbe_decoding_type current_type = _currently_decoding_type;
        _currently_decoding_type = CBE_DECODING_OTHER;

        if(current_type == CBE_DECODING_BINARY)
        {
            return set_next(enc::bin(_currently_decoding_data));
        }
        else
        {
            return set_next(enc::str(std::string(_currently_decoding_data.begin(), _currently_decoding_data.end())));
        }
    }

    return true;
}
