#pragma once

#include "fast_ber/compiler/CppGeneration.hpp"

#include "absl/memory/memory.h"
#include "absl/types/optional.h"
#include "absl/types/variant.h"

#include <algorithm>
#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum class TaggingMode
{
    explicit_,
    implicit,
    automatic
};

enum class Class
{
    universal,
    application,
    context_specific,
    private_,
};

struct ComponentType;
struct TaggedType;
using ComponentTypeList = std::vector<ComponentType>;

struct NamedType;
struct NamedNumber
{
    std::string name;
    int64_t     number;
};
struct EnumerationValue
{
    std::string             name;
    absl::optional<int64_t> value;
};

struct AnyType
{
};
struct BitStringType
{
};
struct BooleanType
{
};
struct CharacterStringType
{
};
struct ChoiceType;
struct DateType
{
};
struct DateTimeType
{
};
struct DurationType
{
};
struct EmbeddedPDVType
{
};
struct EnumeratedType
{
    std::vector<EnumerationValue> enum_values;
    bool                          accept_anything;
};
struct ExternalType
{
};
struct GeneralizedTimeType
{
};
struct InstanceOfType
{
};
struct IntegerType
{
    std::vector<NamedNumber> named_numbers;
};
struct IRIType
{
};
struct NullType
{
};
struct ObjectClassFieldType;
struct ObjectDescriptorType
{
};
struct ObjectIdentifierType
{
};
struct OctetStringType
{
};
struct RealType
{
};
struct RelativeIRIType
{
};
struct RelativeOIDType
{
};
struct SequenceType
{
    ComponentTypeList components;
};
struct SequenceOfType;
struct SetType
{
    ComponentTypeList components;
};
struct SetOfType;

// unique ptr used to avoid circular reference on stack
struct PrefixedType
{
    PrefixedType() = default;
    PrefixedType(const TaggedType& type) : tagged_type(absl::make_unique<TaggedType>(type)) {}
    PrefixedType(const PrefixedType& rhs)
    {
        if (rhs.tagged_type)
        {
            tagged_type = absl::make_unique<TaggedType>(*rhs.tagged_type);
        }
    }
    PrefixedType& operator=(const PrefixedType& rhs)
    {
        if (rhs.tagged_type)
        {
            tagged_type = absl::make_unique<TaggedType>(*rhs.tagged_type);
        }
        return *this;
    }
    std::unique_ptr<TaggedType> tagged_type;
};
struct TimeType
{
};
struct TimeOfDayType
{
};
struct UTCTimeType
{
};
struct DefinedType;

using BuiltinType =
    absl::variant<AnyType, BitStringType, BooleanType, CharacterStringType, ChoiceType, DateType, DateTimeType,
                  DurationType, EmbeddedPDVType, EnumeratedType, ExternalType, GeneralizedTimeType, InstanceOfType,
                  IntegerType, IRIType, NullType, ObjectClassFieldType, ObjectDescriptorType, ObjectIdentifierType,
                  OctetStringType, RealType, RelativeIRIType, RelativeOIDType, SequenceType, SequenceOfType, SetType,
                  SetOfType, PrefixedType, TimeType, TimeOfDayType, UTCTimeType>;
using Type = absl::variant<BuiltinType, DefinedType>;

struct DefinedType
{
    absl::optional<std::string> module_reference;
    std::string                 type_reference;
    std::vector<Type>           parameters;
};
struct SequenceOfType
{
    // Unique pointers used to prevent circular references
    bool                       has_name;
    std::unique_ptr<NamedType> named_type;
    std::unique_ptr<Type>      type;

    SequenceOfType() = default;
    SequenceOfType(bool, std::unique_ptr<NamedType>&&, std::unique_ptr<Type>&&);
    SequenceOfType(const SequenceOfType& rhs);
    SequenceOfType& operator=(const SequenceOfType& rhs);
};
struct SetOfType
{
    // Unique pointers used to prevent circular references
    bool                       has_name;
    std::unique_ptr<NamedType> named_type;
    std::unique_ptr<Type>      type;

    SetOfType() = default;
    SetOfType(bool, std::unique_ptr<NamedType>&&, std::unique_ptr<Type>&&);
    SetOfType(const SetOfType& rhs);
    SetOfType& operator=(const SetOfType& rhs);
};

struct ChoiceType
{
    std::vector<NamedType> choices;
};

struct DefinedValue
{
    std::string reference;
};

