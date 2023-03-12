#define STB_IMAGE_IMPLEMENTATION

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GL_Window.h"
#include "utility.h"
#include "simulation.h"
#include "constraint.h"
#include "particle.h"
#include "path_planner.h"
#include "wall.h"

GLFWwindow* window;
GL_Window* gl_window;

bool stop = true;
bool left_control_button_down;
bool right_control_button_down;

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

	if (key == GLFW_KEY_LEFT_CONTROL)
	{
		if (action == GLFW_PRESS)
			left_control_button_down = true;
		else if(action == GLFW_RELEASE)
			left_control_button_down = false;
	}

	if (key == GLFW_KEY_RIGHT_CONTROL)
	{
		if (action == GLFW_PRESS)
			right_control_button_down = true;
		else if (action == GLFW_RELEASE)
			right_control_button_down = false;
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
	if (mouse_button_down_left && (!left_control_button_down && !right_control_button_down)) {
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

	if (mouse_button_down_left && left_control_button_down)
	{
		gl_window->send_ray(ray_clip, sim, 0);
	}

	if (mouse_button_down_left && right_control_button_down)
	{
		gl_window->send_ray(ray_clip, sim, 1);
	}
}

void Init() {
	glfwInit();

	// using glfw 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// GLFW를 활용해 window 생성
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		abort();
	}
	// window의 context를 현재 스레드의 주 context로 지정
	glfwMakeContextCurrent(window);

	// glfw에 콜백함수 등록
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetKeyCallback(window, key_callback);

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLAD Loader를 활용하여, OpenGL의 함수를 관리
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initilize GLAD" << std::endl;

		abort();
	}

	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
}

int main()
{
	Init();

	gl_window = new GL_Window(WINDOW_WIDTH, WINDOW_HEIGHT);

	double lastTime = glfwGetTime();
	double frameCount = 0;
	int min_frame = 10000;
	int max_frame = 0;

	int num_particles = ROWS * COLS;
	char* output = (char*)"blender.txt";
	Simulation sim(num_particles, 0, MS_PER_UPDATE, output);

	gl_window->dummy_init(&sim);

	while (!glfwWindowShouldClose(window))
	{
#pragma region FRAME_COUNT
		/*
		double currentTime = glfwGetTime();
		frameCount++;

		if (currentTime - lastTime >= 1.0) 
		{ // If last prinf() was more than 1 sec ago
			// previousTime and reset timer

			if (frameCount > 10 && frameCount < min_frame)
			{
				min_frame = frameCount;
				cout << min_frame << " : min_frame / ";
				cout << max_frame << " : MAX_FRAME" << std::endl;
			}
			if (frameCount < 60 && frameCount > max_frame)
			{
				max_frame = frameCount;
				cout << min_frame << " : min_frame / ";
				cout << max_frame << " : MAX_FRAME" << std::endl;
			}
			
			printf("%f ms/frame\n", 1000.0 / double(frameCount));
			
			frameCount = 0;
			lastTime += 1.0;
		}
		*/
#pragma endregion
		int display_w, display_h;

		// 버퍼 사이즈 가져오기
		glfwGetFramebufferSize(window, &display_w, &display_h);

		if (!stop)
			sim.do_time_step();

		sim.stop = stop;

		// 그리기
		gl_window->draw(&sim);

		// 버퍼 교체 (버퍼 돌려쓰기)
		glfwSwapBuffers(window);
		glfwPollEvents();

		mouseDragging(display_w, display_h);

		if (stop)
		{
			send_mouse_cursor_pos(&sim);
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}