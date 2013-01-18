#include "PCH.h"

/*! \file    shapes.c
    \ingroup demos

    This program is a test harness for the various shapes
    in OpenGLUT.  It may also be useful to see which
    parameters control what behavior in the OpenGLUT
    objects.
 
    Spinning wireframe and solid-shaded shapes are
    displayed.  Some parameters can be adjusted.
 
   Keys:
      -    <tt>Esc &nbsp;</tt> Quit
      -    <tt>q Q &nbsp;</tt> Quit
      -    <tt>i I &nbsp;</tt> Show info
      -    <tt>d D &nbsp;</tt> Show debug
      -    <tt>= + &nbsp;</tt> Increase \a slices
      -    <tt>- _ &nbsp;</tt> Decreate \a slices
      -    <tt>, < &nbsp;</tt> Decreate \a stacks
      -    <tt>. > &nbsp;</tt> Increase \a stacks
      -    <tt>9 ( &nbsp;</tt> Decreate \a depth  (Sierpinski Sponge)
      -    <tt>0 ) &nbsp;</tt> Increase \a depth  (Sierpinski Sponge)
      -    <tt>up&nbsp; &nbsp;</tt> Increase "outer radius"
      -    <tt>down&nbsp;</tt> Decrease "outer radius"
      -    <tt>left&nbsp;</tt> Decrease "inner radius"
      -    <tt>right</tt> Increase "inner radius"
      -    <tt>PgUp&nbsp;</tt> Next shape-drawing function
      -    <tt>PgDn&nbsp;</tt> Prev shape-drawing function

    \author  Written by Nigel Stewart November 2003

    \author  Portions Copyright (C) 2004, the OpenGLUT project contributors. <br>
             OpenGLUT branched from freeglut in February, 2004.
 
    \image   html openglut_shapes.png OpenGLUT Geometric Shapes Demonstration
    \include demos/shapes/shapes.c
*/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Octree.h"
#include "OctreeDebugger.h"

#ifdef _MSC_VER
/* DUMP MEMORY LEAKS */
#include <crtdbg.h>
#endif


/*
 * These global variables control which object is drawn,
 * and how it is drawn.  No object uses all of these
 * variables.
 */
static glm::vec3 v3CameraPosition(0.0f, 0.0f, 10.0f);
static glm::vec3 v3CameraLookAt(0.0f, 0.0f, 0.0f);
static glm::vec3 v3CameraUp(0.0f, 1.0f, 0.0f);

static GLboolean s_bShowInfo = true;
static bool s_bRenderPoints = true;
static bool s_bRenderFaces = true;
static bool s_bDebug = true;
static size_t s_nDebugTreeLevel = 0;

static std::vector<glm::vec3>       s_aPoints;
static std::vector<glm::lowp_uvec3> s_aTriangles;
static std::vector<glm::lowp_uvec4> s_aQuads;

static Octree* s_pTree = NULL;

