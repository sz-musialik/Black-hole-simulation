#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <random>
#include <raylib.h>
#include <vector>
#include "raymath.h"

// Randomness
static std::random_device rd;
static std::mt19937 gen(rd());

static std::normal_distribution<double> mass_dist(0.0, 100);

// Constants
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 800;

const double G = 6.6743e-11;
const double c = 299792458.0;
const double LY_TO_METERS = 9.4607e15; // 1 Light year in meters
const double SOLAR_RADIUS = 6.9634e8;
const double SOLAR_MASS = 1.989e30;

const double SIM_WIDTH_LY = 1;
const double SIM_HEIGHT_LY = 1;
const double SIM_DEPTH_LY = 1;

static Camera3D camera = {.position = {-1.0f, 0.0f, 0.0f},
                          .target = {0.0f, 0.0f, 0.0f},
                          .up = {0.0f, 1.0f, 0.0f},
                          .fovy = 60.0f,
                          .projection = CAMERA_PERSPECTIVE};

const int FPS = 60;
double DT;

// float RadiusMetersToLy(double radius_m) {
//   double radius_ratio = radius_m / SOLAR_RADIUS;
//   float base_ly = 1;
//
//   return base_ly * static_cast<float>(pow(radius_ratio, 0.4));
// }

// Helper functions
// float MetersToPixels(double meters) {
//   return static_cast<float>(meters * METERS_TO_PIXELS);
// }
//
// double PixelsToMeters(double pixels) {
//   return static_cast<double>(pixels) / METERS_TO_PIXELS;
// }
//
// Vector2 PositionToPixels(Vector2 position) {
//   return Vector2{MetersToPixels(position.x), MetersToPixels(position.y)};
// }
double GetRandomFloat(float min, float max) {
  std::uniform_real_distribution<float> dist(min, max);
  return dist(gen);
};

class BlackHole {
public:
  Vector3 pos;
  double mass; // in kg
  double r_s;  // in meters

  BlackHole(double mass) {
    pos = {0.0f, 0.0f, 0.0f};

    this->mass = mass;
    r_s = (2 * G * mass) / (c * c);
  }

  void draw() { DrawSphere(pos, r_s / LY_TO_METERS, BLACK); }
};

// Ton618* Black Hole object
BlackHole Ton618(1.31e41);

class Star {
public:
  Vector3 pos;
  Vector3 vel;
  Vector3 acc; // acceleration
  Vector3 F;   // Force

  double mass;   // in kg
  double radius; // in meters
  Color color;
  bool active;

  Star() {
    // Random position in light years
    pos.x = GetRandomFloat(-SIM_WIDTH_LY, SIM_WIDTH_LY);
    pos.y = GetRandomFloat(-SIM_HEIGHT_LY, SIM_HEIGHT_LY);
    pos.z = GetRandomFloat(-SIM_DEPTH_LY, SIM_DEPTH_LY);

    // Random velocity in m/s
    vel.x = GetRandomFloat(-2000, 2000);
    vel.y = GetRandomFloat(-2000, 2000);
    vel.z = GetRandomFloat(-2000, 2000);

    acc = {0, 0, 0};
    F = {0, 0, 0};

    InitializeMassRadius();

    color = GetStarColor();

    active = true;
  }

  Matrix GetTransformMatrix() const {
    float radius_ly = radius / LY_TO_METERS;
    Matrix scale = MatrixScale(radius_ly, radius_ly, radius_ly);
    Matrix translation = MatrixTranslate(pos.x, pos.y, pos.z);

    return MatrixMultiply(scale, translation);
  }

  void ResetForce() {
    F.x = 0;
    F.y = 0;
    F.z = 0;
  }

  void Update(double sim_dt) {
    if (!active)
      return;

    dt = sim_dt;

    UpdateAcceleration();
    UpdateVelocity();
    UpdatePosition();
  }

private:
  float dt;

  void InitializeMassRadius() {
    // double m_ratio = mass_dist(gen);
    // if (m_ratio < 0.1)
    //   m_ratio = 0.1;
    //
    // mass = m_ratio * SOLAR_MASS;
    //
    // if (mass < SOLAR_MASS) {
    //   radius = SOLAR_RADIUS * std::pow(m_ratio, 0.9);
    // } else {
    //   radius = SOLAR_RADIUS * std::pow(m_ratio, 0.6);
    // }
		mass = SOLAR_MASS * 10;
		radius = (2 * G * mass) / (c * c);
  }

  Color GetStarColor() {
    // Will eventually return/modify a color associated to its fields

    return WHITE;
  }

  void UpdateVelocity() {
    if (!active)
      return;

    vel.x = vel.x + acc.x * dt;
    vel.y = vel.y + acc.y * dt;
    vel.z = vel.z + acc.z * dt;
  }

  void UpdateAcceleration() {
    if (!active)
      return;

    acc.x = F.x / mass;
    acc.y = F.y / mass;
    acc.z = F.z / mass;
  }

