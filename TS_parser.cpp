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
  uint8_t *header = new uint8_t[4];
 
  // TODO - check if file if opened

  xTS_PacketHeader    TS_PacketHeader;
  xTS_AdaptationField   TS_AdaptationField;

  int32_t TS_PacketId = 0;
  while(infile.good() && !infile.eof() && TS_PacketId <=33)
  {
    // TODO - read from file
    infile.read (buffer, xTS::TS_PacketLength);

    header[0] = buffer[0];
    header[1] = buffer[1];
    header[2] = buffer[2];
    header[3] = buffer[3];

    TS_PacketHeader.Reset();
    TS_PacketHeader.Parse(header);
    printf("%010d ", TS_PacketId);
    TS_PacketHeader.Print();
    printf("\n");
    TS_AdaptationField.Reset();

    TS_PacketId++;
  }
  
  // TODO - close file
  infile.close();
  return EXIT_SUCCESS;
}

//=============================================================================================================================================================================
