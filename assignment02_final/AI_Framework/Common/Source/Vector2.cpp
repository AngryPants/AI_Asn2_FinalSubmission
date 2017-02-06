#include "Vector2.h"

// NOTE : TODO

Vector2::Vector2(float _x, float _y)
{
	Set(_x, _y);
}

Vector2::Vector2(const Vector2& _rhs)
{
	Set(_rhs.x, _rhs.y);
}

bool Vector2::IsEqual(float a, float b) const
{
	return a - b <= Math::EPSILON && b - a <= Math::EPSILON;
}

void Vector2::Set(float _x, float _y)
{
	x = _x;
	y = _y;
}

Vector2 Vector2::operator+( const Vector2& _rhs ) const
{
	return Vector2(x + _rhs.x, y + _rhs.y);
}

Vector2& Vector2::operator+=(const Vector2& _rhs) {
	x += _rhs.x;
	y += _rhs.y;
	return *this;
}

Vector2 Vector2::operator-( const Vector2& _rhs ) const
{
	return Vector2(x - _rhs.x, y - _rhs.y);
}

Vector2& Vector2::operator-=(const Vector2& _rhs) {
	x -= _rhs.x;
	y -= _rhs.y;
	return *this;
}

Vector2 Vector2::operator-( void ) const
{
	return Vector2(-x, -y);
}

Vector2 Vector2::operator*( float _scalar ) const
{
	return Vector2(x*_scalar, y*_scalar);
}

Vector2& Vector2::operator*=(float _scalar) {
	x *= _scalar;
	y *= _scalar;
	return *this;
}

Vector2& Vector2::operator=(const Vector2& _rhs) {
	this->x = _rhs.x;
	this->y = _rhs.y;

	return *this;
}

bool Vector2::operator==(const Vector2& _rhs) const {
	return IsEqual(x, _rhs.x) && IsEqual(y, _rhs.y);
}

bool Vector2::operator!=(const Vector2& _rhs) const {
	return !IsEqual(x, _rhs.x) || !IsEqual(y, _rhs.y);
}

float Vector2::LengthSquared() const {
	return x*x + y*y;
}

float Vector2::Length( void ) const
{
	return sqrt(LengthSquared());
}

float Vector2::Dot( const Vector2& _rhs ) const
{
	return x*_rhs.x + y*_rhs.y;
}

Vector2 Vector2::Normalized( void ) const
{
	float length = Length();
	return Vector2(x / length, y / length);
}

Vector2& Vector2::Normalize() {
	float length = Length();
	x /= length;
	y /= length;
	return *this;
}

Vector2 operator*( float scalar, const Vector2& rhs ) {
	return rhs * scalar;
}

Vector3 Vector2::ChangeToVec3()
{
	return Vector3(this->x, this->y, 0);
}