struct ObjectClassFieldType
{
    DefinedType              referenced_object_class;
    std::vector<std::string> fieldnames;
};

struct BitStringValue
{
    std::string value;
};

struct HexStringValue
{
    std::string value;
};

struct CharStringValue
{
    std::string value;
};

struct Value
{
    absl::variant<std::vector<Value>, int64_t, std::string, NamedNumber, BitStringValue, HexStringValue,
                  CharStringValue, DefinedValue>
        value_selection;
};

struct NamedType
{
    std::string name;
    Type        type;
};

struct NamedValue
{
    std::string name;
    Value       type;
};

struct ComponentType
{
    NamedType             named_type;
    bool                  is_optional;
    absl::optional<Value> value;
    absl::optional<Type>  components_of;
};

struct Tag
{
    Class class_;
    int   tag_number;
};

struct TaggedType
{
    Tag         tag;
    TaggingMode tagging_mode;
    Type        type;
};

struct TypeAssignment
{
    Type type;
};

struct ValueAssignment
{
    Type  type;
    Value value;
};

struct TypeField
{
};

struct FixedTypeValueField
{
    Type type;
};

struct ClassField
{
    std::string                                   name;
    absl::variant<TypeField, FixedTypeValueField> field;
};

struct ObjectClassAssignment
{
    std::vector<ClassField> fields;
};

struct ObjectSetAssignment
{
};

struct Parameter
{
    absl::optional<Type> governor;
    std::string          reference;
};

struct Assignment
{
    std::string                                                                                name;
    absl::variant<TypeAssignment, ValueAssignment, ObjectClassAssignment, ObjectSetAssignment> specific;
    std::vector<std::string>                                                                   depends_on;
    std::vector<Parameter>                                                                     parameters;
};

struct Import
{
    std::string              module_reference;
    std::vector<std::string> imports;
};

struct Export
{
};

struct Module
{
    std::string             module_reference;
    TaggingMode             tagging_default;
    std::vector<Export>     exports;
    std::vector<Import>     imports;
    std::vector<Assignment> assignments;
};

struct Asn1Tree
{
    std::vector<Module> modules;
};

struct TaggingInfo
{
    std::string tag;
    bool        is_default_tagged;
};

struct ObjectIdComponentValue
{
    absl::optional<std::string> name;
    absl::optional<int64_t>     value;
};

struct ObjectIdComponents
{
    ObjectIdComponents(const Value& value)
    {
        if (!absl::holds_alternative<std::vector<Value>>(value.value_selection))
        {
            throw std::runtime_error("Failed to interpret value as object identifier");
        }
        const std::vector<Value>& value_list = absl::get<std::vector<Value>>(value.value_selection);
        components.reserve(value_list.size());
        for (const Value& component : value_list)
        {
            if (absl::holds_alternative<DefinedValue>(component.value_selection))
            {
                const std::string& name = absl::get<DefinedValue>(component.value_selection).reference;
                components.push_back(ObjectIdComponentValue{name, absl::nullopt});
            }
            else if (absl::holds_alternative<std::string>(component.value_selection))
            {
                const std::string& name = absl::get<std::string>(component.value_selection);
                components.push_back(ObjectIdComponentValue{name, absl::nullopt});
            }
            else if (absl::holds_alternative<int64_t>(component.value_selection))
            {
                const int64_t& number = absl::get<int64_t>(component.value_selection);
                components.push_back(ObjectIdComponentValue{absl::nullopt, number});
            }
            else if (absl::holds_alternative<NamedNumber>(component.value_selection))
            {
                const NamedNumber& named_number = absl::get<NamedNumber>(component.value_selection);
                components.push_back(ObjectIdComponentValue{named_number.name, named_number.number});
            }
            else
            {
                throw std::runtime_error("Failed to interpret value as object identifier");
            }
        }
    }
    std::vector<ObjectIdComponentValue> components;
};

std::string to_string(Class class_);

// Switch asn '-' for C++ '_'
// Rename any names which are reserved in C++
std::string santize_name(const std::string& name);

std::string make_type_optional(const std::string& type);

bool is_bit_string(const Type& type);
bool is_set(const Type& type);
bool is_sequence(const Type& type);
bool is_set_of(const Type& type);
bool is_sequence_of(const Type& type);
bool is_enumerated(const Type& type);
bool is_choice(const Type& type);
bool is_prefixed(const Type& type);
bool is_integer(const Type& type);
bool is_oid(const Type& type);
bool is_defined(const Type& type);

struct Context;
