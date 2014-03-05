#include "GraphcutStereo.h"
#include "cvd/videodisplay.h"
#include "cvd/gl_helpers.h"

using namespace CVD;

GraphcutStereo::GraphcutStereo(int argc, char **argv)
{
    CVD::Image<CVD::Rgb8> img[2];
    CVD::img_load(img[0], argv[1]);
    CVD::img_load(img[1], argv[2]);

    image[0].resize(img[0].size());
    image[1].resize(img[1].size());
    disparity.resize(img[1].size());

    for(int i=0; i< img[0].size().y; i++)
        for(int j=0; j< img[0].size().x; j++)
        {
            image[0][i][j] = RGB2Y(img[0][i][j]);
            image[1][i][j] = RGB2Y(img[1][i][j]);
        }
}

GraphcutStereo::~GraphcutStereo()
{

}

void GraphcutStereo::Run()
{

    Display();
}

void GraphcutStereo::Display()
{
    VideoDisplay display(image[0].size());
    glDrawPixels(image[0]);

    glColor3f(1,0,0);
    glBegin(GL_LINES);
    glVertex(image[0].size()/2);
    glVertex(image[0].size());
    glEnd();
    glFlush();

    std::cin.get();
}
