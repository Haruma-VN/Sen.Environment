#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::ReflectionObjectNotation
{
    using namespace Definition;

    struct Common
    {
        inline static constexpr auto k_magic_identifier = "RTON"_sv;

        inline static constexpr auto k_done_identifier = "DONE"_sv;

        inline static constexpr auto k_version = 1_ui;

        struct TypeIdentifierEnumeration
        {
            enum class Type : uint8_t
            {
                // boolean
                boolean_false = 0x00_byte,
                boolean_true = 0x01_byte,
                // integer
                integer_signed_8 = 0x08_byte,
                integer_signed_8_zero = 0x09_byte,
                integer_unsigned_8 = 0x0A_byte,
                integer_unsigned_8_zero = 0x0B_byte,
                integer_signed_16 = 0x10_byte,
                integer_signed_16_zero = 0x11_byte,
                integer_unsigned_16 = 0x12_byte,
                integer_unsigned_16_zero = 0x13_byte,
                integer_signed_32 = 0x20_byte,
                integer_signed_32_zero = 0x21_byte,
                integer_unsigned_32 = 0x26_byte,
                integer_unsigned_32_zero = 0x27_byte,
                integer_signed_64 = 0x40_byte,
                integer_signed_64_zero = 0x41_byte,
                integer_unsigned_64 = 0x46_byte,
                integer_unsigned_64_zero = 0x47_byte,
                // floater
                floater_signed_32 = 0x22_byte,
                floater_signed_32_zero = 0x23_byte,
                floater_signed_64 = 0x42_byte,
                floater_signed_64_zero = 0x43_byte,
                // variable length integer
                integer_variable_length_unsigned_32 = 0x24_byte,
                integer_variable_length_signed_32 = 0x25_byte,
                integer_variable_length_unsigned_32_equivalent = 0x28_byte,
                integer_variable_length_unsigned_64 = 0x44_byte,
                integer_variable_length_signed_64 = 0x45_byte,
                integer_variable_length_unsigned_64_equivalent = 0x48_byte,
                // native string
                string_native = 0x81_byte,
                string_native_indexing = 0x90_byte,
                string_native_indexed = 0x91_byte,
                // unicode string
                string_unicode = 0x82_byte,
                string_unicode_indexing = 0x92_byte,
                string_unicode_indexed = 0x93_byte,
                // rtid string
                string_rtid = 0x83_byte,
                string_rtid_null = 0x84_byte,
                // array
                array_begin = 0x86_byte,
                array_size = 0xFD_byte,
                array_end = 0xFE_byte,
                // object
                object_begin = 0x85_byte,
                object_end = 0xFF_byte,
                // never appeared in known rton file
                _string_literal_star = 0x02_byte,
                _string_binary_blob = 0x87_byte,
                _string_native_x1 = 0xB0_byte,
                _string_native_x2 = 0xB1_byte,
                _string_unicode_x1 = 0xB2_byte,
                _string_unicode_x2 = 0xB3_byte,
                _string_native_or_unicode_x1 = 0xB4_byte,
                _string_native_or_unicode_x2 = 0xB5_byte,
                _string_native_or_unicode_x3 = 0xB6_byte,
                _string_native_or_unicode_x4 = 0xB7_byte,
                _object_begin_x1 = 0xB8_byte,
                _array_begin_x1 = 0xB9_byte,
                _string_native_x3 = 0xBA_byte,
                _string_binary_blob_x1 = 0xBB_byte,
                _boolean_x1 = 0xBC_byte
            };
        };


        struct RTIDTypeIdentifierEnumeration {
			enum class Type : uint8_t {
				null  = 0x00_byte,
				uid   = 0x02_byte,
				alias = 0x03_byte
			};
		};


        struct RTIDString {
            inline static constexpr auto null = "RTID(0)";

            inline static constexpr auto uid = "RTID({:d}.{:d}.{:08x}@{:s})"_sv;

            inline static constexpr auto alias = "RTID({:s}@{:s})"_sv;
        };

        inline static constexpr auto k_binary_blob = R"($BINARY("{:s}", {:d}))"_sv;
    };
}