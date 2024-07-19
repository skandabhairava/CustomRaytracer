#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <iomanip>

#include "Helper.h"

#include "Image/Image.h"

#include "Units/Units.h"
#include "Scene/Scene.h"
#include "Objects/Objects.h"
#include "Shader/Shader.h"

void tutorial() {
	Camera cam = Camera(0, Vector(0, -400, 0), Vector(0, 1, 0), 900, 900, 500, 5000.0, 0.0); // set width to 1422 to get 1920:1080 like view
	// to get parallel view: Camera cam = Camera(0, Vector(0, -1000, 0), Vector(0, 1, 0), 900, 900, 1100, 5000.0, 0.0); // set width to 1422 to get 1920:1080 like view

	//spherezz
	Image image = Image(Color(255,255,255));
	//Image red = Image(Color(0xAA0000, 1.0f));

	Image specular_map = Image(Color(0,0,0)); // FileType::GrayScale

	Sphere test = Sphere(1, Vector(0, 0, 0), Vector(0, 1, 0), Material(&image, &specular_map, nullptr, nullptr), 200.0);
	PointLighting light = PointLighting(2, Vector(0, -500, 0), Vector(0, 1, 0), 1.0);
	Scene main_scene = Scene(Color(0x000000), 0.0); // dark blue

	main_scene.add_obj((Object3D*)&cam);
	main_scene.add_obj((Object3D*)&test);
	main_scene.add_obj((Object3D*)&light);

	Image* render = cam.render(main_scene);
	ASSERT(render != nullptr);
	render->compile("tutorial");
	delete render;
}


void single_sphere() {
	Camera cam = Camera(0, Vector(0, -400, 100), Vector(0, 1, -0.25), 900, 900, 700, 12000.0, 0.0); // set width to 1422 to get 1920:1080 like view
	// to get parallel view: Camera cam = Camera(0, Vector(0, -1000, 0), Vector(0, 1, 0), 900, 900, 1100, 5000.0, 0.0); // set width to 1422 to get 1920:1080 like view

	//spherezz
	Image image = Image("earth-squash", FileType::RGBA);
	//Image dull_red = Image(Color(0xFF0000, 1.0f));

	Image specular_map = Image("earth-squash-specular", FileType::Grayscale); // FileType::GrayScale
	Image normal_map = Image("earth-squash-normal", FileType::RGBA);
	//Image grey = Image(Color(0xFFFFFF, 1.0f)); // FileType::GrayScale

	// from north pole +deg is anticlockwise
	double rot = 3*PI/2;
	//double rot = 0;
	Sphere test = Sphere(1, Vector(0, 0, 0), Vector(sin(rot), cos(rot), 0).normalize(), Material(&image, &specular_map, &normal_map, nullptr), 90.0);

	Image starmap = Image("starmap", FileType::RGBA);
	double star_rot = PI / 2.65;
	Sphere skysphere = Sphere(2, Vector(0, -400.1, 100), Vector(sin(star_rot), cos(star_rot), 0).normalize(), Material(&starmap, nullptr, nullptr, (Image*)&WHITE_BLANK_IMAGE), 8000.0);

	
	PointLighting light = PointLighting(4, Vector(110, -250, 80), Vector(0, 1, 0), 1.0);
	
	Scene main_scene = Scene(Color(0x0000AA), 0.0); // dark blue

	main_scene.add_obj((Object3D*)&cam);
	main_scene.add_obj((Object3D*)&test);
	main_scene.add_obj((Object3D*)&skysphere);
	main_scene.add_obj((Object3D*)&light);

	Image* render = cam.render(main_scene);
	ASSERT(render != nullptr);
	render->compile("test");
	delete render;

	//red.compile("test");

	/*
	double rot = 0;
	//__debugbreak();
	for (int i = 0; i < 100; i++, rot += (2 * PI) / 100) {

		if (i == 50) {
			//__debugbreak();
		}

		// turning clockwise
		//Vector vec = Vector(sin(rot), cos(rot), 0).normalize();

		// turning up
		Vector vec = Vector(0, cos(rot), sin(rot)).normalize();

		DD testmm = test.get_uv(vec);
		//std::cout << "cos(" << (rot*180.0)/PI <<  "deg) = " << cos(rot) << "; " << testmm.a << " : " << testmm.b << std::endl;
		std::cout << ~vec << "  " << "Rot: " << (rot * 180.0) / PI << "... " << testmm.a << " : " << testmm.b << std::endl;
	}
	std::cin.get();
	*/
}

