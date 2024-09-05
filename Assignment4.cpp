// CS4533 ASSIGNMENT 4, BY CONSTANTINOS VAFIAS
#include "Angel-yjc.h"
#include <cmath>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>

#ifdef __APPLE__  // include Mac OS X verions of headers
#include <GLUT/glut.h>
#else // non-Mac OS X operating systems
#include <GL/glut.h>
#endif

#define XOFF          50
#define YOFF          50
#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600

#include <iostream>
#include <fstream>
using std::ifstream;
using namespace std;

typedef Angel::vec4 color4;
typedef Angel::vec4 point4;
typedef Angel::vec3  color3;
typedef Angel::vec3  point3;

/* global definitions for constants and global image arrays */
// for assignment 4  - texture mapping
#define ImageWidth  64
#define ImageHeight 64
GLubyte Image[ImageHeight][ImageWidth][4];

#define	stripeImageWidth 32
GLubyte stripeImage[4 * stripeImageWidth];

GLuint texName;     // the checkerboard
GLuint texName2;    // the stripe

GLuint Angel::InitShader(const char* vShaderFile, const char* fShaderFile);

GLuint program;       /* shader program object id */
GLuint floor_buffer;  /* vertex buffer object id for floor  */
GLuint x_axis_buffer; /* vertex buffer object id for x-axis */
GLuint y_axis_buffer; /* vertex buffer object id for y-axis */
GLuint z_axis_buffer; /* vertex buffer object id for z-axis */
GLuint sphere_buffer; /* vertex buffer object id for sphere */
GLuint shadow_buffer; /* vertex buffer object id for sphere shadow */

// Projection transformation parameters
GLfloat  fovy = 45.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect;       // Viewport aspect ratio
GLfloat  zNear = 0.5, zFar = 30.0f; //3.0f

GLfloat angle = 0.0; // rotation angle in degrees
vec4 init_eye(7, 3, -10, 1); // initial viewer position
vec4 eye = init_eye;               // current viewer position

// Various flags for animation, and toggling effects/shading/object display
int animationFlag = 1;     // 1: animation; 0: non-animation.
int fillFlag = 1;          // 1: solid sphere/shadow; 0: wireframe sphere/shadow
int shadowFlag = 1;        // 1== there is a shadow, 0 == there isn't
int lightingFlag = 1;      // 1 == there is lighting, 0 == there isn't
int shadingFlag = 1;       // flat shading == 1, smooth shading == 0
int light_source_flag = 1; // 1 - Spot Light, 0 - Point Source
int fogFlag = 0.0f;        // 0 - no fog, 1 - linear, 2 - exp, 3 - exp sq
int blendingFlag = 0;      // 0 - no blending, 1 - blending
int textureFlag = 0;       // 0 - no texture, 1 - texture color, 2 - obj * texture color
int modeFlag = 0;          // 0 - vertical, 1 - slanted
int spaceFlag = 1;         // 1 - object, 0 - eye
int verFlag = 0;           // 0 - stripe, 1 - checkerboard
int utFlag = 1;            // 1 - upright, 0 - tilted
int latticeFlag = 0;       // 0 - lattice off, 1- lattice on

bool draw_sp_texture;

// variables used for keeping track of the sphere's polygons
int total_polygons;

#if 0
color3 cube_colors[cube_NumVertices]; // colors for all vertices
#endif
#if 1
color3 cube_colors[100];
#endif

const int floor_NumVertices = 6; //(1 face)*(2 triangles/face)*(3 vertices/triangle)
point3 floor_points[floor_NumVertices]; // positions for all vertices
color4 floor_colors[floor_NumVertices]; // colors for all vertices
vec3 floor_normals[floor_NumVertices]; //arr of normals for the floor


// defining variables for the axes (X, Y, Z)
const int axes_NumVertices = 2;
point3 x_axis_points[axes_NumVertices]; // positions for the two vertices
color4 x_axis_colors[axes_NumVertices]; // colors for the two vertices

point3 y_axis_points[axes_NumVertices]; // positions for the two vertices
color4 y_axis_colors[axes_NumVertices]; // colors for the two vertices

point3 z_axis_points[axes_NumVertices]; // positions for the two vertices
color4 z_axis_colors[axes_NumVertices]; // colors for the two vertices

//defining variables for the sphere
vector<float> vec;

const int sphere_NumVertices = 1024 * 3;  //FIX LATER!
point3 sphere_points[sphere_NumVertices];
point3 shadow_points[sphere_NumVertices];
vec3 normals[sphere_NumVertices];
vec3 flat_normals[sphere_NumVertices];
vec3 smooth_normals[sphere_NumVertices];

color4 sphere_colors[sphere_NumVertices];
color4 shadow_colors[sphere_NumVertices];

color4 light_source = color4(-14.0f, 12.0f, -3.0f, 1.0f);
color4 sp_point = color4(-6.0f, 0.0f, -4.5f, 1.0f);
vec4 light_source_eye;

// FUNCTION PROTOTYPES:
void read_file(void);
//void set_lighting_param(color4 ambient, color4 diffuse, color4 specular, float shininess);

