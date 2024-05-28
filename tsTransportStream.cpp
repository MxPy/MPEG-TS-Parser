#include "tsTransportStream.h"
#include <string.h>

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
  std::cout<<"TS: SB="<<(int)m_SB<<" E="<<m_E<<" S="<<m_S<<" P="<<m_T<<" PID="<<m_PID<<" TSC="<<(int)m_TSC<<" AFC="<<(int)m_AFC<<" CC="<<(int)m_CC;
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
  m_AdaptationFieldControl = (uint8_t) AdaptationFieldControl;
  // std::cout<<std::endl;
  // std::cout<<(int)m_AdaptationFieldControl<<std::endl;
  // std::cout<<std::endl;  
  m_AdaptationFieldLength = PacketBuffer[4];
  m_DC = PacketBuffer[5] & 0x80;
  m_RA = PacketBuffer[5] & 0x40;
  m_SP = PacketBuffer[5] & 0x20;
  m_PR = PacketBuffer[5] & 0x10;
  m_OR = PacketBuffer[5] & 0x08;
  m_SF = PacketBuffer[5] & 0x04;
  m_TP = PacketBuffer[5] & 0x02;
  m_EX = PacketBuffer[5] & 0x01;

  if(m_PR){
    m_PCR = (((uint64_t)PacketBuffer[6]) << 25) | 
            (((uint64_t)PacketBuffer[7]) << 17) |
            (((uint64_t)PacketBuffer[8]) << 9) | 
            (((uint64_t)PacketBuffer[9]) << 1) |
            ((0b10000000 & PacketBuffer[10])?1:0);
    int extension = (1 & PacketBuffer[10]);
    extension = (extension << 8) | PacketBuffer[11];
    m_PCR = (m_PCR * 300) +  extension;
  }

//parsing
}
/// @brief Print all TS packet header fields
void xTS_AdaptationField::Print() const
{
  std::cout<<" AF: L="<<(int)m_AdaptationFieldLength<<" DC="<<m_DC<<" RA="<<m_RA<<" SP="<<m_SP<<" PR="<<m_PR<<" OR="<<m_OR<<" SF="<<m_SF<<" TP="<<m_TP<<" EX="<<m_EX;
  if(m_PR) std::cout<<" PCR="<<int(m_PCR);
  
//print print print
}

void xPES_PacketHeader::Reset(){
   m_PacketStartCodePrefix = 0;               
  m_StreamId = 0;
  m_PacketLength = 0;
}

int32_t xPES_PacketHeader::Parse(const uint8_t* Input){
  m_PacketStartCodePrefix = (((uint32_t)Input[0]) << 16) |
                            (((uint32_t)Input[1]) << 8) | 
                            (((uint32_t)Input[2]));                   
  m_StreamId = Input[3];
  m_PacketLength =  (((uint64_t)Input[4]) << 8) | 
                    (((uint64_t)Input[5]));
  m_HeaderLength = 6;
  if(m_StreamId == 0xBD || m_StreamId >= 0xC0 && m_StreamId <= 0xDF || m_StreamId >= 0xE0 && m_StreamId <= 0xEF){
    m_HeaderLength+=3;
    m_HeaderLength += (int) Input[8];
  }
  return 1;            
}

void xPES_PacketHeader::Print() const{
  printf("PES: PSCP=%06X SID=%02X L=%d",m_PacketStartCodePrefix, (int) m_StreamId, m_PacketLength);
}

