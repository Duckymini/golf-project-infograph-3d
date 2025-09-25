#include "scene.hpp"
#include "terrain.hpp"
#include "tree.hpp"

using namespace cgp;




void scene_structure::initialize()
{
	// Initialisations
	display_info();
	initialize_camera();
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());
	initialize_skybox();
	initialize_terrain();
	initialize_water();
	initialize_circle();
	initialize_grass();
	initialize_trees();
	initialize_flag();
	initialize_hole();
	initialize_ball();
	initialize_arrow();
}



void scene_structure::display_frame()
{
	environment.uniform_generic.uniform_float["time"] = timer.t;
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();

	if (gui.display_frame)
		draw(global_frame, environment);

	glDepthMask(GL_FALSE);
	draw(skybox, environment);
	glDepthMask(GL_TRUE);

	draw(terrain, environment);
	if (gui.display_wireframe)
		draw_wireframe(terrain, environment);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	draw(water, environment);
	glDisable(GL_BLEND);
	if (gui.display_wireframe)
		draw_wireframe(water, environment);
	
	draw(hole, environment);
	if (gui.display_wireframe) 
		draw_wireframe(hole, environment);
	draw(circle, environment); 
	if (gui.display_wireframe) 
		draw_wireframe(circle, environment);
	draw(flag_pole, environment);
	if (gui.display_wireframe) 
		draw_wireframe(flag_pole, environment);
	draw(flag, environment);
	if (gui.display_wireframe) 
		draw_wireframe(flag, environment);
	draw(ball, environment);
	if (gui.display_wireframe) 
		draw_wireframe(ball, environment);

	display_trees();
	display_grass();

	// Affichage de la flèche en cas de balle arrêtée
	if (ball_is_stopped) {
		vec3 dir = {
			shoot_speed * std::sin(shoot_theta) * std::cos(shoot_phi),
			shoot_speed * std::sin(shoot_theta) * std::sin(shoot_phi),
			shoot_speed * std::cos(shoot_theta)
		};
		vec3 base = ball_position + vec3{0, 0, ball_radius}; // départ au-dessus de la balle

		shoot_arrow.model.translation = base;
		shoot_arrow.model.rotation = rotation_transform::from_vector_transform({0, 0, 1}, normalize(dir));
		float alpha = (shoot_speed - 1.0f) / (30.0f - 1.0f);
		alpha = clamp(alpha, 0.0f, 1.0f);
		vec3 color = (1 - alpha) * vec3{0, 0, 1} + alpha * vec3{1, 0, 0};
		shoot_arrow.material.color = color;

		draw(shoot_arrow, environment);
	}
}

void scene_structure::display_trees()
{
	vec3 const offset = { 0,0,0.05f };
	for (size_t k = 0; k < tree_position.size(); ++k) {
		trunk.model.translation = tree_position[k] - offset;
		branches.model.translation = tree_position[k] - offset;
		foliage.model.translation = tree_position[k] - offset;
		draw(trunk, environment);
		draw(branches, environment);
		draw(foliage, environment);
	}
}


void scene_structure::display_grass()
{
	auto const& camera = camera_control.camera_model;
	vec3 const right = camera.right();
	rotation_transform R = rotation_transform::from_frame_transform({ 1,0,0 }, { 0,0,1 }, right, { 0,0,1 });
	grass.model.rotation = R;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	vec3 const offset = { 0,0,0.02f };
	for (vec3 position : grass_position) {
		grass.model.translation = position - offset;
		draw(grass, environment);
	}

	glDisable(GL_BLEND);
	if (gui.display_wireframe) {
		for (vec3 position : grass_position) {
			grass.model.translation = position - offset;
			draw_wireframe(grass, environment);
		}
	}
}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
	std::string club_name;
	switch (current_club) {
		case ClubType::Iron7: club_name = "Iron 7"; break;
		case ClubType::Wedge: club_name = "Wedge"; break;
		case ClubType::Putter: club_name = "Putter"; break;
	}
	ImGui::Text("Club: %s", club_name.c_str());
	ImGui::Text("Shoot speed: %.2f", shoot_speed);
    ImGui::Text("Shoot theta (degrees): %.2f", 90-shoot_theta * 180.0f / Pi);
	ImGui::Text("Ball position:");
	ImGui::Text("x = %.2f", ball_position.x);
	ImGui::Text("y = %.2f", ball_position.y);
	ImGui::Text("z = %.2f", ball_position.z);
	ImGui::Text("Shoot number : %d", shoot_number);
	if (show_goal_message) {
		ImGui::SetNextWindowPos(ImVec2(300, 50), ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.7f); // transparence
		ImGui::Begin("GoalMessage", nullptr,
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoInputs |
			ImGuiWindowFlags_NoSavedSettings);
		
		ImGui::TextColored(ImVec4(1, 0.8f, 0, 1), "Ball in the hole!");
		ImGui::End();
	}
}

void scene_structure::mouse_move_event()
{
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move(environment.camera_view);
}
void scene_structure::mouse_click_event()
{
	camera_control.action_mouse_click(environment.camera_view);
}
void scene_structure::keyboard_event()
{
	camera_control.action_keyboard(environment.camera_view);
}