// RGBA colors
color4 vertex_colors[8] = {
    color4(0.0, 0.0, 0.0, 1.0),  // black
    color4(1.0, 0.0, 0.0, 1.0),  // red
    color4(1.0, 1.0, 0.0, 1.0),  // yellow
    color4(0.0, 1.0, 0.0, 1.0),  // green
    color4(0.0, 0.0, 1.0, 1.0),  // blue
    color4(1.0, 0.0, 1.0, 1.0),  // magenta
    color4(1.0, 1.0, 1.0, 1.0),  // white
    color4(0.0, 1.0, 1.0, 1.0)   // cyan
};
// Vertices of the floor
point3 floor_vertices[4] = {
    point3(5.0,   0.0,   8.0),
    point3(5.0,   0.0,  -4.0),
    point3(-5.0,  0.0,  -4.0),
    point3(-5.0,  0.0,   8.0)
};
// Vertices of the x axis
point3 x_axis_vertices[2] = {
    point3(0.0, 0.0, 0.0),
    point3(10.0, 0.02, 0.0)
};
// Vertices of the y axis
point3 y_axis_vertices[2] = {
    point3(0.0, 0.0, 0.0),
    point3(0.0, 10.0, 0.0)
};
// Vertices of the z axis
point3 z_axis_vertices[2] = {
    point3(0.0, 0.0, 0.0),
    point3(0.0, 0.02, 10.0)
};

// defined points for ball rolling
float curr_x = 3.0f, curr_y = 1.0f, curr_z = 5.0f;

// flags defined for rolling
bool is_rolling = true;
bool ab_traveling = true;
bool bc_traveling = false;
bool ac_traveling = false;

// flags for key presses, menu
bool begin_roll = false;
// defined matrix M for part d of Assignment 2 (correct rolling)
mat4 matrixM = (1.0f);
// defined shadow projection matrix
mat4 shadow_projection = mat4(vec4(12.0f, 14.0f, 0.0f, 0.0f), vec4(0.0f, 0.0f, 0.0f, 0.0f), vec4(0.0f, 3.0f, 12.0f, 0.0f), vec4(0.0f, -1.0f, 0.0f, 12.0f));
//----------------------------------------------------------------------------
int Index = 0; // YJC: This must be a global variable since quad() is called
               //      multiple times and Index should then go up to 36 for
               //      the 36 vertices and colors

//-------------------------------
// generate 2 triangles: 6 vertices and 6 colors
void floor()
{
    floor_colors[0] = vertex_colors[3]; floor_points[0] = floor_vertices[1];     floor_normals[0] = vec3(0.0f, 1.0f, 0.0f);
    floor_colors[1] = vertex_colors[3]; floor_points[1] = floor_vertices[2];     floor_normals[1] = vec3(0.0f, 1.0f, 0.0f);
    floor_colors[2] = vertex_colors[3]; floor_points[2] = floor_vertices[3];     floor_normals[2] = vec3(0.0f, 1.0f, 0.0f);

    floor_colors[3] = vertex_colors[3]; floor_points[3] = floor_vertices[3];     floor_normals[3] = vec3(0.0f, 1.0f, 0.0f);
    floor_colors[4] = vertex_colors[3]; floor_points[4] = floor_vertices[0];     floor_normals[4] = vec3(0.0f, 1.0f, 0.0f);
    floor_colors[5] = vertex_colors[3]; floor_points[5] = floor_vertices[1];     floor_normals[5] = vec3(0.0f, 1.0f, 0.0f);
}
//----------------------------------------------------------------------------
// generate the texture coordinates for the floor
vec2 floor_texCoord[6] = {
  vec2(1.25, 0.0),  // for a
  vec2(0.0, 0.0),  // for b
  vec2(0.0, 1.5),  // for c

  vec2(0.0, 1.5),  // for c
  vec2(1.25, 1.5),  // for d
  vec2(1.25, 0.0),  // for a 
};
//----------------------------------------------------------------------------
// Generate 2 vertices for the x-axis
void x_axis() {
    x_axis_colors[0] = vertex_colors[1]; x_axis_points[0] = x_axis_vertices[0];
    x_axis_colors[1] = vertex_colors[1]; x_axis_points[1] = x_axis_vertices[1];
}
//----------------------------------------------------------------------------
// Generate 2 vertices for the y-axis
void y_axis() {
    y_axis_colors[0] = vertex_colors[5]; y_axis_points[0] = y_axis_vertices[0];
    y_axis_colors[1] = vertex_colors[5]; y_axis_points[1] = y_axis_vertices[1];
}
//----------------------------------------------------------------------------
// Generate 2 vertices for the z-axis
void z_axis() {
    z_axis_colors[0] = vertex_colors[4]; z_axis_points[0] = z_axis_vertices[0];
    z_axis_colors[1] = vertex_colors[4]; z_axis_points[1] = z_axis_vertices[1];
}

/*----- Shader Lighting Parameters -----*/
// global lighting parameters
color4 global_ambient(1.0f, 1.0f, 1.0f, 1.0f);
color4 distant_light_ambient(0.0f, 0.0f, 0.0f, 1.0f);
color4 distant_light_diffuse(0.8f, 0.8f, 0.8f, 1.0f);
color4 distant_light_specular(0.2f, 0.2f, 0.2f, 1.0f);
vec4 direction(0.1f, 0.0f, -1.0f, 0.0f);

color4 positional_light_ambient(0.0f, 0.0f, 0.0f, 1.0f);
color4 positional_light_diffuse(1.0f, 1.0f, 1.0f, 1.0f);
color4 positional_light_specular(1.0f, 1.0f, 1.0f, 1.0f);

// floor lighting parameters
color4 floor_light_ambient(0.2f, 0.2f, 0.2f, 1.0f);
color4 floor_light_diffuse(0.0f, 1.0f, 0.0f, 1.0f);
color4 floor_light_specular(0.0f, 0.0f, 0.0f, 1.0f);

// sphere lighting parameters
color4 sphere_light_ambient(0.2f, 0.2f, 0.2f, 1.0f);
color4 sphere_light_diffuse(1.0f, 0.84f, 0.0f, 1.0f);
color4 sphere_light_specular(1.0f, 0.84f, 0.0f, 1.0f);

