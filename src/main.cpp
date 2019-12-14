#include <GLFW/glfw3.h>
#include "gl.h"
#include "gl3.h"
#include "trimesh.hpp"
#include "shader.hpp"
#include "mat4.hpp"
#include "vec3.hpp"
#include <cstring>

#define WIN_WIDTH 1000
#define WIN_HEIGHT 1000

using namespace std;

namespace Globals {
    // cursor positions
    double cursorX, cursorY;
    // window size
    float winWidth, winHeight;
    float aspect;
    GLuint vertsVbo[1], colorsVbo[1], normalsVbo[1], facesIbo[1], trisVao;
    TriMesh mesh;

    // Model, view and projection matrices, initialized to the identity
    Mat4 modelMatrix;
    Mat4 viewMatrix;
    Mat4 projectionMatrix;
}

Vec3f transformVector(Mat4 const &mat, Vec3f const &v) {
    vector<float> values = mat.getAll();
    Vec3f result(values[0] * v[0] + values[1] * v[1] + values[2] * v[2] + values[3] * 0,
                 values[4] * v[0] + values[5] * v[1] + values[6] * v[2] + values[7] * 0,
                 values[8] * v[0] + values[9] * v[1] + values[10] * v[2] + values[11] * 0);

    return result;
}

void resetViewMatrix(Vec3 eye = Vec3(0, 0, 0)) {
    Vec3 viewDir(1, 0, -1);
    Vec3 upDir(0, 1, 0);
    Vec3 n = viewDir.unit() * -1;
    Vec3 u = upDir.unit().cross(n);
    Vec3 v = n.cross(u);
    Vec3 d(eye.dot(u) * -1, eye.dot(v) * -1, eye.dot(n) * -1);
    Globals::viewMatrix.setAsViewMatrix(u, v, n, d);
}

static void errorCallback(int error, const char *description) {
    cerr << "Error: " << description << endl;
}

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        Vec3 viewDirection = Globals::viewMatrix.getViewDirection();
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_Q:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_R:
                resetViewMatrix();
                break;
            case GLFW_KEY_UP:
                Globals::viewMatrix = Mat4(viewDirection * 0.05) * Globals::viewMatrix;
                break;
            case GLFW_KEY_DOWN:
                Globals::viewMatrix = Mat4(viewDirection * -0.05) * Globals::viewMatrix;
                break;
                // ToDo: update the viewing transformation matrix according to key presses
        }
    }
}

static void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    Globals::winWidth = float(width);
    Globals::winHeight = float(height);
    Globals::aspect = Globals::winWidth / Globals::winHeight;

    glViewport(0, 0, width, height);

    // ToDo: update the perspective matrix according to the new window size
}

void initScene();

