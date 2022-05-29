/***************************************************
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#define protected public
#include "Adafruit_ThinkInk.h"

ThinkInk_290_Grayscale4_T5 display(EPD_DC, EPD_RESET, EPD_CS, -1, -1);

#define COLOR1 EPD_BLACK
#define COLOR2 EPD_LIGHT
#define COLOR3 EPD_DARK



namespace magtag
{
constexpr uint16_t ScreenWidth = 128;
constexpr uint16_t ScreenHeight = 296;

enum ScreenMode { Mono, Grey4 };

namespace pin
{
static constexpr uint8_t Reset = 6;
static constexpr uint8_t DataSelect = 7;   // HIGH => the SPI transfer is data, LOW => command
static constexpr uint8_t ChipSelect = 8;
}

// datasheet: https://v4.cecdn.yun300.cn/100001_1909185148/GDEW029T5D.pdf
namespace cmd
{
static constexpr uint8_t PanelSetting = 0x00;
static constexpr uint8_t PowerSetting = 0x01;
static constexpr uint8_t PowerOff = 0x02;
static constexpr uint8_t PowerOffSequenceSetting = 0x03;
static constexpr uint8_t PowerOn = 0x04;
static constexpr uint8_t PowerOnMeasure = 0x05;
static constexpr uint8_t BoosterSoftStart = 0x06;
static constexpr uint8_t DeepSleep = 0x07;
static constexpr uint8_t SendPlane0 = 0x10;   // aka DTM1
static constexpr uint8_t DataStop = 0x11;
static constexpr uint8_t DisplayRefresh = 0x12;
static constexpr uint8_t SendPlane1 = 0x13;   // aka DTM2
static constexpr uint8_t VComLut = 0x20;
static constexpr uint8_t W2WLut = 0x21;
static constexpr uint8_t B2WLut = 0x22;
static constexpr uint8_t W2BLut = 0x23;
static constexpr uint8_t B2BLut = 0x24;
static constexpr uint8_t PLLControl = 0x30;
static constexpr uint8_t IntervalSetting = 0x50;    // aka CDI
static constexpr uint8_t ResolutionSetting = 0x61;
static constexpr uint8_t GetStatus = 0x71;
static constexpr uint8_t VcmDcSetting = 0x82;
}

static constexpr uint16_t DisplayRefreshDelay = 1000;  // ms


// these are the LUTs calibrated for the screens used in the magtag
// taken from ThinkInk_290_Grayscale4_T5.h from Adafruit's EPD Arduino library
constexpr uint8_t VcomLut_Mono[] = {
  0x00, 0x01, 0x0E, 0x00, 0x00, 0x01,  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00,
};
constexpr uint8_t W2WLut_Mono[] = {
  0x00, 0x01, 0x0E, 0x00, 0x00, 0x01, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
constexpr uint8_t B2WLut_Mono[] = {
  0x20, 0x01, 0x0E, 0x00, 0x00, 0x01, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
constexpr uint8_t W2BLut_Mono[] = {
  0x10, 0x01, 0x0E, 0x00, 0x00, 0x01, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
constexpr uint8_t B2BLut_Mono[] = {
  0x00, 0x01, 0x0E, 0x00, 0x00, 0x01, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

constexpr uint8_t VcomLut_Grey[] = {
  0x00, 0x0A, 0x00, 0x00, 0x00, 0x01,
  0x60, 0x14, 0x14, 0x00, 0x00, 0x01,
  0x00, 0x14, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x13, 0x0A, 0x01, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00,
  // NOTE: docs say this should be a 45B command, but then suggest that it should really be 44B?
};
constexpr uint8_t W2WLut_Grey[] = {
  0x40, 0x0A, 0x00, 0x00, 0x00, 0x01,
  0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
  0x10, 0x14, 0x0A, 0x00, 0x00, 0x01,
  0xA0, 0x13, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
constexpr uint8_t B2WLut_Grey[] = {
  0x40, 0x0A, 0x00, 0x00, 0x00, 0x01,
  0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
  0x00, 0x14, 0x0A, 0x00, 0x00, 0x01,
  0x99, 0x0C, 0x01, 0x03, 0x04, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
constexpr uint8_t W2BLut_Grey[] = {
  0x40, 0x0A, 0x00, 0x00, 0x00, 0x01,
  0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
  0x00, 0x14, 0x0A, 0x00, 0x00, 0x01,
  0x99, 0x0B, 0x04, 0x04, 0x01, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
constexpr uint8_t B2BLut_Grey[] = {
  0x80, 0x0A, 0x00, 0x00, 0x00, 0x01,
  0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
  0x20, 0x14, 0x0A, 0x00, 0x00, 0x01,
  0x50, 0x13, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


// ------ S P I   L I N E S ------

SPISettings spiSetting(4 * 1000 * 1000, MSBFIRST, SPI_MODE0);

inline void beginSpiCmd()
{
  digitalWrite(pin::ChipSelect, 1);
  digitalWrite(pin::DataSelect, 0);
  
  SPI.beginTransaction(spiSetting);
  digitalWrite(pin::ChipSelect, 0);
}
inline void contSpiData()   // call after beginSpiCmd to mark the start of data
{
  digitalWrite(pin::DataSelect, 1);
}
inline void endSpi()
{
  digitalWrite(pin::ChipSelect, 1);
  SPI.endTransaction();
}

inline uint8_t sendSpiByte(const uint8_t data)
{
  return SPI.transfer(data);
}

// ------ C O M M A N D S ------

uint8_t sendCommand(const uint8_t cmd)
{
  Serial.print("CMD0 ");
  Serial.println(cmd, 16);
  beginSpiCmd();
  uint8_t result = sendSpiByte(cmd);
  endSpi();

  return result;
}
void sendCommand(const uint8_t cmd, const uint8_t arg0)
{
  Serial.print("CMD1 ");
  Serial.print(cmd, 16);
  Serial.print(' ');
  Serial.print(arg0, 16);
  Serial.println();

  beginSpiCmd();
  sendSpiByte(cmd);

  contSpiData();
  sendSpiByte(arg0);

  endSpi();
}
void sendCommand(const uint8_t cmd, const uint8_t arg0, const uint8_t arg1)
{
  Serial.print("CMD2 ");
  Serial.print(cmd, 16);
  Serial.print(' ');
  Serial.print(arg0, 16);
  Serial.print(' ');
  Serial.print(arg1, 16);
  Serial.println();

  beginSpiCmd();
  sendSpiByte(cmd);

  contSpiData();
  sendSpiByte(arg0);
  sendSpiByte(arg1);

  endSpi();
}
void sendCommand(const uint8_t cmd, const uint8_t arg0, const uint8_t arg1, const uint8_t arg2)
{
  Serial.print("CMD3 ");
  Serial.print(cmd, 16);
  Serial.print(' ');
  Serial.print(arg0, 16);
  Serial.print(' ');
  Serial.print(arg1, 16);
  Serial.print(' ');
  Serial.print(arg2, 16);
  Serial.println();

  beginSpiCmd();
  sendSpiByte(cmd);

  contSpiData();
  sendSpiByte(arg0);
  sendSpiByte(arg1);
  sendSpiByte(arg2);

  endSpi();
}
void sendCommand(const uint8_t cmd, const uint8_t arg0, const uint8_t arg1, const uint8_t arg2, const uint8_t arg3)
{
  Serial.print("CMD4 ");
  Serial.print(cmd, 16);
  Serial.print(' ');
  Serial.print(arg0, 16);
  Serial.print(' ');
  Serial.print(arg1, 16);
  Serial.print(' ');
  Serial.print(arg2, 16);
  Serial.print(' ');
  Serial.print(arg3, 16);
  Serial.println();

  beginSpiCmd();
  sendSpiByte(cmd);

  contSpiData();
  sendSpiByte(arg0);
  sendSpiByte(arg1);
  sendSpiByte(arg2);
  sendSpiByte(arg3);

  endSpi();
}
void sendCommand(const uint8_t cmd, const uint8_t arg0, const uint8_t arg1, const uint8_t arg2, const uint8_t arg3, const uint8_t arg4)
{
  Serial.print("CMD5 ");
  Serial.print(cmd, 16);
  Serial.print(' ');
  Serial.print(arg0, 16);
  Serial.print(' ');
  Serial.print(arg1, 16);
  Serial.print(' ');
  Serial.print(arg2, 16);
  Serial.print(' ');
  Serial.print(arg3, 16);
  Serial.print(' ');
  Serial.print(arg4, 16);
  Serial.println();

  beginSpiCmd();
  sendSpiByte(cmd);

  contSpiData();
  sendSpiByte(arg0);
  sendSpiByte(arg1);
  sendSpiByte(arg2);
  sendSpiByte(arg3);
  sendSpiByte(arg4);

  endSpi();
}
template<typename BufType>
void sendBuffer(const uint8_t cmd, const BufType& buf)
{
  const uint32_t bytes = sizeof(buf);
  
  Serial.print("CMDBUF ");
  Serial.print(cmd, 16);
  Serial.print(" ");
  Serial.print(bytes);
  Serial.println("bytes");

  beginSpiCmd();
  sendSpiByte(cmd);

  contSpiData();
  for (const uint8_t* c = &buf[0]; c != &buf[bytes]; ++c)
    sendSpiByte(*c);
  
  endSpi();
}
void sendConstantData(const uint8_t cmd, const uint32_t bytes, const uint8_t val)
{
  Serial.print("CMDCON ");
  Serial.print(cmd, 16);
  Serial.print(" ");
  Serial.print(bytes);
  Serial.println("bytes");

  beginSpiCmd();
  sendSpiByte(cmd);

  contSpiData();
  for (uint32_t i=0; i<bytes; ++i)
    sendSpiByte(val);
  
  endSpi();
}

void resetDisplay()
{
  pinMode(pin::Reset, OUTPUT);

  // make sure we have a clean start
  digitalWrite(pin::Reset, 1);
  delay(10);

  // drop low for a moment and then bring back up
  digitalWrite(pin::Reset, 0);
  delay(10);
  digitalWrite(pin::Reset, 1);
  delay(10);
}


void powerUp(ScreenMode screenMode = Grey4)
{
  resetDisplay();

  const bool mono = (screenMode == Mono);
  
  uint8_t vdhr = mono ? 0x03 : 0x13;
  sendCommand(cmd::BoosterSoftStart, 0x17, 0x17, 0x17);  // defaults
  sendCommand(cmd::PowerSetting, 0x03, 0x00, 0x2b, 0x2b, vdhr);    // defaults + +-11V, 6.2V
  sendCommand(cmd::PowerOn);
  delay(200);

  sendCommand(cmd::PanelSetting, 0xbf/*, 0x0d*/);  // defaults + LUT from register, B&W panel
  sendCommand(cmd::PLLControl, 0x3c);
  
  static_assert(ScreenWidth < 256);
  sendCommand(cmd::ResolutionSetting, uint8_t(ScreenWidth), uint8_t(ScreenHeight >> 8), uint8_t(ScreenHeight & 0xff));
  
  sendCommand(cmd::VcmDcSetting, 0x12);  // -1.0V
  sendCommand(cmd::IntervalSetting, 0x97);
  
  if (screenMode == Grey4)
  {
    sendBuffer(cmd::VComLut, VcomLut_Grey);
    sendBuffer(cmd::W2WLut, W2WLut_Grey);
    sendBuffer(cmd::B2WLut, B2WLut_Grey);
    sendBuffer(cmd::W2BLut, W2BLut_Grey);
    sendBuffer(cmd::B2BLut, B2BLut_Grey);
  }
  else
  {
    sendBuffer(cmd::VComLut, VcomLut_Mono);
    sendBuffer(cmd::W2WLut, W2WLut_Mono);
    sendBuffer(cmd::B2WLut, B2WLut_Mono);
    sendBuffer(cmd::W2BLut, W2BLut_Mono);
    sendBuffer(cmd::B2BLut, B2BLut_Mono);
  }
}

