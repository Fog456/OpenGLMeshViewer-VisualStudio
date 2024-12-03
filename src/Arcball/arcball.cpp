#include "Arcball.h"
#include <stdio.h>
#include<iostream>

CArcball::CArcball( int win_width, int win_height, int ox, int oy )
{
  m_radius = sqrt(win_width / 2 * win_width / 2 + win_height / 2 * win_height / 2);
  m_center = Vector2D( win_width/2, win_height/2 ); 

  width = win_width;
  height = win_height;

  Vector2D p(ox,oy);

  _plane2sphere( p, m_position );
}

void CArcball::_plane2sphere( const Vector2D & p, Vector3D & q ) //窗口空间转换为单位球空间
{

  Vector2D f = p;
  f /= m_radius;

  double l = sqrt( f*f );

  if( l > 1.0 ){ //若x*x+y*y>=1,则z=0
      q=Vector3D( f.x/l, f.y/l,0);
      return;
  }

  double fz = sqrt( 1 - l * l ); //若x*x+y*y<1,则z*z=1-x*x-y*y

  q = Vector3D( f.x,f.y,fz );
}

CQrot CArcball::update( int nx, int ny ) //单位球空间到模型空间转换
{
    Vector3D position;
    _plane2sphere( Vector2D(nx,ny), position );
    Vector3D cp = m_position^position;
    //std::cout << m_radius << std::endl;
    CQrot r(m_position * position, cp.x,cp.y,cp.z); //四元数
    m_position = position;

    return r;
}