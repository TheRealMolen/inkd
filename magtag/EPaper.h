#pragma once

#include <cstdint>


namespace magtag
{
constexpr uint16_t ScreenWidth = 128;
constexpr uint16_t ScreenHeight = 296;

enum ScreenMode { Mono, Grey4 };
}


class EPaperDisplay
{
    static constexpr uint16_t ScreenWidth = magtag::ScreenWidth;
    static constexpr uint16_t ScreenHeight = magtag::ScreenHeight;
    static constexpr uint16_t NumPixels = ScreenWidth * ScreenHeight;
    static constexpr uint16_t BufferBytes = NumPixels / 8;

public:
    EPaperDisplay();
    ~EPaperDisplay();

    void clear();
    void drawBlart();

    void redrawScreen() const;

private:
    // the two bitplanes for the screen data
    uint8_t     m_plane0[BufferBytes];
    uint8_t     m_plane1[BufferBytes];
};