float  const_att = 2.0f;
float  linear_att = 0.01f;
float  quad_att = 0.001f;
float  material_shininess = 125.0f;

void read_file(void) {
    ifstream data_reading;
    string file_name;

    // request the file name (sphere.8.txt/sphere.128.txt)
    cout << "Input the name of the file: ";
    cin >> file_name;

    // data to be read from the file
    int num_polygons;
    float x_coord, y_coord, z_coord;

    data_reading.open(file_name);
    if (!data_reading) {
        cerr << "filenotfound" << endl;
    }
    else {
        data_reading >> total_polygons; //read 8 or 128
        // for every polygon... 
        for (int i = 0; i < total_polygons; i++) {
            data_reading >> num_polygons;
            // for however many sides per polygon (3) ... 
            for (int j = 0; j < num_polygons; j++) {
                data_reading >> x_coord >> y_coord >> z_coord;
                //cout << x_coord << "\t" << y_coord << "\t" << z_coord << endl;
                vec.push_back(x_coord);
                vec.push_back(y_coord);
                vec.push_back(z_coord);
            }
        }
    }
}
void sphere() {
    int sphere_index = 0;
    for (int i = 0; i < vec.size(); i += 9) {
        //FLAT SHADING
        //computing the normals of each triangle
        //each vertex will have the normal associated with the triangle its a part of
        vec3 edge1 = vec3(vec[i + 3], vec[i + 4], vec[i + 5]) - vec3(vec[i], vec[i + 1], vec[i + 2]); //B - A
        vec3 edge2 = vec3(vec[i + 6], vec[i + 7], vec[i + 8]) - vec3(vec[i], vec[i + 1], vec[i + 2]); //C - A

        vec3 normal = normalize(cross(edge1, edge2));

        // SMOOTH SHADING
        // computing the normals based on each vertex's normal
        sphere_colors[sphere_index] = color4(1.0, 0.84, 0, 1.0); sphere_points[sphere_index] = point3(vec[i], vec[i + 1], vec[i + 2]);
        flat_normals[sphere_index] = normal;
        smooth_normals[sphere_index] = vec3(vec[i], vec[i + 1], vec[i + 2]);
        sphere_index++;

        sphere_colors[sphere_index] = color4(1.0, 0.84, 0, 1.0); sphere_points[sphere_index] = point3(vec[i + 3], vec[i + 4], vec[i + 5]);
        flat_normals[sphere_index] = normal;
        smooth_normals[sphere_index] = vec3(vec[i + 3], vec[i + 4], vec[i + 5]);
        sphere_index++;

        sphere_colors[sphere_index] = color4(1.0, 0.84, 0, 1.0); sphere_points[sphere_index] = point3(vec[i + 6], vec[i + 7], vec[i + 8]);
        flat_normals[sphere_index] = normal;
        smooth_normals[sphere_index] = vec3(vec[i + 6], vec[i + 7], vec[i + 8]);
        sphere_index++;
    }
}

