#include "core/SearchEngine.h"

#include <cwctype>

namespace advanced_alt_tab {

namespace {

wchar_t lower(wchar_t value) {
    return static_cast<wchar_t>(std::towlower(value));
}

} // namespace

WindowList SearchEngine::filter(const WindowList& windows, const std::wstring& query) const {
    if (query.empty()) {
        return windows;
    }

    WindowList matches;
    for (const auto& window : windows) {
        if (isSubsequenceMatch(query, window.title)) {
            matches.push_back(window);
        }
    }

    return matches;
}

bool SearchEngine::isSubsequenceMatch(std::wstring_view query, std::wstring_view text) {
    size_t queryIndex = 0;

    for (const wchar_t character : text) {
        if (queryIndex < query.size() && lower(character) == lower(query[queryIndex])) {
            ++queryIndex;
        }
    }

    return queryIndex == query.size();
}

} // namespace advanced_alt_tab