/*!
    Does printf()-like work using freeglut/OpenGLUT
    glutBitmapString().  Uses a fixed font.  Prints
    at the indicated row/column position.

    Limitation: Cannot address pixels.
    Limitation: Renders in screen coords, not model coords.
*/
static void shapesPrintf (int row, int col, const char *fmt, ...)
{
    static char buf[256];
    int viewport[4];
    void *font = GLUT_BITMAP_HELVETICA_18;
    va_list args;

    va_start(args, fmt);
#if defined(WIN32) && !defined(__CYGWIN__)
    (void) _vsnprintf (buf, sizeof(buf), fmt, args);
#else
    (void) vsnprintf (buf, sizeof(buf), fmt, args);
#endif
    va_end(args);

    glGetIntegerv(GL_VIEWPORT,viewport);

    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

        glOrtho(0,viewport[2],0,viewport[3],-1,1);

        //TODO: Port to OSX
        glRasterPos2i
        (
              glutBitmapWidth(font, ' ') * col,
            - 20 /*glutBitmapHeight(font)*/ * (row+2) + viewport[3]
        );
    
        for(size_t i = 0; i < 256 && buf[i] != 0; ++i )
        {
            glutBitmapCharacter(font, buf[i]);
        }

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

/* GLUT callback Handlers */

static void
resize(int width, int height)
{
    glViewport(0, 0, width, height);

    const float fAspectRatio = (float) width / (float) height;

    
    glm::mat4 aProjection = glm::perspective(45.0f, fAspectRatio, 0.1f, 1000.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
//    gluPerspective(45.0, fAspectRatio, 0.1, 1000.0);
    glLoadMatrixf(glm::value_ptr(aProjection));
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 m4Modelview = glm::lookAt(
        v3CameraPosition,
        v3CameraLookAt,
        v3CameraUp);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
    glLoadMatrixf(glm::value_ptr(m4Modelview));

    glEnable(GL_LIGHTING);

    if (s_bRenderPoints && s_aPoints.size())
    {
        glColor3f(1.0f,0.0f,0.0f);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, s_aPoints.data());
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(s_aPoints.size()));
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    if (s_bRenderFaces)
    {
        glColor3f(1.0f,0.0f,0.0f);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, s_aPoints.data());
        
        if(s_aTriangles.size())
        {
            glDrawElements(GL_TRIANGLES, s_aTriangles.size() * 3, GL_UNSIGNED_SHORT, s_aTriangles.data());
        }
        if (s_aQuads.size())
        {
            glDrawElements(GL_QUADS, s_aTriangles.size() * 4, GL_UNSIGNED_SHORT, s_aQuads.data());
        }
        
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    if (s_bDebug)
    {
        OctreeDebugger::render(*s_pTree, s_nDebugTreeLevel, s_bRenderPoints);
    }

    glDisable(GL_LIGHTING);
    
    
    glColor3d(0.1,0.1,0.1);

    if( s_bShowInfo )
    {
        shapesPrintf (1, 3, "Octree depth: %u", s_pTree->getDepth());
        shapesPrintf (2, 3, "Curent KD-Tree debug depth: %u", s_nDebugTreeLevel);
        shapesPrintf (3, 3, "Camera LookAt: (%f, %f, %f)", v3CameraLookAt.x, v3CameraLookAt.y, v3CameraLookAt.z);
        shapesPrintf (4, 3, "Camera Position: (%f, %f, %f)", v3CameraPosition.x, v3CameraPosition.y, v3CameraPosition.z);
        shapesPrintf (5, 3, "Camera Up: (%f, %f, %f)", v3CameraUp.x, v3CameraUp.y, v3CameraUp.z);
    }

    glutSwapBuffers();
}


static void
key(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27 :
    case 'Q':
    case 'q':
        exit(0);
        break;

    case 'P':
    case 'p':
        s_bRenderPoints = !s_bRenderPoints;
        glutPostRedisplay();
        break;

    case 'T':
    case 't':
        s_bRenderFaces = !s_bRenderFaces;
        glutPostRedisplay();
        break;

    case 'D':
    case 'd':
        s_bDebug = !s_bDebug;
        glutPostRedisplay();
        break;

    case 'I':
    case 'i':
        s_bShowInfo = !s_bShowInfo;
        glutPostRedisplay();
        break;

    case '+':
        s_nDebugTreeLevel++;
        glutPostRedisplay();
        break;

    case '-':
        if (s_nDebugTreeLevel>0)
        {
            s_nDebugTreeLevel--;
            glutPostRedisplay();
        }
        break;
    case ' ':
        v3CameraPosition = glm::vec3(0.0f, 0.0f, 10.0f);
        v3CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glutPostRedisplay();
        break;
            

    default:
        break;
    }

    glutPostRedisplay();
}

static void special (int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_PAGE_UP:
        {
            // rotate the camera position around the X-Axis of the camera
            glm::vec3 v3CameraX = glm::cross(-glm::normalize(v3CameraPosition),v3CameraUp);
            v3CameraPosition = ( glm::rotate(10.0f, v3CameraX) * glm::vec4(v3CameraPosition, 1.0f) ).xyz();
            v3CameraUp = ( glm::rotate(10.0f, v3CameraX) * glm::vec4(v3CameraUp, 1.0f) ).xyz();
            
            glutPostRedisplay();
        }
            
        break;
    case GLUT_KEY_PAGE_DOWN:
        {
            // rotate the camera position around the X-Axis of the camera
            glm::vec3 v3CameraX = glm::cross(-glm::normalize(v3CameraPosition),v3CameraUp);
            v3CameraPosition = ( glm::rotate(-10.0f, v3CameraX) * glm::vec4(v3CameraPosition, 1.0f) ).xyz();
            v3CameraUp = ( glm::rotate(-10.0f, v3CameraX) * glm::vec4(v3CameraUp, 1.0f) ).xyz();

            glutPostRedisplay();
        }
            
        break;


    case GLUT_KEY_UP:
        v3CameraPosition = 0.8f * v3CameraPosition;
        glutPostRedisplay();
        break;
    case GLUT_KEY_DOWN:
        v3CameraPosition = 1.2f * v3CameraPosition;
        glutPostRedisplay();
        break;

    case GLUT_KEY_RIGHT:
        // rotate the camera position around the up vector of the camera
        v3CameraPosition = (glm::rotate(glm::mat4(1.0f), 10.0f, v3CameraUp) * glm::vec4(v3CameraPosition, 1.0f)).xyz();
        glutPostRedisplay();
        break;
    case GLUT_KEY_LEFT:
        // rotate the camera position around the Y-Axis
        v3CameraPosition = (glm::rotate(glm::mat4(1.0f), -10.0f, v3CameraUp) * glm::vec4(v3CameraPosition, 1.0f)).xyz();
        glutPostRedisplay();
        break;

    default:
        break;
    }
}


