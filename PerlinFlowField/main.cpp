#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <cmath>
#include <algorithm>

// ============================================================================
// PERLIN NOISE IMPLEMENTATION
// ============================================================================
// Perlin noise is a gradient noise function that produces smooth, natural-looking
// random values. It works by interpolating between random gradients at integer
// lattice points in n-dimensional space.

class PerlinNoise {
private:
    // Permutation table for random gradient selection
    // This table is used to generate consistent pseudo-random values
    std::vector<int> permutation;

    // Fade function: 6t^5 - 15t^4 + 10t^3
    // This provides smooth interpolation with zero first and second derivatives at t=0 and t=1
    // which eliminates visual artifacts at grid boundaries
    float fade(float t) const {
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    }

    // Linear interpolation between a and b using weight t
    float lerp(float t, float a, float b) const {
        return a + t * (b - a);
    }

    // Calculate dot product between gradient vector and distance vector
    // The gradient vectors are chosen from a set of 12 edge vectors of a cube
    // This gives more uniform distribution than random gradients
    float grad(int hash, float x, float y) const {
        int h = hash & 7; // Convert to 0-7
        float u = h < 4 ? x : y;
        float v = h < 4 ? y : x;
        return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
    }

public:
    PerlinNoise(unsigned int seed = 0) {
        // Initialize permutation table with values 0-255
        permutation.resize(512);

        // Fill first 256 values
        for (int i = 0; i < 256; i++) {
            permutation[i] = i;
        }

        // Shuffle using seed
        // Fisher-Yates shuffle for uniform randomization
        std::srand(seed);
        for (int i = 255; i > 0; i--) {
            int j = std::rand() % (i + 1);
            std::swap(permutation[i], permutation[j]);
        }

        // Duplicate to avoid buffer overflow in hash lookups
        for (int i = 0; i < 256; i++) {
            permutation[256 + i] = permutation[i];
        }
    }

    // Main Perlin noise function
    // Returns a value typically in range [-1, 1]
    float noise(float x, float y) const {
        // Find unit grid cell containing point
        int X = static_cast<int>(std::floor(x)) & 255;
        int Y = static_cast<int>(std::floor(y)) & 255;

        // Get relative position within cell (0.0 to 1.0)
        x -= std::floor(x);
        y -= std::floor(y);

        // Compute fade curves for x and y
        float u = fade(x);
        float v = fade(y);

        // Hash coordinates of the 4 square corners
        int A = permutation[X] + Y;
        int B = permutation[X + 1] + Y;

        // Blend results from the 4 corners of the square
        return lerp(v,
                   lerp(u, grad(permutation[A], x, y),
                          grad(permutation[B], x - 1, y)),
                   lerp(u, grad(permutation[A + 1], x, y - 1),
                          grad(permutation[B + 1], x - 1, y - 1)));
    }

    // Fractional Brownian Motion (fBm) - layering multiple octaves of noise
    // Creates more complex, natural-looking patterns by combining noise at different scales
    // Higher octaves = more detail, persistence = how much each octave contributes
    float fbm(float x, float y, int octaves = 4, float persistence = 0.5f) const {
        float total = 0.0f;
        float frequency = 1.0f;
        float amplitude = 1.0f;
        float maxValue = 0.0f;

        for (int i = 0; i < octaves; i++) {
            total += noise(x * frequency, y * frequency) * amplitude;
            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= 2.0f;
        }

        return total / maxValue;
    }
};

// ============================================================================
// PARTICLE SYSTEM FOR FLOW FIELD VISUALIZATION
// ============================================================================
// Particles follow the flow field direction, creating organic flowing lines
// Each particle has a position, velocity, and lifetime

struct Particle {
    Vector2 position;
    Vector2 velocity;
    Vector2 prevPosition;
    float lifetime;      // Current age
    float maxLifetime;   // When to reset
    Color color;

    Particle(float x, float y) {
        position = {x, y};
        prevPosition = position;
        velocity = {0, 0};
        maxLifetime = GetRandomValue(100, 300); // Variable lifetimes create visual interest
        lifetime = GetRandomValue(0, static_cast<int>(maxLifetime));

        // Colorful particles with varied hues
        float hue = GetRandomValue(0, 360);
        color = ColorFromHSV(hue, 0.7f, 0.9f);
    }

    void update(const PerlinNoise& perlin, float time, float noiseScale, float forceStrength, int width, int height) {
        // Sample Perlin noise at particle position
        // Adding time creates animated, evolving flow field
        float noiseX = position.x * noiseScale;
        float noiseY = position.y * noiseScale;
        float noiseTime = time * 0.0005f; // Slow evolution

        // Use fBm for more complex, detailed flow patterns
        float angle = perlin.fbm(noiseX, noiseY + noiseTime, 4, 0.5f) * PI * 4.0f;

        // Convert angle to force vector
        Vector2 force = {std::cos(angle), std::sin(angle)};
        force = Vector2Scale(force, forceStrength);

        // Apply force to velocity (Euler integration)
        velocity = Vector2Add(velocity, force);

        // Limit maximum speed to prevent particles escaping
        float speed = Vector2Length(velocity);
        float maxSpeed = 3.0f;
        if (speed > maxSpeed) {
            velocity = Vector2Scale(Vector2Normalize(velocity), maxSpeed);
        }

        // Apply slight damping for natural deceleration
        velocity = Vector2Scale(velocity, 0.95f);

        // Store previous position for line drawing
        prevPosition = position;

        // Update position
        position = Vector2Add(position, velocity);

        // Age the particle
        lifetime++;

        // Reset if too old or out of bounds
        if (lifetime >= maxLifetime ||
            position.x < 0 || position.x >= width ||
            position.y < 0 || position.y >= height) {
            reset(width, height);
        }
    }