void single_sphere_rot_animation() {
	Camera cam = Camera(0, Vector(0, -400, 100), Vector(0, 1, -0.25), 900, 900, 700, 12000.0, 0.0); // set width to 1422 to get 1920:1080 like view
	// to get up view Camera cam = Camera(0, Vector(0, -1000, 900), Vector(0, 1, -1.0), 900, 900, 1100, 5000.0, 0.0); // set width to 1422 to get 1920:1080 like view

	//spherezz
	Image image = Image("earth-squash", FileType::RGBA);
	//Image red = Image(Color(0xAA0000, 1.0f));

	Image specular_map = Image("earth-squash-specular", FileType::Grayscale); // FileType::GrayScale
	Image normal_map = Image("earth-squash-normal", FileType::RGBA);

	// from north pole +deg is anticlockwise
	double rotation = 0;
	Sphere test = Sphere(1, Vector(0, 0, 0), Vector(sin(rotation), cos(rotation), 0).normalize(), Material(&image, &specular_map, &normal_map, nullptr), 90.0);

	Image starmap = Image("starmap", FileType::RGBA);
	double star_rot = PI / 2.65;
	Sphere skysphere = Sphere(2, Vector(0, -400.1, 100), Vector(sin(star_rot), cos(star_rot), 0).normalize(), Material(&starmap, nullptr, nullptr, (Image*)&WHITE_BLANK_IMAGE), 8000.0);


	PointLighting light = PointLighting(4, Vector(110, -250, 80), Vector(0, 1, 0), 1.0);

	Scene main_scene = Scene(Color(0x000000), 0.0);

	main_scene.add_obj((Object3D*)&cam);
	main_scene.add_obj((Object3D*)&test);
	main_scene.add_obj((Object3D*)&skysphere);
	main_scene.add_obj((Object3D*)&light);

	int fps = 30, length_of_vid = 10;
	int total_frames = fps * length_of_vid;

	for (int frame = 0; frame < total_frames; frame++, rotation -= (2*PI) / total_frames) {

		//test.origin.z = 80 * sin(2 * rotation);

		test.normal = Vector(sin(rotation), cos(rotation), 0).normalize();

		Image* render = cam.render(main_scene);
		ASSERT(render != nullptr);

		std::stringstream ss;
		ss << std::setw(4) << std::setfill('0') << frame;

		render->compile("renders/" + ss.str(), false);
		delete render;

		std::cout << "Done " << frame << std::endl;
	}

	//Image* render = cam.render(main_scene);
	//ASSERT(render != nullptr);
	//render->compile("test");
	//delete render;

	//red.compile("test");
}

void moon() {
	Camera cam = Camera(0, Vector(0, -600, 0), Vector(0, 1, 0), 1536, 2048, 800, 50000000.0, 0.1); // set width to 1422 to get 1920:1080 like view
	Image moon_diffuse = Image("moon_render/moon", FileType::RGBA);
	double rotation = PI*1.0;
	Sphere moon = Sphere(1, Vector(-120, 0, 60), Vector(sin(rotation), cos(rotation), 0).normalize(), Material(&moon_diffuse, nullptr, nullptr, nullptr), 50.0);

	Image starmap = Image("moon_render/star_field", FileType::RGBA);
	double star_rot = PI / 2.0;
	Sphere skysphere = Sphere(2, Vector(0, -600.5, 0), Vector(sin(star_rot), cos(star_rot), 0).normalize(), Material(&starmap, nullptr, nullptr, (Image*)&WHITE_BLANK_IMAGE), 4000000.0);

	PointLighting light = PointLighting(3, Vector(150, -400, 108), Vector(0, 1, 0), 1.0);
	PointLighting light2 = PointLighting(4, Vector(150, -400, 130), Vector(0, 1, 0), 1.0);

	Scene main_scene = Scene(Color(0x000000), 0.01);
	main_scene.add_obj((Object3D*)&cam);
	main_scene.add_obj((Object3D*)&moon);
	main_scene.add_obj((Object3D*)&skysphere);
	main_scene.add_obj((Object3D*)&light);
	main_scene.add_obj((Object3D*)&light2);


	Image* render = cam.render(main_scene);
	ASSERT(render != nullptr);
	render->compile("moon_render/render");
	delete render;
}


