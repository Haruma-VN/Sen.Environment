#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::RenderEffects
{

	struct Common {

		protected: 

			inline static constexpr auto magic = std::string_view{"xfcp"};

            inline static constexpr auto version = 0x01_byte;

            inline static constexpr auto Block1SectionHeadOffset = 0x08_byte;

            inline static constexpr auto Block1SectionSize = 0x18_byte;

            inline static constexpr auto Block2SectionHeadOffset = 0x14_byte;

            inline static constexpr auto Block2SectionSize = 0x08_byte; // 0x08 && 0x0C

            inline static constexpr auto Block3SectionHeadOffset = 0x20_byte;

            inline static constexpr auto Block3SectionSize = 0x0C_byte;

            inline static constexpr auto Block4SectionHeadOffset = 0x2C_byte;

            inline static constexpr auto Block4SectionSize = 0x14_byte;

            inline static constexpr auto Block5SectionHeadOffset = 0x38_byte;

            inline static constexpr auto Block5SectionSize = 0x1C_byte;

            inline static constexpr auto Block6SectionHeadOffset = 0x44_byte;

            inline static constexpr auto Block6SectionSize = 0x14_byte;

            inline static constexpr auto Block7SectionHeadOffset = 0x50_byte;

            inline static constexpr auto Block7SectionSize = 0x08_byte;

            inline static constexpr auto Block8SectionHeadOffset = 0x5C_byte;

            inline static constexpr auto Block8SectionSize = 0x14_byte; // 0x0C && 0x14

            inline static constexpr auto BlockSectionOffset = 0x6Cl;

            inline static constexpr auto k_none_size = 0;

        public:
        
            uint32_t block1_size;

            uint32_t block1_section_offset;

            uint32_t block2_size;

            uint32_t block2_section_offset;

            uint32_t block3_size;

            uint32_t block3_section_offset;

            uint32_t block4_size;

            uint32_t block4_section_offset;

            uint32_t block5_size;

            uint32_t block5_section_offset;

            uint32_t block6_size;

            uint32_t block6_section_offset;

            uint32_t block7_size;

            uint32_t block7_section_offset;

            uint32_t block8_size;

            uint32_t block8_section_offset;

            uint32_t string_section_offset;

            explicit constexpr Common(

			) = default;


            explicit constexpr Common(
                uint32_t block1_size,
                uint32_t block1_section_offset,
                uint32_t block2_size,
                uint32_t block2section_offset,
                uint32_t block3_size,
                uint32_t block3_section_offset,
                uint32_t block4_size,
                uint32_t block4_section_offset,
                uint32_t block5_size,
                uint32_t block5_section_offset,
                uint32_t block6_size,
                uint32_t block6_section_offset,
                uint32_t block7_size,
                uint32_t block7_section_offset,
                uint32_t block8_size,
                uint32_t block8_section_offset,
                uint32_t string_section_offset
            ) : block1_size(block1_size), block1_section_offset(block1_section_offset), block2_size(block2_size), block2_section_offset(block2_section_offset), block3_size(block3_size), block3_section_offset(block3_section_offset), block4_size(block4_size), block4_section_offset(block4_section_offset), block5_size(block5_size), block5_section_offset(block5_section_offset), block6_size(block6_size), block6_section_offset(block6_section_offset), block7_size(block7_size), block7_section_offset(block7_section_offset), block8_size(block8_size), block8_section_offset(block8_section_offset) ,string_section_offset(string_section_offset)
            {

            }

			~Common(

			) = default;
	};
}
