#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "shader_compiler.hpp"
#include <glm/gtc/type_ptr.hpp>

#define DEBUG 1

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

using namespace std;
using namespace glm;

typedef struct {
    GLfloat x, y;
} FloatType2D;

typedef struct {
    GLfloat r, g, b;
} ColorType3D;

const int nvertices = 4;

// General transformation matrix initialized to identity matrix
mat4 M(1.0f);
// Some assorted global variables, defined as such to make life easier
GLint mLocation;
GLdouble mouseX, mouseY;
GLboolean doRotate = GL_FALSE;
GLint windowWidth = 1000;
GLint windowHeight = 1000;
GLdouble pi = 4.0 * atan(1.0);
GLFWcursor *hand_cursor, *arrow_cursor;
const float SMALL_ANGLE = 1.0 / 180 * M_PI;
const float RfCCW[16] = {
        cos(SMALL_ANGLE), sin(SMALL_ANGLE), 0, 0,
        -sin(SMALL_ANGLE), cos(SMALL_ANGLE), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
};
const float RfCW[16] = {
        cos(SMALL_ANGLE), -sin(SMALL_ANGLE), 0, 0,
        sin(SMALL_ANGLE), cos(SMALL_ANGLE), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
};
glm::mat4 RCCW = glm::make_mat4(RfCCW);
glm::mat4 RCW = glm::make_mat4(RfCW);

static void errorCallback(int error, const char *description) {
    cerr << "Error code: " << error << ": " << description << endl;
}

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    switch (key) {
        case GLFW_KEY_ESCAPE:
        case GLFW_KEY_Q:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_R:
            M = mat4(1.0f);
            break;
        case GLFW_KEY_RIGHT:
            M = scale(M, vec3(1.02f, 1.0f, 0));
            break;
        case GLFW_KEY_LEFT:
            M = scale(M, vec3(0.98f, 1.0f, 0));
            break;
        case GLFW_KEY_UP:
            M = scale(M, vec3(1.0f, 1.02f, 0));
            break;
        case GLFW_KEY_DOWN:
            M = scale(M, vec3(1.0f, 0.98f, 0));
            break;
        default:
            break;
    }
}

static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    // Check which mouse button triggered the event, e.g. GLFW_MOUSE_BUTTON_LEFT, etc.
    // and what the button action was, e.g. GLFW_PRESS, GLFW_RELEASE, etc.
    // (Note that ordinary trackpad click = mouse left button)
    // Also check if any modifier keys were active at the time of the button press, e.g. GLFW_MOD_ALT, etc.
    // Take the appropriate action, which could (optionally) also include changing the cursor's appearance
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && mods != GLFW_MOD_CONTROL) {
        glfwGetCursorPos(window, &mouseX, &mouseY);
        glfwSetCursor(window, glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR));
        doRotate = GL_TRUE;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE && mods != GLFW_MOD_CONTROL) {
        glfwSetCursor(window, glfwCreateStandardCursor(GLFW_ARROW_CURSOR));
        doRotate = GL_FALSE;
    }
}

static void cursorPositionCallback(GLFWwindow *window, double x, double y) {
    // determine the direction of the mouse or cursor motion
    // update the current mouse or cursor location
    //  (necessary to quantify the amount and direction of cursor motion)
    // take the appropriate action
    if (doRotate) {
        if (x - mouseX > 0) {
            // moved right => rotate clockwise
            M = RCW * M;
        } else if (x - mouseX < 0) {
            // moved left => rotate counter-clockwise
            M = RCCW * M;
        }
        mouseX = x;
    }
}

