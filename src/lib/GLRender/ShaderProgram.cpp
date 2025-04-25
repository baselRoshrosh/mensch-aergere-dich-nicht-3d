#include "GLRender/ShaderProgram.h"

#include <iostream>
#include <cmath>



// constructor
ShaderProgram::ShaderProgram()
  : m_uiShaderPrg(0)
{
  // create program object
  m_uiShaderPrg = glCreateProgram();
  m_auiShaderIDs.resize(0);
}


// constructor
ShaderProgram::~ShaderProgram()
{
  // free all shader objects
  for( auto i=m_auiShaderIDs.begin(); i != m_auiShaderIDs.end(); ++i)
  {
    glDeleteShader( *i );
  }

  // free program
  if( m_uiShaderPrg ) glDeleteProgram( m_uiShaderPrg );
}



int 
ShaderProgram::addShader( const GLchar * const * ppcSrc, GLenum eShaderType )
{
  GLuint  uiID;
  GLint   iCompiled;

  // create vertex shader
  uiID = glCreateShader( eShaderType );
  if(0 == uiID) return -1;

  m_auiShaderIDs.push_back( uiID );

  // compile shader
  glShaderSource( uiID, 1, ppcSrc, NULL);
  glCompileShader( uiID );

  // check for errors and output them
  glGetShaderiv( uiID, GL_COMPILE_STATUS, &iCompiled);
  if(!iCompiled)
  {
    GLint iLength;
    GLchar* pcMessage;

    glGetShaderiv( uiID, GL_INFO_LOG_LENGTH, &iLength);
    pcMessage = new GLchar[iLength];
    if(NULL == pcMessage) return -1;
    glGetShaderInfoLog( uiID, iLength, &iLength, pcMessage);
    std::cout << "compile error for shader " << eShaderType << ": " << pcMessage << std::endl;
    delete[] pcMessage;
    return -1;
  }

  // attach shader
  glAttachShader( m_uiShaderPrg, uiID);

  return 0;
}



int
ShaderProgram::linkShaders()
{
  GLint   iLinked;

  // link shaders to program
  glLinkProgram(m_uiShaderPrg);

  // check for errors and output them
  glGetProgramiv(m_uiShaderPrg, GL_LINK_STATUS, &iLinked);
  if(!iLinked)
  {
    GLint iLength;
    GLchar* pcMessage;

    glGetProgramiv(m_uiShaderPrg, GL_INFO_LOG_LENGTH, &iLength);
    pcMessage = new GLchar[iLength];
    if(NULL == pcMessage) return -1;
    glGetProgramInfoLog(m_uiShaderPrg, iLength, &iLength, pcMessage);
    std::cout << "linker error: " << pcMessage << std::endl;
    delete[] pcMessage;
    return -1;
  }

  useProgram();

  return 0;
}


void
ShaderProgram::useProgram()
{
  // set as active program
  glUseProgram(m_uiShaderPrg);
}