void scene_structure::update_ball(float dt)
{
    if (ball_is_stopped)
        return;

    // Mouvement de la balle
    ball_velocity += g * dt;
    ball_position += ball_velocity * dt;

    // Infos du terrain
    float terrain_height = evaluate_terrain_height(ball_position.x, ball_position.y);
    vec3 p_ground = { ball_position.x, ball_position.y, terrain_height };
    vec3 terrain_normal = evaluate_terrain_normal(ball_position.x, ball_position.y);

    vec3 delta_p = ball_position - p_ground;
    float distance_along_normal = dot(delta_p, terrain_normal);
    bool on_ground = (distance_along_normal < ball_radius + 0.001f);

	// Cas ou la balle 'touche' le sol
    if (on_ground)
    {
        float penetration = ball_radius - distance_along_normal;
        ball_position += penetration * terrain_normal;

        // Rebond vertical
        float v_n = dot(ball_velocity, terrain_normal);
        if (v_n < 0)
            ball_velocity -= (1.0f + restitution) * v_n * terrain_normal;

        // Frottement tangent
        vec3 v_normal_component = dot(ball_velocity, terrain_normal) * terrain_normal;
        vec3 v_tangent = ball_velocity - v_normal_component;
        float ground_friction = 0.8f;
		// Frottements diminués si la balle est sur le green
		if (is_on_green(ball_position.xy())) ground_friction = 0.97f;
        v_tangent *= ground_friction;
        ball_velocity = v_normal_component + v_tangent;

        // Test d'arrêt
        float total_speed = norm(ball_velocity);
        float tangent_speed = norm(v_tangent);
        if (total_speed < 0.15f && tangent_speed < 0.15f && std::abs(v_n) < 0.15f)
        {
            ball_velocity = {0, 0, 0};
            ball_is_stopped = true;
        }
    }
	// Cas où la balle est dans l'air
    else
    {
        // Frottement dans l'air
        ball_velocity *= 0.995f;
    }

    // Hors limites
	if (ball_position.z < -0.6f || std::abs(ball_position.x) > 40.0f || std::abs(ball_position.y) > 15.0f) {
		ball_position = {34.0f, 0.0f, 1.0f};
		ball_velocity = {0.0f, 0.0f, 0.0f};
	}

	// Infos sur le trou
	vec2 hole_position = {-17.0f, 6.0f};
	float hole_radius = 0.1f;
	float dist_to_hole = norm(vec2{ball_position.x, ball_position.y} - hole_position);
	float speed = norm(ball_velocity);

	// Cas où la balle rentre dans le trou
	if (dist_to_hole < hole_radius && speed < 1.0f) {
		std::cout << "Ball in the hole! Respawning..." << std::endl;
		ball_position = {34.0f, 0.0f, 1.0f}; // position de spawn
		ball_velocity = {0.0f, 0.0f, 0.0f};
		shoot_number = 0;
		show_goal_message = true;
		goal_message_timer = 2.0f;
	}

	ball.model.translation = ball_position;
}
	

void scene_structure::idle_frame()
{
	camera_control.idle_frame(environment.camera_view);
	float dt = timer.update();

	update_ball(dt);

	// Binds
	if (inputs.keyboard.is_pressed(GLFW_KEY_D))  shoot_phi -= delta_angle;
	if (inputs.keyboard.is_pressed(GLFW_KEY_A)) shoot_phi += delta_angle;
	if (inputs.keyboard.is_pressed(GLFW_KEY_W))    shoot_theta -= delta_angle;
	if (inputs.keyboard.is_pressed(GLFW_KEY_S))  shoot_theta += delta_angle;
	if (inputs.keyboard.is_pressed(GLFW_KEY_Q)) shoot_speed += delta_speed; 
	if (inputs.keyboard.is_pressed(GLFW_KEY_E)) shoot_speed -= delta_speed;
	if (inputs.keyboard.is_pressed(GLFW_KEY_1)) current_club = ClubType::Iron7;
	if (inputs.keyboard.is_pressed(GLFW_KEY_2)) current_club = ClubType::Wedge;
	if (inputs.keyboard.is_pressed(GLFW_KEY_3)) current_club = ClubType::Putter;

	// Valeurs minimales et maximales pour les paramètres de tir en fonction du club
	float theta_min, theta_max, speed_min, speed_max;
	switch (current_club) {
		case ClubType::Iron7:
			theta_min = cgp::Pi / 6.0f;       // 30°
			theta_max = cgp::Pi / 3.0f;       // 60°
			speed_min = 10.0f;
			speed_max = 30.0f;
			break;
		case ClubType::Wedge:
			theta_min = cgp::Pi / 18.0f;       // 50°
			theta_max = cgp::Pi / 6.0f; // 80°
			speed_min = 5.0f;
			speed_max = 20.0f;
			break;
		case ClubType::Putter:
			theta_min = theta_max = cgp::Pi / 2.0f; // 90°, visée horizontale
			speed_min = 1.0f;
			speed_max = 10.0f;
			break;
	}
	shoot_theta = clamp(shoot_theta, theta_min, theta_max);
	shoot_speed = clamp(shoot_speed, speed_min, speed_max);
	shoot_phi = fmod(shoot_phi + 2 * Pi, 2 * Pi);

	// Tir
	if (inputs.keyboard.is_pressed(GLFW_KEY_SPACE) && ball_is_stopped){
		vec3 dir = {
			shoot_speed * std::sin(shoot_theta) * std::cos(shoot_phi),
			shoot_speed * std::sin(shoot_theta) * std::sin(shoot_phi),
			shoot_speed * std::cos(shoot_theta)
		};
		ball_velocity = dir;
		ball_is_stopped = false;
		shoot_number++;
	}

	// Suivi de la balle
	if (inputs.keyboard.is_pressed(GLFW_KEY_C)) follow_ball_orbit = !follow_ball_orbit;
	if (follow_ball_orbit) camera_control.camera_model.center_of_rotation = ball_position;
	
	// Balle dans le trou
	if (show_goal_message) {
		goal_message_timer -= dt;
		if (goal_message_timer <= 0.0f) {
			show_goal_message = false;
		}
	}

}

void scene_structure::display_info()
{
	std::cout << "\nCAMERA CONTROL:" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << camera_control.doc_usage() << std::endl;
	std::cout << "-----------------------------------------------\n" << std::endl;
}
