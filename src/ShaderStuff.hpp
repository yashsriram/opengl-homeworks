#ifndef SHADER_STUFF
#define SHADER_STUFF 1

using namespace std;

#define DEBUG_ON 1
#define BUFFER_OFFSET(bytes) ((GLvoid*) (bytes))

// Create a NULL-terminated string by reading the provided file
static char *readFile(const char *shaderFile) {
    FILE *fp;
    long length;
    char *buffer;

    // Open the file containing the text of the shader code
    fp = fopen(shaderFile, "r");
    // Check for errors in opening the file
    if (fp == nullptr) {
        cerr << "Can't open shader source file: " << shaderFile << endl;
        return nullptr;
    }
    // Determine the file size
    // Move position indicator to the end of the file;
    fseek(fp, 0, SEEK_END);
    // Return the value of the current position
    length = ftell(fp);
    // Allocate a buffer with the indicated number of bytes, plus one
    buffer = new char[length + 1];
    // Read the appropriate number of bytes from the file
    // Move position indicator to the start of the file
    fseek(fp, 0, SEEK_SET);
    // Read all of the bytes
    fread(buffer, 1, length, fp);
    // Append a NULL character to indicate the end of the string
    buffer[length] = '\0';
    // Close the file
    fclose(fp);
    // Return the string
    return buffer;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint InitShader(const char *vShaderFileName, const char *fShaderFileName) {
    // Check GLSL version
    cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    // Read source code from file
    GLchar *vertexShaderCode = readFile(vShaderFileName);
    GLchar *fragmentShaderCode = readFile(fShaderFileName);
    // Error check
    if (vertexShaderCode == nullptr) {
        cerr << "Failed to read from vertex shader file: " << vShaderFileName << endl;
        if (DEBUG_ON) {
            cerr << "Read shader code:" << endl << vertexShaderCode << endl;
        }
        exit(EXIT_FAILURE);
    }
    if (fragmentShaderCode == nullptr) {
        cerr << "Failed to read from fragment shader file: " << fShaderFileName << endl;
        if (DEBUG_ON) {
            cerr << "Read shader code:" << endl << fragmentShaderCode << endl;
        }
        exit(EXIT_FAILURE);
    }
    // Create shader handlers
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCode, nullptr);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, nullptr);
    // Compile shaders
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
    // Check for errors in compiling shaders
    GLint compiled;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        cerr << "Vertex shader failed to compile" << endl;
        if (DEBUG_ON) {
            GLint logMaxSize, logLength;
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logMaxSize);
            cerr << "Printing error message of " << logMaxSize << " bytes" << endl;
            char *logMsg = new char[logMaxSize];
            glGetShaderInfoLog(vertexShader, logMaxSize, &logLength, logMsg);
            cerr << logLength << " bytes retrieved" << endl;
            cerr << "Error message: " << logMsg << endl;
            delete[] logMsg;
        }
        exit(EXIT_FAILURE);
    }
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        cerr << "Fragment shader failed to compile" << endl;
        if (DEBUG_ON) {
            GLint logMaxSize, logLength;
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logMaxSize);
            cerr << "Printing error message of " << logMaxSize << " bytes" << endl;
            char *logMsg = new char[logMaxSize];
            glGetShaderInfoLog(fragmentShader, logMaxSize, &logLength, logMsg);
            cerr << logLength << " bytes retrieved" << endl;
            cerr << "Error message: " << logMsg << endl;
            delete[] logMsg;
        }
        exit(EXIT_FAILURE);
    }

    // Create the program
    GLuint program = glCreateProgram();

    // Attach shaders to program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // Link and set program to use
    glLinkProgram(program);
    glUseProgram(program);

    return program;
}

#endif
