#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <windows.h>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "vec.h"
#include "satellite.h"

// ─── Constants ────────────────────────────────────────────────────────────────
const double G     = 6.674e-11;
const double M     = 5.972e24;
const double DT    = 60.0;
const double SCALE = 1.0 / 6371000.0;

// ─── Satellite colors ─────────────────────────────────────────────────────────
const glm::vec3 SAT_COLORS[] = {
    {0.2f,  0.85f, 1.0f },
    {1.0f,  0.52f, 0.1f },
    {0.35f, 1.0f,  0.45f},
    {1.0f,  0.28f, 0.52f},
    {1.0f,  0.95f, 0.2f },
    {0.75f, 0.38f, 1.0f },
    {1.0f,  0.78f, 0.35f},
    {0.35f, 0.85f, 0.65f},
};
const int NUM_COLORS = 8;

// ─── Per-satellite runtime state ──────────────────────────────────────────────
struct SatEntry {
    Satellite              sat;
    std::vector<glm::vec3> trail;
    glm::vec3              color;
    bool                   visible    = true;
    double                 trailTimer = 0.0;
};

std::vector<SatEntry> satellites;

// ─── Time / camera ────────────────────────────────────────────────────────────
double lastTime        = 0.0;
double timeAccumulator = 0.0;

float  camYaw   = 0.0f, camPitch = 20.0f, camDist = 4.0f;
double lastMouseX, lastMouseY;
bool   mouseDown  = false;
int    simSpeed   = 20;
int    focusedSat = -1;

// ─── Physics ──────────────────────────────────────────────────────────────────
vec acceleration(const vec& pos) {
    double r = pos.length();
    return pos * (-G * M / (r * r * r));
}

void stepRK4(Satellite& s, double dt) {
    vec k1v = acceleration(s.pos),               k1x = s.vel;
    vec k2v = acceleration(s.pos + k1x*(dt/2)),  k2x = s.vel + k1v*(dt/2);
    vec k3v = acceleration(s.pos + k2x*(dt/2)),  k3x = s.vel + k2v*(dt/2);
    vec k4v = acceleration(s.pos + k3x*dt),      k4x = s.vel + k3v*dt;
    s.pos += (k1x + k2x*2 + k3x*2 + k4x) * (dt/6);
    s.vel += (k1v + k2v*2 + k3v*2 + k4v) * (dt/6);
    s.updatePower(dt);
    s.updateTemp(dt);
}

// ─── Input helpers ────────────────────────────────────────────────────────────
double promptDouble(const char* label, double def) {
    printf("    %-28s [%.1f] : ", label, def);
    fflush(stdout);
    std::cin.clear();
    std::string line;
    std::getline(std::cin, line);
    if (line.empty()) return def;
    try { return std::stod(line); } catch (...) { return def; }
}

std::string promptStr(const char* label, const char* def) {
    printf("    %-28s [%s] : ", label, def);
    fflush(stdout);
    std::cin.clear();
    std::string line;
    std::getline(std::cin, line);
    return line.empty() ? std::string(def) : line;
}

// ─── Satellite factory ────────────────────────────────────────────────────────
SatEntry makeSatEntry(const std::string& name, double altKm, double incDeg, double raanDeg, double mass, double radiusM, 
    double powerGen, double batCap, double powerUsage, double absorptivity, double emissivity, int index)
{
    double r    = altKm * 1000.0 + 6371000.0;
    double v    = sqrt(G * M / r);
    double inc  = glm::radians(incDeg);
    double raan = glm::radians(raanDeg);

    vec pos(r * cos(raan), 0.0, r * sin(raan));
    vec vel(-v * sin(raan) * cos(inc),
        v * sin(inc),
        v * cos(raan) * cos(inc));

    double surfaceArea = 4.0 * glm::pi<double>() * radiusM * radiusM;
    ThermalModel thermal(mass, surfaceArea, absorptivity, emissivity, 900.0);

    Satellite sat(name, pos, vel, powerGen, batCap, powerUsage);
    sat.setThermal(thermal);

    return SatEntry{ sat, {}, SAT_COLORS[index % NUM_COLORS], true, 0.0 };
}