void main_test() {
	Camera cam = Camera(0, Vector(0, -450, 0), Vector(0, 1, 0), 900, 900, 500, 1000.0, 0.0); // set width to 1422 to get 1920:1080 like view

	
	// red
	Image red = Image(Color(0xAA0000, 0.5f));
	Sphere test = Sphere(1, Vector(0, 0, 50), Vector(0, 1, 0), Material(&red, nullptr, nullptr, nullptr), 80.0);

	// green
	Image green = Image(Color(0x00AA00, 1.0f));
	Sphere test2 = Sphere(2, Vector(-50, 0, -50), Vector(0, 1, 0), Material(&green, nullptr, nullptr, nullptr), 80.0);

	// blue
	Image blue = Image(Color(0x0000AA, 1.0f));
	Sphere test3 = Sphere(3, Vector(50, 0, -50), Vector(0, 1, 0), Material(&blue, nullptr, nullptr, nullptr), 80.0);

	PointLighting light = PointLighting(4, Vector(-50, -200, 20), Vector(0, 1, 0), 1.0);

	//Scene main_scene = Scene(Color(0x05D5FA), 0.1); // blue
	Scene main_scene = Scene(Color(0x0000AA), 0.1); // dark blue
	
	main_scene.add_obj((Object3D*)&cam);
	main_scene.add_obj((Object3D*)&test);
	main_scene.add_obj((Object3D*)&test2);
	main_scene.add_obj((Object3D*)&test3);
	main_scene.add_obj((Object3D*)&light);

	Image* render = cam.render(main_scene);
	ASSERT(render != nullptr);
	render->compile("test-transparent-red");
	delete render;
}


void water_molecule_still() {

	double rotation = 0;

	Camera cam = Camera(0, Vector(0, -450, 0), Vector(0, 1, 0), 900, 900, 500, 1000.0, 0.0); // set width to 1422 to get 1920:1080 like view

	Image specular = Image(Color(0xFFFFFF));

	// red Oxygen
	Image red = Image(Color(0xFF0000, 0.5f));
	Sphere test = Sphere(1, Vector(0, 0, 0), Vector(0, -1, 0), Material(&red, nullptr, nullptr, nullptr), 90.0);

	// white Hydrogen
	Image white = Image(Color(0xAAAAAA, 1.0f));
	Sphere test2 = Sphere(2, Vector(cos(rotation) * 90.0 * cos(-PI / 2 - 0.911934), 90.0 * sin(rotation), 90.0 * sin(-PI / 2 - 0.911934)), Vector(0, 1, 0), Material(&white, &specular, nullptr, nullptr), 70.0);

	// white Hydrogen
	Sphere test3 = Sphere(3, Vector(cos(rotation) * 90.0 * cos(-PI / 2 + 0.911934), -90.0 * sin(rotation), 90.0 * sin(-PI / 2 + 0.911934)), Vector(0, 1, 0), Material(&white, &specular, nullptr, nullptr), 70.0);

	PointLighting light = PointLighting(4, Vector(-50, -200, 20), Vector(0, 1, 0), 1.0);

	//Scene main_scene = Scene(Color(0xAAAAAA), 0.1); // white
	Scene main_scene = Scene(Color(0x0000AA), 0.0); // dark blue

	main_scene.add_obj((Object3D*)&cam);
	main_scene.add_obj((Object3D*)&test);
	main_scene.add_obj((Object3D*)&test2);
	main_scene.add_obj((Object3D*)&test3);
	main_scene.add_obj((Object3D*)&light);
	//main_scene.add_obj((Object3D*)&light2);

	Image* render = cam.render(main_scene);
	ASSERT(render != nullptr);
	render->compile("water-mol");
	delete render;
}


/*
void water_molecule_animate() {
	Camera cam = Camera(0, Vector(0, -450, 0), Vector(0, 1, 0), 900, 900, 500, 1000.0, 0.0); // set width to 1422 to get 1920:1080 like view

	double rotation = 0;

	// red Oxygen
	Sphere test = Sphere(1, Vector(0, 0, 0), Vector(0, 1, 0), Material(Color(0xAA0000, 1.0f), 2), 90.0);

	// white Hydrogen
	Sphere test2 = Sphere(2, Vector(cos(rotation) * 90.0 * cos(-PI / 2 - 0.911934), 90.0 * sin(rotation), 90.0 * sin(-PI / 2 - 0.911934)), Vector(0, 1, 0), Material(Color(0xAAAAAA, 1.0f), 32), 70.0);

	// white Hydrogen
	Sphere test3 = Sphere(3, Vector(cos(rotation) * 90.0 * cos(-PI / 2 + 0.911934), -90.0 * sin(rotation), 90.0 * sin(-PI / 2 + 0.911934)), Vector(0, 1, 0), Material(Color(0xAAAAAA, 1.0f), 32), 70.0);

	PointLighting light = PointLighting(4, Vector(-50, -200, 20), Vector(0, 1, 0), 1.0);
	//PointLighting light = PointLighting(5, Vector(0, 1, 0), Vector(0, 1, 0), 1.0);


	//Scene main_scene = Scene(Color(0xAAAAAA), 0.1); // white
	Scene main_scene = Scene(Color(0x0000AA), 0.1); // dark blue

	main_scene.add_obj((Object3D*)&cam);
	main_scene.add_obj((Object3D*)&test);
	main_scene.add_obj((Object3D*)&test2);
	main_scene.add_obj((Object3D*)&test3);
	main_scene.add_obj((Object3D*)&light);
	//main_scene.add_obj((Object3D*)&light2);

	int fps = 30, length_of_vid = 5;
	int total_frames = fps * length_of_vid;

	for (int frame = 0; frame < total_frames; frame++, rotation += (PI) / total_frames) {

		test.origin.z = 80 * sin(2 * rotation);

		test2.origin.x = cos(rotation) * 90.0 * cos(-PI / 2 - 0.911934);
		test2.origin.y = 90.0 * sin(rotation);
		test2.origin.z = 90.0 * sin(-PI / 2 - 0.911934) + 80 * sin(2 * rotation);

		test3.origin.x = cos(rotation) * 90.0 * cos(-PI / 2 + 0.911934);
		test3.origin.y = -90.0 * sin(rotation);
		test3.origin.z = 90.0 * sin(-PI / 2 + 0.911934) + 80 * sin(2 * rotation);

		Image* render = cam.render(main_scene);
		ASSERT(render != nullptr);

		std::stringstream ss;
		ss << std::setw(4) << std::setfill('0') << frame;

		render->compile("renders/" + ss.str());
		delete render;

		std::cout << "Done " << frame << std::endl;
	}

	Image* render = cam.render(main_scene);
	ASSERT(render != nullptr);
	render->compile("test");
	delete render;
}
*/

