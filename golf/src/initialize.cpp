#include "scene.hpp"
#include "terrain.hpp"
#include "tree.hpp"

using namespace cgp;

void scene_structure::initialize_camera()
{
	camera_control.initialize(inputs, window);
	camera_control.set_rotation_axis_z();
	vec3 cam_init = ball_position + vec3{orbit_radius, 0, orbit_height};
	camera_control.look_at(cam_init, ball_position);
}

void scene_structure::initialize_skybox()
{
	image_structure image_skybox_template = image_load_file(project::path + "assets/skybox_02.jpg");
	std::vector<image_structure> image_grid = image_split_grid(image_skybox_template, 4, 3);
	skybox.initialize_data_on_gpu();
	skybox.texture.initialize_cubemap_on_gpu(image_grid[1], image_grid[7], image_grid[5], image_grid[3], image_grid[10], image_grid[4]);
	skybox.model.rotation = rotation_transform::from_axis_angle({1, 0, 0}, 1.5709);
}

void scene_structure::initialize_terrain()
{
	int N_terrain_samples = 100;
	float terrain_length_x = 80;
	float terrain_length_y = 30;
	mesh const terrain_mesh = create_terrain_mesh(N_terrain_samples, terrain_length_x, terrain_length_y);
	terrain.initialize_data_on_gpu(terrain_mesh);
	terrain.material.color = {1.0f, 1.0f, 1.0f};
	terrain.material.phong.specular = 0.0f;
	terrain.texture.load_and_initialize_texture_2d_on_gpu("assets/texture_grass.jpg", GL_REPEAT, GL_REPEAT);
}

void scene_structure::initialize_water()
{
	float sea_w = 25.0f;
	float sea_z = -0.5f;
	int N_sea_samples = 100;
	water.initialize_data_on_gpu(create_sea_mesh(N_sea_samples, sea_w, sea_w));
	water.texture.load_and_initialize_texture_2d_on_gpu("assets/sea2.jpg");
	water.model.translation = {-12.5, -2.5, sea_z};
	water.material.alpha = 0.8f;
	water.material.phong = {0.4f, 0.6f, 0.5f};
}

void scene_structure::initialize_circle()
{
	mesh circle_mesh = mesh_primitive_disc(4.0f, vec3{-15.0f, 5.0f, 0.02f}, vec3{0.0f, 0.0f, 1.0f}, 60);
	circle.initialize_data_on_gpu(circle_mesh);
	circle.texture.load_and_initialize_texture_2d_on_gpu("assets/green.jpg", GL_REPEAT, GL_REPEAT);
	circle.material.color = {1.0f, 1.0f, 1.0f};
	circle.material.phong = {0.3f, 0.6f, 0.2f};
}

void scene_structure::initialize_grass()
{
	grass.initialize_data_on_gpu(mesh_primitive_quadrangle({-0.5f, 0, 0}, {0.5f, 0, 0}, {0.5f, 0, 1}, {-0.5f, 0, 1}));
	grass.texture.load_and_initialize_texture_2d_on_gpu("assets/grass.png");
	grass.material.phong = {0.4f, 0.6f, 0, 1};
	grass.model.scaling = 0.6f;
	grass_position = generate_positions_on_terrain(8000, 70, 30);
}

void scene_structure::initialize_trees()
{
	trunk.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/trunk.obj"));
	trunk.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/trunk.png");
	branches.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/branches.obj"));
	branches.material.color = {0.45f, 0.41f, 0.34f};
	foliage.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/foliage.obj"));
	foliage.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/pine.png");
	foliage.shader.load(project::path + "shaders/mesh_transparency/mesh_transparency.vert.glsl", project::path + "shaders/mesh_transparency/mesh_transparency.frag.glsl");
	foliage.material.phong = {0.4f, 0.6f, 0, 1};
	trunk.model.rotation = rotation_transform::from_axis_angle({1, 0, 0}, 1.5709);
	branches.model.rotation = rotation_transform::from_axis_angle({1, 0, 0}, 1.5709);
	foliage.model.rotation = rotation_transform::from_axis_angle({1, 0, 0}, 1.5709);
	tree_position = {
		{18.0f, 9.0f, evaluate_terrain_height(18.0f, 9.0f)},
		{11.0f, 7.0f, evaluate_terrain_height(11.0f, 7.0f)},
		{6.0f, 6.0f, evaluate_terrain_height(6.0f, 6.0f)},
		{-5.0f, 8.0f, evaluate_terrain_height(-5.0f, 8.0f)},
		{15.0f, -6.0f, evaluate_terrain_height(15.0f, -6.0f)},
		{6.0f, -8.0f, evaluate_terrain_height(6.0f, -8.0f)},
        {31.0f, -12.0f, evaluate_terrain_height(31.0f, -12.0f)},
        {29.0f, 11.0f, evaluate_terrain_height(29.0f, 11.0f)},
        {-19.0f, -12.0f, evaluate_terrain_height(-19.0f, -12.0f)},
        {-30.0f, 12.0f, evaluate_terrain_height(35.0f, 15.0f)},
        {-25.0f, -10.0f, evaluate_terrain_height(24.0f, 11.0f)}
	};
}

void scene_structure::initialize_flag()
{
	mesh flag_pole_mesh = mesh_primitive_cylinder(0.02f, {-17.0f, 6.0f, 0.0f}, {-17.0f, 6.0f, 2.0f}, 20, 5, true);
	mesh flag_mesh = mesh_primitive_quadrangle({-17.0f, 6.0f, 1.5f}, {-17.0f, 6.7f, 1.5f}, {-17.0f, 6.7f, 2.0f}, {-17.0f, 6.0f, 2.0f});
	flag_pole.initialize_data_on_gpu(flag_pole_mesh);
	flag.initialize_data_on_gpu(flag_mesh);
	flag.shader.load("shaders/flag/flag.vert.glsl", "shaders/flag/flag.frag.glsl");
	flag_pole.material.color = {0.7f, 0.7f, 0.7f};
	flag.texture.load_and_initialize_texture_2d_on_gpu("assets/epfl.png");
}

void scene_structure::initialize_hole()
{
	mesh hole_mesh = mesh_primitive_disc(0.1f, {-17.0f, 6.0f, 0.03f}, {0, 0, 1}, 60);
	hole.initialize_data_on_gpu(hole_mesh);
	hole.material.color = {0.0f, 0.0f, 0.0f};
}

void scene_structure::initialize_ball()
{
	ball_position = {31.0f, 0.0f, 1.0f};
	ball_velocity = {0.0f, 0.0f, 0.0f};
	mesh ball_mesh = mesh_primitive_sphere(ball_radius);
	ball.initialize_data_on_gpu(ball_mesh);
	ball.material.color = {0.90f, 0.90f, 0.90f};
	ball.model.translation = ball_position;
}

void scene_structure::initialize_arrow()
{
	mesh arrow_mesh = mesh_primitive_cylinder(0.02f, {0, 0, 0}, {0, 0, 1}, 20, 5, false);
	shoot_arrow.initialize_data_on_gpu(arrow_mesh);
	shoot_arrow.material.color = {1, 0, 0};
}