void shadow() {
    int shadow_index = 0;
    for (int i = 0; i < vec.size(); i += 9) {
        shadow_colors[shadow_index] = color4(0.25, 0.25, 0.25, .65); shadow_points[shadow_index] = point3(vec[i], vec[i + 1], vec[i + 2]); shadow_index++;
        shadow_colors[shadow_index] = color4(0.25, 0.25, 0.25, .65); shadow_points[shadow_index] = point3(vec[i + 3], vec[i + 4], vec[i + 5]); shadow_index++;
        shadow_colors[shadow_index] = color4(0.25, 0.25, 0.25, .65); shadow_points[shadow_index] = point3(vec[i + 6], vec[i + 7], vec[i + 8]); shadow_index++;
    }
}
//----------------------------------------------------------------------
// SetUp_Lighting_Uniform_Vars(mat4 mv):
// Set up lighting parameters that are uniform variables in shader.
//----------------------------------------------------------------------
void SetUp_Lighting_Uniform_Vars(mat4 mv) {
    // GLOBAL AMBIENT LIGHT
    glUniform4fv(glGetUniformLocation(program, "g_ambient"),
        1, global_ambient);
    
    // ALL DISTANT LIGHT PARAMETERS
    glUniform4fv(glGetUniformLocation(program, "d_direction"),
        1, direction);

    glUniform4fv(glGetUniformLocation(program, "d_ambient"),
        1, distant_light_ambient);


    glUniform4fv(glGetUniformLocation(program, "d_diffuse"),
        1, distant_light_diffuse);

    glUniform4fv(glGetUniformLocation(program, "d_specular"),
        1, distant_light_specular);

    // ALL POSITIONAL LIGHT PARAMETERS
    glUniform4fv(glGetUniformLocation(program, "p_ambient"),
        1, positional_light_ambient);

    glUniform4fv(glGetUniformLocation(program, "p_diffuse"),
        1, positional_light_diffuse);

    glUniform4fv(glGetUniformLocation(program, "p_specular"),
        1, positional_light_specular);

    // ALL POSITIONAL ATTENUATION VALUES
    glUniform1f(glGetUniformLocation(program, "const_att"),
        const_att);

    glUniform1f(glGetUniformLocation(program, "linear_att"),
        linear_att);

    glUniform1f(glGetUniformLocation(program, "quad_att"),
        quad_att);

    // ALL SPOTLIGHT VALUES
    glUniform4fv(glGetUniformLocation(program, "sp_direction"),
        1, normalize(sp_point) * normalize(light_source));

    glUniform1f(glGetUniformLocation(program, "sp_angle"), 
        20.0f);

    glUniform1f(glGetUniformLocation(program, "sp_exponent"), 
        15.0f);

}
// Set up the lighting parameters for each object (sphere, floor)
void set_lighting_param(color4 ambient, color4 diffuse, color4 specular, point4 light, float shininess, bool shade, bool source) {
    glUniform4fv(glGetUniformLocation(program, "m_ambient"),
        1, ambient);
    glUniform4fv(glGetUniformLocation(program, "m_diffuse"),
        1, diffuse);
    glUniform4fv(glGetUniformLocation(program, "m_specular"),
        1, specular);
    glUniform1f(glGetUniformLocation(program, "m_shininess"), shininess);
    
    glUniform1f(glGetUniformLocation(program, "m_shading"), shade);

    glUniform1f(glGetUniformLocation(program, "m_source"), source);
    // LIGHT POSITION
    glUniform4fv(glGetUniformLocation(program, "m_light_source"),
        1, light);
}
// OpenGL initialization
/*************************************************************
void image_set_up(void):
  generate checkerboard and stripe images.

* Inside init(), call this function and set up texture objects
  for texture mapping.
  (init() is called from main() before calling glutMainLoop().)
***************************************************************/
void image_set_up(void)
{
    int i, j, c;

    /* --- Generate checkerboard image to the image array ---*/
    for (i = 0; i < ImageHeight; i++)
        for (j = 0; j < ImageWidth; j++)
        {
            c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0));

            if (c == 1) /* white */
            {
                c = 255;
                Image[i][j][0] = (GLubyte)c;
                Image[i][j][1] = (GLubyte)c;
                Image[i][j][2] = (GLubyte)c;
            }
            else  /* green */
            {
                Image[i][j][0] = (GLubyte)0;
                Image[i][j][1] = (GLubyte)150;
                Image[i][j][2] = (GLubyte)0;
            }

            Image[i][j][3] = (GLubyte)255;
        }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /*--- Generate 1D stripe image to array stripeImage[] ---*/
    for (j = 0; j < stripeImageWidth; j++) {
        /* When j <= 4, the color is (255, 0, 0),   i.e., red stripe/line.
           When j > 4,  the color is (255, 255, 0), i.e., yellow remaining texture
         */
        stripeImage[4 * j] = (GLubyte)255;
        stripeImage[4 * j + 1] = (GLubyte)((j > 4) ? 255 : 0);
        stripeImage[4 * j + 2] = (GLubyte)0;
        stripeImage[4 * j + 3] = (GLubyte)255;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    /*----------- End 1D stripe image ----------------*/

    /*--- texture mapping set-up is to be done in
          init() (set up texture objects),
          display() (activate the texture object to be used, etc.)
          and in shaders.
     ---*/

} /* end function */
void init()
{
    x_axis();
    // Creating an initializing buffer for the x-axis object
    glGenBuffers(1, &x_axis_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, x_axis_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(x_axis_points) + sizeof(x_axis_colors),
        NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(x_axis_points), x_axis_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(x_axis_points), sizeof(x_axis_colors),
        x_axis_colors);

    y_axis();
    // Creating an initializing buffer for the x-axis object
    glGenBuffers(1, &y_axis_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, y_axis_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(y_axis_points) + sizeof(y_axis_colors),
        NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(y_axis_points), y_axis_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(y_axis_points), sizeof(y_axis_colors),
        y_axis_colors);

    z_axis();
    // Creating an initializing buffer for the x-axis object
    glGenBuffers(1, &z_axis_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, z_axis_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(z_axis_points) + sizeof(z_axis_colors),
        NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(z_axis_points), z_axis_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(z_axis_points), sizeof(z_axis_colors),
        z_axis_colors);

    floor();
    // Create and initialize a vertex buffer object for floor, to be used in display()
    glGenBuffers(1, &floor_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, floor_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors) + sizeof(floor_normals) + sizeof(floor_texCoord),
        NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), floor_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points), sizeof(floor_colors),
        floor_colors);
    // Assigning room for the normals
    glBufferSubData(GL_ARRAY_BUFFER,
        sizeof(floor_points) + sizeof(floor_colors),
        sizeof(floor_normals),
        floor_normals);
    // Assigning room for the texture coordinates
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors) + sizeof(floor_normals),
        sizeof(floor_texCoord), 
        floor_texCoord);

    sphere();
    // Creating an initializing buffer for the sphere object
    glGenBuffers(1, &sphere_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_buffer);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(point3) * sphere_NumVertices + sizeof(color4) * sphere_NumVertices +  sizeof(point3) * sphere_NumVertices + sizeof(point3) * sphere_NumVertices,
        NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
        sizeof(point3) * sphere_NumVertices, sphere_points);
    glBufferSubData(GL_ARRAY_BUFFER,
        sizeof(point3) * sphere_NumVertices,
        sizeof(color4) * sphere_NumVertices,
        sphere_colors);
    // Assigning room for the normals
    glBufferSubData(GL_ARRAY_BUFFER,
        sizeof(point3) * sphere_NumVertices + sizeof(color4) * sphere_NumVertices,
        sizeof(flat_normals),
        flat_normals);
    glBufferSubData(GL_ARRAY_BUFFER,
        sizeof(point3) * sphere_NumVertices + sizeof(color4) * sphere_NumVertices + sizeof(point3) * sphere_NumVertices,
        sizeof(smooth_normals),
        smooth_normals);

    shadow();
    glGenBuffers(1, &shadow_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, shadow_buffer);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(point3) * sphere_NumVertices + sizeof(color4) * sphere_NumVertices,
        NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
        sizeof(point3) * sphere_NumVertices, shadow_points);
    glBufferSubData(GL_ARRAY_BUFFER,
        sizeof(point3) * sphere_NumVertices,
        sizeof(color4) * sphere_NumVertices,
        shadow_colors);
    glBufferSubData(GL_ARRAY_BUFFER,
        sizeof(point3) * sphere_NumVertices + sizeof(color4) * sphere_NumVertices,
        sizeof(shadow_colors),
        shadow_colors);
    // Load shaders and create a shader program (to be used in display())
    program = InitShader("vshader42.glsl", "fshader42.glsl");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.529, 0.807, 0.92, 0.0); //set background to sky blue
    glLineWidth(2.0);

    image_set_up();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /*--- Create and Initialize a texture object ---*/
    glGenTextures(1, &texName);      // Generate texture obj name(s)
    glActiveTexture(GL_TEXTURE0);  // Set the active texture unit to be 0 
    glBindTexture(GL_TEXTURE_2D, texName); // Bind the texture to this texture unit

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageWidth, ImageHeight,
        0, GL_RGBA, GL_UNSIGNED_BYTE, Image);

    /*--- Create and Initialize a second texture object ---*/
    glGenTextures(1, &texName2);    // Generate the second texture object name
    glActiveTexture(GL_TEXTURE1);   // set active texture unit to 1
    glBindTexture(GL_TEXTURE_1D, texName2); // Bind the texture to this texture unit

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, stripeImageWidth,
        0, GL_RGBA, GL_UNSIGNED_BYTE, stripeImage);
}
//----------------------------------------------------------------------------
// drawObj(buffer, num_vertices):
//   draw the object that is associated with the vertex buffer object "buffer"
//   and has "num_vertices" vertices.
//
void drawObj(GLuint buffer, int num_vertices, bool arr, int buffer_setting)
{
    //--- Activate the vertex buffer object to be drawn ---//
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    /*----- Set up vertex attribute arrays for each vertex attribute -----*/
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET(0));

    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET(sizeof(point3) * num_vertices));
    // the offset is the (total) size of the previous vertex attribute array(s)

    // Adding new things to be added into the vertex shader
    GLuint vNormal = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(vNormal);
    if (buffer_setting == 1) { // sphere
        if (shadingFlag == 0) {
            glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
                BUFFER_OFFSET(sizeof(point3) * num_vertices + sizeof(color4) * num_vertices + sizeof(point3) * num_vertices));
        }
        else {
            glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
                BUFFER_OFFSET(sizeof(point3) * num_vertices + sizeof(color4) * num_vertices));
        }
    }
    else if (buffer_setting == 2) { // shadow
        glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
            BUFFER_OFFSET(sizeof(point3) * num_vertices));
    }
    else { // everything else (Floor)
        glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
            BUFFER_OFFSET(sizeof(floor_points) + sizeof(floor_colors)));
    }

    GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
    glEnableVertexAttribArray(vTexCoord);

    //Adding buffer offset for texture coordinates for every buildable object
    // because every object now has 
    if (buffer_setting == 1) { // sphere
        if (shadingFlag == 0) { // smooth
            glVertexAttribPointer(vTexCoord, 3, GL_FLOAT, GL_FALSE, 0,
                BUFFER_OFFSET(sizeof(point3) * num_vertices + sizeof(color4) * num_vertices + sizeof(point3) * num_vertices));
        }
        else { //flat
            glVertexAttribPointer(vTexCoord, 3, GL_FLOAT, GL_FALSE, 0,
                BUFFER_OFFSET(sizeof(point3) * num_vertices + sizeof(color4) * num_vertices + sizeof(point3) * num_vertices));
        }
    }
    else if (buffer_setting == 2) { // shadow
        glVertexAttribPointer(vTexCoord, 3, GL_FLOAT, GL_FALSE, 0,
            BUFFER_OFFSET(sizeof(point3) * num_vertices + sizeof(color4) * num_vertices));
    }
    else { // everything else (for the floor)
        glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
            BUFFER_OFFSET(sizeof(floor_points) + sizeof(floor_colors) + sizeof(floor_normals))); //onlt getting half of the texture? 
    }


    /* Draw a sequence of geometric objs (triangles) from the vertex buffer
       (using the attributes specified in each enabled vertex attribute array) */

       // arr boolean used to distingush that the axes vs the other objects being drawn
    if (arr) {
        glDrawArrays(GL_LINES, 0, num_vertices);
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, num_vertices);
    }

    /*--- Disable each vertex attribute array being enabled ---*/
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vColor);
    glDisableVertexAttribArray(vNormal);
    glDisableVertexAttribArray(vTexCoord);
}

