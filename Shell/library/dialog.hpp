#pragma once

#include "deps/tinyfiledialogs/tinyfiledialogs.h"
#include "library/macro.hpp"
#if WINDOWS
#include <windows.h>
#include <codecvt>
#include <ShObjIdl_core.h>
#endif

namespace Sen::Shell::Dialog {

	#if WINDOWS

		#pragma warning disable

		inline auto pick_path (
			bool is_directory,
			bool is_multiply
		) -> std::vector<std::string> {
			auto state_h = HRESULT{};
			auto result = std::vector<std::string>{};
			CoInitialize(nullptr);
			auto dialog = std::add_pointer_t<IFileOpenDialog>{nullptr};
			state_h = CoCreateInstance(__uuidof(FileOpenDialog), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dialog));
			assert_conditional(state_h == S_OK, "state_h == S_OK");
			auto option = FILEOPENDIALOGOPTIONS{};
			state_h = dialog->GetOptions(&option);
			assert_conditional(state_h == S_OK, "state_h == S_OK");
			option |= FOS_FORCEFILESYSTEM | FOS_NODEREFERENCELINKS | FOS_DONTADDTORECENT | FOS_FORCESHOWHIDDEN;
			if (is_directory) {
				option |= FOS_PICKFOLDERS;
			}
			if (is_multiply) {
				option |= FOS_ALLOWMULTISELECT;
			}
			state_h = dialog->SetOptions(option);
			assert_conditional(state_h == S_OK, "state_h == S_OK");
			state_h = dialog->Show(nullptr);
			if (state_h != HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
				assert_conditional(state_h == S_OK, "state_h == S_OK");
				auto selection_item_list = std::add_pointer_t<IShellItemArray>{nullptr};
				state_h = dialog->GetResults(&selection_item_list);
				assert_conditional(state_h == S_OK, "state_h == S_OK");
				auto count = DWORD{0};
				state_h = selection_item_list->GetCount(&count);
				assert_conditional(state_h == S_OK, "state_h == S_OK");
				result.reserve(count);
				for (auto index = DWORD{0}; index < count; ++index) {
					auto item = std::add_pointer_t<IShellItem>{nullptr};
					auto display_name = LPWSTR{nullptr};
					state_h = selection_item_list->GetItemAt(index, &item);
					assert_conditional(state_h == S_OK, "state_h == S_OK");
					state_h = item->GetDisplayName(SIGDN_FILESYSPATH, &display_name);
					assert_conditional(state_h == S_OK, "state_h == S_OK");
					auto display_name_8 = utf16_to_utf8(std::u16string_view{reinterpret_cast<char16_t const *>(display_name)});
					result.emplace_back(std::move(reinterpret_cast<std::string &>(display_name_8)));
					CoTaskMemFree(display_name);
					item->Release();
				}
				selection_item_list->Release();
			}
			dialog->Release();
			for (auto & path : result) {
				for (auto & character : path) {
					if (character == '\\') {
						character = '/';
					}
				}
			}
			return result;
		}
	#endif

}