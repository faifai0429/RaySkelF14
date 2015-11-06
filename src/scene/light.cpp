#include <cmath>

#include "light.h"
#include "../ui/TraceUI.h"

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.

	const vec3f &dir = getDirection(P);
	vec3f result(1.0, 1.0, 1.0);
	// push the point outwards a bit so that the ray won't hit itself
	vec3f point = P + dir * RAY_EPSILON;
	while (!result.iszero())
	{
		isect i;
		ray shadow_ray(point, dir);
		if (!scene->intersect(shadow_ray, i))
		{
			return result;
		}

		result = prod(result, i.getMaterial().kt);
		// slightly push the point forward to prevent hitting itself
		point = shadow_ray.at(i.t) + dir * RAY_EPSILON;
	}
	return result;
}

vec3f DirectionalLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection( const vec3f& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0
	const double d2 = (position - P).length_squared();
	const double d = sqrt(d2);

	const double divisor = TraceUI::getInstance()->getAttenConstant() + TraceUI::getInstance()->getAttenLinear() * d + TraceUI::getInstance()->getAttenQuadratic() * d2;
	
	return (divisor == 0.0) ? 1.0 : 1.0 / std::max<double>(1.0, divisor);
}

vec3f PointLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 

	return color;
}

vec3f PointLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	//push the isect point outward a bit
	vec3f result(1.0, 1.0, 1.0);
	// push the point outwards a bit so that the ray won't hit itself
	const vec3f &dir = getDirection(P);
	vec3f point = P + dir * RAY_EPSILON;
	while (!result.iszero())
	{
		isect i;
		ray shadow_ray(point, dir);
		const double light_t = (position - point).length();
		if (!scene->intersect(shadow_ray, i) || i.t >= light_t)
		{
			// if no intersection or the object is behind the light
			return result;
		}

		result = prod(result, i.getMaterial().kt);
		// slightly push the point forward to prevent hitting itself
		point = shadow_ray.at(i.t) + dir * RAY_EPSILON;
	}
	return result;
}

vec3f AmbientLight::getColor() const
{
	return color;
}