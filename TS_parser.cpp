#include "tsCommon.h"
#include "tsTransportStream.h"

#include<iostream>
#include<fstream>

//=============================================================================================================================================================================

int main(int argc, char *argv[ ], char *envp[ ])
{
  // TODO - open file
  std::ifstream infile;
  infile.open("example_new.ts",std::ios::binary);
  char *buffer = new char[xTS::TS_PacketLength];
 
  // TODO - check if file if opened

  xTS_PacketHeader    TS_PacketHeader;
  xTS_AdaptationField   TS_AdaptationField;

  int32_t TS_PacketId = 0;
  while(infile.good() && !infile.eof() && TS_PacketId <=191)
  {
    // TODO - read from file
    infile.read (buffer, xTS::TS_PacketLength);

    TS_PacketHeader.Reset();
    TS_PacketHeader.Parse((uint8_t*)buffer);
    printf("%010d ", TS_PacketId);
    TS_PacketHeader.Print();

    if(TS_PacketHeader.hasAdaptationField()){
      TS_AdaptationField.Reset();
      TS_AdaptationField.Parse((uint8_t*)buffer, TS_PacketHeader.getAdaptationFieldControl());
      TS_AdaptationField.Print();
    }

    printf("\n");

    TS_PacketId++;
  }
  
  // TODO - close file
  infile.close();
  return EXIT_SUCCESS;
}

//=============================================================================================================================================================================
