#ifndef THEMES_HPP
#define THEMES_HPP

#include <cstdint>

namespace gruvbox {
// Colors use packed 0xRRGGBB values.
constexpr std::uint32_t bg = 0x282828;
constexpr std::uint32_t fg = 0xebdbb2;

constexpr std::uint32_t black = 0x3c3836;
constexpr std::uint32_t red = 0xcc241d;
constexpr std::uint32_t green = 0x98971a;
constexpr std::uint32_t yellow = 0xd79921;
constexpr std::uint32_t blue = 0x458588;
constexpr std::uint32_t purple = 0xb16286;
constexpr std::uint32_t aqua = 0x689d6a;
constexpr std::uint32_t gray = 0x928374;

constexpr std::uint32_t bright_red = 0xfb4934;
constexpr std::uint32_t bright_green = 0xb8bb26;
constexpr std::uint32_t bright_yellow = 0xfabd2f;
constexpr std::uint32_t bright_blue = 0x83a598;
constexpr std::uint32_t orange = 0xd65d0e;
} // namespace gruvbox
#endif
