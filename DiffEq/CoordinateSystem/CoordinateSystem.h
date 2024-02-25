#pragma once

struct Color;

class CoordinateSystem
{
public:
	CoordinateSystem(int step, float (*function)(float, float));

	void Update();
	void Draw();
	void DrawIzocline(float K, const Color& color);
	void DrawSolutionEuler(float x0, float y0, const Color& color);
	void DrawSolutionRungeKutta(float x0, float y0, const Color& color);

private:
	int Step;
	int OffsetX, OffsetY;
	float (*Function)(float, float);

private:
	static const float UNIT;
	static const float STEP_SIZE;
};