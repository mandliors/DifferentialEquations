#include "CoordinateSystem.h"

#include <raylib.h>
#include <raymath.h>

#include <vector>

const float CoordinateSystem::UNIT = 1 / 80.0f;
const float CoordinateSystem::STEP_SIZE = 5.0f;

static float Equals(float a, float b);
static Vector2 VecFromSlope(float slope);

CoordinateSystem::CoordinateSystem(int step, float (*function)(float, float))
	: Step(step), OffsetX(0), OffsetY(0), Function(function) { }

void CoordinateSystem::Update()
{
	Vector2 delta = GetMouseDelta();
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
		OffsetX += delta.x;
		OffsetY += delta.y;
	}
	else
		SetMouseCursor(MOUSE_CURSOR_DEFAULT);
}

void CoordinateSystem::Draw()
{
	//points
	int width = GetScreenWidth();
	int height = GetScreenHeight();
	int xMiddle = width / 2 + OffsetX;
	int yMiddle = height / 2 + OffsetY;	
	int xMin = xMiddle - (xMiddle / Step + 1) * Step;
	int yMin = yMiddle - (yMiddle / Step + 1) * Step;

	//axis
	DrawLine(0, yMiddle, width, yMiddle, BLACK);
	DrawLine(xMiddle, 0, xMiddle, height, BLACK);

	//line elements
	for (int x = xMin; x < width + Step; x += Step)
	{
		for (int y = yMin; y < height + Step; y += Step)
		{
			Vector2 screenPoint = Vector2{ (float)x, (float)y };

			float xVal = (x - xMiddle) * 1 / 80.0f;
			float yVal = (yMiddle - y) * 1 / 80.0f;
			Vector2 point = Vector2{ xVal, yVal };

			float deriv = Function(xVal, yVal);
			Vector2 slopeScaledFlipped = Vector2Scale(VecFromSlope(deriv), 6.0f); slopeScaledFlipped.y *= -1;
			DrawLineV(Vector2Add(screenPoint, slopeScaledFlipped), Vector2Subtract(screenPoint, slopeScaledFlipped), GRAY);
		}
	}
}

void CoordinateSystem::DrawIzocline(float K, const Color& color)
{
	//points
	int width = GetScreenWidth();
	int height = GetScreenHeight();
	int xMiddle = width / 2 + OffsetX;
	int yMiddle = height / 2 + OffsetY;
	int xMin = xMiddle - (xMiddle / Step + 1) * Step;
	int yMin = yMiddle - (yMiddle / Step + 1) * Step;

	for (int x = xMin; x < width + Step; x++)
	{
		for (int y = yMin; y < height + Step; y++)
		{
			float xVal = (x - xMiddle) * 1 / 80.0f;
			float yVal = (yMiddle - y) * 1 / 80.0f;

			if (Equals(Function(xVal, yVal), K))
				DrawPixel(x, y, color);
		}
	}
}