//----------------------------------------------------------------------------
void display(void)
{
    GLuint  model_view;  // model-view matrix uniform shader variable location
    GLuint  projection;  // projection matrix uniform shader variable location

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program); // Use the shader program

    model_view = glGetUniformLocation(program, "ModelView");
    projection = glGetUniformLocation(program, "Projection");

    /*---  Set up and pass on Projection matrix to the shader ---*/
    mat4  p = Perspective(fovy, aspect, zNear, zFar);
    glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major

       // Set the value of the fragment shader texture sampler variable
   //   ("texture_2D") to the appropriate texture unit. In this case,
   //   0, for GL_TEXTURE0 which was previously set in init() by calling
   //   glActiveTexture( GL_TEXTURE0 ).
    glUniform1i(glGetUniformLocation(program, "texture_2D"), 0);
    glUniform1i(glGetUniformLocation(program, "texture_1D"), 1);

    // Pass on the value of texture_app_flag to the fragment shader
    glUniform1i(glGetUniformLocation(program, "textureFlag"),
        textureFlag);
    glUniform1i(glGetUniformLocation(program, "draw_floor"),
        true);
    glUniform1i(glGetUniformLocation(program, "draw_sphere"),
        false);
    glUniform1i(glGetUniformLocation(program, "space"),
        spaceFlag);
    glUniform1i(glGetUniformLocation(program, "mode"),
        modeFlag);
    glUniform1i(glGetUniformLocation(program, "draw_sp_tex"),
        draw_sp_texture);
    glUniform1i(glGetUniformLocation(program, "draw_sp_ver"),
        verFlag);
    glUniform1i(glGetUniformLocation(program, "upright_tilted"),
        utFlag);
    glUniform1i(glGetUniformLocation(program, "lattice_shadow"),
        false);
    glUniform1i(glGetUniformLocation(program, "lattice"),
        latticeFlag);
