#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <memory>
#include <sstream>
#include <thread>
#include <utility>

#ifdef ENABLE_GLFW
#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#undef APIENTRY
#endif // ENABLE_GLFW

#include <stb/stb_image_write.h>

#include <libRay/Math/MathUtils.hpp>
#include <libRay/Math/Ray.hpp>
#include <libRay/Math/Vector.hpp>
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

#ifdef ENABLE_GLFW
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

void ShowDebugWindow(
	LibRay::Camera const &camera,
	LibRay::RayTracer const &rayTracer,
	LibRay::Image const &normalizedImage);
#endif // ENABLE_GLFW

#if defined(_WIN32) && defined(UNICODE)
extern "C" int wmain();

extern "C" int wmain()
#else
int main()
#endif
{
	using namespace LibRay;
	using namespace LibRay::Math;

	Camera camera(
		Transform(Vector3(0, 5, 7), Vector3(-Math::PI * 0.15f, 0, 0)),
		Vector2st(1280, 720),
		Math::Radians(90.f),
		1.f,
		500.f);

	std::unique_ptr<Scene> scene;
	try
	{
		scene = std::make_unique<Scene>(
			std::move(camera),
			0,
			Materials::Color::White(),
			0.01f);
	}
	catch(std::exception const &e)
	{
		std::fprintf(stderr, "Caught exception: %s\n", e.what());
		return EXIT_FAILURE;
	}

	RayTracerConfiguration config(
		4,
#ifdef DEBUG
		1,
		RayTracerConfiguration::AntiAliasingMode::AA1);
#else
		std::uint8_t(std::thread::hardware_concurrency()),
		RayTracerConfiguration::AntiAliasingMode::AA16);
#endif

	RayTracer rayTracer(*scene, std::move(config));

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

	auto const result = WriteImage(normalizedOutput);
	if(!result.first)
		return EXIT_FAILURE;

#ifdef ENABLE_GLFW
	try
	{
		ShowDebugWindow(camera, rayTracer, normalizedOutput);
	}
	catch(std::exception const &e)
	{
		std::fprintf(stderr, "Caught exception: %s\n", e.what());
		return EXIT_FAILURE;
	}
#endif // ENABLE_GLFW

	//system(result.second.c_str());

	return EXIT_SUCCESS;
}

LibRay::Image NormalizeImage(LibRay::Image const &image)
{
	using namespace LibRay;
	using namespace LibRay::Materials;

	Image normalizedImage = image;

	// Saturate and clamp all colors.
	std::transform(
		normalizedImage.pixels.begin(),
		normalizedImage.pixels.end(),
		normalizedImage.pixels.begin(),
		[](Color const &color)
		{
			auto const &ACESFilm = [](Color const &color) -> Color
			{
				float a = 2.51f;
				float b = 0.03f;
				float c = 2.43f;
				float d = 0.59f;
				float e = 0.14f;

				Color const partA = (color * (a * color + b));
				Color const partB = (color * (c * color + d) + e);

				return (partA / partB).Clamped();
			};

			Color out = ACESFilm(color);
			out.r = std::pow(out.r, 1.f / 2.2f);
			out.g = std::pow(out.g, 1.f / 2.2f);
			out.b = std::pow(out.b, 1.f / 2.2f);

			return out;
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

#if defined(_WIN32)
	localtime_s(&buf, &now);
#else
	localtime_r(&now, &buf);
#endif

	std::stringstream stream;
	stream << "Rendered-" << std::put_time(&buf, "%Y-%m-%d-%H-%M-%S") << ".png";
	std::string output = stream.str();

	if(!stbi_write_png(
			output.c_str(),
			int(normalizedImage.sizeX),
			int(normalizedImage.sizeY),
			4,
			ABGRData.data(),
			0))
	{
		std::fprintf(stderr, "Failed to write to output <%s>.\n", output.c_str());
		return {false, std::string()};
	}

	return {true, output};
}

#ifdef ENABLE_GLFW
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
		RayTracer::RayState state;
		rayTracer.TraceRay(mouseRay, state, true);
	}
	catch(std::exception const &e)
	{
		std::fprintf(stderr, "Caught exception: %s\n", e.what());
	}

	std::fflush(stdout);
}

void ShowDebugWindow(
	LibRay::Camera const &camera,
	LibRay::RayTracer const &rayTracer,
	LibRay::Image const &normalizedImage)
{
	using namespace LibRay;

	if(!glfwInit())
		throw std::runtime_error("Unable to initialize GLFW");

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	Observer<GLFWwindow> window = glfwCreateWindow(
		int(camera.ScreenSize().x),
		int(camera.ScreenSize().y),
		"Ray tracer",
		nullptr,
		nullptr);

	glfwSetWindowUserPointer(window, const_cast<Observer<RayTracer>>(&rayTracer));

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseCallback);
	glfwMakeContextCurrent(window);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		GLsizei(normalizedImage.sizeX),
		GLsizei(normalizedImage.sizeY),
		0,
		GL_RGB,
		GL_FLOAT,
		normalizedImage.pixels.data());

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
}
#endif // ENABLE_GLFW
