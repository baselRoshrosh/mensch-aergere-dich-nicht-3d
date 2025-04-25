#include "GLRender/RenderTriangle.h"

#include <iostream>
#include <cmath>




// vertex shader program
const GLchar* vertexShaderRTSrc =
"#version 150\n"
""
"uniform mat4  cProjectionMatrix;"
"uniform mat4  cModelviewMatrix;"
""
"in vec4 in_Position;"
""
"void main()"
"{"
"  gl_Position = cProjectionMatrix * cModelviewMatrix * in_Position;"
"} ";



// fragment shader program
const GLchar* fragShaderRTSrc =
"#version 150\n"
"\n"
"out vec4 out_color;\n"
"\n"
"void main()\n"
"{\n"
"  out_color = vec4( 1.0, 1.0, 1.0, 1.0 );\n"
"}\n";






// constructor
RenderTriangle::RenderTriangle()
  : m_iWidth( 800 )
  , m_iHeight( 600 )
  , m_fHeightAngle( 0.4f )
  , m_fNearDistance( 5.0f )
  , m_fFarDistance( 15.0f )
  , m_fRotX( 0.0f )
  , m_fRotY( 0.0f )
  , m_fTransZ(-10.0f)
  , m_iVertexAttribPosition(0)
  , m_iProjectionMatrixID(0)
  , m_iModelviewMatrixID(0)
  , m_uiVAO(0)
  , m_uiVBOindices(0)
  , m_uiVBOcoords(0)
{
  for (unsigned int i = 0; i < 16; i++)
  {
    m_afModelViewMatrix[i] = 0.0f;
    m_afProjectionMatrix[i] = 0.0f;
  }
}



void
RenderTriangle::setWindowSize( int iWidth, int iHeight )
{
  m_iWidth = iWidth;
  m_iHeight = iHeight;
  glViewport( 0, 0, m_iWidth, m_iHeight );      // tells OpenGL the new size of the render area
  renderCamera();                               // recompute projection matrix
}


void
RenderTriangle::initGL()
{
  //----------------------------------------------------------------------
  // create program and link
  //----------------------------------------------------------------------
  if( m_cProg.addShader( &vertexShaderRTSrc, GL_VERTEX_SHADER ) ) return;
  if(m_cProg.addShader(&fragShaderRTSrc, GL_FRAGMENT_SHADER)) return;
  if( m_cProg.linkShaders() ) return;

  // determine bindings with shader
  m_iVertexAttribPosition = glGetAttribLocation(m_cProg.getPrgID(), "in_Position");
  m_iProjectionMatrixID = glGetUniformLocation(m_cProg.getPrgID(), "cProjectionMatrix");
  m_iModelviewMatrixID = glGetUniformLocation(m_cProg.getPrgID(), "cModelviewMatrix");

  //----------------------------------------------------------------------
  // create object
  //----------------------------------------------------------------------

  // create vertex array object
  glGenVertexArrays(1, &m_uiVAO);
  glBindVertexArray(m_uiVAO);

  // create buffer object for indices
  glGenBuffers(1, &m_uiVBOindices);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiVBOindices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*sizeof(unsigned int), auiIndexList, GL_STATIC_DRAW);

  // create coords object
  glGenBuffers(1, &m_uiVBOcoords);
  glBindBuffer(GL_ARRAY_BUFFER, m_uiVBOcoords);
  glBufferData(GL_ARRAY_BUFFER, 3*3*sizeof(float), afVertexList, GL_STATIC_DRAW);
  glEnableVertexAttribArray(m_iVertexAttribPosition);
  glVertexAttribPointer(m_iVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


  //-----------------------------------------------------------------
  // init GL
  //-----------------------------------------------------------------

  // set background color to black
  glClearColor(0.0f,0.0f,0.0f,0.0f);
  // set depth buffer to far plane
  glClearDepth(1.0f);
  // enable depth test with the z-buffer
  glEnable(GL_DEPTH_TEST);

  // fill the polygon
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

  // do not use culling
  glDisable(GL_CULL_FACE);

  // initialize the camera
  renderCamera();
}


void
RenderTriangle::uninitGL()
{
  //-----------------------------------------------------------------
  // uninit GL
  //-----------------------------------------------------------------

  // delete buffer objects
  glDeleteBuffers(1, &m_uiVBOindices);
  glDeleteBuffers(1, &m_uiVBOcoords);

  // destroy vertex array object
  glDeleteVertexArrays(1, &m_uiVAO);
}



void
RenderTriangle::renderCamera()
{
  // setup projection matrix
  float fLeft, fRight, fBottom, fTop;
  fTop = m_fNearDistance * tanf(m_fHeightAngle / 2.0f);
  fRight = fTop * (float)m_iWidth / (float)m_iHeight;
  fBottom = -fTop;
  fLeft = -fRight;

  // builds projection matrix like glFrustum
  for(unsigned int i = 0; i < 16; i++) m_afProjectionMatrix[i] = 0.0f;
  m_afProjectionMatrix[0] = (2.0f * (float)m_fNearDistance) / (fRight - fLeft);
  m_afProjectionMatrix[5] = (2.0f * m_fNearDistance) / (fTop - fBottom);
  m_afProjectionMatrix[8] = (fRight + fLeft) / (fRight - fLeft);
  m_afProjectionMatrix[9] = (fTop + fBottom) / (fTop - fBottom);
  m_afProjectionMatrix[10] = -(m_fFarDistance + m_fNearDistance) / (m_fFarDistance - m_fNearDistance);
  m_afProjectionMatrix[11] = -1.0f;
  m_afProjectionMatrix[14] = -2.0f * m_fFarDistance * m_fNearDistance / (m_fFarDistance - m_fNearDistance);
  glUniformMatrix4fv(m_iProjectionMatrixID, 1, false, m_afProjectionMatrix);

  // setup modelview matrix
  float fAngX = m_fRotX * 3.14159265f / 180.0f;
  float fAngY = m_fRotY * 3.14159265f / 180.0f;
  for(unsigned int i = 0; i < 16; i++) m_afModelViewMatrix[i] = 0.0f;

  m_afModelViewMatrix[0] = std::cos(fAngY);
  m_afModelViewMatrix[2] = -std::sin(fAngY);
  m_afModelViewMatrix[4] = std::sin(fAngX)*std::sin(fAngY);
  m_afModelViewMatrix[5] = std::cos(fAngX);
  m_afModelViewMatrix[6] = std::sin(fAngX)*std::cos(fAngY);
  m_afModelViewMatrix[8] = std::cos(fAngX)*std::sin(fAngY);
  m_afModelViewMatrix[9] = -std::sin(fAngX);
  m_afModelViewMatrix[10] = std::cos(fAngX)*std::cos(fAngY);
  m_afModelViewMatrix[14] = m_fTransZ;
  m_afModelViewMatrix[15] = 1.0f;
  glUniformMatrix4fv(m_iModelviewMatrixID, 1, false, m_afModelViewMatrix);
}



void
RenderTriangle::render()
{
  // clear frame and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // render vertex array
  glBindVertexArray(m_uiVAO);
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)0);
}




float RenderTriangle::afVertexList[] =
{
  -1.0f, -1.0f, 0.0f,
  1.0f, -1.0f, 0.0f,
  0.0f, 1.0f, 0.0f
};

unsigned int RenderTriangle::auiIndexList[] =
{
  0, 1, 2
};