  void UpdatePosition() {
    if (!active)
      return;

    pos.x += (vel.x * dt) / LY_TO_METERS;
    pos.y += (vel.y * dt) / LY_TO_METERS;
    pos.z += (vel.z * dt) / LY_TO_METERS;
  }
};

void RenderStarsInstanced(const std::vector<Star> &stars, Mesh sphereMesh,
                          Material sphereMaterial, int colorLoc) {
  // static std::vector<Matrix> whiteStars;
  //
  // whiteStars.clear();
  // whiteStars.reserve(stars.size());
  //
  // for (const auto &star : stars) {
  //   if (star.active) {
  //     Matrix tx = star.GetTransformMatrix();
  //
  // 	whiteStars.push_back(tx);
  //   }
  // }
  //
  // if (!whiteStars.empty()) {
  //   float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  //   SetShaderValue(sphereMaterial.shader, colorLoc, color, SHADER_UNIFORM_VEC4);
  //   DrawMeshInstanced(sphereMesh, sphereMaterial, whiteStars.data(), whiteStars.size());
  // }
	for (const auto& star : stars) {
		// std::cout << "Pozycja: [x: " << star.pos.x << ", " << star.pos.y << ", " << star.pos.y << "], promien: " << star.radius / LY_TO_METERS << "\n";
		// std::cout << "Pozycja: [x: " << Ton618.pos.x << ", " << Ton618.pos.y << ", " << Ton618.pos.y << "], promien: " << Ton618.r_s / LY_TO_METERS << "\n";
		DrawSphere(star.pos, 0.015, star.color);
	}
	// std::cout << "\n\nKLATKA\n\n";
}









Vector3 SphericalToCartesian(double r_meters, double phi, double theta) {
  float r_ly = r_meters / LY_TO_METERS;

  // Projection of r
  float r_p = static_cast<float>(r_ly * sin(theta));

  float x = static_cast<float>(r_p * cos(phi));
  float y = static_cast<float>(r_p * sin(phi));
  float z = static_cast<float>(r_ly * cos(theta));

  return {x, y, z};
}

class Photon {
public:
  double r;
  double phi;
  double theta;
  double vr;
  double vphi;
  double vtheta;

  struct Deriv {
    double r_vel;			// radial velocity
    double theta_vel;	// polar velocity
    double phi_vel;		// azimutal angular velocity
    double r_acc;			// radial acceleration
    double theta_acc; // polar acceleration
    double phi_acc;   // azimutal angular acceleratrion
  };

  bool is_active;

  std::vector<Vector3> trail;
  double speed;

  Photon(Vector3 pos, Vector3 dir) {
    // Camera3D .up = {0.0f, 1.0f, 0.0f}
    // Switching roles of Y and Z might be necessary in the future
    r = hypot(pos.x, std::hypot(pos.y, pos.z)) * LY_TO_METERS;
    phi = atan2(pos.y, pos.x);
    theta = acos(pos.z / (r / LY_TO_METERS));

    speed = c;

    dir = Vector3Normalize(dir);
    double vx = c * dir.x;
    double vy = c * dir.y;
    double vz = c * dir.z;

    vr 	= vx * sin(theta) * cos(phi)
		+ vy * sin(theta) * sin(phi)
		+ vz * cos(theta);

    vphi = (-vx * sin(phi) + vy * cos(phi)) / (r * sin(theta));

    vtheta	= vx * cos(theta) * cos(phi)
			+ vy * cos(theta) * sin(phi)
			+ vz * -sin(theta);

    is_active = true;
  }

  Deriv GetDerivs(double curr_r, double curr_r_vel, double curr_phi_vel, double curr_theta_vel) {
    Deriv d;
    d.r_vel = curr_r_vel;
    d.theta_vel = curr_theta_vel;
    d.phi_vel = curr_phi_vel;

    d.r_acc = (curr_r - 1.5 * Ton618.r_s)
						* (curr_theta_vel * curr_theta_vel + curr_phi_vel * curr_phi_vel * sin(theta) * sin(theta));

    d.theta_acc = (curr_phi_vel * curr_phi_vel * sin(theta) * cos(theta))
								+ (curr_r_vel * curr_theta_vel * -2.0) / curr_r;

    d.phi_acc = (-2.0 * curr_r_vel * curr_phi_vel / curr_r)
							+ (curr_theta_vel * curr_phi_vel * -2.0 * (1.0 / tan(theta)));

    return d;
  }

  void draw() {
    Vector3 pos = SphericalToCartesian(r, phi, theta);

    if (trail.size() < 1) {
      // DrawCircleV(PositionToPixels(pos), 1.0f, WHITE);
      return;
    }

    if (trail.size() > 10) {
      trail.erase(trail.begin());
    }

    for (size_t i = 0; i < trail.size() - 1; ++i) {
      int alpha = i * 50;
      Color trail_color = {255, 255, 255, static_cast<unsigned char>(alpha)};
      DrawLine3D(trail[i], trail[i + 1], trail_color);
    }

    DrawLine3D(trail.back(), pos, WHITE);
  }

