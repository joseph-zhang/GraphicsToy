#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;

// Vertex Shader GLSL
const char *vertexShaderSource =
  "#version 330 core\n"
  "layout (location = 0) in vec3 aPos;\n"
  "void main() {\n"
  "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
  "}\0";

// Fragment Shader GLSL
const char *fragmentShaderSourceOrange =
  "#version 330 core\n"
  "out vec4 FragColor;\n"
  "void main() {\n"
  "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
  "}\n\0";
const char *fragmentShaderSourceYellow =
  "#version 330 core\n"
  "out vec4 FragColor;\n"
  "void main() {\n"
  "   FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
  "}\n\0";

void framebuffer_size_call_back(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

// exit if ESC is pressed
void processInput(GLFWwindow* window) {
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

int main() {
  // ----------------------------------------------------------------------
  // GLFW initialization
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // GLFW window creation
  GLFWwindow* window = glfwCreateWindow(800, 600, "window demo", nullptr, nullptr);
  if(window == nullptr) {
    cout << "Failed to create GLFW window" << endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_call_back);

  // GLAD initialization, load all OpenGL function pointers
  if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    cout << "Failed to initialize GLAD" << endl;
    return -1;
  }
  // ----------------------------------------------------------------------

  // Build Vertex Shader
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
  glCompileShader(vertexShader);

  // Check vertex shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
    cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
  }

  // Build Fragment Shader
  unsigned int fragmentShaderOrange = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShaderOrange, 1, &fragmentShaderSourceOrange, nullptr);
  glCompileShader(fragmentShaderOrange);

  unsigned int fragmentShaderYellow = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShaderYellow, 1, &fragmentShaderSourceYellow, nullptr);
  glCompileShader(fragmentShaderYellow);

  // Check fragment shader compile errors
  glGetShaderiv(fragmentShaderOrange, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(fragmentShaderOrange, 512, nullptr, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
  glGetShaderiv(fragmentShaderYellow, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(fragmentShaderYellow, 512, nullptr, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
  // ----------------------------------------------------------------------

  // Link compiled shaders
  unsigned int shaderProgramOrange = glCreateProgram();
  unsigned int shaderProgramYellow = glCreateProgram();
  glAttachShader(shaderProgramOrange, vertexShader);
  glAttachShader(shaderProgramOrange, fragmentShaderOrange);
  glLinkProgram(shaderProgramOrange);

  glAttachShader(shaderProgramYellow, vertexShader);
  glAttachShader(shaderProgramYellow, fragmentShaderYellow);
  glLinkProgram(shaderProgramYellow);

  // Check shader linking errors
  glGetProgramiv(shaderProgramOrange, GL_LINK_STATUS, &success);
  if(!success) {
    glGetProgramInfoLog(shaderProgramOrange, 512, NULL, infoLog);
    cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
  }
  glGetProgramiv(shaderProgramYellow, GL_LINK_STATUS, &success);
  if(!success) {
    glGetProgramInfoLog(shaderProgramYellow, 512, NULL, infoLog);
    cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
  }

  // Delete shaders, we no longer need them anymore
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShaderOrange);
  glDeleteShader(fragmentShaderYellow);
  // ----------------------------------------------------------------------

  // set up vertex data (and buffer(s)) and configure vertex attributes
  float firstTriangle[] = {
                           -0.9f, -0.5f, 0.0f,  // left
                           -0.0f, -0.5f, 0.0f,  // right
                           -0.45f, 0.5f, 0.0f,  // top
  };

  float secondTriangle[] = {
                            0.0f, -0.5f, 0.0f,  // left
                            0.9f, -0.5f, 0.0f,  // right
                            0.45f, 0.5f, 0.0f   // top
  };

  unsigned int VBOs[2], VAOs[2];
  // VAO: makes switching between different vertex data and attribute
  // configurations as easy as binding a different VAO.
  glGenVertexArrays(2, VAOs);
  glGenBuffers(2, VBOs);

  // bind the first VAO
  glBindVertexArray(VAOs[0]);
  // bind the first VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // glBindVertexArray(0); // no need to unbind at all as we directly bind a different VAO the next few lines

  // bind the second VAO
  glBindVertexArray(VAOs[1]);
  // bind the second VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // ----------------------------------------------------------------------

  // render loop
  while(!glfwWindowShouldClose(window)) {
    processInput(window);

    // rendering process
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // use Orange draw program
    glUseProgram(shaderProgramOrange);
    // draw the first triangle
    glBindVertexArray(VAOs[0]);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // use yellow draw program
    glUseProgram(shaderProgramYellow);
    // draw the second triangle
    glBindVertexArray(VAOs[1]);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // check and call events and swap the buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(2, VAOs);
  glDeleteBuffers(2, VBOs);

  glfwTerminate();
  return 0;
}
