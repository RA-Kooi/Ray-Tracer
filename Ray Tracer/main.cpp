#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#define __STDC_WANT_LIB_EXT1__
#include <ctime>
#include <iomanip>
#include <memory>
#include <sstream>
#include <utility>

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#undef APIENTRY

#include <stb/stb_image_write.h>

#include <libRay/Math/MathUtils.hpp>
#include <libRay/Math/Ray.hpp>
#include <libRay/Math/Vector2.cpp>
#include <libRay/Math/Vector3.cpp>
#include <libRay/RayTracer.hpp>
#include <libRay/Scene.hpp>
#include <libRay/Shapes/Shape.hpp>
#include <libRay/Camera.hpp>
#include <libRay/Image.hpp>
#include <libRay/Intersection.hpp>
#include <libRay/Transform.hpp>
#include <libRay/Utilites.hpp>

// Disable TGA compression, for easy viewing in a hex editor
extern "C" void DisableTGARLE();

LibRay::Image NormalizeImage(LibRay::Image const &image);

std::pair<bool, std::string> WriteImage(LibRay::Image const &normalizedImage);

void KeyCallback(
	Observer<GLFWwindow> window,
	int key,
	int scancode,
	int action,
	int mods) noexcept;

void MouseCallback(
	Observer<GLFWwindow> window,
	int button,
	int action,
	int mods) noexcept;

int main(int, char*[])
{
	using namespace LibRay;
	using namespace LibRay::Math;

	DisableTGARLE();

	if(!glfwInit())
		return EXIT_FAILURE;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	Camera camera(
		Transform(Vector3(0, 15, 30), Vector3(-Math::PI * 0.125f)),
		Vector2(640.f, 480.f),
		Math::Radians(90.f),
		1.f,
		200.f);

	std::unique_ptr<Scene> scene;
	try
	{
		scene = std::make_unique<Scene>(
			std::move(camera),
			0,
			Shapes::Color::White(),
			0.01f);
	}
	catch(std::exception const &e)
	{
		std::fprintf(stderr, "Caught exception: %s\n", e.what());
		return EXIT_FAILURE;
	}

	RayTracerConfiguration config(4);
	RayTracer rayTracer(*scene, std::move(config));

	Observer<GLFWwindow> window = glfwCreateWindow(
		int(camera.ScreenSize().x),
		int(camera.ScreenSize().y),
		"Ray tracer",
		nullptr,
		nullptr);

	glfwSetWindowUserPointer(window, &rayTracer);

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseCallback);
	glfwMakeContextCurrent(window);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);

	Image output(0, 0);

	try
	{
		output = rayTracer.Trace();
	}
	catch(std::exception const &e)
	{
		std::fprintf(stderr, "Caught exception: %s\n", e.what());
		return EXIT_FAILURE;
	}

	Image const normalizedOutput = NormalizeImage(output);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		GLsizei(output.sizeX),
		GLsizei(output.sizeY),
		0,
		GL_RGB,
		GL_FLOAT,
		output.pixels.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glEnable(GL_TEXTURE_2D);

	while(!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glLoadIdentity();

		glBegin(GL_QUADS);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f( 1.0f, 1.0f, 0.0f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f( 1.0f,-1.0f, 0.0f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1.0f,-1.0f, 0.0f);
		glEnd();

		glfwSwapBuffers(window);

		glfwWaitEvents();
	}

	glfwTerminate();

	glDeleteTextures(1, &texture);

	auto const result = WriteImage(normalizedOutput);
	if(!result.first)
		return EXIT_FAILURE;

	//system(result.second.c_str());

	return EXIT_SUCCESS;
}

LibRay::Image NormalizeImage(LibRay::Image const &image)
{
	using namespace LibRay;

	Image normalizedImage = image;

	// Clamp all colors.
	std::transform(
		normalizedImage.pixels.begin(),
		normalizedImage.pixels.end(),
		normalizedImage.pixels.begin(),
		[](Shapes::Color const &color)
		{
			return color.Clamped();
		});

	return normalizedImage;
}

std::pair<bool, std::string> WriteImage(LibRay::Image const &normalizedImage)
{
	// Convert the floats to RGBA data 0..255
	std::vector<std::uint32_t> ABGRData;
	ABGRData.reserve(normalizedImage.pixels.size());

	auto const &pixels = normalizedImage.pixels;

	std::generate_n(
		std::back_inserter(ABGRData),
		normalizedImage.pixels.size(),
		[i = 0, &pixels]() mutable
		{
			std::uint32_t pixel =
				(std::uint32_t(255) << 24)
				| (std::uint32_t(255.99f * pixels[std::size_t(i)].b) << 16)
				| (std::uint32_t(255.99f * pixels[std::size_t(i)].g) << 8)
				| (std::uint32_t(255.99f * pixels[std::size_t(i)].r));

			++i;

			return pixel;
		});

	// Generate a filename in the form of
	// Rendered-*year*-*month*-*day*-*hour*-*minute*-*seconds*.tga
	using clock = std::chrono::system_clock;

	std::time_t now = clock::to_time_t(clock::now());
	std::tm buf;
	localtime_s(&buf, &now);

	std::stringstream stream;
	stream << "Rendered-" << std::put_time(&buf, "%Y-%m-%d-%H-%M-%S") << ".tga";
	std::string output = stream.str();

	if(!stbi_write_tga(
			output.c_str(),
			int(normalizedImage.sizeX),
			int(normalizedImage.sizeY),
			4,
			ABGRData.data()))
	{
		std::fprintf(stderr, "Failed to write to output <%s>.\n", output.c_str());
		return {false, std::string()};
	}

	return {true, output};
}

void KeyCallback(
	Observer<GLFWwindow> window,
	int key,
	int,
	int action,
	int) noexcept
{
	if(action != GLFW_PRESS)
		return;

	switch(key)
	{
	case GLFW_KEY_ESCAPE:
	{
		glfwSetWindowShouldClose(window, 1);
	} break;
	}
}

void MouseCallback(
	Observer<GLFWwindow> window,
	int button,
	int action,
	int) noexcept
{
	using namespace LibRay;
	using namespace LibRay::Math;
	using namespace LibRay::Shapes;

	RayTracer const &rayTracer = *static_cast<Observer<RayTracer const>>(
		glfwGetWindowUserPointer(window));

	if(action != GLFW_PRESS || button != GLFW_MOUSE_BUTTON_LEFT)
		return;

	double x, y;
	glfwGetCursorPos(window, &x, &y);

	Ray const mouseRay = rayTracer.MakeMouseRay(int(x), int(y));

	try
	{
		rayTracer.TraceRay(mouseRay, 0, true);
	}
	catch(std::exception const &e)
	{
		std::fprintf(stderr, "Caught exception: %s\n", e.what());
	}
}
