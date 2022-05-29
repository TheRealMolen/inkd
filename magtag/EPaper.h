#pragma once

#include <algorithm>
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
    static constexpr uint16_t ScreenBytes = NumPixels / 8;

public:
    using ScreenMode = magtag::ScreenMode;

    EPaperDisplay();
    ~EPaperDisplay();

    void clear();
    
    void drawBlart();
    inline void plot(uint8_t x, uint16_t y, uint8_t col);

    void redrawScreen() const;

    void setScreenMode(ScreenMode mode)   { m_screenMode = mode; }

private:
    void init();

    bool        m_needToInit = true;
    ScreenMode  m_screenMode = magtag::Grey4;

    uint8_t     m_dirtyMinX = 0xff;
    uint8_t     m_dirtyMaxX = 0;
    uint16_t    m_dirtyMinY = 0xffff;
    uint16_t    m_dirtyMaxY = 0;
    
    // the two bitplanes for the screen data
    uint8_t     m_plane0[ScreenBytes];
    uint8_t     m_plane1[ScreenBytes];
};


void EPaperDisplay::plot(uint8_t x, uint16_t y, uint8_t col)
{
  uint8_t xbit = x & 7;
  uint8_t x8 = x / 8;
  uint8_t mask = 1 << xbit;

  uint32_t ix = x8 + y * (ScreenWidth / 8);

  if (col & 1)
    m_plane0[ix] |= mask;
  else
    m_plane0[ix] &= ~mask;
    
  if (col & 2)
    m_plane1[ix] |= mask;
  else
    m_plane1[ix] &= ~mask;

  m_dirtyMinX = std::min<uint8_t>(m_dirtyMinX, (x & ~7));
  m_dirtyMaxX = std::max<uint8_t>(m_dirtyMaxX, (x & ~7) + 8);
  m_dirtyMinY = std::min<uint16_t>(m_dirtyMinY, y);
  m_dirtyMaxY = std::max<uint16_t>(m_dirtyMaxY, y + 1);
}
