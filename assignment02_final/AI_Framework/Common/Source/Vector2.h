#ifndef VECTOR2_H
#define VECTOR2_H


#include "Vector3.h"
#include "MyMath.h"

struct Vector2
{
	float x, y;

	bool IsEqual(float a, float b) const;
	Vector2(float _x = 0, float _y = 0); //default constructor
	Vector2(const Vector2& _rhs); //copy constructor
	void Set(float _x, float _y); //Set all data
	Vector2 operator+(const Vector2& _rhs) const; //Vector addition
	Vector2& operator+=(const Vector2& _rhs); //Vector addition
	Vector2 operator-(const Vector2& _rhs) const; //Vector subtraction
	Vector2& operator-=(const Vector2& _rhs); //Vector subtraction
	Vector2 operator-(void) const; //Unary negation
	Vector2 operator*(float _scalar) const; //Scalar multiplication
	Vector2& operator*=(float _scalar); //Scalar multiplication
	Vector2& operator=(const Vector2& _rhs);
	bool operator==(const Vector2& _rhs) const;
	bool operator!=(const Vector2& _rhs) const;
	float Length(void) const; //Get magnitude
	float LengthSquared() const;
	float Dot(const Vector2& _rhs) const; //Dot product
	Vector2 Normalized(void) const; //Return a copy of this vector, normalized
	Vector2& Normalize();
	Vector3 ChangeToVec3();
	

	friend Vector2 operator*( float scalar, const Vector2& rhs );
};
#endif