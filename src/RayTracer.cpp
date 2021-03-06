// The main ray tracer.

#include <Fl/fl_ask.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include "ui/TraceUI.h"

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
    ray r( vec3f(0,0,0), vec3f(0,0,0) );
    scene->getCamera()->rayThrough( x,y,r );
	material_stack.clear();
	const Material air;
	material_stack.push_back(&air);
	return traceRay( scene, r, vec3f(1.0,1.0,1.0), 0 ).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth )
{
	isect i;
	const double maxDepth = TraceUI::getInstance()->getDepth();

	if (scene->intersect(r, i)) {
		// YOUR CODE HERE
		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.

		const Material& m = i.getMaterial();
		if (isLeavingObject(&m)) {
			i.N = -i.N;								// Normal has to be flipped when leaving object
		}

		vec3f result = m.shade(scene, r, i);

		if (depth < maxDepth) {
			//reflection
			const vec3f& u = r.getDirection();										//ray direction
			const vec3f& refl_dir = (u - 2.0 * u.dot(i.N) * i.N).normalize();		//reflection direction
			const vec3f& isect_pos = r.at(i.t);										//intersection point
			//new ray, push forward a bit to avoid intersect itself
			const ray refl_r(isect_pos + refl_dir * RAY_EPSILON, refl_dir);
			const vec3f& refl_contri = prod(m.kr, traceRay(scene, refl_r, thresh, depth + 1));
			result = result + refl_contri;

			//refraction
			if (!m.kt.iszero()) {
				double eta;
				if (isLeavingObject(&m)) {
					material_stack.pop_front();
					eta = m.index / material_stack.front()->index; 			// refraction index ratio
				}
				else {
					eta = material_stack.front()->index / m.index;			// refraction index ratio
					material_stack.push_front(&m);
				}

				const double k = 1.0 - eta * eta * (1.0 - u.dot(i.N) * u.dot(i.N));

				if (!(k < 0.0)) {							// k < 0.0 = internal reflection
					const vec3f& refr_dir = eta * u - (eta * u.dot(i.N) + sqrt(k)) * u;
					const ray refr_r(isect_pos + refr_dir * RAY_EPSILON, refr_dir);
					const vec3f& refr_contri = prod(m.kt, traceRay(scene, refr_r, thresh, depth + 1));
					result = result + refr_contri;
				}
			}
		}

		return result;
	
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.
		return vec3f( 0.0, 0.0, 0.0 );
	}
}

bool RayTracer::isLeavingObject(const Material* const material) {
	return material == material_stack.front();
}


RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;
	m_bSceneLoaded = false;
}


RayTracer::~RayTracer()
{
	delete [] buffer;
	delete scene;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene( char* fn )
{
	try
	{
		scene = readScene( fn );
	}
	catch( ParseError pe )
	{
		fl_alert( "ParseError: %s\n", pe );
		return false;
	}

	if( !scene )
		return false;
	
	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[ bufferSize ];
	
	// separate objects into bounded and unbounded
	scene->initScene();
	
	// Add any specialized scene loading code here
	
	m_bSceneLoaded = true;

	return true;
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset( buffer, 0, w*h*3 );
}

void RayTracer::traceLines( int start, int stop )
{
	vec3f col;
	if( !scene )
		return;

	if( stop > buffer_height )
		stop = buffer_height;

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j);
}

void RayTracer::tracePixel( int i, int j )
{
	vec3f col;

	if( !scene )
		return;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	col = trace( scene,x,y );

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}