    void reset(int width, int height) {
        position.x = GetRandomValue(0, width);
        position.y = GetRandomValue(0, height);
        prevPosition = position;
        velocity = {0, 0};
        lifetime = 0;

        // New color on reset
        float hue = GetRandomValue(0, 360);
        color = ColorFromHSV(hue, 0.7f, 0.9f);
    }

    void draw() const {
        // Draw line from previous to current position
        // Alpha fades with age for trailing effect
        float alpha = 1.0f - (lifetime / maxLifetime);
        Color drawColor = color;
        drawColor.a = static_cast<unsigned char>(alpha * 150);

        // Draw the trail line
        DrawLineEx(prevPosition, position, 1.5f, drawColor);

        // Optional: draw small circle at current position
        // DrawCircleV(position, 1.5f, drawColor);
    }
};

// ============================================================================
// MAIN PROGRAM
// ============================================================================

int main() {
    // Canvas configuration
    const int canvasWidth = 1200;
    const int canvasHeight = 800;

    InitWindow(canvasWidth, canvasHeight, "Perlin Flow Field - Organic Flowing Lines");
    SetTargetFPS(60);

    // Initialize Perlin noise generator
    // Seed determines the flow field pattern
    PerlinNoise perlin(12345);

    // Flow field parameters
    // These control how the noise translates to particle movement
    float noiseScale = 0.003f;      // Smaller = larger flow features
    float forceStrength = 0.15f;    // Strength of directional forces

    // Create particle system
    const int numParticles = 3000;  // More particles = denser visualization
    std::vector<Particle> particles;
    particles.reserve(numParticles);

    // Initialize particles at random positions
    for (int i = 0; i < numParticles; i++) {
        particles.emplace_back(
            GetRandomValue(0, canvasWidth),
            GetRandomValue(0, canvasHeight)
        );
    }

    float time = 0.0f;
    bool paused = false;
    bool showHelp = true;

    // Main render loop
    while (!WindowShouldClose()) {
        // ===== INPUT HANDLING =====
        if (IsKeyPressed(KEY_SPACE)) {
            paused = !paused;
        }

        if (IsKeyPressed(KEY_R)) {
            // Reset all particles
            for (auto& p : particles) {
                p.reset(canvasWidth, canvasHeight);
            }
        }

        if (IsKeyPressed(KEY_H)) {
            showHelp = !showHelp;
        }

        // Adjust parameters with arrow keys
        if (IsKeyDown(KEY_UP)) {
            forceStrength += 0.001f;
        }
        if (IsKeyDown(KEY_DOWN)) {
            forceStrength = std::max(0.01f, forceStrength - 0.001f);
        }
        if (IsKeyDown(KEY_RIGHT)) {
            noiseScale += 0.0001f;
        }
        if (IsKeyDown(KEY_LEFT)) {
            noiseScale = std::max(0.0001f, noiseScale - 0.0001f);
        }

        // ===== UPDATE =====
        if (!paused) {
            time++;

            // Update all particles
            for (auto& particle : particles) {
                particle.update(perlin, time, noiseScale, forceStrength, canvasWidth, canvasHeight);
            }
        }

        // ===== RENDER =====
        BeginDrawing();

        // Semi-transparent background creates trailing effect
        // Lower alpha = longer trails
        DrawRectangle(0, 0, canvasWidth, canvasHeight, ColorAlpha(BLACK, 0.08f));

        // Draw all particles
        for (const auto& particle : particles) {
            particle.draw();
        }

        // ===== UI =====
        if (showHelp) {
            DrawRectangle(10, 10, 300, 160, ColorAlpha(BLACK, 0.7f));
            DrawText("PERLIN FLOW FIELD", 20, 20, 20, WHITE);
            DrawText("SPACE - Pause/Resume", 20, 50, 16, LIGHTGRAY);
            DrawText("R - Reset particles", 20, 70, 16, LIGHTGRAY);
            DrawText("H - Toggle help", 20, 90, 16, LIGHTGRAY);
            DrawText("UP/DOWN - Force strength", 20, 110, 16, LIGHTGRAY);
            DrawText("LEFT/RIGHT - Noise scale", 20, 130, 16, LIGHTGRAY);

            DrawText(TextFormat("Force: %.3f", forceStrength), 20, 150, 14, YELLOW);
        }

        // FPS counter
        DrawText(TextFormat("FPS: %d", GetFPS()), canvasWidth - 100, 10, 20, GREEN);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
