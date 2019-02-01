#pragma once

#include <cstddef>

#include "fast_ber/ber_types/Identifier.hpp"

#include "absl/types/span.h"

namespace fast_ber
{

class Boolean;
class Integer;
class Null;
class OctetString;

struct EncodeResult
{
    bool   success;
    size_t length;
};

inline EncodeResult wrap_with_ber_header(absl::Span<uint8_t> output, Class class_, Tag tag,
                                         size_t content_length) noexcept
{
    std::array<uint8_t, 30> buffer;
    size_t header_length = create_header(absl::MakeSpan(buffer.data(), buffer.size()), Construction::constructed,
                                         class_, tag, content_length);
    assert(header_length != 0);
    if (header_length + content_length > output.length())
    {
        return EncodeResult{false, 0};
    }

    std::memmove(output.data() + header_length, output.data(), content_length);
    std::memcpy(output.data(), buffer.data(), header_length);
    return EncodeResult{true, header_length + content_length};
}

template <typename T, UniversalTag T2>
EncodeResult encode_impl(absl::Span<uint8_t> output, const T& object, const ExplicitIdentifier<T2>&)
{
    if (output.empty())
    {
        return EncodeResult{false, 0};
    }
    constexpr auto tag_num    = val(ExplicitIdentifier<T2>::tag());
    constexpr auto class_     = ExplicitIdentifier<T2>::class_();
    constexpr auto id_length  = 1;
    constexpr auto encoded_id = create_short_identifier(Construction::primitive, class_, tag_num);
    static_assert(tag_num < 31, "Tag must be short form!");

    output[0] = encoded_id;
    output.remove_prefix(id_length);

    EncodeResult encode_res = object.encode_content_and_length(output);
    encode_res.length += id_length;
    return encode_res;
}

template <typename T, Class T2, Tag T3, typename T4>
EncodeResult encode_impl(absl::Span<uint8_t> output, const T& object, const TaggedExplicitIdentifier<T2, T3, T4>& id)
{
    EncodeResult inner_encoding = encode(output, object, id.inner_id());
    if (!inner_encoding.success)
    {
        return EncodeResult{false, 0};
    }

    return wrap_with_ber_header(output, id.outer_class(), id.outer_tag(), inner_encoding.length);
}

template <typename T, Class T2, Tag T3>
EncodeResult encode_impl(absl::Span<uint8_t> output, const T& object, const ImplicitIdentifier<T2, T3>& id)
{
    size_t id_length = create_identifier(output, Construction::primitive, id.class_(), id.tag());
    if (id_length == 0)
    {
        return EncodeResult{false, 0};
    }
    assert(id_length <= output.size());

    output.remove_prefix(id_length);

    EncodeResult encode_res = object.encode_content_and_length(output);
    encode_res.length += id_length;
    return encode_res;
}

template <typename ID = ExplicitIdentifier<UniversalTag::integer>>
EncodeResult encode(absl::Span<uint8_t> output, const Integer& object, const ID& id = ID{})
{
    return encode_impl(output, object, id);
}

template <typename ID = ExplicitIdentifier<UniversalTag::octet_string>>
EncodeResult encode(absl::Span<uint8_t> output, const OctetString& object, const ID& id = ID{})
{
    return encode_impl(output, object, id);
}

template <typename ID = ExplicitIdentifier<UniversalTag::boolean>>
EncodeResult encode(absl::Span<uint8_t> output, const Boolean& object, const ID& id = ID{})
{
    return encode_impl(output, object, id);
}

template <typename ID = ExplicitIdentifier<UniversalTag::null>>
EncodeResult encode(absl::Span<uint8_t> output, const Null& object, const ID& id = ID{})
{
    return encode_impl(output, object, id);
}

} // namespace fast_ber
