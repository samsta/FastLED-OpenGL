#ifndef CONTROLS_HPP
#define CONTROLS_HPP

namespace flogl {

void computeMatricesFromInputs(GLFWwindow* window);
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

}
#endif
