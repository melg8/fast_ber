#pragma once

#include "absl/types/span.h"

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/util/BerView.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"
#include "fast_ber/util/EncodeIdentifiers.hpp"
#include "fast_ber/util/Extract.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>

namespace fast_ber
{

template <typename Identifier = ExplicitIdentifier<UniversalTag::boolean>>
class Boolean
{
  public:
    Boolean() noexcept : Boolean(false) {}
    Boolean(bool val) noexcept { assign(val); }
    explicit Boolean(BerView& view) noexcept { assign_ber(view); }
    explicit Boolean(absl::Span<const uint8_t> ber_data) noexcept { assign_ber(ber_data); }

    // Implicit conversion to bool
         operator bool() const noexcept { return value(); }
    bool value() const noexcept { return static_cast<bool>(m_data[1]); }

    Boolean& operator=(bool rhs) noexcept;
    Boolean& operator=(const Boolean& rhs) noexcept;
    Boolean& operator=(const BerView& rhs) noexcept;
    void     assign(bool val) noexcept;
    void     assign(const Boolean& rhs) noexcept;
    size_t   assign_ber(const BerView& rhs) noexcept;
    size_t   assign_ber(absl::Span<const uint8_t> buffer) noexcept;

    EncodeResult encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept;

  private:
    std::array<uint8_t, 2> m_data;
}; // namespace fast_ber

template <typename Identifier>
constexpr Identifier identifier(const Boolean<Identifier>*, IdentifierAdlToken = IdentifierAdlToken{}) noexcept
{
    return {};
}

template <typename Identifier>
inline Boolean<Identifier>& Boolean<Identifier>::operator=(bool rhs) noexcept
{
    assign(rhs);
    return *this;
}

template <typename Identifier>
inline Boolean<Identifier>& Boolean<Identifier>::operator=(const Boolean<Identifier>& rhs) noexcept
{
    assign(rhs);
    return *this;
}

template <typename Identifier>
inline Boolean<Identifier>& Boolean<Identifier>::operator=(const BerView& rhs) noexcept
{
    assign_ber(rhs);
    return *this;
}

template <typename Identifier>
inline void Boolean<Identifier>::assign(bool val) noexcept
{
    m_data = {0x1, static_cast<uint8_t>(val ? 0xFF : 0x00)};
}

template <typename Identifier>
inline void Boolean<Identifier>::assign(const Boolean& rhs) noexcept
{
    m_data = rhs.m_data;
}

template <typename Identifier>
inline size_t Boolean<Identifier>::assign_ber(const BerView& rhs) noexcept
{
    if (!rhs.is_valid() || rhs.construction() != Construction::primitive || rhs.content_length() != 1)
    {
        return 0;
    }

    m_data = {0x1, static_cast<uint8_t>((*(rhs.content_data()) == 0) ? 0x00 : 0xFF)};
    return 2 + rhs.identifier_length();
}

template <typename Identifier>
inline size_t Boolean<Identifier>::assign_ber(absl::Span<const uint8_t> buffer) noexcept
{
    return assign_ber(BerView(buffer));
}

template <typename Identifier>
inline EncodeResult Boolean<Identifier>::encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept
{
    if (buffer.size() < m_data.size())
    {
        return EncodeResult{false, 0};
    }

    buffer[0] = m_data[0];
    buffer[1] = m_data[1];
    return EncodeResult{true, m_data.size()};
}

template <typename DefaultIdentifier, typename ID = ExplicitIdentifier<UniversalTag::boolean>>
EncodeResult encode(absl::Span<uint8_t> output, const Boolean<DefaultIdentifier>& object, const ID& id = ID{})
{
    return encode_impl(output, object, id);
}

template <typename DefaultIdentifier, typename ID = ExplicitIdentifier<UniversalTag::boolean>>
DecodeResult decode(BerViewIterator& input, Boolean<DefaultIdentifier>& output, const ID& id = {}) noexcept
{
    return decode_impl(input, output, id);
}

} // namespace fast_ber
