#include <cmath>

#include "light.h"

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
    return vec3f(1,1,1);
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
	const double EPS = std::numeric_limits<double>::epsilon();
	const double d2 = (position - P).length_squared();
	const double d = sqrt(d2);
	const double constant_attenuation_coeff = 0.25;
	const double linear_attenuation_coeff = 0.003372407;
	const double quadratic_attenuation_coeff = 0.000045492;
	const double divisor = constant_attenuation_coeff + linear_attenuation_coeff * d + quadratic_attenuation_coeff * d2;
	return (divisor < EPS) ? 1.0 : std::max<double>(1.0, 1.0 / divisor);
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
	
	const double EPS = std::numeric_limits<double>::epsilon();
	ray r(P + getDirection(P) * EPS, getDirection(P));			//push the isect point outward a bit
	isect i;

	if (scene->intersect(r, i)) {
		const double light_t = (position - P).length();
		if (i.t < light_t) {
			return vec3f(0.0, 0.0, 0.0);
		}
		else {
			return vec3f(1.0, 1.0, 1.0);
		}
	}

	return vec3f(0.0, 0.0, 0.0);
}
