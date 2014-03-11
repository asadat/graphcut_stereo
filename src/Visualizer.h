#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <vector>

class Visualizer
{
public:
    Visualizer();
    ~Visualizer();

    virtual void glDraw() = 0;
    static void glDrawAll();

private:

    static std::vector<Visualizer*> objects;
};

#endif