static void idle(void)
{

}

static void initPoints(size_t nCount, std::vector<glm::vec3>& rPoints)
{
    rPoints.resize(nCount, glm::vec3(0.0f));

    for (size_t i=0; i<nCount; i++)
    {
        rPoints.at(i) = glm::linearRand(glm::vec3(-1.0f), glm::vec3(1.0f));
    }
}

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };


// Stolen from http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Load_OBJ
void load_obj(const char* pFilename,
              std::vector<glm::vec3>& rVertices,
              std::vector<glm::vec3>& rNormals,
              std::vector<glm::lowp_uvec3>& rTriangles,
              std::vector<glm::lowp_uvec4>& rQuads)
{
    
    std::ifstream aFileStream(pFilename, std::ios::in);
    if (!aFileStream) {
        std::cerr << "Cannot open " << pFilename << std::endl;
        exit(1);
    }
    
    rVertices.clear();
    rNormals.clear();
    rTriangles.clear();
    rQuads.clear();

    std::string aLine;
    while (std::getline(aFileStream, aLine)) {
        if (aLine.substr(0,2) == "v ") {
            std::istringstream s(aLine.substr(2));
            
            glm::vec3 v;
            s >> v.x; s >> v.y; s >> v.z;
            rVertices.push_back(v);
        }  else if (aLine.substr(0,2) == "f ") {
            std::istringstream s(aLine.substr(2));
            GLushort a,b,c;
            s >> a; s >> b; s >> c;
            if ( s.bad() )
            {
                std::cerr << pFilename << " contains elements with less than 3 points" << std::endl;
            }
            else
            {
                GLushort d;
                s >> d;
                if ( !s.bad() && !s.fail())
                {
                    rQuads.push_back(glm::lowp_uvec4(a-1,b-1,c-1,d-1));
                }
                else
                {
                    rTriangles.push_back(glm::lowp_uvec3(a-1,b-1,c-1));
                }
            }
        }
        else if (aLine[0] == '#') { /* ignoring this line */ }
        else { /* ignoring this line */ }
    }
    
    std::cout   << "Loaded " << pFilename << " containing:" << std::endl
    << rVertices.size() << " vertices" << std::endl
    << rTriangles.size() << " triangles" << std::endl
    << rQuads.size() << " quads" << std::endl;
    


//    normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
//    for (size_t i = 0; i < elements.size(); i+=3) {
//        GLushort ia = elements[i];
//        GLushort ib = elements[i+1];
//        GLushort ic = elements[i+2];
//        glm::vec3 normal = glm::normalize(glm::cross(
//            glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
//            glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
//        normals[ia] = normals[ib] = normals[ic] = normal;
//    }
}

/* Program entry point */

int main(int argc, char *argv[])
{
    std::vector<glm::vec3>  aNormals;
    
    load_obj("data/bunny.obj", s_aPoints, aNormals, s_aTriangles, s_aQuads);
    
    BBox aBBox = BBox(s_aPoints);
    
    s_pTree = new Octree(aBBox,10);


    glutInitWindowSize(1024,768);
    glutInitWindowPosition(0,0);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

    glutCreateWindow("OpenGLUT KD Tree");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(special);
    glutIdleFunc(idle);

    glClearColor(1,1,1,1);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPointSize(2.0f);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    glutMainLoop();

    delete s_pTree;

#ifdef _MSC_VER
    /* DUMP MEMORY LEAK INFORMATION */
    _CrtDumpMemoryLeaks () ;
#endif

    return EXIT_SUCCESS;
}
