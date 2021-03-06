#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>
#include <stdio.h>
using namespace std;

#include "vector.h"
#include "ray.h"


#define PI				3.141592653589793238462f
const float focalFactor = 1.5f;

class Camera
{

private:
	Vector eye, at, up; 
	float fovy, vnear, vfar, plane_dist, focal_ratio, aperture;
	float w, h;
	int res_x, res_y;
	Vector u, v, n;

public:

	int GetResX()  { return res_x; }
    int GetResY()  { return res_y; }
	float GetFov() { return fovy; }
	float GetPlaneDist() { return plane_dist; }
	float GetFar() {return vfar; }
	float GetAperture() { return aperture; }

    Camera( Vector from, Vector At, Vector Up, float angle, float hither, float yon, int ResX, int ResY, float Aperture_ratio, float Focal_ratio) {
	    eye = from;
	    at = At;
	    up = Up;
	    fovy = angle;
	    vnear = hither;
	    vfar = yon;
	    res_x = ResX;
	    res_y = ResY;
		focal_ratio = Focal_ratio;

        // set the camera frame uvn
        n = ( eye - at );
        plane_dist = n.length();
	    n = n / plane_dist;

	    u = up % n;
	    u = u / u.length();

	    v = n % u;

        //Dimensions of the vis window
	    h = 2 * plane_dist * tan( (PI * angle / 180) / 2.0f );
        w = ( (float) res_x / res_y ) * h;  

		aperture = Aperture_ratio * (w / res_x); //Lens aperture = aperture_ratio * pixel_size

		printf("\nwidth=%f height=%f fov=%f, viewplane distance=%f, pixel size=%.3f\n", w,h, fovy,plane_dist, w/res_x);
		if (Aperture_ratio != 0) printf("\nDepth-Of-Field effect enabled with a lens aperture = %.1f\n", Aperture_ratio);
    }

	Ray PrimaryRay(const Vector& pixel_sample) //  Rays cast from the Eye to a pixel sample which is in Viewport coordinates
	{
		Vector ray_dir;
		Vector ze = (eye - at).normalize();
		Vector xe = (up % ze).normalize();
		Vector ye = (ze % xe).normalize();

		ray_dir = ze * (-plane_dist) + ye * (h * (pixel_sample.y / res_y - 0.5f)) + xe * (w * (pixel_sample.x / res_x - 0.5f));
		ray_dir.normalize();

		return Ray(eye, ray_dir);  
	}

	Ray PrimaryRay(const Vector& lens_sample, const Vector& pixel_sample) // DOF: Rays cast from  a thin lens sample to a pixel sample
	{

		Vector ze = (eye - at).normalize();
		Vector xe = (up % ze).normalize();
		Vector ye = (ze % xe).normalize();
		Vector eye_offset = xe * lens_sample.x + ye * lens_sample.y + eye;
		Vector viewPoint;
		viewPoint.x = w * (pixel_sample.x / res_x - 0.5f);
		viewPoint.y = h * (pixel_sample.y / res_y - 0.5f);
		viewPoint.z = -plane_dist;
		Vector focal_point;
		focal_point.x = viewPoint.x * focal_ratio;
		focal_point.y = viewPoint.y * focal_ratio;
		focal_point.z = -plane_dist * focal_ratio;
		Vector ray_dir = xe * (focal_point.x - lens_sample.x) + ye * (focal_point.y - lens_sample.y) + ze * focal_point.z;
		ray_dir.normalize();

		return Ray(eye_offset, ray_dir);
	}
};

#endif