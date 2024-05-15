#include "tsCommon.h"
#include "tsTransportStream.h"

#include<iostream>
#include<fstream>

//=============================================================================================================================================================================

int main(int argc, char *argv[ ], char *envp[ ])
{
  // TODO - open file
  std::ifstream infile;
  std::ofstream outfile;
  FILE* AudioMP2 = fopen("PID136.mp2", "wb");
  infile.open("example_new.ts",std::ios::binary);
  char *buffer = new char[xTS::TS_PacketLength];
 
  // TODO - check if file if opened

  xTS_PacketHeader    TS_PacketHeader;
  xTS_AdaptationField   TS_AdaptationField;
  xPES_Assembler PES_Assembler;
  PES_Assembler.Init(-1);
  int32_t TS_PacketId = 0;
  while(infile.good() && !infile.eof() && TS_PacketId<20)
  {
    // TODO - read from file
    infile.read (buffer, xTS::TS_PacketLength);

    TS_PacketHeader.Reset();
    TS_PacketHeader.Parse((uint8_t*)buffer);
    printf("%010d ", TS_PacketId);
    TS_PacketHeader.Print();
    TS_AdaptationField.Reset();
    if(TS_PacketHeader.hasAdaptationField()){
      TS_AdaptationField.Parse((uint8_t*)buffer, TS_PacketHeader.getAdaptationFieldControl());
      TS_AdaptationField.Print();
    }
    
    xPES_Assembler::eResult Result = PES_Assembler.AbsorbPacket((uint8_t*)buffer, &TS_PacketHeader, &TS_AdaptationField);
    switch(Result)
    {
    case xPES_Assembler::eResult::StreamPackedLost : printf("PcktLost "); break;
    case xPES_Assembler::eResult::AssemblingStarted : printf(" Started "); PES_Assembler.PrintPESH(); break;
    case xPES_Assembler::eResult::AssemblingContinue: printf(" Continue "); break;
    case xPES_Assembler::eResult::AssemblingFinished: printf(" Finished "); printf("PES: Len=%d", PES_Assembler.getNumPacketBytes());
    PES_Assembler.saveBufferToFile(AudioMP2);break;
    default: break;
    }
    printf("\n");

    TS_PacketId++;
  }
  
  // TODO - close file
  infile.close();
  fclose(AudioMP2);
  return EXIT_SUCCESS;
}

//=============================================================================================================================================================================
