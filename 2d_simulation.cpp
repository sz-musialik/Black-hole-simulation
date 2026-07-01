#include <cmath>
#include <cstddef>
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
  Vector2 position;
  double mass;

  double r_s;

  BlackHole(Vector2 position, double mass) {
    this->position = position;
    this->mass = mass;
    this->r_s = (2 * G * mass) / (c * c);
  }

  void draw() {
    DrawCircleV(PositionToPixels(position), MetersToPixels(r_s), BLACK);
  }
};
BlackHole SagA({static_cast<float>(PixelsToMeters(window_width / 2.0)),
                static_cast<float>(PixelsToMeters(window_height / 2.0))},
               8.54e36);

class LightRay {
public:
  Vector2 pos;

  double r, phi;

  struct TailPoint {
    Vector2 point_pos;
    Color color;
  };

  std::vector<TailPoint> tail;

  void updateTail() {
    double x_rel = pos.x - SagA.position.x;
    double y_rel = pos.y - SagA.position.y;

    r = hypot(x_rel, y_rel);
    if (r < SagA.r_s) {
      return;
    }

    float factor = 0.01f;

    for (size_t i = 0; i < tail.size(); ++i) {
      int n = tail.size() - 1;

      float alpha = 1.0f - factor * (n - i);

      if (alpha < 0.0f)
        alpha = 0.0f;

      tail[i].color = ColorAlpha(WHITE, alpha);
    }

    if (tail.size() > 10) {
      tail.erase(tail.begin() + 1);
      std::cout << "REMOVED: \n";
    }

    tail.push_back({pos, WHITE});
  }

  void drawTail() {
    for (auto &point : tail) {
      DrawCircleV(PositionToPixels(point.point_pos), 2.0f, point.color);
    }
  }

  // Constructor
  LightRay(Vector2 position) {
    this->pos = position;
    // std::cout << " x: " << position.x << " y: " << position.y;

    double x_rel = pos.x - SagA.position.x;
    double y_rel = pos.y - SagA.position.y;
    r = hypot(x_rel, y_rel);
    phi = atan2(y_rel, x_rel);
  }

  void update() {
    drawTail();

    // std::cout << "SIZE: " << tail.size() << "\n";
    std::cout << "Upadting tail" << "\n";
    updateTail();
    pos.x += c * DT;
  }
};

int main() {

  LightRay light(
      {0.0, static_cast<float>(PixelsToMeters(window_height / 2.0))});

  InitWindow(window_width, window_height, "Black Hole");
  SetTargetFPS(FPS);

  // Simulation loop
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(DARKBLUE);
    DrawFPS(10, 10);

    std::cout << "TEST\n";

    SagA.draw();

    light.update();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
