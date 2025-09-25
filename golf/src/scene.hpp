#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"


// This definitions allow to use the structures: mesh, mesh_drawable, etc. without mentionning explicitly cgp::
using cgp::mesh;
using cgp::mesh_drawable;
using cgp::vec3;
using cgp::numarray;
using cgp::timer_basic;

// Variables associated to the GUI
struct gui_parameters {
	bool display_frame = false;
	bool display_wireframe = false;
};

// The structure of the custom scene
struct scene_structure : cgp::scene_inputs_generic {
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	//camera_controller_orbit_euler camera_control;
	camera_projection_perspective camera_projection;
	window_structure window;

	mesh_drawable global_frame;          // The standard global frame
	environment_structure environment;   // Standard environment controler
	input_devices inputs;                // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;                  // Standard GUI element storage
	

	camera_controller_orbit_euler camera_control; // Camera controler for 2D displacement (no rotation, no zoom)
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //

	timer_basic timer;

	cgp::skybox_drawable skybox;

	cgp::mesh_drawable terrain;

	cgp::mesh_drawable water;

	cgp::mesh_drawable circle;
	cgp::mesh_drawable tree;
	std::vector<cgp::vec3> tree_position;
	void display_trees();


	cgp::mesh_drawable grass;
	std::vector<cgp::vec3> grass_position;
	void display_grass();


	cgp::mesh_drawable trunk;
	cgp::mesh_drawable branches;
	cgp::mesh_drawable foliage;

	cgp::mesh_drawable flag_pole;
	cgp::mesh_drawable flag;
	cgp::mesh_drawable hole;
	cgp::mesh_drawable ball;


	

	vec3 ball_position;
	vec3 ball_velocity;
	vec3 g = {0.0f, 0.0f, -9.81f};
	float ball_radius = 0.05f;
	float restitution = 0.5f;
	float air_friction = 0.995f;
	float ground_friction;
	bool ball_is_stopped = false;


	bool follow_ball_orbit = true;
	float orbit_radius = 4.0f;        // distance horizontale à la balle
	float orbit_height = 2.0f;        // hauteur au-dessus de la balle
	float orbit_angle = 0.0f;         // angle sur le cercle autour de la balle (en radians)
	float orbit_rotation_speed = 0.1f;

	float shoot_theta = Pi / 2.0f; // angle par rapport à z (0 = vers le haut, pi/2 = horizontal)
	float shoot_phi = Pi;        // angle azimutal dans le plan x-y
	float shoot_speed = 5.0f;
	mesh_drawable shoot_arrow;
	float delta_angle = 0.02f;
	float delta_speed = 0.1f;

	enum class ClubType { Iron7, Wedge, Putter };
	ClubType current_club = ClubType::Iron7;

	int shoot_number = 0;
	bool show_goal_message = false;
	float goal_message_timer = 0.0f;


	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();    // Standard initialization to be called before the animation loop
	void initialize_camera();
	void initialize_skybox();
	void initialize_terrain();
	void initialize_water();
	void initialize_circle();
	void initialize_grass();
	void initialize_trees();
	void initialize_flag();
	void initialize_hole();
	void initialize_ball();
	void initialize_arrow();


	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop


	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();
	void update_ball(float dt);

	void display_info();
};