// ─── GLFW callbacks ───────────────────────────────────────────────────────────
void mouseButtonCB(GLFWwindow*, int button, int action, int) {
    if (button == GLFW_MOUSE_BUTTON_LEFT)
        mouseDown = (action == GLFW_PRESS);
}
void cursorCB(GLFWwindow*, double x, double y) {
    double dx = x - lastMouseX;
    double dy = y - lastMouseY;
    if (dx >  50.0) dx =  50.0;
    if (dx < -50.0) dx = -50.0;
    if (dy >  50.0) dy =  50.0;
    if (dy < -50.0) dy = -50.0;
    lastMouseX = x; lastMouseY = y;
    if (!mouseDown) return;
    camYaw   += (float)dx * 0.3f;
    camPitch += (float)dy * 0.3f;
    if (camPitch >  89.0f) camPitch =  89.0f;
    if (camPitch < -89.0f) camPitch = -89.0f;
}

void scrollCB(GLFWwindow*, double, double dy) {
    camDist -= (float)dy * 0.2f;
    if (camDist < 1.5f)  camDist = 1.5f;
    if (camDist > 20.0f) camDist = 20.0f;
}

void keyCB(GLFWwindow*, int key, int, int action, int) {
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
    if (key == GLFW_KEY_EQUAL) { simSpeed += 5; if (simSpeed > 100) simSpeed = 100; }
    if (key == GLFW_KEY_MINUS) { simSpeed -= 5; if (simSpeed <   0) simSpeed =   0; }
    if (key == GLFW_KEY_R)     simSpeed = 20;

    if (key >= GLFW_KEY_1 && key <= GLFW_KEY_8) {
        int idx = key - GLFW_KEY_1;
        if (idx < (int)satellites.size())
            satellites[idx].visible = !satellites[idx].visible;
    }
    if (key == GLFW_KEY_TAB) {
        int n = (int)satellites.size();
        if (n > 0)
            focusedSat = (focusedSat + 2) % (n + 1) - 1;
    }
}

// ─── Draw helpers ─────────────────────────────────────────────────────────────
void drawSphere(float radius, int slices, int stacks) {
    for (int i = 0; i < stacks; ++i) {
        float lat0 = glm::pi<float>() * (-0.5f + (float)i       / stacks);
        float lat1 = glm::pi<float>() * (-0.5f + (float)(i + 1) / stacks);
        float z0 = sinf(lat0), z1 = sinf(lat1);
        float r0 = cosf(lat0), r1 = cosf(lat1);
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float lng = 2.0f * glm::pi<float>() * (float)j / slices;
            float cx = cosf(lng), sy = sinf(lng);
            glVertex3f(radius*cx*r0, radius*sy*r0, radius*z0);
            glVertex3f(radius*cx*r1, radius*sy*r1, radius*z1);
        }
        glEnd();
    }
}

void drawTrail(const std::vector<glm::vec3>& trail, const glm::vec3& col) {
    if (trail.size() < 2) return;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.0f, -1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.0f);

    bool loop = (trail.size() >= 590);
    glBegin(loop ? GL_LINE_LOOP : GL_LINE_STRIP);
    for (size_t i = 0; i < trail.size(); ++i) {
        float alpha = 0.25f + 0.75f * (float)i / trail.size();
        glColor4f(col.r, col.g, col.b, alpha);
        glVertex3f(trail[i].x, trail[i].y, trail[i].z);
    }
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_POLYGON_OFFSET_LINE);
}

void drawEarthMarkings() {
    const float R      = 1.002f;
    const int   SEGS   = 128;
    const float TWO_PI = 2.0f * glm::pi<float>();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-2.0f, -2.0f);

    // Equator
    glLineWidth(2.0f);
    glColor4f(1.0f, 0.9f, 0.3f, 0.85f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= SEGS; ++i) {
        float a = TWO_PI * (float)i / SEGS;
        glVertex3f(R * cosf(a), 0.0f, R * sinf(a));
    }
    glEnd();

    // Prime meridian
    glLineWidth(1.0f);
    glColor4f(1.0f, 0.9f, 0.3f, 0.3f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= SEGS; ++i) {
        float lat = glm::pi<float>() * (-0.5f + (float)i / SEGS);
        glVertex3f(R * cosf(lat), R * sinf(lat), 0.0f);
    }
    glEnd();

    // North pole
    glLineWidth(2.5f);
    glColor4f(0.55f, 0.85f, 1.0f, 0.95f);
    {
        float arm = 0.07f, py = R;
        glBegin(GL_LINES);
        glVertex3f(-arm, py, 0.0f); glVertex3f( arm, py, 0.0f);
        glVertex3f(0.0f, py, -arm); glVertex3f(0.0f, py,  arm);
        glEnd();
        glPointSize(6.0f);
        glBegin(GL_POINTS);
        glVertex3f(0.0f, py, 0.0f);
        glEnd();
    }

    // South pole
    glLineWidth(2.0f);
    glColor4f(0.55f, 0.85f, 1.0f, 0.7f);
    {
        float arm = 0.06f, py = -R;
        glBegin(GL_LINES);
        glVertex3f(-arm, py, 0.0f); glVertex3f( arm, py, 0.0f);
        glVertex3f(0.0f, py, -arm); glVertex3f(0.0f, py,  arm);
        glEnd();
        glPointSize(5.0f);
        glBegin(GL_POINTS);
        glVertex3f(0.0f, py, 0.0f);
        glEnd();
    }

    glDisable(GL_BLEND);
    glDisable(GL_POLYGON_OFFSET_LINE);
}

