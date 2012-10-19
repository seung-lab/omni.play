//
// Copyright (C) 2010  Aleksandar Zlateski <zlateski@mit.edu>
// ----------------------------------------------------------
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

//
// link against glut and glu ( e.g. -lglut -lGLU )
//

#include <zi/gl/glut.hpp>
#include <zi/gl/camera.hpp>
#include <string>
#include <fstream>
#include <sstream>

#include <zi/mesh/marching_cubes.hpp>
#include <zi/mesh/quadratic_simplifier.hpp>
#include <zi/mesh/int_mesh.hpp>
#include <zi/mesh/face_mesh.hpp>
#include <zi/time.hpp>


#include <cstdlib>
#include <list>
#include <map>
#include <set>

#include <zlib.h>

#include "rtm.hpp"


//zi::mesh::simplifier<double> s;
zi::mesh::int_mesh im;
zi::mesh::face_mesh<double> fm;
zi::mesh::face_mesh<double> fm1;
zi::mesh::face_mesh<double> fm2;

inline std::string file_get_contents(const std::string& f)
{
    std::ifstream t(f.c_str());
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

bool is_wire = false;
bool is_zoom = false;
zi::gl::camera camera;

void reshape_func( int w, int h )
{
    camera.dimensions( w, h );

    camera.viewport( 0, 0, w, h );
    camera.apply_viewport();

    zi::gl::glMatrixMode( zi::gl::projection );
    zi::gl::glLoadIdentity();

    camera.perspective( 50 );
    camera.apply_perspective();
}

void keyboard_func( unsigned char key, int, int )
{
    switch ( key )
    {
    case 27:
        exit( 0 );
        break;
    case 'z':
        is_zoom = !is_zoom;
        break;
    default:
        is_wire = !is_wire;
    }

    zi::gl::glutPostRedisplay();
}

void draw_int_mesh( float xtrans, float ytrans, float ztrans, float xscale, float yscale, float zscale )
{
    const std::vector< zi::vl::vec<uint64_t,3> >& data = im.data();

    zi::gl::glBegin(zi::gl::triangles);

    FOR_EACH( it, data )
    {
        zi::vl::vec3f a, b, c;
        //ret.add_face( pts[ it->at(0) ], pts[ it->at(1) ], pts[ it->at(2) ] );

        a[0] = zi::mesh::marching_cubes<int>::unpack_x< float >( it->at(0), xtrans, xscale );
        a[1] = zi::mesh::marching_cubes<int>::unpack_y< float >( it->at(0), ytrans, yscale );
        a[2] = zi::mesh::marching_cubes<int>::unpack_z< float >( it->at(0), ztrans, zscale );

        b[0] = zi::mesh::marching_cubes<int>::unpack_x< float >( it->at(1), xtrans, xscale );
        b[1] = zi::mesh::marching_cubes<int>::unpack_y< float >( it->at(1), ytrans, yscale );
        b[2] = zi::mesh::marching_cubes<int>::unpack_z< float >( it->at(1), ztrans, zscale );

        c[0] = zi::mesh::marching_cubes<int>::unpack_x< float >( it->at(2), xtrans, xscale );
        c[1] = zi::mesh::marching_cubes<int>::unpack_y< float >( it->at(2), ytrans, yscale );
        c[2] = zi::mesh::marching_cubes<int>::unpack_z< float >( it->at(2), ztrans, zscale );

        zi::vl::vec3f n = zi::vl::norm(zi::vl::cross(c-a,c-b));

        zi::gl::glNormal3f(n[0], n[1], n[2]);
        zi::gl::glVertex3f(a[0], a[1], a[2]);
        zi::gl::glVertex3f(b[0], b[1], b[2]);
        zi::gl::glVertex3f(c[0], c[1], c[2]);
    }

    std::cout << "DONE\n";



    zi::gl::glEnd();
}

void draw_tri_strip( zi::mesh::simplifier<double>& s )
{
    std::vector< zi::vl::vec< double, 3 > > points;
    std::vector< zi::vl::vec< double, 3 > > normals;
    std::vector< uint32_t > indices;
    std::vector< uint32_t > strip_begins;
    std::vector< uint32_t > strip_length;

    s.stripify(points, normals, indices, strip_begins, strip_length);

    zi::gl::glPushMatrix();
    zi::gl::glScalef(0.05,0.05,0.05);
    //zi::gl::glScalef(10,10,10);

    zi::gl::glBegin(zi::gl::triangles);

    for ( std::size_t i = 0; i < strip_begins.size(); ++i )
    {
        bool even = true;

        //std::cout << "L: " << strip_length[i] << '\n';

        for ( uint32_t j = strip_begins[i]; j < strip_begins[i] + strip_length[i] - 2; ++j )
        {
            if ( !even )
            {
                //std::cout << points[indices[j]] << '\n';
                //zi::vl::vec3d n = zi::vl::norm(normals[indices[j]]+normals[indices[j+1]]+normals[indices[j+2]]);
                //zi::gl::glNormal3f(n[0], n[1], n[2]);
                zi::gl::glNormal3f(normals[indices[j]][0],normals[indices[j]][1],normals[indices[j]][2]);
                zi::gl::glVertex3d(points[indices[j]][0],points[indices[j]][1],points[indices[j]][2]);
                zi::gl::glNormal3f(normals[indices[j+1]][0],normals[indices[j+1]][1],normals[indices[j+1]][2]);
                zi::gl::glVertex3d(points[indices[j+1]][0],points[indices[j+1]][1],points[indices[j+1]][2]);
                zi::gl::glNormal3f(normals[indices[j+2]][0],normals[indices[j+2]][1],normals[indices[j+2]][2]);
                zi::gl::glVertex3d(points[indices[j+2]][0],points[indices[j+2]][1],points[indices[j+2]][2]);

            }
            else
            {
                //zi::vl::vec3d n = zi::vl::norm(normals[indices[j]]+normals[indices[j+1]]+normals[indices[j+2]]);
                //zi::gl::glNormal3f(n[0], n[1], n[2]);
                zi::gl::glNormal3f(normals[indices[j+2]][0],normals[indices[j+2]][1],normals[indices[j+2]][2]);
                zi::gl::glVertex3d(points[indices[j+2]][0],points[indices[j+2]][1],points[indices[j+2]][2]);
                zi::gl::glNormal3f(normals[indices[j+1]][0],normals[indices[j+1]][1],normals[indices[j+1]][2]);
                zi::gl::glVertex3d(points[indices[j+1]][0],points[indices[j+1]][1],points[indices[j+1]][2]);
                zi::gl::glNormal3f(normals[indices[j]][0],normals[indices[j]][1],normals[indices[j]][2]);
                zi::gl::glVertex3d(points[indices[j]][0],points[indices[j]][1],points[indices[j]][2]);





            }
            even = !even;
        }
    }

    zi::gl::glEnd();
    zi::gl::glPopMatrix();

}


void draw_faces( zi::mesh::face_mesh<double>& fm )
{
    std::vector< zi::vl::vec< double, 3 > >& points = fm.points();
    std::vector< zi::vl::vec< double, 3 > >& normals = fm.normals();
    std::vector< zi::vl::vec< uint32_t, 3 > >& indices = fm.faces();

    // std::vector< zi::vl::vec< double, 3 > >   points;
    // std::vector< zi::vl::vec< double, 3 > >   normals;
    // std::vector< zi::vl::vec< uint32_t, 3 > > indices;

    //s.get_faces(points, normals, indices);

    //std::cout << indices.size() << ' ' << points.size() << " ----\n";

    zi::gl::glPushMatrix();
    zi::gl::glScalef(0.05,0.05,0.05);

    zi::gl::glBegin(zi::gl::triangles);

    for ( std::size_t i = 0; i < indices.size(); ++i )
    {
        for ( uint32_t j = 0; j < 3; ++j )
        {
            //std::cout << i << " out i of " << indices.size() << '\n';
            //std::cout << i << " out v of " << points.size() << '\n';
            //std::cout << i << " out n of " << normals.size() << '\n';

            zi::vl::vec< double, 3 > p = points[indices[i][j]];
            zi::vl::vec< double, 3 > n = zi::vl::norm(normals[indices[i][j]]);

            //std::cout << p << " -- " << n << '\n';

            zi::gl::glNormal3d(n[0],n[1],n[2]);
            zi::gl::glVertex3d(p[0],p[1],p[2]);
        }
    }

    zi::gl::glEnd();
    zi::gl::glPopMatrix();
}


void draw_scene(void)
{
    zi::gl::glClear(zi::gl::color_buffer_bit | zi::gl::depth_buffer_bit);

    zi::gl::glMatrixMode( zi::gl::modelview );
    zi::gl::glLoadIdentity();

    camera.apply_modelview();

    float lt_diff[] = { 1.0, 1.0, 1.0, 1.0 };
    float lt_pos[]  = { -130.0, -130.0, 150.0, 1.0 };

    zi::gl::glLightfv(zi::gl::light0, zi::gl::diffuse,  lt_diff );
    zi::gl::glLightfv(zi::gl::light0, zi::gl::position, lt_pos  );

    zi::gl::glEnable( zi::gl::depth_test );
    zi::gl::glEnable( zi::gl::lighting );
    zi::gl::glEnable( zi::gl::light0 );
    zi::gl::glEnable( zi::gl::normalize );

    zi::gl::glShadeModel( zi::gl::smooth );
    zi::gl::glPolygonMode( zi::gl::front_and_back, zi::gl::fill );

    zi::gl::glClearColor( 0, 0, 0, 1 );

    float diff_color[] = { 0.4, 0.4, 0.4, 1 };
    float spec_color[] = { 0.9, 0.9, 0.9, 1 };
    float shininess[] = { 50.0 };

    zi::gl::glMaterialfv( zi::gl::front_and_back, zi::gl::ambient_and_diffuse, diff_color );
    zi::gl::glMaterialfv( zi::gl::front_and_back, zi::gl::specular, spec_color );
    zi::gl::glMaterialfv( zi::gl::front_and_back, zi::gl::shininess, shininess );

    zi::gl::glEnable( zi::gl::color_material );

    zi::gl::glColor4f( 0.4, 0.9, 0.1, 1 );
    draw_faces(fm);

    zi::gl::glColor4f( 0.9, 0.1, 0.4, 1 );
    draw_faces(fm1);

    zi::gl::glColor4f( 0.1, 0.4, 0.9, 1 );
    draw_faces(fm2);

    if ( is_wire )
    {
        //zi::gl::glutWireTeapot( 20.0 );
    }
    else
    {
        //zi::gl::glutSolidTeapot( 20.0 );
    }

    //draw_tri_strip(s);
    //draw_int_mesh(0,0,0,21.0/256,21.0/256,21.0/256);


    zi::gl::glutSwapBuffers();
}


void timer_func( int t )
{
    // zi::gl::glutPostRedisplay();
    zi::gl::glutTimerFunc(t, timer_func, t);
}

void mouse_func( int button, int state, int x, int y )
{
    if ( state == zi::glut::down )
    {
        switch ( button )
        {
        case zi::glut::left_button:
            camera.start_translation( x, y );
            break;

        case zi::glut::middle_button:
            camera.start_zoom( x, y );
            break;

        case zi::glut::right_button:
            if ( is_zoom )
            {
                camera.start_zoom( x, y );
            }
            else
            {
                camera.start_rotation( x, y );
            }
            break;

        }
    }
    else
    {
        camera.finish_operation( x, y );
        zi::gl::glutPostRedisplay();
    }

}

void motion_func( int x, int y )
{
    camera.update_operation( x, y );
    camera.apply_modelview();
    zi::gl::glutPostRedisplay();
}



int main( int argc, char* argv[] ) {

    zi::mesh::rtm rtm;

    //return 0;

    if (0)
    for ( uint32_t i = 0;i < 10; ++i )
        for ( uint32_t j = 0; j < 10; ++j )
            for ( uint32_t k = 0; k < 10; ++k )
            {
                std::string s = file_get_contents
                    ( boost::str( boost::format( "./zt/raw_%04d_%04d_%04d.raw")
                                  % i % j % k) );
                if ( s.size() ) {
                    rtm.chunk_update(i,j,k,s.data());
                }
            }
    //  }

    uint32_t* d = new uint32_t[128*128*128];

    for ( uint32_t i = 0, idx = 0; i < 128; ++i )
        for ( uint32_t j = 0; j < 128; ++j )
            for ( uint32_t k = 0; k < 128; ++k )
            {
                if ( i > 2 && i < 120 && k > 2 && k < 120 )
                {
                    d[idx] = ( j > 50 ) ? 61 : 0;
                }
                d[idx] = 61;
                idx++;
            }

    // rtm.volume_update( 50, 50, 50, 128, 128, 128,
    //                    reinterpret_cast<char*>(d));

    // rtm.volume_update( 51, 51, 51, 128, 128, 128,
    //                    reinterpret_cast<char*>(d));

    rtm.chunk_update(1,1,1,reinterpret_cast<char*>(d));
    rtm.chunk_update(1,2,1,reinterpret_cast<char*>(d));
    rtm.chunk_update(2,1,1,reinterpret_cast<char*>(d));
//    rtm.chunk_update(1,2,1,reinterpret_cast<char*>(d));
//    rtm.chunk_update(1,1,1,reinterpret_cast<char*>(d));
    // rtm.chunk_update(1,1,2,reinterpret_cast<char*>(d));
    // rtm.chunk_update(2,1,2,reinterpret_cast<char*>(d));
    // rtm.chunk_update(2,1,1,reinterpret_cast<char*>(d));

    // for ( uint32_t i = 0, idx = 0; i < 128; ++i )
    //     for ( uint32_t j = 0; j < 128; ++j )
    //         for ( uint32_t k = 0; k < 128; ++k )
    //         {
    //             if ( i > 2 && i < 120 && k > 2 && k < 120 )
    //             {
    //                 d[idx] = ( j < 50 ) ? 61 : 0;
    //             }
    //             idx++;
    //         }

    // // rtm.volume_update( 0, 100, 0, 100, 50, 100,
    // //                    reinterpret_cast<char*>(d));

    // rtm.chunk_update(1,2,1,reinterpret_cast<char*>(d));
    // rtm.chunk_update(1,2,2,reinterpret_cast<char*>(d));
    // rtm.chunk_update(2,2,1,reinterpret_cast<char*>(d));
    // rtm.chunk_update(2,2,2,reinterpret_cast<char*>(d));

    std::cout << "HEEEEEEEEEERE!\n" << std::endl;

    for ( uint32_t i = 0; i < 3; ++i )
        for ( uint32_t j = 0; j < 3; ++j )
            for ( uint32_t k = 0; k < 3; ++k )
            {
                zi::mesh::face_mesh_ptr xp = zi::mesh::mesh_io.read(zi::mesh::vec5u(i,j,k,1,61));
                fm.add(xp,(256<<1)*k,(256<<1)*j,(256<<1)*i);
    //             xp = zi::mesh::vol::load_face_mesh<double>(zi::mesh::vec5u(3,i,j,k,81));

    //             fm1.add(xp,(256<<3)*k,(256<<3)*j,(256<<3)*i);
    //             xp = zi::mesh::vol::load_face_mesh<double>(zi::mesh::vec5u(3,i,j,k,71));

    //             fm2.add(xp,(256<<3)*k,(256<<3)*j,(256<<3)*i);
    //         }
            }

    zi::gl::glutInit( &argc, argv );
    zi::gl::glutInitDisplayMode( zi::glut::t_double |
                                 zi::glut::rgb |
                                 zi::glut::depth );

    zi::gl::glutInitWindowPosition( 60, 60 );
    zi::gl::glutInitWindowSize( 600, 600 );
    zi::gl::glutCreateWindow("Test zi/gl/glut.hpp");

    zi::gl::glutReshapeFunc( reshape_func );
    zi::gl::glutDisplayFunc( draw_scene );

    camera.dimensions( 600, 600 );
    camera.distance( 150 );
    camera.center( 0, 0, 0 );
    camera.viewport( 0, 0, 600, 600 );
    camera.perspective( 50 );

    zi::gl::glutTimerFunc( 10, timer_func, 10 );
    zi::gl::glutKeyboardFunc( keyboard_func );
    zi::gl::glutMouseFunc( mouse_func );
    zi::gl::glutMotionFunc( motion_func );


    zi::gl::glutMainLoop();

}