void CoordinateSystem::DrawSolutionEuler(float x0, float y0, const Color& color)
{
	//points
	int width = GetScreenWidth();
	int height = GetScreenHeight();
	int xMiddle = width / 2 + OffsetX;
	int yMiddle = height / 2 + OffsetY;
	int xMin = xMiddle - (xMiddle / Step + 1) * Step;
	int yMin = yMiddle - (yMiddle / Step + 1) * Step;

	//calculate points to the left
	std::vector<Vector2> leftPoints; leftPoints.reserve(width / STEP_SIZE + 3);
	float _x0 = (x0 - xMiddle) * UNIT;
	float _y0 = (yMiddle - y0) * UNIT;
	for (int x = x0 - STEP_SIZE; x > -STEP_SIZE; x -= STEP_SIZE)
	{
		float slope = Function(_x0, _y0);
		_x0 -= STEP_SIZE * UNIT;
		_y0 -= STEP_SIZE * UNIT * slope; //subtract because of moving backwards
		leftPoints.push_back(Vector2{ xMiddle + _x0 / UNIT, yMiddle - _y0 / UNIT });
	}
	//calculate points to the right
	std::vector<Vector2> rightPoints; rightPoints.reserve(width / STEP_SIZE + 3);
	_x0 = (x0 - xMiddle) * UNIT;
	_y0 = (yMiddle - y0) * UNIT;
	for (int x = x0; x < width + STEP_SIZE; x += STEP_SIZE)
	{
		float slope = Function(_x0, _y0);
		_x0 += STEP_SIZE * UNIT;
		_y0 += STEP_SIZE * UNIT * slope;
		rightPoints.push_back(Vector2{ xMiddle + _x0 / UNIT, yMiddle - _y0 / UNIT });
	}

	//draw solution
	if (leftPoints.size() < 1 || rightPoints.size() < 1) return;
	for (int i = 0; i < leftPoints.size() - 1; i++)
		DrawLineEx(leftPoints[i], leftPoints[i + 1], 2.0f, color);
	DrawLineEx(leftPoints[0], rightPoints[0], 2.0f, color);
	for (int i = 0; i < rightPoints.size() - 1; i++)
		DrawLineEx(rightPoints[i], rightPoints[i + 1], 2.0f, color);
	DrawCircle(x0, y0, 3.0f, color);
}
void CoordinateSystem::DrawSolutionRungeKutta(float x0, float y0, const Color& color)
{
	//points
	int width = GetScreenWidth();
	int height = GetScreenHeight();
	int xMiddle = width / 2 + OffsetX;
	int yMiddle = height / 2 + OffsetY;
	int xMin = xMiddle - (xMiddle / Step + 1) * Step;
	int yMin = yMiddle - (yMiddle / Step + 1) * Step;
	float k1, k2, k3, k4, slope;

	//calculate points to the left
	std::vector<Vector2> leftPoints; leftPoints.reserve(width / STEP_SIZE + 3);
	float _x0 = (x0 - xMiddle) * UNIT;
	float _y0 = (yMiddle - y0) * UNIT;
	for (int x = x0 - STEP_SIZE; x > -STEP_SIZE; x -= STEP_SIZE)
	{
		k1 = Function(_x0, _y0);
		k2 = Function(_x0 + STEP_SIZE * UNIT * 0.5f, _y0 + k1 * UNIT * 0.5f);
		k3 = Function(_x0 + STEP_SIZE * UNIT * 0.5f, _y0 + k2 * UNIT * 0.5f);
		k4 = Function(_x0 + STEP_SIZE * UNIT, _y0 + k3 * UNIT);
		slope = (k1 + 2 * k2 + 2 * k3 + k4) / 6.0f;
		_x0 -= STEP_SIZE * UNIT;
		_y0 -= STEP_SIZE * UNIT * slope; //subtract because of moving backwards
		leftPoints.push_back(Vector2{ xMiddle + _x0 / UNIT, yMiddle - _y0 / UNIT });
	}
	//calculate points to the right
	std::vector<Vector2> rightPoints; rightPoints.reserve(width / STEP_SIZE + 3);
	_x0 = (x0 - xMiddle) * UNIT;
	_y0 = (yMiddle - y0) * UNIT;
	for (int x = x0; x < width + STEP_SIZE; x += STEP_SIZE)
	{
		k1 = Function(_x0, _y0);
		k2 = Function(_x0 + STEP_SIZE * UNIT * 0.5f, _y0 + k1 * UNIT * 0.5f);
		k3 = Function(_x0 + STEP_SIZE * UNIT * 0.5f, _y0 + k2 * UNIT * 0.5f);
		k4 = Function(_x0 + STEP_SIZE * UNIT, _y0 + k3 * UNIT);
		slope = (k1 + 2 * k2 + 2 * k3 + k4) / 6.0f;
		_x0 += STEP_SIZE * UNIT;
		_y0 += STEP_SIZE * UNIT * slope;
		rightPoints.push_back(Vector2{ xMiddle + _x0 / UNIT, yMiddle - _y0 / UNIT });
	}

	//draw solution
	if (leftPoints.size() < 1 || rightPoints.size() < 1) return;
	for (int i = 0; i < leftPoints.size() - 1; i++)
		DrawLineEx(leftPoints[i], leftPoints[i + 1], 2.0f, color);
	DrawLineEx(leftPoints[0], rightPoints[0], 2.0f, color);
	for (int i = 0; i < rightPoints.size() - 1; i++)
		DrawLineEx(rightPoints[i], rightPoints[i + 1], 2.0f, color);
	DrawCircle(x0, y0, 3.0f, color);
}

static float Equals(float a, float b)
{
	static const float EPS = 0.02f;
	return fabs(a - b) < EPS;
}
static Vector2 VecFromSlope(float slope)
{
	Vector2 dir{ 1.0f, slope };
	return Vector2Normalize(dir);
}