void drawSatMarker(const SatEntry& e) {
    float t          = (float)glm::clamp((e.sat.temp - 200.0) / 200.0, 0.0, 1.0);
    float brightness = e.sat.inEclipse ? 0.4f : 1.0f;
    glColor3f(e.color.r * brightness * (1.0f - t * 0.5f),
        e.color.g * brightness * (1.0f - t * 0.3f),
        e.color.b * brightness);
    glPushMatrix();
    glTranslatef((float)(e.sat.pos[0]*SCALE),
        (float)(e.sat.pos[1]*SCALE),
        (float)(e.sat.pos[2]*SCALE));
    drawSphere(0.025f, 10, 10);
    glPopMatrix();
}

// ─── Telemetry ────────────────────────────────────────────────────────────────
void printTelemetry(double simMult) {
    int n = (int)satellites.size();
    printf("\033[%dA", n + 2);

    printf("  Speed: %6.0fx   [+/-] change   [R] reset   [Tab] focus   [1-%d] toggle\n",
        simMult, n);
    printf("  %-3s %-12s %11s %11s %16s %13s %10s %13s\n",
        "   ", "Name", "Altitude", "Speed", "Battery", "Temp", "Status", "Surf Area");

    for (int i = 0; i < n; ++i) {
        const auto& e  = satellites[i];
        const auto& s  = e.sat;
        double alt     = (s.pos.length() - 6371000.0) / 1000.0;
        double spd     = s.vel.length() / 1000.0;
        double sa      = s.thermal.surfaceArea;
        glm::vec3 c    = e.color * 255.0f;
        char focused   = (focusedSat == i) ? '>' : ' ';
        char vis       = e.visible ? '*' : ' ';
        const char* status = s.inEclipse ? "ECLIPSE" : "SUNLIT ";
        printf("  %c[%d]%c \033[38;2;%d;%d;%dm%-12s\033[0m  %8.1f km  %7.2f km/s  %7.1f/%7.1f Wh  %6.1f K  %s  %7.2f m2\n",
            focused, i + 1, vis,
            (int)c.r, (int)c.g, (int)c.b,
            s.name.c_str(), alt, spd,
            s.power, s.BatCap,
            s.temp, status, sa);
    }
    fflush(stdout);
}

