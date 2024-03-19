#include "tsTransportStream.h"

//=============================================================================================================================================================================
// xTS_PacketHeader
//=============================================================================================================================================================================


/// @brief Reset - reset all TS packet header fields
void xTS_PacketHeader::Reset()
{
  m_SB = 0;
  m_E = 0;
  m_S = 0;
  m_T = 0;
  m_PID = 0;
  m_TSC = 0;
  m_AFC = 0;
  m_CC = 0;
}

/**
  @brief Parse all TS packet header fields
  @param Input is pointer to buffer containing TS packet
  @return Number of parsed bytes (4 on success, -1 on failure) 
 */
int32_t xTS_PacketHeader::Parse(const uint8_t* Input)
{
  int32_t header = (Input[0] << 24) + (Input[1] << 16) + (Input[2] << 8) + Input[3];
  // std::cout<<std::endl;
  // std::cout<<xSwapBytes32(header & 0x30) << " " << (header & 0x1fff00);
  // std::cout<<std::endl;
  m_SB = xSwapBytes32(header & 0xff000000);
  m_E = xSwapBytes32(header & 0x800000) >> 8;
  m_S = xSwapBytes32(header & 0x400000) >> 9;
  m_T = xSwapBytes32(header & 0x200000) >> 10;
  m_PID = xSwapBytes32(header & 0x1fff00) >> 2;
  m_TSC = xSwapBytes32(header & 0xc0) >> 24;
  m_AFC = xSwapBytes32(header & 0x30) >> 26;
  m_CC = xSwapBytes32(header & 0xf) >> 28;
  return NOT_VALID;
}

/// @brief Print all TS packet header fields
void xTS_PacketHeader::Print() const
{
  //print sth
  std::cout<<"TS: SB="<<(int)m_SB<<" E="<<m_E<<" S="<<m_S<<" P="<<m_T<<" PID="<<m_PID<<" TSC="<<m_TSC<<" AFC="<<m_AFC<<" CC="<<m_CC;
}

//=============================================================================================================================================================================
