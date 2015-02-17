//
//  Framework for a raytracer
//  File: scene.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Maarten Everts
//    Jasper van de Gronde
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#include "scene.h"
#include "material.h"

Color Scene::trace(const Ray &ray)
{
    // Find hit object and distance
    Hit min_hit(std::numeric_limits<double>::infinity(),Vector());
    Object *obj = NULL;
    
    for (unsigned int i = 0; i < objects.size(); ++i) {
        Hit hit(objects[i]->intersect(ray));
        if (hit.t<min_hit.t) {
            min_hit = hit;
            obj = objects[i];
        }
    }
    

    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);

    Material *material = obj->material;            //the hit objects material
    Point hit = ray.at(min_hit.t);                 //the hit point
    Vector N = min_hit.N;                          //the normal at hit point
    Vector V = -ray.D;                             //the view vector


    /****************************************************
    * This is where you should insert the color
    * calculation (Phong model).
    *
    * Given: material, hit, N, V, lights[]
    * Sought: color
    *
    * Hints: (see triple.h)
    *        Triple.dot(Vector) dot product
    *        Vector+Vector      vector sum
    *        Vector-Vector      vector difference
    *        Point-Point        yields vector
    *        Vector.normalize() normalizes vector, returns length
    *        double*Color        scales each color component (r,g,b)
    *        Color*Color        dito
    *        pow(a,b)           a to the power of b
    ****************************************************/
    
    Vector aux_light(lights.at(0)->position - hit);
    Vector unit_light = aux_light.normalized();
    
    double cosine_factor = N.normalized().dot(unit_light);
    double specular_factor;
    if (cosine_factor < 0)
    {
        cosine_factor = 0;
        specular_factor = 0;
    }
    else
    {
        Vector reflection_vector((2*(unit_light.dot(N)))*N-unit_light);
        specular_factor = material->ks * pow(cosine_factor,material->n);
    }
    
    double r = (specular_factor * material->color.r) + (material->ka * material->color.r) + (material->kd * cosine_factor * material->color.r);
    double g = (specular_factor * material->color.g) + (material->ka * material->color.g) +(material->kd * cosine_factor * material->color.g);
    double b = (specular_factor * material->color.b) + (material->ka * material->color.b) +(material->kd * cosine_factor * material->color.b);
    if (cosine_factor < 0) cosine_factor = 0;
    
    
    /* eu tinha feito isso manualmente, nao achei que nao tratar o 0 daria erro, dai tentei enxugar ali embaixo e resultou no negocio certo
    if (N.x < 0) N.x = (N.x / 2) + 0.5;
    if (N.y < 0) N.y = (N.y / 2) + 0.5;
    if (N.z < 0) N.z = (N.z / 2) + 0.5;
    if (N.x > 0) N.x = (N.x / 2) + 0.5;
    if (N.y > 0) N.y = (N.y / 2) + 0.5;
    if (N.z > 0) N.z = (N.z / 2) + 0.5;*/
    
    
    // transformar de -1,1 a 0,1
    N = N / 2 + 0.5;
    
    
    Color color(N);
    return color;
}

Color Scene::zBufferTrace(const Ray &ray)
{
    // Find hit object and distance
    Hit min_hit(std::numeric_limits<double>::infinity(),Vector());
    Object *obj = NULL;
    
    for (unsigned int i = 0; i < objects.size(); ++i) {
        Hit hit(objects[i]->intersect(ray));
        if (hit.t<min_hit.t) {
            min_hit = hit;
            obj = objects[i];
        }
    }
    
    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);
    
    Material *material = obj->material;            //the hit objects material
    Point hit = ray.at(min_hit.t);                 //the hit point
    Vector N = min_hit.N;                          //the normal at hit point
    Vector V = -ray.D;                             //the view vector

    return Color(0,0,0);
}


void Scene::render(Image &img)
{
    int w = img.width();
    int h = img.height();
    
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Point pixel(x+0.5, h-1-y+0.5, 0);
            Ray ray(eye, (pixel-eye).normalized());
            Color col = trace(ray);
            col.clamp();
            img(x,y) = col;
        }
    }
}

void Scene::addObject(Object *o)
{
    objects.push_back(o);
}

void Scene::addLight(Light *l)
{
    lights.push_back(l);
}

void Scene::setEye(Triple e)
{
    eye = e;
}
