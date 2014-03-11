#ifndef _STEREO
#define _STEREO

#include <stdio.h>

#include "GraphcutStereo.h"

class Stereo
{
public:

    ~Stereo();

    static Stereo * Instance(int argc=0, char **argv=NULL)
    {
        if(instance == NULL)
        {
            instance = new Stereo(argc, argv);
        }

        return instance;
    }

    void idle();
    void mainLoop();
    void hanldeKeyPressed(std::map<unsigned char, bool> &key, bool &updateKey);


private:

    Stereo(int argc, char **argv);
    static Stereo* instance;
    GraphcutStereo *gstereo;


};

#endif
