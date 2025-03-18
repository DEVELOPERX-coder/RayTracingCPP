#include <stdio.h>
#include <math.h>
#include "./src/include/SDL3/SDL.h"

#define WIDTH 900
#define HEIGHT 600
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLACK 0x00000000
#define COLOR_GRAY 0xefefefef
#define RAYS_NUMBER 500

struct Circle
{
    double x;
    double y;
    double radius;
};

struct Ray
{
    double x_Start, y_Start;
    double angle;
};

void FillCircle(SDL_Surface *surface, struct Circle circle, Uint32 color)
{
    double radius_square = pow(circle.radius, 2);
    for (double x = circle.x - circle.radius; x <= circle.x + circle.radius; ++x)
    {
        for (double y = circle.y - circle.radius; y <= circle.y + circle.radius; ++y)
        {
            double distance_squared = pow(x - circle.x, 2) + pow(y - circle.y, 2);
            if (distance_squared < radius_square)
            {
                SDL_Rect pixel = (SDL_Rect){static_cast<int>(x), static_cast<int>(y), 1, 1};
                SDL_FillSurfaceRect(surface, &pixel, color);
            }
        }
    }
}

void generate_rays(struct Circle circle, struct Ray rays[RAYS_NUMBER])
{
    for (int i = 0; i < RAYS_NUMBER; ++i)
    {
        double angle = ((double)i / RAYS_NUMBER) * 2 * M_PI;
        struct Ray ray = {circle.x, circle.y, angle};
        rays[i] = ray;
        // printf("angle: %f\n", angle);
    }
}

void FillRays(SDL_Surface *surface, struct Ray rays[RAYS_NUMBER], Uint32 color, struct Circle object)
{
    double radius_square = pow(object.radius, 2);
    for (int i = 0; i < RAYS_NUMBER; ++i)
    {
        struct Ray ray = rays[i];

        int end_of_screen = 0;
        int object_hit = 0;

        double step = 1;
        double x_draw = ray.x_Start;
        double y_draw = ray.y_Start;

        while (!end_of_screen && !object_hit)
        {
            x_draw += step * cos(ray.angle);
            y_draw += step * sin(ray.angle);

            SDL_Rect pixel = (SDL_Rect){static_cast<int>(x_draw), static_cast<int>(y_draw), 1, 1};
            SDL_FillSurfaceRect(surface, &pixel, color);

            if (x_draw < 0 || x_draw > WIDTH)
                end_of_screen = 1;
            if (y_draw < 0 || y_draw > HEIGHT)
                end_of_screen = 1;

            double distance_squared = pow(x_draw - object.x, 2) + pow(y_draw - object.y, 2);
            if (distance_squared < radius_square)
            {
                break;
            }
        }
    }
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("RayTracing", WIDTH, HEIGHT, 0);

    SDL_Surface *surface = SDL_GetWindowSurface(window);

    struct Circle circle = {200, 200, 80};
    struct Circle shadow_circle = {650, 300, 140};
    SDL_Rect erase_rect = (SDL_Rect){0, 0, WIDTH, HEIGHT};

    struct Ray rays[RAYS_NUMBER];
    generate_rays(circle, rays);

    int simulation_running = 1;
    SDL_Event event;
    while (simulation_running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                simulation_running = 0;
            }
            if (event.type == SDL_EVENT_MOUSE_MOTION && event.motion.state != 0)
            {
                circle.x = event.motion.x;
                circle.y = event.motion.y;
                generate_rays(circle, rays);
            }
        }
        SDL_FillSurfaceRect(surface, &erase_rect, COLOR_BLACK);
        FillCircle(surface, circle, COLOR_WHITE);

        FillCircle(surface, shadow_circle, COLOR_WHITE);
        FillRays(surface, rays, COLOR_GRAY, shadow_circle);

        SDL_UpdateWindowSurface(window);
        SDL_Delay(10);
    }
}