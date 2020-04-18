#pragma once

struct Point
{
	float x;
	float y;
	float z;
};

struct TriAngle
{
	Point vertex1;
	Point color1;
	Point vertex2;
	Point color2;
	Point vertex3;
	Point color3;
};

struct TriAngle_Pos
{
	Point vertex[3];
};



struct TriAngle_Color
{
	float x;
	float y;
	float z;
};

struct TriAngle_Norm
{
	float x;
	float y;
	float z;
};

struct sunVec
{
	float x;
	float y;
	float z;
};





