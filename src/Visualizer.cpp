#include "stdio.h"
#include "Visualizer.h"

std::vector<Visualizer*> Visualizer::objects;


Visualizer::Visualizer()
{
     objects.push_back(this);
}


Visualizer::~Visualizer()
{
    for(int i=0; i<objects.size(); i++)
    {
        if(objects[i] == this)
        {
            objects.erase(objects.begin()+i);
            break;
        }
    }
}

void Visualizer::glDrawAll()
{
    for(int i=0; i < objects.size(); i++)
        objects[i]->glDraw();
}