int main(int argc, char *argv[]) {
    // Load the mesh
    std::stringstream objFile;
    objFile << MY_DATA_DIR << "sibenik/sibenik.obj";
    if (!Globals::mesh.load_obj(objFile.str())) { return 0; }
    Globals::mesh.print_details();

    // Scale to fit in (-1,1): a temporary measure to allow the entire model to be visible
    // Should be replaced by the use of an appropriate projection matrix
    // Original model dimensions: center = (0,0,0); height: 30.6; length: 40.3; width: 17.0
    float min = Globals::mesh.vertices[0][0];
    float max = Globals::mesh.vertices[0][0];
    for (auto &vertex : Globals::mesh.vertices) {
        if (vertex[0] < min) { min = vertex[0]; }
        else if (vertex[0] > max) { max = vertex[0]; }
        if (vertex[1] < min) { min = vertex[1]; }
        else if (vertex[1] > max) { max = vertex[1]; }
        if (vertex[2] < min) { min = vertex[2]; }
        else if (vertex[2] > max) { max = vertex[2]; }
    }
    float scale;
    if (min < 0) { min = -min; }
    if (max > min) { scale = 1 / max; }
    else { scale = 1 / min; }

    Mat4 mScale(scale, scale, scale);
    for (auto &vert : Globals::mesh.vertices) {
        vert = transformVector(mScale, vert);
    }

    // Set up window
    GLFWwindow *window;
    glfwSetErrorCallback(&errorCallback);

    // Initialize the window
    if (!glfwInit()) { return EXIT_FAILURE; }

    // Ask for OpenGL 3.2
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create the glfw window
    Globals::winWidth = WIN_WIDTH;
    Globals::winHeight = WIN_HEIGHT;
    window = glfwCreateWindow(int(Globals::winWidth), int(Globals::winHeight), "HW2c - OpenGL", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Bind callbacks to the window
    glfwSetKeyCallback(window, &keyCallback);
    glfwSetFramebufferSizeCallback(window, &framebufferSizeCallback);

    // Make current
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialize glew AFTER the context creation and before loading the shader.
    // Note we need to use experimental because we're using a modern version of opengl.
#ifdef USE_GLEW
    glewExperimental = GL_TRUE;
    glewInit();
#endif

    // Initialize the shader (which uses glew, so we need to init that first).
    // MY_SRC_DIR is a define that was set in CMakeLists.txt which gives
    // the full path to this project's src/ directory.
    mcl::Shader shader;
    std::stringstream ss;
    ss << MY_SRC_DIR << "shader.";
    shader.init_from_files(ss.str() + "vert", ss.str() + "frag");

    // Initialize the scene
    // IMPORTANT: Only call after gl context has been created
    initScene();
    framebufferSizeCallback(window, int(Globals::winWidth), int(Globals::winHeight));

    // Initialize OpenGL
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.f, 1.f, 1.f, 1.f);

    // Enable the shader, this allows us to set uniforms and attributes
    shader.enable();

    // Initialize the eye position
    Vec3 eye(0, 0, 0);
    resetViewMatrix(eye);

    // Bind buffers
    glBindVertexArray(Globals::trisVao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Globals::facesIbo[0]);

    // Game loop
    while (!glfwWindowShouldClose(window)) {

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Send updated info to the GPU
        float model[16];
        float view[16];
        float projection[16];
        Globals::modelMatrix.dumpColumnWise(model);
        Globals::viewMatrix.dumpColumnWise(view);
        Globals::projectionMatrix.dumpColumnWise(projection);
        glUniformMatrix4fv(shader.uniform("model"), 1, GL_FALSE, model); // model transformation
        glUniformMatrix4fv(shader.uniform("view"), 1, GL_FALSE, view); // viewing transformation
        glUniformMatrix4fv(shader.uniform("projection"), 1, GL_FALSE, projection); // projection matrix
        glUniform3f(shader.uniform("eye"), eye.x, eye.y, eye.z); // used in fragment shader

        // Draw
        glDrawElements(GL_TRIANGLES, Globals::mesh.faces.size() * 3, GL_UNSIGNED_INT, 0);

        // Finalize
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // end game loop

    // Unbind
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Disable the shader, we're done using it
    shader.disable();

    return EXIT_SUCCESS;
}


void initScene() {

    using namespace Globals;

    // Create the buffer for vertices
    glGenBuffers(1, vertsVbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertsVbo[0]);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(mesh.vertices[0]), &mesh.vertices[0][0],
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create the buffer for colors
    glGenBuffers(1, colorsVbo);
    glBindBuffer(GL_ARRAY_BUFFER, colorsVbo[0]);
    glBufferData(GL_ARRAY_BUFFER, mesh.colors.size() * sizeof(mesh.colors[0]), &mesh.colors[0][0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create the buffer for normals
    glGenBuffers(1, normalsVbo);
    glBindBuffer(GL_ARRAY_BUFFER, normalsVbo[0]);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(mesh.normals[0]), &mesh.normals[0][0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create the buffer for indices
    glGenBuffers(1, facesIbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facesIbo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.faces.size() * sizeof(mesh.faces[0]), &mesh.faces[0][0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Create the VAO
    glGenVertexArrays(1, &trisVao);
    glBindVertexArray(trisVao);

    int vertDim = 3;

    // location=0 is the vertex
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertsVbo[0]);
    glVertexAttribPointer(0, vertDim, GL_FLOAT, GL_FALSE, sizeof(mesh.vertices[0]), 0);

    // location=1 is the color
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorsVbo[0]);
    glVertexAttribPointer(1, vertDim, GL_FLOAT, GL_FALSE, sizeof(mesh.colors[0]), 0);

    // location=2 is the normal
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalsVbo[0]);
    glVertexAttribPointer(2, vertDim, GL_FLOAT, GL_FALSE, sizeof(mesh.normals[0]), 0);

    // Done setting data for the vao
    glBindVertexArray(0);

}

