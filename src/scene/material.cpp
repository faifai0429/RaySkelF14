#include "ray.h"
#include "material.h"
#include "light.h"

vec3f getAmibientLightsIntensity(Scene *scene)
{
	vec3f intensity;
	for (AmbientLight* ambientLight : *(scene->getAmbientLights()))
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
	
	vec3f u = r.getDirection();										//ray direction
	vec3f n = i.N;													//surface normal
	vec3f refl_dir = (u - 2.0 * (u.dot(i.N)) * i.N).normalize();	//reflection direction
	vec3f isect_pos = r.at(i.t);									//intersection point

	vec3f color = ke + ka.cross(getAmibientLightsIntensity(scene)).clamp();
	vec3f intensity, atten, diffuse, specular;

	for (Light* light : *(scene->getLights())) {
		intensity = light->getColor(isect_pos);
		atten = light->distanceAttenuation(isect_pos) * light->shadowAttenuation(isect_pos);
		diffuse = kd.cross(intensity * std::max<double>(0, i.N.dot(light->getDirection(isect_pos))));
		specular = ks.cross(intensity * pow(refl_dir.dot(-u), shininess * 128));
		color = color + atten.cross(diffuse.clamp() + specular.clamp());
	}

	return color.clamp();
}
