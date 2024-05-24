#pragma once
#include "tsCommon.h"
#include <string>
#include<iostream>

/*
MPEG-TS packet:
`        3                   2                   1                   0  `
`      1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0  `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `
`   0 |                             Header                            | `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `
`   4 |                  Adaptation field + Payload                   | `
`     |                                                               | `
` 184 |                                                               | `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `


MPEG-TS packet header:
`        3                   2                   1                   0  `
`      1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0  `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `
`   0 |       SB      |E|S|T|           PID           |TSC|AFC|   CC  | `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `

Sync byte                    (SB ) :  8 bits
Transport error indicator    (E  ) :  1 bit
Payload unit start indicator (S  ) :  1 bit
Transport priority           (T  ) :  1 bit
Packet Identifier            (PID) : 13 bits
Transport scrambling control (TSC) :  2 bits
Adaptation field control     (AFC) :  2 bits
Continuity counter           (CC ) :  4 bits
*/


//=============================================================================================================================================================================

class xTS
{
public:
  static constexpr uint32_t TS_PacketLength  = 188;
  static constexpr uint32_t TS_HeaderLength  = 4;

  static constexpr uint32_t PES_HeaderLength = 6;

  static constexpr uint32_t BaseClockFrequency_Hz         =    90000; //Hz
  static constexpr uint32_t ExtendedClockFrequency_Hz     = 27000000; //Hz
  static constexpr uint32_t BaseClockFrequency_kHz        =       90; //kHz
  static constexpr uint32_t ExtendedClockFrequency_kHz    =    27000; //kHz
  static constexpr uint32_t BaseToExtendedClockMultiplier =      300;
};

//=============================================================================================================================================================================

class xTS_PacketHeader
{
public:
  enum class ePID : uint16_t
  {
    PAT  = 0x0000,
    CAT  = 0x0001,
    TSDT = 0x0002,
    IPMT = 0x0003,
    NIT  = 0x0010, //DVB specific PID
    SDT  = 0x0011, //DVB specific PID
    NuLL = 0x1FFF,
  };

protected:
  //TODO - header fields, e.g.:
  uint8_t  m_SB;
  bool m_E;
  bool m_S;
  bool m_T;
  //TODO change it
  uint16_t m_PID;
  uint8_t m_TSC;
  uint8_t m_AFC;
  uint8_t m_CC;



public:
  void     Reset();
  int32_t  Parse(const uint8_t* Input);
  void     Print() const;

public:
  //TODO - direct acces to header field value, e.g.:
  uint8_t  getSyncByte() const { return m_SB; }  
  bool getS() const { return m_S; }
  uint16_t getPID() const { return m_PID; }
  uint8_t getCC() const { return m_CC; }
  uint16_t getAdaptationFieldControl() const { return m_AFC; }  

public:
  //TODO - derrived informations
  bool     hasAdaptationField() const { return (m_AFC > 0b01) ? true : false; }
  bool     hasPayload        () const { return (m_AFC == 0b11 or m_AFC == 0b01) ? true : false; }
};

//=============================================================================================================================================================================

class xTS_AdaptationField
{
protected:
//setup
uint8_t m_AdaptationFieldControl;
//mandatory fields
uint8_t m_AdaptationFieldLength;
//optional fields - PCR
bool m_E;
bool m_DC;
bool m_RA;
bool m_SP;
bool m_PR;
bool m_OR;
bool m_SF;
bool m_TP;
bool m_EX;

uint64_t m_PCR;
uint64_t m_OPCR;
uint8_t m_SC;
uint8_t m_TPDL;
uint8_t* m_TPD;
uint8_t* m_AE;
uint8_t* m_SB;

public:
xTS_AdaptationField(): m_TPD(nullptr), m_AE(nullptr), m_SB(nullptr){}
void Reset();
int32_t Parse(const uint8_t* PacketBuffer, uint8_t AdaptationFieldControl);
void Print() const;
public:
//mandatory fields
uint8_t getAdaptationFieldLength () const { return
m_AdaptationFieldLength ; }
//derived values
//uint32_t getNumBytes () const { }
};



class xPES_PacketHeader
{
public:
enum eStreamId : uint8_t
{
eStreamId_program_stream_map = 0xBC,
eStreamId_padding_stream = 0xBE,
eStreamId_private_stream_2 = 0xBF,
eStreamId_ECM = 0xF0,
eStreamId_EMM = 0xF1,
eStreamId_program_stream_directory = 0xFF,
eStreamId_DSMCC_stream = 0xF2,
eStreamId_ITUT_H222_1_type_E = 0xF8,
};
protected:
//PES packet header
uint32_t m_PacketStartCodePrefix;
uint8_t m_StreamId;
uint16_t m_PacketLength;
uint16_t m_HeaderLength;
public:
void Reset();
int32_t Parse(const uint8_t* Input);
void Print() const;
public:
//PES packet header
uint32_t getPacketStartCodePrefix() const { return m_PacketStartCodePrefix; }
uint8_t getStreamId () const { return m_StreamId; }
uint16_t getPacketLength () const { return m_PacketLength; }
uint16_t getHeaderLength () const { return m_HeaderLength; }
};


class xPES_Assembler
{
public:
enum class eResult : int32_t
{
UnexpectedPID = 1,
StreamPackedLost ,
AssemblingStarted ,
AssemblingContinue,
AssemblingFinished,
};
protected:
//setup
int32_t m_PID;
//buffer
uint8_t* m_Buffer;
uint32_t m_BufferSize;
uint32_t m_DataOffset;
//operation
int8_t m_LastContinuityCounter;
bool m_Started;
xPES_PacketHeader m_PESH;
public:
xPES_Assembler (){};
~xPES_Assembler(){};
void Init (int32_t PID);
eResult AbsorbPacket(const uint8_t* TransportStreamPacket, const xTS_PacketHeader* PacketHeader, const xTS_AdaptationField* AdaptationField);
void PrintPESH () const { m_PESH.Print(); }
uint8_t* getPacket () { return m_Buffer; }
int32_t getNumPacketBytes() const { return m_DataOffset; }
void saveBufferToFile (FILE* AudioMP2);
protected:
void xBufferReset ();
void xBufferAppend(const uint8_t* Data, int32_t Size);
};