xPES_Assembler::eResult xPES_Assembler::AbsorbPacket(const uint8_t* TransportStreamPacket, const xTS_PacketHeader* PacketHeader, const xTS_AdaptationField* AdaptationField){
  if(PacketHeader->getS() == 1 && PacketHeader->getPID() == 136 && m_PID == -1){
    uint8_t* PES_Head_Input = new uint8_t[9]; 
    if(PacketHeader->hasAdaptationField()){
      for (int i {0}; i<9; i++){
        PES_Head_Input[i]=TransportStreamPacket[5+i+AdaptationField->getAdaptationFieldLength()];
      }
    }else{
      for (int i {0}; i<9; i++){
        PES_Head_Input[i]=TransportStreamPacket[4+i];
      }
    }
    m_PESH.Parse(PES_Head_Input);
    m_PID = 136;
    m_BufferSize = m_PESH.getPacketLength()+6;
    m_DataOffset = m_PESH.getHeaderLength();
    m_Buffer = new uint8_t[m_BufferSize];
    for (int i {0}; i<m_BufferSize; ++i){
      m_Buffer[i] = 0;
    }
    if(PacketHeader->hasAdaptationField()){
      for (int i {0}; i<184-AdaptationField->getAdaptationFieldLength()-1 - m_PESH.getHeaderLength(); i++){
        m_Buffer[m_DataOffset-m_PESH.getHeaderLength()]=TransportStreamPacket[5+i+AdaptationField->getAdaptationFieldLength() + m_PESH.getHeaderLength()];
        //std::cout<<"1H==== "<<std::hex<<(int)m_Buffer[i]<<" ===="<<std::endl;
        m_DataOffset++;

      }
    }else{
      for (int i {0}; i<184; i++){
        m_Buffer[m_DataOffset-m_PESH.getHeaderLength()]=TransportStreamPacket[4+i];
        //std::cout<<"2H==== "<<std::hex<<(int)m_Buffer[i]<<" ===="<<std::endl;
        m_DataOffset++;
      }
    }
    std::cout<<std::endl<<(int)m_Buffer[8]<<std::endl;
    return xPES_Assembler::eResult::AssemblingStarted;
  }
  else if(PacketHeader->getS() == 0 && PacketHeader->getPID() == 136 && m_PID != -1){
      if(PacketHeader->hasAdaptationField()){
        for (int i {0}; i<184-AdaptationField->getAdaptationFieldLength()-1; i++){
          m_Buffer[m_DataOffset-m_PESH.getHeaderLength()]=TransportStreamPacket[5+i+AdaptationField->getAdaptationFieldLength()];
          //std::cout<<"1R==== "<<std::hex<<(int)m_Buffer[i]<<" ===="<<std::endl;
          m_DataOffset++;
        }
      }else{
        for (int i {0}; i<184; i++){
          m_Buffer[m_DataOffset-m_PESH.getHeaderLength()]=TransportStreamPacket[4+i];
          //std::+<<"2R==== "<<std::hex<<(int)m_Buffer[i]<<" ===="<<std::endl;
          m_DataOffset++;
        }
      }
      
      if(m_DataOffset == m_BufferSize){
        return xPES_Assembler::eResult::AssemblingFinished;
      }
    return xPES_Assembler::eResult::AssemblingContinue;
  }
  // else if(PacketHeader->getS() == 1 && PacketHeader->getPID() == 136 && ){
  //   return xPES_Assembler::eResult::AssemblingFinished;
  // }
  return xPES_Assembler::eResult::UnexpectedPID;
}

void xPES_Assembler::xBufferReset (){
  m_PESH.Reset();
  memset(m_Buffer, 0, sizeof(m_Buffer));
  m_PID = -1;
  m_BufferSize = 0;
  m_DataOffset = 0;
  m_PESH.Reset();
}

void xPES_Assembler::xBufferAppend(const uint8_t* Data, int32_t Size){

}

void xPES_Assembler::saveBufferToFile(FILE* AudioMP2) {
  // std::cout<<std::endl<<"======First bytes======"<<std::endl;
  //   for (int i {0}; i<31; ++i){
  //     std::cout<<std::hex<<(int)m_Buffer[i]<<" ";
  //   }
  // std::cout<<std::endl<<"======Last bytes======"<<std::endl;
  //   for (int i {m_DataOffset-30}; i<=m_DataOffset; ++i){
  //     std::cout<<std::hex<<(int)m_Buffer[i]<<" ";
  //   }
    fwrite(getPacket(), 1, getNumPacketBytes()-m_PESH.getHeaderLength(), AudioMP2);
    xBufferReset();
}

void xPES_Assembler::Init (int32_t PID){
  m_PID = -1;
  m_BufferSize = 0;
  m_DataOffset = 0;
}