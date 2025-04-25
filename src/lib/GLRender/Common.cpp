#include "GLRender/Common.h"
#include "GLRender/lodepng.h"

#include <cmath>
#include <iostream>
#include <fstream>





std::string getDirectoryName(const std::string cFName)
{
  std::string sPath;
  std::ifstream cStrm;

  sPath = "./";
  cStrm.open(sPath + cFName);
  if (cStrm.is_open())
  {
    cStrm.close();
    return sPath;
  }

  sPath = "../../../../../GLSamples/";
  cStrm.open(sPath + cFName);
  if (cStrm.is_open())
  {
    cStrm.close();
    return sPath;
  }

  sPath = "../../../../../../../../GLSamples/";
  cStrm.open(sPath + cFName);
  if (cStrm.is_open())
  {
    cStrm.close();
    return sPath;
  }

  sPath = "GLSamples/";
  cStrm.open(sPath + cFName);
  if (cStrm.is_open())
  {
    cStrm.close();
    return sPath;
  }

  std::cerr << "cannot open file: " << cFName << std::endl;
  return std::string("./");
}



int loadPNG(const std::string cFName, std::vector<unsigned char>& rcImgData, unsigned int& ruiWidth, unsigned int& ruiHeight)
{
  //decode
  unsigned int error = lodepng::decode( rcImgData, ruiWidth, ruiHeight, cFName );

  //if there's an error, display it
  if(error) std::cout << "cannot read image " << cFName << " error: " << error << ": " << lodepng_error_text(error) << std::endl;

  return 0;
}


