#include <cmath>
#include <assert.h>

#include "Box.h"

bool Box::intersectLocal(const ray& r, isect& i) const
{
	// YOUR CODE HERE:
	// Add box intersection code here.
	// it currently ignores all boxes and just returns false.

	double iNear = -std::numeric_limits<double>::max();
	double iFar = std::numeric_limits<double>::max();

	vec3f rayDir = r.getDirection();
	vec3f rayPos = r.getPosition();

	// t1, t2 is used to compute the intersection distance of the planes
	double t1 = 0.0f, t2 = 0.0f;
	int intersect = -1;

	// for each pair of planes associated with X, Y and Z
	for (int i = 0; i < 3; i++)
	{
		if (rayDir[i] == 0)
		{
			if (rayDir[i] < -0.5 || rayDir[i] > 0.5)
				return false;
		}

		t1 = (-0.5 - rayPos[i]) / rayDir[i];
		t2 = (0.5 - rayPos[i]) / rayDir[i];

		// t1 intersection with near plane
		if (t1 > t2)
		{
			double temp = t2;
			t2 = t1;
			t1 = temp;
		}

		if (t1 > iNear){
			iNear = t1;
			intersect = i;
		}
		if (t2 < iFar)
			iFar = t2;
		if (iNear > iFar || iFar < 0)
			return false;
	}

	i.obj = this;
	i.t = iNear;

	if (intersect == 0)
	{
		if (rayDir[0] < 0.0)
			i.N = vec3f(1.0, 0.0, 0.0);
		else
			i.N = vec3f(-1.0, 0.0, 0.0);
	}
	else if (intersect == 1)
	{
		if (rayDir[1] < 0.0)
			i.N = vec3f(0.0, 1.0, 0.0);
		else
			i.N = vec3f(0.0, -1.0, 0.0);
	}
	else if (intersect == 2)
	{
		if (rayDir[2] < 0.0)
			i.N = vec3f(0.0, 0.0, 1.0);
		else
			i.N = vec3f(0.0, 0.0, -1.0);
	}


	return true;
}
