#define STB_IMAGE_IMPLEMENTATION

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GL_Window.h"
#include "common.h"
#include "simulation.h"
#include "constraint.h"
#include "particle.h"
#include "path_planner.h"
#include "wall.h"
#include "Viewer.h"

GLFWwindow* window;
GL_Window* gl_window;

bool stop = true;
bool A_button_down;
bool S_button_down;
bool D_button_down;

bool mouse_button_down_left;
bool mouse_button_down_right;
bool mouse_button_down_center;
double last_mouse_X;
double last_mouse_Y;
double cx, cy;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		stop = !stop;

	if (key == GLFW_KEY_A)
	{
		if (action == GLFW_PRESS)
			A_button_down = true;
		else if(action == GLFW_RELEASE)
			A_button_down = false;
	}

	if (key == GLFW_KEY_S)
	{
		if (action == GLFW_PRESS)
			S_button_down = true;
		else if (action == GLFW_RELEASE)
			S_button_down = false;
	}

	if (key == GLFW_KEY_D)
	{
		if (action == GLFW_PRESS)
			D_button_down = true;
		else if (action == GLFW_RELEASE)
			D_button_down = false;
	}

}
void window_size_callback(GLFWwindow* window, int width, int height)
{
	gl_window->set_size(width, height);
}
static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	cx = xpos;
	cy = ypos;
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		last_mouse_X = xpos;
		last_mouse_Y = ypos;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (GLFW_PRESS == action)
			mouse_button_down_left = true;
		else if (GLFW_RELEASE == action)
			mouse_button_down_left = false;
	}

	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (GLFW_PRESS == action)
			mouse_button_down_right = true;
		else if (GLFW_RELEASE == action)
			mouse_button_down_right = false;
	}

	else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		if (GLFW_PRESS == action)
			mouse_button_down_center = true;
		else if (GLFW_RELEASE == action)
			mouse_button_down_center = false;
	}
}
void mouseDragging(double width, double height)
{
	if (mouse_button_down_left && (!A_button_down && !S_button_down && !D_button_down)) {
		float fractionChangeX = static_cast<float>(cx - last_mouse_X) / static_cast<float>(width);
		float fractionChangeY = static_cast<float>(last_mouse_Y - cy) / static_cast<float>(height);
		gl_window->viewer->rotate(fractionChangeX, fractionChangeY);
	}
	else if (mouse_button_down_center) {
		float fractionChangeX = static_cast<float>(cx - last_mouse_X) / static_cast<float>(width);
		float fractionChangeY = static_cast<float>(last_mouse_Y - cy) / static_cast<float>(height);
		gl_window->viewer->zoom(fractionChangeY);
	}
	else if (mouse_button_down_right) {
		float fractionChangeX = static_cast<float>(cx - last_mouse_X) / static_cast<float>(width);
		float fractionChangeY = static_cast<float>(last_mouse_Y - cy) / static_cast<float>(height);
		gl_window->viewer->translate(-fractionChangeX, -fractionChangeY, 1);
	}

	last_mouse_X = cx;
	last_mouse_Y = cy;
}
void send_mouse_cursor_pos(Simulation* sim)
{
	float x = ((2.0f * cx) / WINDOW_WIDTH) - 1.0f;
	float y = 1.0f - ((2.0f * cy) / WINDOW_HEIGHT);
	float z = 1.0f;
	glm::vec3 ray_nds = glm::vec3(x, y, z);
	glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);

	if (mouse_button_down_left && A_button_down)
	{
		gl_window->send_ray(ray_clip, sim, 0);
	}

	if (mouse_button_down_left && S_button_down)
	{
		gl_window->send_ray(ray_clip, sim, 1);
	}

	if (mouse_button_down_left && D_button_down)
	{
		gl_window->send_ray(ray_clip, sim, 2);
	}
}

void Init() 
{
	std::cout << "Initialize glfw" << std::endl;
    if (!glfwInit())
    {
        const char *description = nullptr;
        glfwGetError(&description);
        std::cout << "failed to initialize glfw: {" <<  description << " }" << std::endl; 

		abort();
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    std::cout << "Create window!" << std::endl;
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);

    if (!window)
    {
        std::cout << "failed to create glfw window" << std::endl;
        glfwTerminate();

        abort();
    }

	// window context
	glfwMakeContextCurrent(window);

	// set callback
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetKeyCallback(window, key_callback);

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLAD Loader
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initilize GLAD" << std::endl;

		abort();
	}

	// complete OpenGL init
	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
}

int main()
{
	Init();

	gl_window = new GL_Window(WINDOW_WIDTH, WINDOW_HEIGHT);

	int num_particles = ROWS * COLS;
	char* output = (char*)"blender.txt";

	Simulation sim(num_particles, 0, MS_PER_UPDATE, output);

	gl_window->dummy_init(&sim);

	while (!glfwWindowShouldClose(window))
	{
		if (!stop)
			sim.do_time_step();

		sim.stop = stop;

		// process : rendering
		gl_window->draw(&sim);

		// process : buffer swap
		glfwSwapBuffers(window);

		// process : event callback
		glfwPollEvents();
		
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		mouseDragging(display_w, display_h);

		// if use path drawing, must stop program
		if (stop)
		{
			send_mouse_cursor_pos(&sim);
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}