void powerDown()
{
  sendCommand(cmd::IntervalSetting, 0x17);  // default
  sendCommand(cmd::VcmDcSetting, 0x00);  // default
  sendCommand(cmd::PowerOff);
}


} // namespace


static constexpr uint16_t ScreenWidth = magtag::ScreenWidth;
static constexpr uint16_t ScreenHeight = magtag::ScreenHeight;
static constexpr uint16_t NumPixels = ScreenWidth * ScreenHeight;
static constexpr uint16_t ScreenBytes = NumPixels / 8;
uint8_t     m_plane0[ScreenBytes];
uint8_t     m_plane1[ScreenBytes];

void setup() {
  Serial.begin(115200);
  
  pinMode(13, OUTPUT);
  digitalWrite(13, 0);

  memset(m_plane0, 0x0f, sizeof(m_plane0));
  memset(m_plane1, 0x0f, sizeof(m_plane1));
  for (int y=20; y<ScreenHeight/2; ++y) {
    int x8;
    for (x8=4; x8<ScreenWidth/16; ++x8) {
      m_plane0[x8 + (y*ScreenWidth/8)] = 0xff;
      m_plane1[x8 + (y*ScreenWidth/8)] = 0xff;
    }
    for (; x8<ScreenWidth/8 - 4; ++x8) {
      m_plane1[x8 + (y*ScreenWidth/8)] = 0xff;
    }
  }
  int ox8 = 0;
  for (int y=90; y<ScreenHeight-20; ++y) {
    int x8 = ox8;
    ox8 = (ox8 < ScreenWidth/8) ? ox8+1: 0;
    m_plane0[x8 + (y*ScreenWidth/8)] = 0;
    m_plane1[x8 + (y*ScreenWidth/8)] = 0;
  }
}


