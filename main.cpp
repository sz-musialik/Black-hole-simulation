#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <raylib.h>
#include <vector>

// Constants
const int window_width = 800;
const int window_height = 600;

const double G = 6.6743e-11;
const double c = 299792458.0;

const double SIM_WIDTH_METERS = 1e11;
const double SIM_HEIGHT_METERS =
    SIM_WIDTH_METERS * (static_cast<double>(window_height) / window_width);
const double METERS_TO_PIXELS = window_width / SIM_WIDTH_METERS;

const int FPS = 60;
const double DT = (1.0 / static_cast<float>(FPS)) * 100.0;

// Helper functions
float MetersToPixels(double meters) {
  return static_cast<float>(meters * METERS_TO_PIXELS);
}

double PixelsToMeters(double pixels) {
  return static_cast<double>(pixels) / METERS_TO_PIXELS;
}

Vector2 PositionToPixels(Vector2 position) {
  return Vector2{MetersToPixels(position.x), MetersToPixels(position.y)};
}

class BlackHole {
public:
  Vector2 pos;
  double mass;
  double r_s;

  BlackHole(Vector2 position, double mass) {
    this->pos = position;
    this->mass = mass;
    this->r_s = (2 * G * mass) / (c * c);
  }

  void draw() {
    DrawCircleV(PositionToPixels(pos), MetersToPixels(r_s), BLACK);
  }
};
BlackHole SagA({static_cast<float>(PixelsToMeters(window_width / 2.0)),
                static_cast<float>(PixelsToMeters(window_height / 2.0))},
               8.54e36);

Vector2 PolarToCartesian(double r, double phi) {
  float x = r * cos(phi) + SagA.pos.x;
  float y = r * sin(phi) + SagA.pos.y;

  return {x, y};
}

class Photon {
public:
  // Vector2 pos;
  // Vector2 dir;
  double r;
  double phi;
  double dr;
  double dphi;

  bool is_active;

  bool is_falling;

  std::vector<Vector2> trail;
  double speed;
  double L; // Angular momentum

  Photon(Vector2 pos, Vector2 dir) {
    double dx = pos.x - SagA.pos.x;
    double dy = pos.y - SagA.pos.y;

    this->r = hypot(dx, dy);
    this->phi = atan2(dy, dx);

    this->speed = c;

    this->L = dx * (speed * dir.y) - dy * (speed * dir.x);
    this->is_falling = true;
    this->is_active = true;
  }

  void draw() {
    Vector2 pos = PolarToCartesian(r, phi);

    if (!this->is_active) {
      std::cout << "NOT ACTIVE\n";
    }

    if (trail.size() < 1) {
      // DrawCircleV(PositionToPixels(pos), 1.0f, WHITE);
      return;
    }

    for (size_t i = 0; i < trail.size() - 1; ++i) {
      int alpha = i * 50;
      Color trail_color = {255, 255, 255, static_cast<unsigned char>(alpha)};
      DrawLineV(PositionToPixels(trail[i]), PositionToPixels(trail[i + 1]),
                trail_color);
    }

    DrawLineV(PositionToPixels(trail.back()), PositionToPixels(pos), WHITE);
  }

  void update() {
    if (!this->is_active && trail.size() > 0) {
      trail.erase(trail.begin());
      return;
    }

    if (r < SagA.r_s) {
      this->is_active = false;
      return;
    }

    Vector2 pos = PolarToCartesian(r, phi);

    if (trail.size() > 5) {
      trail.erase(trail.begin());
    }

    trail.push_back(pos);

    dphi = (L / (r * r)) * DT;
    phi += dphi;

    double r_term = ((L * L) / (r * r)) * (1.0 - SagA.r_s / r);
    double argument = c * c - r_term;

    if (argument < 0) {
      if (r > 1.5 * SagA.r_s) {
        is_falling = false;
        dr = 0;
      } else {
        dr = c;
        r -= dr * DT;
      }
    } else {
      dr = sqrt(argument);

      if (is_falling) {
        r -= dr * DT;
      } else {
        r += dr * DT;
      }
    }

    // if (argument <= 0) {
    //   if (r > 1.5 * SagA.r_s) {
    //     is_falling = false;
    //   }
    // }
  }

  void geodesic() {
    dr += r * dphi * dphi - (c * c * SagA.r_s) / (2.0 * r * r);
    dphi = -2.0 * dr * dphi / r;
  }
};

int main() {

  InitWindow(window_width, window_height, "Black Hole");
  SetTargetFPS(FPS);

  std::vector<Photon> photons;
  for (int i = 0; i < 60; ++i) {
    Photon photon({0.0, static_cast<float>(PixelsToMeters(10 * i))}, {1, 0});
    photons.push_back(photon);
  }

  // Simulation loop
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(DARKBLUE);
    DrawFPS(10, 10);

    SagA.draw();

    for (auto &photon : photons) {
      photon.draw();
      photon.update();
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
