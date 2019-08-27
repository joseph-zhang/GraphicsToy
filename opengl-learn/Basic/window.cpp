#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;

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
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "window demo", nullptr, nullptr);
  if(window == nullptr) {
    cout << "Failed to create GLFW window" << endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // initialize GLAD before we call any OpenGL functions
  if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    cout << "Failed to initialize GLAD" << endl;
    return -1;
  }

  // tell OpenGL the size of the rendering window
  // set the leftcorner of the window
  glViewport(0, 0, 800, 600);

  // register callback function to deal with window resize
  glfwSetFramebufferSizeCallback(window, framebuffer_size_call_back);

  // render loop, keeps on drawing until we tell GLFW to stop
  while(!glfwWindowShouldClose(window)) {
    processInput(window);

    // rendering process
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // check and call events and swap the buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // delete or clean GLFW resources that were allocated
  glfwTerminate();
  return 0;
}