void initStaticDataAndShaders() {
    ColorType3D colors[4];
    // Hard code the geometry of interest
    // This part can be customized if you want to define a different object,
    // or if you prefer to read in an object description from a file
    colors[0].r = 1;
    colors[0].g = 0;
    colors[0].b = 0;  // red
    colors[1].r = 1;
    colors[1].g = 1;
    colors[1].b = 0;  // yellow
    colors[2].r = 0;
    colors[2].g = 1;
    colors[2].b = 0;  // green
    colors[3].r = 0;
    colors[3].g = 0;
    colors[3].b = 1;  // blue

    FloatType2D vertices[4];
    vertices[0].x = -0.2;
    vertices[0].y = -0.2; // lower left
    vertices[1].x = 0.2;
    vertices[1].y = -0.2; // lower right
    vertices[2].x = 0.2;
    vertices[2].y = 0.2; // upper right
    vertices[3].x = -0.2;
    vertices[3].y = 0.2; // upper left

    // Create and bind a vertex array object
    GLuint vao[1];
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    // Create and initialize a buffer object large enough to hold both vertex position and color data
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), vertices, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);
    // Define the names of the shader files
    stringstream vshader, fshader;
    vshader << SRC_DIR << "/vertex_shader.glsl";
    fshader << SRC_DIR << "/fragment_shader.glsl";
    // Load the shaders and use the resulting shader program
    GLuint program = compileShader(vshader.str().c_str(), fshader.str().c_str());
    // Determine locations of the necessary attributes and matrices used in the vertex shader
    GLuint vertexPositionLocation = glGetAttribLocation(program, "vertexPosition");
    glEnableVertexAttribArray(vertexPositionLocation);
    glVertexAttribPointer(vertexPositionLocation, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    GLuint vertexColorLocation = glGetAttribLocation(program, "vertexColor");
    glEnableVertexAttribArray(vertexColorLocation);
    glVertexAttribPointer(vertexColorLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertices)));
    mLocation = glGetUniformLocation(program, "M");
    // Define static OpenGL state variables
    glClearColor(1.0, 1.0, 1.0, 1.0);
    // Define some GLFW cursors (in case you want to dynamically change the cursor's appearance)
    // If you want, you can add more cursors, or even define your own cursor appearance
    arrow_cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    hand_cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
}

int main(int argc, char **argv) {
    // -------- -------- GLFW setup -------- --------
    GLFWwindow *window;
    // Define the error callback function
    glfwSetErrorCallback(errorCallback);
    // Initialize GLFW (performs platform-specific initialization)
    if (!glfwInit()) exit(EXIT_FAILURE);
    // Ask for OpenGL 3.2
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // Use GLFW to open a window within which to display your graphics
    window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL app", nullptr, nullptr);
    // Verify that the window was successfully created; if not, print error message and terminate
    if (!window) {
        cerr << "GLFW failed to create window; terminating" << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    // Makes the newly-created context current
    glfwMakeContextCurrent(window);
    // Tells the system to wait to swap buffers until monitor refresh has completed; necessary to avoid tearing
    glfwSwapInterval(1);
    // Define the keyboard callback function
    glfwSetKeyCallback(window, keyCallback);
    // Define the mouse button callback function
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    // Define the mouse motion callback function
    glfwSetCursorPosCallback(window, cursorPositionCallback);

    // -------- -------- Using GLAD to load openGL functions -------- --------
    // Load opengl functions
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        cerr << "Failed to gladLoadGLLoader" << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // -------- -------- Init data and compile shaders -------- --------
    initStaticDataAndShaders();

    // -------- -------- Graphics rendering loop -------- --------
    while (!glfwWindowShouldClose(window)) {
        // Fill the window with the background color
        glClear(GL_COLOR_BUFFER_BIT);
        // Sanity check that your matrix contents are what you expect them to be
        if (DEBUG) {
            const float *m = (const float *) value_ptr(M);
            cout << m[0] << " " << m[1] << " " << m[2] << " " << m[3] << endl;
            cout << m[4] << " " << m[5] << " " << m[6] << " " << m[7] << endl;
            cout << m[8] << " " << m[9] << " " << m[10] << " " << m[11] << endl;
            cout << m[12] << " " << m[13] << " " << m[14] << " " << m[15] << endl;
        }
        // Send the model transformation matrix to the GPU
        glUniformMatrix4fv(mLocation, 1, GL_FALSE, value_ptr(M));
        // Draw a triangle between the first vertex and each successive vertex pair
        glDrawArrays(GL_TRIANGLE_FAN, 0, nvertices);
        // Ensure that all OpenGL calls have executed before swapping buffers
        glFlush();
        // Swap buffers
        glfwSwapBuffers(window);
        // Wait for an event, then handle it
        glfwWaitEvents();
    }

    // -------- -------- GLFW cleanup -------- --------
    // Clean up
    glfwDestroyWindow(window);
    // Destroys any remaining objects, frees resources allocated by GLFW
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
