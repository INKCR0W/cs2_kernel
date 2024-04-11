#pragma once

class Vec2
{
public:
	float x, y;
public:
	Vec2() :x(0.f), y(0.f) {}
	Vec2(float x_, float y_) :x(x_), y(y_) {}
	Vec2 operator+(Vec2 Vec2_)
	{
		return { x + Vec2_.x,y + Vec2_.y };
	}
	Vec2 operator-(Vec2 Vec2_)
	{
		return { x - Vec2_.x,y - Vec2_.y };
	}
	Vec2 operator*(Vec2 Vec2_)
	{
		return { x * Vec2_.x,y * Vec2_.y };
	}
	Vec2 operator/(Vec2 Vec2_)
	{
		return { x / Vec2_.x,y / Vec2_.y };
	}
	Vec2 operator*(float n)
	{
		return { x * n,y * n };
	}
	Vec2 operator/(float n)
	{
		return { x / n,y / n };
	}
	bool operator==(Vec2 Vec2_)
	{
		return x == Vec2_.x && y == Vec2_.y;
	}
	bool operator!=(Vec2 Vec2_)
	{
		return x != Vec2_.x || y != Vec2_.y;
	}
	float Length()
	{
		return sqrtf(powf(x, 2) + powf(y, 2));
	}
	float DistanceTo(const Vec2& Pos)
	{
		return sqrtf(powf(Pos.x - x, 2) + powf(Pos.y - y, 2));
	}
};

class Vec3
{
public:
	float x, y, z;
public:
	Vec3() :x(0.f), y(0.f), z(0.f) {}
	Vec3(float x_, float y_, float z_) :x(x_), y(y_), z(z_) {}
	Vec3 operator+(Vec3 Vec3_)
	{
		return { x + Vec3_.x,y + Vec3_.y,z + Vec3_.z };
	}
	Vec3 operator-(Vec3 Vec3_)
	{
		return { x - Vec3_.x,y - Vec3_.y,z - Vec3_.z };
	}
	Vec3 operator*(Vec3 Vec3_)
	{
		return { x * Vec3_.x,y * Vec3_.y,z * Vec3_.z };
	}
	Vec3 operator/(Vec3 Vec3_)
	{
		return { x / Vec3_.x,y / Vec3_.y,z / Vec3_.z };
	}
	Vec3 operator*(float n)
	{
		return { x * n,y * n,z * n };
	}
	Vec3 operator/(float n)
	{
		return { x / n,y / n,z / n };
	}
	bool operator==(Vec3 Vec3_)
	{
		return x == Vec3_.x && y == Vec3_.y && z == Vec3_.z;
	}
	bool operator!=(Vec3 Vec3_)
	{
		return x != Vec3_.x || y != Vec3_.y || z != Vec3_.z;
	}
	float Length()
	{
		return sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2));
	}
	float DistanceTo(const Vec3& Pos)
	{
		return sqrtf(powf(Pos.x - x, 2) + powf(Pos.y - y, 2) + powf(Pos.z - z, 2));
	}
};

struct BoneJointData
{
	Vec3 Pos;
	char pad[0x14];
};

struct C_UTL_VECTOR
{
	DWORD_PTR count = 0;
	DWORD_PTR data = 0;
};

struct view_matrix_t {
	float* operator[ ](int index) {
		return matrix[index];
	}
	float matrix[4][4];
};