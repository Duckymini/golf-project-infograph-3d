
#include "terrain.hpp"


using namespace cgp;

// Evaluate 3D position of the terrain for any (x,y)

float smoothstep(float edge0, float edge1, float x) {
    x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return x * x * (3.0f - 2.0f * x);
}

float evaluate_terrain_height(float x, float y)
{

      
    //vec2 p_i[4] = { {-10,-10}, {5,5}, {-3,4}, {6,4} };
    std::array<vec2, 13> p_i = {vec2{20.0f, 10.0f},vec2{16.0f, 8.0f},vec2{13.2f, 7.6f},vec2{8.4f, 8.8f},vec2{3.6f, 6.0f},vec2{-0.8f, 7.6f},
            vec2{-6.0f, 7.6f},vec2{-10.8f, 8.8f},vec2{18.0f, -8.8f},vec2{12.8f, -7.6f},vec2{8.4f, -8.4f},vec2{6.0f, -8.8f},vec2{-16.0f, -10.0f}
            };
    float h_i[13] = {1.6f, 0.8f, 1.2f, 0.8f, 1.6f, 0.8f, 1.6f, 0.6f, 1.2f, 0.8f, 1.2f, 0.8f, -3.0f};
    float sigma_i[13] = {4.4f, 2.4f, 2.0f, 2.8f, 3.2f, 1.6f, 2.4f, 2.0f,2.8f, 2.0f, 2.4f, 1.6f, 8.0f};

    float z = 0.0f;
    for (int k = 0; k < 13; ++k)
    {
        float d = norm(vec2(x, y) - p_i[k]) / sigma_i[k];
        z += h_i[k] * std::exp(-(d * d));
        float fade = 0.5f * (1.0f + std::cos(Pi * clamp(std::abs(y) / 10.0f, 0.0f, 1.0f)));
        z += 0.2f * noise_perlin({ x/10.0f, y/10.0f }, 4, 0.20f, 1.5f) * (1-fade);
    }

    const float circle_radius = 4.0f;
    const float influence_radius = 8.0f;
    vec2 circle_center = {-15.0f, 5.0f};
    float dist_to_circle = std::sqrt((x - circle_center.x)*(x - circle_center.x) + (y - circle_center.y)*(y - circle_center.y));
    float smooth_factor = smoothstep(circle_radius, influence_radius, dist_to_circle);

    if (dist_to_circle <= circle_radius) {
        z = 0.0f;
    } else if (dist_to_circle <= influence_radius) {
        z *= smooth_factor;
    }

    return z;
    
}

cgp::vec3 evaluate_terrain_normal(float x, float y)
{
    float dx = 0.1f;
    float dy = 0.1f;

    // Approximation des dérivées partielles par différences centrées
    float hL = evaluate_terrain_height(x - dx, y); // gauche
    float hR = evaluate_terrain_height(x + dx, y); // droite
    float hD = evaluate_terrain_height(x, y - dy); // bas
    float hU = evaluate_terrain_height(x, y + dy); // haut

    float dzdx = (hR - hL) / (2.0f * dx);
    float dzdy = (hU - hD) / (2.0f * dy);

    // Calcul de la normale (non normalisée d'abord)
    vec3 n = normalize(vec3{-dzdx, -dzdy, 1.0f});
    return n;
}



mesh create_terrain_mesh(int N, float terrain_length_x, float terrain_length_y)
{

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N*N);
    terrain.uv.resize(N*N);

    // Fill terrain geometry
    for(int ku=0; ku<N; ++ku)
    {
        for(int kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            float u = ku/(N-1.0f);
            float v = kv/(N-1.0f);

            // Compute the real coordinates (x,y) of the terrain in [-terrain_length/2, +terrain_length/2]
            float x = (u - 0.5f) * terrain_length_x;
            float y = (v - 0.5f) * terrain_length_y;

            // Compute the surface height function at the given sampled coordinate
            float z = evaluate_terrain_height(x,y);

            // Store vertex coordinates
            terrain.position[kv+N*ku] = {x,y,z};
            terrain.uv[kv + N * ku] = { 10*u, 10*v };
        }
    }

    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for(int ku=0; ku<N-1; ++ku)
    {
        for(int kv=0; kv<N-1; ++kv)
        {
            unsigned int idx = kv + N*ku; // current vertex offset

            uint3 triangle_1 = {idx, idx+1+N, idx+1};
            uint3 triangle_2 = {idx, idx+N, idx+1+N};

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    // need to call this function to fill the other buffer with default values (normal, color, etc)
	terrain.fill_empty_field(); 

    return terrain;
}

mesh create_sea_mesh(int N, float terrain_length_x, float terrain_length_y)
{

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N*N);
    terrain.uv.resize(N*N);

    // Fill terrain geometry
    for(int ku=0; ku<N; ++ku)
    {
        for(int kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            float u = ku/(N-1.0f);
            float v = kv/(N-1.0f);

            // Compute the real coordinates (x,y) of the terrain in [-terrain_length/2, +terrain_length/2]
            float x = (u - 0.5f) * terrain_length_x;
            float y = (v - 0.5f) * terrain_length_y;

            // Compute the surface height function at the given sampled coordinate
            float z = 0.1f * noise_perlin({ x/2.5f, y/2.5f }, 6, 0.50f, 1.5f);

            // Store vertex coordinates
            terrain.position[kv+N*ku] = {x,y,z};
            terrain.uv[kv + N * ku] = { 10*u, 10*v };
        }
    }

    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for(int ku=0; ku<N-1; ++ku)
    {
        for(int kv=0; kv<N-1; ++kv)
        {
            unsigned int idx = kv + N*ku; // current vertex offset

            uint3 triangle_1 = {idx, idx+1+N, idx+1};
            uint3 triangle_2 = {idx, idx+N, idx+1+N};

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    // need to call this function to fill the other buffer with default values (normal, color, etc)
	terrain.fill_empty_field(); 

    return terrain;
}

std::vector<vec3> generate_positions_on_terrain(int N, float terrain_length_x, float terrain_length_y)
{
    std::vector<cgp::vec3> position;
    for (int k = 0; k < N; ++k)
    {
        float const x = rand_uniform(-terrain_length_x / 2, terrain_length_x / 2);
        float const y = rand_uniform(-terrain_length_y / 2, terrain_length_y / 2);

        vec3 const p = { x, y, evaluate_terrain_height(x,y) };

        bool to_add = true;
        if (p[2] < 0.75f)// Avoid positions below the terrain
            to_add = false;
        for (int k2 = 0; to_add==true && k2 < position.size(); ++k2) {
            vec3 const& p2 = position[k2];
            if(norm(p2-p)<0.6f)
                to_add=false;
        }
        if(to_add)
            position.push_back(p);
    }
    return position;
}

bool is_on_green(vec2 const& pos)
{
    vec2 hole_pos = vec2{ -15.0f, 5.0f }; // ou une valeur fixe
    float green_radius = 4.0f; // rayon du green (en mètres)
    return norm(pos - hole_pos) < green_radius;
}