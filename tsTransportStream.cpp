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
  m_SB = (header & 0xff000000)>>24;
  m_E = (header & 0x800000)>>23;
  m_S = (header & 0x400000)>>22;
  m_T = (header & 0x200000)>>21;
  m_PID = (header & 0x1fff00)>>8;
  m_TSC = (header & 0xc0)>>6;
  m_AFC = (header & 0x30)>>4;
  m_CC = (header & 0xf);
  return NOT_VALID;
}

/// @brief Print all TS packet header fields
void xTS_PacketHeader::Print() const
{
  //print sth
  std::cout<<"TS: SB="<<(int)m_SB<<" E="<<m_E<<" S="<<m_S<<" P="<<m_T<<" PID="<<m_PID<<" TSC="<<m_TSC<<" AFC="<<m_AFC<<" CC="<<m_CC;
}

//=============================================================================================================================================================================


/// @brief Reset - reset all TS packet header fields
void xTS_AdaptationField::Reset()
{
//setup
m_AdaptationFieldControl = 0;
//mandatory fields
m_AdaptationFieldLength = 0;
//optional fields - PCR
m_E = 0;
m_DC = 0;
m_RA = 0;
m_SP = 0;
m_PR = 0;
m_OR = 0;
m_SF = 0;
m_TP = 0;
m_EX = 0;

m_PCR = 0;
m_OPCR = 0;
m_SC = 0;
m_TPDL = 0;

if(m_TPD) delete m_TPD;
m_TPD = nullptr;
if(m_AE) delete m_AE;
m_AE = nullptr;
if(m_SB) delete m_SB;
m_SB = nullptr;
}
/**
@brief Parse adaptation field
@param PacketBuffer is pointer to buffer containing TS packet
@param AdaptationFieldControl is value of Adaptation Field Control field of
corresponding TS packet header
@return Number of parsed bytes (length of AF or -1 on failure)
*/
int32_t xTS_AdaptationField::Parse(const uint8_t* PacketBuffer, uint8_t
AdaptationFieldControl)
{
//parsing
}
/// @brief Print all TS packet header fields
void xTS_AdaptationField::Print() const
{
//print print print
}