// ─── Main ─────────────────────────────────────────────────────────────────────
int main() {
    // Enable ANSI escape codes on Windows
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD  mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    printf("\n");
    printf("  ╔══════════════════════════════════════════╗\n");
    printf("  ║         ORBIT SIMULATOR SETUP            ║\n");
    printf("  ║   Press Enter to accept default values   ║\n");
    printf("  ╚══════════════════════════════════════════╝\n\n");

    int count = 1;
    printf("  How many satellites? [1] : ");
    fflush(stdout);
    {
        std::string line;
        std::getline(std::cin, line);
        if (!line.empty()) {
            try { count = std::stoi(line); } catch (...) {}
        }
        count = glm::clamp(count, 1, 16);
    }
    printf("\n");

    for (int i = 0; i < count; ++i) {
        glm::vec3 c = SAT_COLORS[i % NUM_COLORS] * 255.0f;
        printf("  \033[38;2;%d;%d;%dm── Satellite %d of %d ──────────────────────\033[0m\n",
            (int)c.r, (int)c.g, (int)c.b, i + 1, count);

        char defName[16];
        snprintf(defName, sizeof(defName), "SAT-%d", i + 1);

        // ── Orbital parameters ──
        printf("  Orbital parameters:\n");
        std::string name = promptStr   ("  Name",                defName);
        double      alt  = promptDouble("  Altitude (km)",        400.0);
        double      inc  = promptDouble("  Inclination (deg)",     28.5);
        double      raan = promptDouble("  RAAN (deg)",             0.0);

        // ── Physical properties ──
        printf("  Physical properties:\n");
        double mass   = promptDouble("  Mass (kg)",              500.0);
        double radius = promptDouble("  Radius (m)",               1.0);

        // ── Power system ──
        printf("  Power system:\n");
        double powerGen   = promptDouble("  Solar power gen (W)",   200.0);
        double batCap     = promptDouble("  Battery capacity (Wh)", 1000.0);
        double powerUsage = promptDouble("  Power usage (W)",        100.0);

        // ── Thermal properties ──
        printf("  Thermal properties:\n");
        double absorpt  = promptDouble("  Absorptivity (0-1)",      0.3);
        double emissiv  = promptDouble("  Emissivity (0-1)",         0.8);

        satellites.push_back(makeSatEntry(
            name, alt, inc, raan,
            mass, radius,
            powerGen, batCap, powerUsage,
            absorpt, emissiv,
            i));
        printf("\n");
    }

    printf("  All satellites configured — launching simulation...\n\n");
    for (int i = 0; i < (int)satellites.size() + 2; ++i) printf("\n");

    // ── GLFW / GLEW ──────────────────────────────────────────────────────────
    if (!glfwInit()) return 1;
    GLFWwindow* window = glfwCreateWindow(900, 900, "Orbit Simulator", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    glewInit();

    glfwSetMouseButtonCallback(window, mouseButtonCB);
    glfwSetCursorPosCallback  (window, cursorCB);
    glfwSetScrollCallback     (window, scrollCB);
    glfwSetKeyCallback        (window, keyCB);

    glEnable(GL_DEPTH_TEST);
    lastTime = glfwGetTime();

    // ── Main loop ─────────────────────────────────────────────────────────────
    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        double dt  = glm::clamp(now - lastTime, 0.0, 0.1);
        lastTime   = now;

        double simMult = (simSpeed == 0) ? 0.0
            : 60.0 * pow(480.0, simSpeed / 100.0);

        timeAccumulator += dt * simMult;

        int steps = 0;
        while (timeAccumulator >= DT && steps < 300) {
            for (auto& e : satellites) stepRK4(e.sat, DT);
            timeAccumulator -= DT;
            ++steps;
        }

        // Trail sampling
        for (auto& e : satellites) {
            if (!e.visible) continue;
            e.trailTimer += dt;
            if (e.trailTimer >= 0.05) {
                e.trailTimer = 0.0;
                glm::vec3 p(e.sat.pos[0]*SCALE,
                    e.sat.pos[1]*SCALE,
                    e.sat.pos[2]*SCALE);
                if (e.trail.empty() || glm::distance(p, e.trail.back()) > 0.0001f)
                    e.trail.push_back(p);
                if (e.trail.size() > 1200)
                    e.trail.erase(e.trail.begin(), e.trail.begin() + 200);
            }
        }

        // Camera
        glm::vec3 lookAt(0.0f);
        if (focusedSat >= 0 && focusedSat < (int)satellites.size()) {
            auto& p = satellites[focusedSat].sat.pos;
            lookAt  = glm::vec3(p[0]*SCALE, p[1]*SCALE, p[2]*SCALE);
        }

        glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.01f, 100.0f);
        glm::vec3 eye  = lookAt + glm::vec3(
            camDist * cosf(glm::radians(camPitch)) * sinf(glm::radians(camYaw)),
            camDist * sinf(glm::radians(camPitch)),
            camDist * cosf(glm::radians(camPitch)) * cosf(glm::radians(camYaw))
        );
        glm::mat4 view = glm::lookAt(eye, lookAt, glm::vec3(0, 1, 0));

        // Render
        glClearColor(0.02f, 0.02f, 0.07f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION); glLoadMatrixf(glm::value_ptr(proj));
        glMatrixMode(GL_MODELVIEW);  glLoadMatrixf(glm::value_ptr(view));

        glColor3f(0.05f, 0.33f, 0.82f);
        drawSphere(1.0f, 64, 64);
        drawEarthMarkings();

        for (const auto& e : satellites) {
            if (!e.visible) continue;
            drawTrail(e.trail, e.color);
            drawSatMarker(e);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        printTelemetry(simMult);
    }

    glfwTerminate();
    return 0;
}
