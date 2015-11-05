#include "ray.h"
#include "material.h"
#include "light.h"

vec3f getAmibientLightsIntensity(Scene *scene)
{
	vec3f intensity;
	for (AmbientLight* ambientLight : scene->getAmbientLights())
	{
		intensity += ambientLight->getColor();
	}
	return intensity;
}

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.
	
	
	const vec3f& isect_pos = r.at(i.t);									//intersection point
	vec3f color = prod(ka, getAmibientLightsIntensity(scene));

	for (Light* light : scene->getLights()) {
		const vec3f& u = light->getDirection(isect_pos);					//ray direction
		const vec3f& n = i.N;												//surface normal
		const vec3f& shadowAtten = light->shadowAttenuation(isect_pos);
		if (u.dot(i.N) > 0.0 && !shadowAtten.iszero()) {
			const vec3f& refl_dir = (u - 2.0 * (u.dot(i.N)) * i.N).normalize();	//reflection direction
			const vec3f& intensity = light->getColor(isect_pos);
			const vec3f& atten = light->distanceAttenuation(isect_pos) * shadowAtten;
			const vec3f& diffuse = kd * i.N.dot(u);
			const vec3f& specular = ks * pow(refl_dir.dot(-r.getDirection()), shininess * 128);
			color += prod(prod(atten, intensity), diffuse + specular);
		}
	}

	return color;
}