/*---  Set up and pass on Model-View matrix to the shader ---*/
    // eye is a global variable of vec4 set to init_eye and updated by keyboard()
    vec4    at(-7, -3, 10, 0);
    vec4    up(0.0, 1.0, 0.0, 0.0);

    mat4  mv = LookAt(eye, at, up);
    mat4 shadowMv = LookAt(eye, at, up);
    mat4 floormv = LookAt(eye, at, up);

    SetUp_Lighting_Uniform_Vars(mv);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    /*----- Set up the Mode-View matrix for the floor -----*/
     // The set-up below gives a new scene (scene 2), using Correct LookAt() function

    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
    //if (floorFlag == 1) // Filled floor
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //else              // Wireframe floor
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    
    glUniform1f(glGetUniformLocation(program, "fog_setting"), fogFlag);
    // Draw floor based on of lighting is enabled/disabled

    light_source_eye = floormv * light_source;
    set_lighting_param(floor_light_ambient, floor_light_diffuse, floor_light_specular, light_source_eye, material_shininess, lightingFlag, light_source_flag);
    //set_lighting_param(floor_light_ambient, floor_light_diffuse, floor_light_specular, material_shininess, false);

    drawObj(floor_buffer, floor_NumVertices, false, 0);  // draw the floor
    glUniform1i(glGetUniformLocation(program, "draw_floor"),
        false);
    
    
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
    // set argument as false to draw objects as is, with no shading
    set_lighting_param(floor_light_ambient, floor_light_diffuse, floor_light_specular, light_source_eye, material_shininess, false, light_source_flag);
    drawObj(x_axis_buffer, axes_NumVertices, true, 0);  // draw the x-axis
    drawObj(y_axis_buffer, axes_NumVertices, true, 0);  // draw the y-axis
    drawObj(z_axis_buffer, axes_NumVertices, true, 0);  // draw the z-axis

    glDepthMask(GL_TRUE);

    if (begin_roll == true) {
        if (is_rolling == true) {
            // Conditions check whether the sphere has reached past the points of A, B, and C
            if (curr_x <= -1.0f && curr_z <= -4) {
                ab_traveling = false, bc_traveling = true, ac_traveling = false;
                curr_x = -1.0f, curr_y = 1.0f, curr_z = -4.0f;
                //storing accumulated rotation matrix M

                matrixM = Rotate(angle, -9, 0, 4) * matrixM;
                angle = 0;
            }
            else if (curr_x >= 3.5 && curr_z >= -2.5) {
                ab_traveling = false, bc_traveling = false, ac_traveling = true;
                curr_x = 3.5f, curr_y = 1.0f, curr_z = -2.5f;
                //storing accumulated rotation matrix M

                matrixM = Rotate(angle, (3 / 2), 0, (-9 / 2)) * matrixM;
                angle = 0;
            }
            else if (curr_x <= 3.0f && curr_z >= 5.0f) {
                ab_traveling = true, bc_traveling = false, ac_traveling = false;
                curr_x = 3.0f, curr_y = 1.0f, curr_z = 5.0f;
                //storing accumulated rotation matrix M

                matrixM = Rotate(angle, (15 / 2), 0, (1 / 2)) * matrixM;
                angle = 0;
            }

            // setting up model view matrix for the sphere //
            if (ab_traveling == true) {
                curr_x += -0.00021;
                curr_y += 0.0f;
                curr_z += -0.00047;

                mv = mv * Translate(curr_x, curr_y, curr_z) * Rotate(angle, -9, 0, 4) * matrixM;
            }
            if (bc_traveling == true) {
                curr_x += .00049;
                curr_y += 0.0f;
                curr_z += .00016;

                mv = mv * Translate(curr_x, curr_y, curr_z) * Rotate(angle, (3 / 2), 0, (-9 / 2)) * matrixM;
            }
            if (ac_traveling == true) {
                curr_x += -.0000346;
                curr_y += 0.0f;
                curr_z += .000519;

                mv = mv * Translate(curr_x, curr_y, curr_z) * Rotate(angle, (15 / 2), 0, (1 / 2)) * matrixM;
            }
        }
    }
    else {
        // Move the sphere to point 'A'
        mv = mv * Translate(curr_x, curr_y, curr_z);
    }

    // Must keep drawing object, regardless of if its rolling or not
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
    //define the normal matrix for the shaded object being drawn
    mat3 normal_matrix = NormalMatrix(mv, 0);
    glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"),
        1, GL_TRUE, normal_matrix);
    
    light_source_eye = floormv * light_source;
    // set shading depending on value of the flag 
    set_lighting_param(sphere_light_ambient, sphere_light_diffuse, sphere_light_specular, light_source_eye, material_shininess, lightingFlag, light_source_flag);
    //set_lighting_param(sphere_light_ambient, sphere_light_diffuse, sphere_light_specular, material_shininess, false);

    glUniform1i(glGetUniformLocation(program, "draw_sphere"),
        true);
    if (fillFlag == 1) { // Filled sphere
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else {           // Wireframe sphere
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        set_lighting_param(sphere_light_ambient, sphere_light_diffuse, sphere_light_specular, light_source_eye, material_shininess, false, light_source_flag);
    }
    drawObj(sphere_buffer, sphere_NumVertices, false, 1); // draw the sphere
    glUniform1i(glGetUniformLocation(program, "draw_sphere"),
        false);
    // set model-view for the shadow...
    glDepthMask(GL_TRUE);
    if (begin_roll == true) {
        if (ab_traveling == true) {
            shadowMv = LookAt(eye, at, up) * shadow_projection * Translate(curr_x, curr_y, curr_z) * Rotate(angle, -9, 0, 4) * matrixM;
        }
        if (bc_traveling == true) {
            shadowMv = LookAt(eye, at, up) * shadow_projection * Translate(curr_x, curr_y, curr_z) * Rotate(angle, (3 / 2), 0, (-9 / 2)) * matrixM;
        }
        if (ac_traveling == true) {
            shadowMv = LookAt(eye, at, up) * shadow_projection * Translate(curr_x, curr_y, curr_z) * Rotate(angle, (15 / 2), 0, (1 / 2)) * matrixM;
        }
    }
    else { //put in correct position is rolling hasn't started
        shadowMv = LookAt(eye, at, up) * shadow_projection * Translate(curr_x, curr_y, curr_z);
    }

    // drawing the shadow
    if (fillFlag == 1) { // Filled shadow
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else {           // Wireframe shadow
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (blendingFlag == 1) { // enable blending with blending function
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
    }
    glUniformMatrix4fv(model_view, 1, GL_TRUE, shadowMv); // GL_TRUE: matrix is row-major
    if (shadowFlag == 1) { //Shadow option is chosen, show shadow by default
        set_lighting_param(sphere_light_ambient, sphere_light_diffuse, sphere_light_specular, light_source_eye,material_shininess, false, light_source_flag);
        //set_lighting_param(sphere_light_ambient, sphere_light_diffuse, sphere_light_specular, material_shininess, false);
        glUniform1i(glGetUniformLocation(program, "lattice_shadow"),
            true);
        drawObj(shadow_buffer, sphere_NumVertices, false, 2); // draw the shadow
        glUniform1i(glGetUniformLocation(program, "lattice_shadow"),
            false);
    }
    if (blendingFlag == 1) {
        glDisable(GL_BLEND); // disable blending
        glDepthMask(GL_TRUE);
    }
    // redraw the floor
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    // set normal_matrix to the mv of the floor (default LookAt())
    normal_matrix = NormalMatrix(floormv, 0);
    // set that value into the vertex shader
    glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"),
        1, GL_TRUE, normal_matrix);

    //draw shading depending on value of lighting flag
    set_lighting_param(floor_light_ambient, floor_light_diffuse, floor_light_specular, light_source_eye, material_shininess, lightingFlag, light_source_flag);
    //set_lighting_param(floor_light_ambient, floor_light_diffuse, floor_light_specular, material_shininess, false);
    glUniform1i(glGetUniformLocation(program, "draw_floor"),
        true);
    drawObj(floor_buffer, floor_NumVertices, false, 0);
    glUniform1i(glGetUniformLocation(program, "draw_floor"),
        false);
    glUniform1i(glGetUniformLocation(program, "draw_sphere"),
        true);
    drawObj(sphere_buffer, sphere_NumVertices, false, 1); // draw the sphere

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    // at the end... 
    glutSwapBuffers();
}
//---------------------------------------------------------------------------
void idle(void)
{
    angle += 0.03f;
    // angle += 1.0f;    //YJC: change this value to adjust the cube rotation speed.
    glutPostRedisplay();
}
//----------------------------------------------------------------------------
void MyMouse(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
        if (begin_roll == true) {
            animationFlag = 1 - animationFlag;
            if (animationFlag == 1) glutIdleFunc(idle);
            else                    glutIdleFunc(NULL);
        }
    }
}
//----------------------------------------------------------------------------
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 033: // Escape Key

    case 'X': eye[0] += 1.0; break;
    case 'x': eye[0] -= 1.0; break;
    case 'Y': eye[1] += 1.0; break;
    case 'y': eye[1] -= 1.0; break;
    case 'Z': eye[2] += 1.0; break;
    case 'z': eye[2] -= 1.0; break;

    case 'b': case 'B': // Key to press to start animation
        begin_roll = true;
        break;
    case 'v': case 'V': // Key to set vertical stripes
        modeFlag = 0;
        break;
    case 's': case 'S': // Key to set slanted stripes
        modeFlag = 1;
        break;
    case 'o': case 'O':
        spaceFlag = 1 - spaceFlag;
        break;
    case 'l': case 'L':
        latticeFlag = 1 - latticeFlag;
        break;
    case 'u': case 'U':
        utFlag = 1;
        break;
    case 't': case 'T':
        utFlag = 0;
        break;
    }
    glutPostRedisplay();

}
//----------------------------------------------------------------------------
// Various menus, all to toggle/feature various parts of the program
//----------------------------------------------------------------------------
void shadow_menu(int id) {
    switch (id) {
    case 1:
        shadowFlag = 0;
        break;
    case 2:
        shadowFlag = 1;
        break;
    }
}
//----------------------------------------------------------------------------
void lighting_menu(int id) {
    switch (id) {
    case 1:
        lightingFlag = 0;
        break;
    case 2:
        lightingFlag = 1;
        break;
    }
}
//----------------------------------------------------------------------------
void shading_menu(int id) {
    switch (id) {
    case 1:
        shadingFlag = 1;
        break;
    case 2:
        shadingFlag = 0;
        break;
    }
}
//----------------------------------------------------------------------------
void source_menu(int id) {
    switch (id) {
    case 1:
        light_source_flag = 0;
        break;
    case 2:
        light_source_flag = 1;
        break;
    }
}
//----------------------------------------------------------------------------
void fog_menu(int id) {
    switch (id) {
    case 1:
        fogFlag = 0.0f;
        break;
    case 2:
        fogFlag = 1.0f;
        break;
    case 3:
        fogFlag = 2.0f;
        break;
    case 4:
        fogFlag = 3.0f;
        break;
    }
}
//----------------------------------------------------------------------------
void blend_menu(int id) {
    switch (id) {
    case 1:
        blendingFlag = 0;
        break;
    case 2:
        blendingFlag = 1;
        break;
    }
}
//----------------------------------------------------------------------------
void texture_ground_menu(int id) {
    switch (id) {
    case 1:
        textureFlag = 0;
        break;
    case 2:
        textureFlag = 1;
        break;
    }
}
//----------------------------------------------------------------------------
void texture_sphere_menu(int id) {
    switch (id) {
    case 1:
        draw_sp_texture = false;
        break;
    case 2:
        draw_sp_texture = true;
        verFlag = 0;
        break;
    case 3:
        draw_sp_texture = true;
        verFlag = 1;
        break;
    }
}
//----------------------------------------------------------------------------
void demo_menu(int id)
{
    switch (id)
    {
    case 1:
        eye = init_eye;
        break;
    case 2:
        exit(0);
        break;
    case 3:
        break;
    case 4:
        break;
    case 5:
        fillFlag = 1 - fillFlag;
        break;
    case 6:
        break;
    }
    glutPostRedisplay();
}

