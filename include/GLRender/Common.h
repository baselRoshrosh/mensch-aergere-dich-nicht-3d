#ifndef COMMON_H
#define COMMON_H


#include "GLRender/GLRenderDecl.h"

#include <string>
#include <vector>



template < class ElementT, unsigned int Rows, unsigned int Cols >
class SimpleMat
{
public:
  SimpleMat() {}
  ~SimpleMat() {}


  // clear all elements
  void clear() { for( unsigned int i=0; i<Rows*Cols;i++) v[i]=0; }


  // scale all elements by s
  SimpleMat<ElementT, Rows, Cols>& operator*= (const ElementT cVal) { for(unsigned int i = 0; i < Cols*Rows; i++) this->v[i] *= cVal; return *this; }

  // add a vector to current one
  SimpleMat<ElementT, Rows, Cols>& operator+= (const SimpleMat<ElementT, Rows, Cols>& rcVec) { for(unsigned int i = 0; i < Cols*Rows; i++) this->v[i] += rcVec.v[i]; return *this; }


  ElementT  v[ Rows*Cols];
};


typedef SimpleMat<float,3,1>          Vec3f;
typedef SimpleMat<unsigned int, 3, 1> Vec3ui;
typedef SimpleMat<float, 2, 1>        Vec2f;


std::string GLRENDER_DECL getDirectoryName( const std::string cFName );

// load and image into memory
int GLRENDER_DECL loadPNG(const std::string cFName, std::vector<unsigned char>& rcImgData, unsigned int& ruiWidth, unsigned int& ruiHeight);




#endif