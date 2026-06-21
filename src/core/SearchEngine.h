#pragma once

#include "core/WindowInfo.h"

#include <string>

namespace advanced_alt_tab {

class SearchEngine {
public:
    WindowList filter(const WindowList& windows, const std::wstring& query) const;

private:
    static bool isSubsequenceMatch(std::wstring_view query, std::wstring_view text);
};

} // namespace advanced_alt_tab
