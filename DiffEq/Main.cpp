#include <raylib.h>

#include "CoordinateSystem/CoordinateSystem.h"

float f(float x0, float y0)
{
	return x0 - y0 * y0;
}

int main()
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(800, 600, "Differential equations");
	SetTargetFPS(60);

	CoordinateSystem cs(20, f);

	while (!WindowShouldClose())
	{
		BeginDrawing();

			cs.Update();

			ClearBackground(RAYWHITE);
			cs.Draw();
			cs.DrawIzocline(0, BLUE);			
			cs.DrawSolutionEuler(GetMouseX(), GetMouseY(), GREEN);
			cs.DrawSolutionRungeKutta(GetMouseX(), GetMouseY(), RED);

		EndDrawing();
	}

	return 0;
}