void clearPlane(uint32_t plane)
{
  using namespace magtag;
  //auto screenMode = magtag::Mono;
  //powerUp(screenMode);

  auto planeCmd = plane == 0 ? cmd::SendPlane0 : cmd::SendPlane1;
  sendConstantData(planeCmd, ScreenBytes, 0xff);
  
  sendCommand(cmd::DataStop);
}

void clearScreen()
{
  magtag::powerUp(magtag::Grey4);
  
  clearPlane(0);
  delay(2);
  clearPlane(1);
  
  magtag::sendCommand(magtag::cmd::DisplayRefresh);
  delay(magtag::DisplayRefreshDelay);
}

void redrawScreen()
{
  using namespace magtag;
  auto screenMode = magtag::Grey4;
  powerUp(screenMode);

  sendBuffer(cmd::SendPlane0, m_plane0);
  delay(2);
  sendBuffer(cmd::SendPlane1, m_plane1); 

  sendCommand(cmd::DisplayRefresh);
  delay(DisplayRefreshDelay);
  
  //display.powerDown();
}

void loop() {

  display.begin(THINKINK_GRAYSCALE4);
  //display.begin(THINKINK_MONO);

  //Serial.print("display buffer1_size=");
  //Serial.println(display.buffer1_size);

  display.clearBuffer();
  display.fillRect(60, 20, 100, 80, EPD_DARK);
  display.fillRect(80, 50, 140, 20, EPD_BLACK);
  display.display();
  delay(2000);

  clearPlane(0);
  redrawScreen();
  //memcpy(display.buffer1, m_plane0, display.buffer1_size);
  //memcpy(display.buffer2, m_plane1, display.buffer2_size);
  //display.display();
  delay(3000);
  
//  display.clearBuffer();
//  display.display();
//  delay(4000);
}