//----------------------------------------------------------------------------
void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    aspect = (GLfloat)width / (GLfloat)height;
    glutPostRedisplay();
}
//----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
#ifdef __APPLE__ // Enable core profile of OpenGL 3.2 on macOS.
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
#else
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
    glutInitWindowSize(512, 512);
    glutCreateWindow("ASSIGNMENT 4");

#ifdef __APPLE__ // on macOS
    // Core profile requires to create a Vertex Array Object (VAO).
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
#else           // on Linux or Windows, we still need glew
    /* Call glewInit() and error checking */
    int err = glewInit();
    if (GLEW_OK != err)
    {
        printf("Error: glewInit failed: %s\n", (char*)glewGetErrorString(err));
        exit(1);
    }
#endif
    // adding read_file() function and glutMouseFunc()
    read_file();
    glutMouseFunc(MyMouse);

    // shadow menu
    int shadow_menu_ID = glutCreateMenu(shadow_menu);
    glutSetMenuFont(shadow_menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" No", 1);
    glutAddMenuEntry(" Yes", 2);

    int lighting_menu_ID = glutCreateMenu(lighting_menu);
    glutSetMenuFont(lighting_menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" No", 1);
    glutAddMenuEntry(" Yes", 2);

    int shading_menu_ID = glutCreateMenu(shading_menu);
    glutSetMenuFont(shading_menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" Flat Shading", 1);
    glutAddMenuEntry(" Smooth Shading", 2);

    int light_source_menu_ID = glutCreateMenu(source_menu);
    glutSetMenuFont(shading_menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" Spot Light", 1);
    glutAddMenuEntry(" Point Source", 2);

    int fog_menu_ID = glutCreateMenu(fog_menu);
    glutSetMenuFont(shading_menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" No Fog", 1);
    glutAddMenuEntry(" Linear", 2);
    glutAddMenuEntry(" Exponential", 3);
    glutAddMenuEntry(" Exponential Square", 4);

    int blending_menu_ID = glutCreateMenu(blend_menu);
    glutSetMenuFont(shading_menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" No", 1);
    glutAddMenuEntry(" Yes", 2);

    int texture_ground_menu_ID = glutCreateMenu(texture_ground_menu);
    glutSetMenuFont(shading_menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" No", 1);
    glutAddMenuEntry(" Yes", 2);

    int texture_sphere_menu_ID = glutCreateMenu(texture_sphere_menu);
    glutSetMenuFont(shading_menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" No", 1);
    glutAddMenuEntry(" Yes - Contour Lines", 2);
    glutAddMenuEntry(" Yes - Checkerboard", 3);

    // Adding main menu functionaility
    int demo_menu_ID;
    demo_menu_ID = glutCreateMenu(demo_menu);
    glutSetMenuFont(demo_menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" Default View Point", 1);
    glutAddMenuEntry(" Quit", 2);
    glutAddSubMenu(" Shadow", shadow_menu_ID);
    glutAddSubMenu(" Enable Lighting", lighting_menu_ID);
    glutAddMenuEntry(" Wire Frame Sphere", 5);
    glutAddSubMenu(" Shading", shading_menu_ID);
    glutAddSubMenu(" Light Source", light_source_menu_ID);
    glutAddSubMenu(" Fog Options", fog_menu_ID);
    glutAddSubMenu(" Blending Shadow", blending_menu_ID);
    glutAddSubMenu(" Texture Mapped Ground", texture_ground_menu_ID);
    glutAddSubMenu(" Texture Mapped Sphere", texture_sphere_menu_ID);

    glutAttachMenu(GLUT_LEFT_BUTTON);

    // Get info of GPU and supported OpenGL version
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);

    init();
    glutMainLoop();
    return 0;
}