  void RK4(double dt) {
    if (!is_active) {
      if (!trail.empty()) {
        trail.erase(trail.begin());
      }

      return;
    }

    if (r < Ton618.r_s) {
      is_active = false;
      return;
    }

    Deriv k1 = GetDerivs(r, vr, vphi, vtheta);
    Deriv k2 = GetDerivs(r + 0.5 * dt * k1.r_vel, vr + 0.5 * dt * k1.r_acc, vphi + 0.5 * dt * k1.phi_acc, vtheta + 0.5 * dt * k1.theta_acc);
    Deriv k3 = GetDerivs(r + 0.5 * dt * k2.r_vel, vr + 0.5 * dt * k2.r_acc, vphi + 0.5 * dt * k2.phi_acc, vtheta + 0.5 * dt * k2.theta_acc);
    Deriv k4 = GetDerivs(r + dt * k3.r_vel, vr + dt * k3.r_acc, vphi + dt * k3.phi_acc, vtheta + dt * k3.theta_acc);

    r 	+= (dt * (k1.r_vel + 2.0f * k2.r_vel + 2.0f * k3.r_vel + k4.r_vel)) / 6;
    phi += (dt * (k1.phi_acc + 2.0f * k2.phi_acc + 2.0f * k3.phi_acc + k4.phi_acc)) / 6;
    vr 	+= (dt * (k1.r_acc + 2.0f * k2.r_acc + 2.0f * k3.r_acc + k4.r_acc)) / 6;
    vphi += (dt * (k1.phi_acc + 2.0f * k2.phi_acc + 2.0f * k3.phi_acc + k4.phi_acc)) / 6;
    vtheta += (dt * (k1.theta_acc + 2.0f * k2.theta_acc + 2.0f * k3.theta_acc + k4.theta_acc)) / 6;

    Vector3 curr_pos = SphericalToCartesian(r, phi, theta);
    trail.push_back(curr_pos);
  }
};

int main() {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Black Hole");
  SetTargetFPS(FPS);

  DisableCursor();

  std::vector<Photon> photons;
  for (int i = 0; i < 10000; ++i) {
    Photon photon({-0.2f, static_cast<float>(0.0 + 0.00001 * i)}, {1, 0});
    photons.push_back(photon);
  }

  unsigned int star_amount = 500;

  std::vector<Star> stars;
  for (size_t i = 0; i < star_amount; ++i) {
    Star star;
    stars.push_back(star);
  }

	// Shader setup
  Mesh sphereMesh = GenMeshSphere(1.0f, 8, 8);

  Shader lightingShader = LoadShader("lighting.vert", "lighting.frag");

  // nullptr uses default raylib vertex shader
  Shader bloomShader = LoadShader(nullptr, "bloom.frag");

  RenderTexture2D target = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);

  lightingShader.locs[SHADER_LOC_MATRIX_MODEL] =
      GetShaderLocationAttrib(lightingShader, "instanceTransform");

  int instanceColorLoc = GetShaderLocation(lightingShader, "u_color");

  Material sphereMaterial = LoadMaterialDefault();
  sphereMaterial.shader = lightingShader;
  sphereMaterial.maps[MATERIAL_MAP_DIFFUSE].color = WHITE;

  // Simulation loop
  while (!WindowShouldClose()) {
    double dt = GetFrameTime();

    // Prevents stutters
    if (dt > 0.1)
      dt = 0.1;

    double time_scale = 0.01 * 365.0 * 24.0 * 3600.0;
    double sim_dt = dt * time_scale;


		// Photon path
    for (auto &photon : photons) {
      photon.RK4(sim_dt);
    }

    // Force reset
    for (Star &star : stars) {
      star.ResetForce();
    }

    // Acceleration, velocity and position update
    for (Star &star : stars) {
      star.Update(sim_dt);
    }

    // UpdateCamera(&camera, CAMERA_ORBITAL);
    UpdateCamera(&camera, CAMERA_FIRST_PERSON);

    BeginTextureMode(target);
    ClearBackground(GetColor(0x000309FF));

    BeginMode3D(camera);
    RenderStarsInstanced(stars, sphereMesh, sphereMaterial, instanceColorLoc);
    Ton618.draw();

    for (auto &photon : photons) {
      photon.draw();
    }

    DrawCubeWiresV({0.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 2.0f}, GetColor(0xFFFFFF22));

    EndMode3D();
    EndTextureMode();

    // Screen rendering
    BeginDrawing();
    ClearBackground(BLACK);

    BeginShaderMode(bloomShader);

    // OpenGL's texture are flipped vertically hence -height
    DrawTextureRec(target.texture, Rectangle{0, 0, static_cast<float>(target.texture.width),
									 static_cast<float>(-target.texture.height)}, Vector2{0, 0}, WHITE);

    EndShaderMode();

    DrawFPS(10, 10);

    EndDrawing();
  }

  // Unloading shaders and meshes
  UnloadMesh(sphereMesh);
  UnloadShader(lightingShader);

  UnloadRenderTexture(target);
  UnloadShader(bloomShader);

  CloseWindow();
  return 0;
}
