#ifndef RENDERIF_H
#define RENDERIF_H


class RenderIf
{
public:
  RenderIf() {}
  virtual ~RenderIf() {}

  // set the size of the window
  virtual void setWindowSize( const int iWidth, const int iHeight ) = 0;

  // initialize the GL state
  virtual void initGL() = 0;

  // free all GL resources
  virtual void uninitGL() = 0;

  // update camera parameters and object orientation
  virtual void renderCamera() = 0;

  // draw the scene
  virtual void render() = 0;

  // pass a key to the renderer
  virtual void keyPressed( unsigned char ucKey) = 0;

  // rotate scene around y axis
  virtual void rotY( const float fAngle ) = 0;

  // rotate scene around x axis
  virtual void rotX( const float fAngle) = 0;

  // translate camera in z direction (scale distance)
  virtual void transZ(const float fZ) = 0;

};



#endif