void animation() {
	Scene main_scene = Scene(Color(0x000000), 0.0);
	PointLighting light = PointLighting(2, Vector(0, -700, 0), Vector(0, 1, 0), 1.0);
	Camera cam = Camera(0, Vector(0, -400, 0), Vector(0, 1, 0), 800, 800, 1000, 12000.0, 0.0);

	int total_frames = 3288;
	//int total_frames = 25;

	Image full_white = Image("animation/full_white", FileType::RGBA);
	Image full_black = Image("animation/full_black", FileType::RGBA);

	// change k back to 1
	int frame = 0;
	std::cin >> frame;

	std::cout << "Starting from: " << frame << std::endl;

	for (int k = frame; k <= total_frames; k++) {
		main_scene.add_obj((Object3D*)&cam);
		main_scene.add_obj((Object3D*)&light);

		int count = 5;

		std::stringstream ss;
		ss << std::setw(4) << std::setfill('0') << k;

		Image frame = Image("animation/frames/out" + ss.str(), FileType::RGBA);

		//Sphere* test1 = new Sphere(count, Vector(-155, 0, 155), Vector(0, 1, 0), Material(nullptr, nullptr, nullptr, nullptr), 5.0);
		//main_scene.add_obj((Object3D*)test1);
		//count++;

		int white_count = 0;

		for (int i = 155, y_index = 0; i >= -155; i -= 10, y_index++)
			for (int j = -155, x_index = 0; j <= 155; j += 10, x_index++) {
				unsigned char* pixel = frame.get_pixel(x_index, y_index);
				if (pixel == nullptr) {
					std::cout << "Encountered null at: " << y_index << ", " << x_index << std::endl;
					continue;
				}

				if (*pixel < 127) {
					continue;
				}

				white_count++;
				Sphere* test1 = new Sphere(count, Vector(j, 0, i), Vector(0, 1, 0), Material(nullptr, nullptr, nullptr, nullptr), 5.0);
				main_scene.add_obj((Object3D*)test1);
				count++;
			}

		/*
		for (int i = -20; i <= 20; i += 10)
			for (int j = 20; j >= -20; j -= 10) {
				Sphere* test1 = new Sphere(count, Vector(i, 0, j), Vector(0, 1, 0), Material(nullptr, nullptr, nullptr, nullptr), 5.0);
				main_scene.add_obj((Object3D*)test1);
				count++;
			}
		*/

		//std::cout << "Done with adding objects. Rendering...\n";
		//if (k >= 15) __debugbreak();
		if (white_count == (32 * 32)) {
			full_white.compile("animation/render/" + ss.str(), false);
		}
		else if (white_count == 0) {
			full_black.compile("animation/render/" + ss.str(), false);
		}
		else {
			Image* render = cam.render(main_scene);
			ASSERT(render != nullptr);

			render->compile("animation/render/" + ss.str(), false);
			delete render;
		}

		for (Object3D* item : main_scene.objects) {
			if (item->type==ObjectType::Sphere_)
				delete item;
		}
		main_scene.objects.clear();

		std::cout << "Done " << k << std::endl;
	}
}

int main() {
	
	//water_molecule_still();
	//single_sphere();
	//single_sphere_rot_animation(); //300 frames
	//moon();
	//tutorial();
	//main_test();
	animation();
	
	/*
	Image image = Image("TEXTURE", FileType::RGBA);
	Material mat = Material(&image, 32);

	Color col = mat.get_pixel_from_uv({0.0, 0.0}, image);

	std::cout << ~col << std::endl;

	std::cin.get();
	*/

}