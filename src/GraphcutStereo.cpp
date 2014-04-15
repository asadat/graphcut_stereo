#include "GraphcutStereo.h"
#include "cvd/videodisplay.h"
#include "cvd/gl_helpers.h"
#include <sys/time.h>
#include "cvd/vision.h"

using namespace CVD;

GraphcutStereo::GraphcutStereo(int argc, char **argv)
{
    if(argc>3)
        alpha_beta_swap = (strcmp(argv[3],"-swap")==0?true:false);
    else
        alpha_beta_swap = false;

    drawHighRes = false;
    nF = 40;
    fname.clear();
    fname.append(argv[1]);


    CVD::img_load(img[0], argv[1]);
    CVD::img_load(img[1], argv[2]);

    ImageRef size = img[0].size();
    imgSize = size;

    image[0].resize(size);
    image[1].resize(size);
    disparity.resize(size);
    img0_4x4 = CVD::halfSample(img[0]);

    for(int i=0; i< size.y; i++)
        for(int j=0; j< size.x; j++)
        {
            image[0][i][j] = RGB2Y(img[0][i][j]);
            image[1][i][j] = RGB2Y(img[1][i][j]);
            disparity[i][j] = rand()%(nF/2);

//            int rr=2;
//            double intSum=0;
//            for(int ii=i-rr; ii<=i+rr; ii++)
//                for(int jj=j-rr; jj<=j+rr; jj++)
//                {
//                    if(ii < 0 || ii>size.y || jj < 0 || jj > size.x)
//                    {
//                        intSum += 127;
//                    }
//                    else
//                    {
//                        intSum += image[0][ii][jj];
//                    }
//                }

//            double intI = intSum/((rr+1)*(rr+1));
//            double sumVar=0;
//            for(int ii=i-rr; ii<=i+rr; ii++)
//                for(int jj=j-rr; jj<=j+rr; jj++)
//                {
//                    int elem = 0;
//                    if(ii < 0 || ii>size.y || jj < 0 || jj > size.x)
//                    {
//                        elem = 127;
//                    }
//                    else
//                    {
//                        elem = image[0][ii][jj];
//                    }

//                    sumVar += (intI - elem)*(intI - elem);

//                }

//            double var = sumVar/((rr+1)*(rr+1));

//            meanI[i][j] = intI;
//            varI[i][j] = var;
        }

    for(int i=0; i< size.y; i++)
        for(int j=0; j< size.x; j++)
        {

        }

    for(int i=0; i< nF; i++)
        F[i] = -1;

    for(int i=0; i< nF; i++)
    {
        F[i]=nF-1-i;
        continue;
//        int rr=2;
//        double intSum=0;
//        for(int ii=i-rr; ii<=i+rr; ii++)
//            for(int jj=j-rr; jj<=j+rr; jj++)
//            {
//                if(ii < 0 || ii>size.y || jj < 0 || jj > size.x)
//                {
//                    intSum += 127;
//                }
//                else
//                {
//                    intSum += image[0][ii][jj];
//                }
//            }

        while(true)
        {
            int j=rand()%(nF);
            if(F[j] == -1)
            {
                F[j] = i;
                break;
            }
        }
    }

    g = new GraphType(size.x*size.y*2,size.x*size.y*2*4);
    currentF = 0;

    std::string output_name = fname;
    output_name.erase(output_name.begin()+output_name.length()-4,output_name.begin()+output_name.length());
    CVD::img_save(image[0],output_name+ "disparity.png");

    //display = new VideoDisplay(image[0].size());
    //display = new VideoDisplay(0,0,400,400);
}

GraphcutStereo::~GraphcutStereo()
{

}

void GraphcutStereo::Run()
{
    int n=0;
    bool flag = true;
    while(flag)
    {
        n++;

//        timeval t2;
//        timeval t1;
//       gettimeofday(&t1, NULL);


        flag = AlphaExpansion();
//        gettimeofday(&t2, NULL);

//        double elapsedTime = (-t1.tv_sec + t2.tv_sec) * 1000.0;      // sec to ms
//        elapsedTime += (-t1.tv_usec + t2.tv_usec) / 1000.0;   // us to ms

//        printf("DTime: %f \n", elapsedTime);
        Display();
        if(n>4)
            return;
    }
}

void GraphcutStereo::Update()
{
    currentF++;
    if(currentF >= (alpha_beta_swap?nF*nF/4:nF))
        currentF = 0;

    bool flag = drawHighRes;
    if(alpha_beta_swap)
        drawHighRes = AlphaBetaSwap(currentF%(nF/2), currentF/(nF/2));
    else
        drawHighRes = AlphaExpansion(currentF);

    if(drawHighRes != flag)
    {
        Display();

        //DisparityMedian();
    }
}

void GraphcutStereo::glDraw()
{
//    float w=10;
//    glLineWidth(2);
//    glColor3f(0,0,0);
//    glBegin(GL_LINES);
//    for(int i=0; i<=w; i++)
//    {
//        glVertex3f(-w/2, -w/2+i, 0);
//        glVertex3f( w/2, -w/2+i, 0);
//    }

//    for(int i=0; i<=w; i++)
//    {
//        glVertex3f(-w/2+i, -w/2, 0);
//        glVertex3f(-w/2+i,  w/2, 0);
//    }

//    glEnd();



    float dx = -0.1;
    float dy = 0.1;
    float dz = 0.5;
    int ds = 1;
   // if(drawHighRes)
   //     ds=4;

    ImageRef size = disparity4x4.size();
    glPointSize(5);
    glBegin(GL_QUADS);
    for(int i=0; i<size.y-ds; i+=ds)
        for(int j=0; j<size.x-ds; j+=ds)
        {
	    

            CVD::Rgb<CVD::byte> c = img0_4x4[i][j];
            glColor(c);
//            float d = disparity4x4[i][j]; ;
//            d = (d<=0)?1:d;
//            float c = 0.5-1/d;
//            glColor3f(c,c,c);

            glVertex3f(j*dy,Disparity2Depth(disparity4x4[i][j])*dz,i*dx);
            glVertex3f(j*dy,Disparity2Depth(disparity4x4[i+ds][j])*dz,(i+ds)*dx);
            glVertex3f((j+ds)*dy,Disparity2Depth(disparity4x4[i+ds][j+ds])*dz,(i+ds)*dx);
            glVertex3f((j+ds)*dy,Disparity2Depth(disparity4x4[i][j+ds])*dz,i*dx);
        }
    glEnd();
}

void GraphcutStereo::Display()
{
    Image<byte> disimg;
    disimg.resize(disparity.size());

    for(int i=0; i< disparity.size().y; i++)
        for(int j=0; j< disparity.size().x; j++)
        {
            int cd = disparity[i][j]*12.5;
            if(cd > 255)
                cd= 255;

            disimg[i][j] = cd;
        }

    std::string output_name = fname;
    output_name.erase(output_name.begin()+output_name.length()-4,output_name.begin()+output_name.length());
    if(alpha_beta_swap)
        CVD::img_save(disimg,output_name+ "disparity-swap.png");
    else
        CVD::img_save(disimg,output_name+ "disparity-expansion.png");

//    glDrawPixels(disimg);

////    glColor3f(1,0,0);
////    glBegin(GL_LINES);
////    glVertex(image[0].size()/2);
////    glVertex(image[0].size());
////    glEnd();
//    glFlush();

    //std::cin.get();
}

double GraphcutStereo::SubPixVal(int x, float y, int imageIdx)
{
    int inty = floor(y);
    float prey = y - inty;
    return (1-prey)*image[imageIdx][x][inty] + prey*image[imageIdx][x][inty+1];
}

double GraphcutStereo::D(int i, int j, double dp)
{

    double val=0;
    int w=1;
    for(int ii=i; ii<i+1;ii++)
        for(int jj=j-w; jj<=j+w;jj++)
        {
            int pixVal = 0;
            int jj2 = jj-dp;
            if(ii <0 || jj < 0 || ii >= imgSize.y || jj >= imgSize.x || jj2 < 0)
                pixVal =255;
            else
            {
                pixVal = fabs(image[0][ii][jj] - SubPixVal(ii,jj2,1));
            }

//            double sum = 0;
//            int rr=1;
//            for(int iii=ii-2*rr; iii<=ii+2*rr; iii++)
//                for(int jjj=jj-2*rr; jjj<=jj+2*rr; jjj++)
//                {
//                    if(iii < 0 || iii>imgSize.y || jjj < 0 || jjj > imgSize.x)
//                    {
//                        continue;
//                    }

//                    if(jj2>0 && jj2 < imgSize.x)
//                        sum = 1+ (image[0][iii][jjj] -meanI[iii][jjj])*(image[0][iii][jj2] -meanI[iii][jjj])/(varI[iii][jjj]+0.0001);
//                }
//            val += pixVal * sum/(2*2*rr+1);
            val+=pixVal;
        }

    double n = 3;
    val = (val/(n))<200?(val/(n)):200;

    return val*val;

//    int j2 = j - dp;
//    double val = 255;
//    if(j2 >= 0)
//    {

//        double val1 = fabs(image[0][i][j] - (image[1][i][j2]));
//        double val2 = 255*255;
//        if(j2-1>=0 )
//            val2 = fabs(image[0][i][j] - (0.5*image[1][i][j2-1] + 0.5*image[1][i][j2]));

//        double val3 = 255*255;
//        if(j2+1 < image[0].size().x)
//            val3 = fabs(image[0][i][j] - (0.5*image[1][i][j2] + 0.5*image[1][i][j2+1]));

//        if(val1<val2)
//        {
//            if(val3<val1)
//                val = val3;
//            else
//                val = val1;
//        }
//        else
//        {
//            if(val3<val2)
//                val = val3;
//            else
//                val = val2;
//        }

//        //val *= (1.0/255.0);
//        val = fabs(val*val);
//    }

//    return val;
}

double GraphcutStereo::V(int i1, int j1, double dp1, int i2, int j2, double dp2)
{
    //Potts model for now;
//    double val=0;
//    if(fabs(dp1 - dp2) > 0.5)
//    {
//        if(fabs(image[0][i1][j1] - image[0][i2][j2]) <= 5)
//            val = 100;
//        else
//            val = 50;
//    }

//    return val;

    double val = 0;
    if(alpha_beta_swap)
        val = 50*fabs(dp1-dp2); //alpha beta swap
    else
        val = 5*fabs(dp1-dp2); //alpha expansion

    if(val >= 50)
    {
        val = 50;
    }

    return val;
}

double GraphcutStereo::E(Image<float> &disp)
{
    double energy = 0;
    ImageRef size = image[0].size();
    for(int i=0; i<size.y; i++)
        for(int j=0; j<size.x; j++)
        {
            //printf("D1\n");
            energy += D(i,j,disp[i][j]);
            //printf("D2\n");

            if(j+1 < size.x)
            {
                //printf("V1\n");
                energy += V(i,j,disp[i][j],i,j+1,disp[i][j+1]);
                //printf("V2\n");
            }
        }

    return energy;

}

float GraphcutStereo::Disparity2Depth(int disp)
{
    return nF - disp;
}

void GraphcutStereo::DisparityMedian()
{
    Image<float> tmp;
    tmp.copy_from(disparity);

    int rr=5;
    for(int i=0; i<imgSize.y; i++)
        for(int j=0; j<imgSize.x; j++)
        {
            double intSum=0;
            int n=0;
            for(int ii=i-rr;ii<=i+rr;ii++)
                for(int jj=j-rr;jj<=j+rr;jj++)
                {
                    if(ii < 0 || ii>=imgSize.y || jj < 0 || jj >= imgSize.x)
                    {
                        continue;
                    }
                    else
                    {
                        n++;
                        intSum += tmp[ii][jj];
                    }
                }

            disparity[i][j] = intSum/n;
        }

    //CVD::median_filter_3x3(tmp,disparity);
}

bool GraphcutStereo::AlphaBetaSwap(int f1, int f2)
{
    static int round = 0;
    round++;
    if(round > nF*nF)
        return false;

    ImageRef size = image[0].size();

    static double lastE=99999999999;

    Image<float> newdisp(disparity.copy_from_me());
    //std::map<std::pair<int,int>, GraphType::node_id> nodeidMap;

    //int initF = (f==-1)?0:f;
    //int endF = (f==-1)?nF:(f+1);

    //for(int k=initF; k<endF; k++)
    {
        g->reset();
      //  nodeidMap.clear();

        timeval t2;
        timeval t1;
       gettimeofday(&t1, NULL);

        //double f = ((double)(F[k]))*0.5;
        //printf("alpha = %f\n", f);

        for(int i=0; i<size.y; i++)
            for(int j=0; j<size.x; j++)
            {
                node_id_map[i][j] = -1;
            }

        for(int i=1; i<size.y-1; i++)
            for(int j=1; j<size.x-1; j++)
            {
                newdisp[i][j] = disparity[i][j];

                if(((int)newdisp[i][j]) != f1 && ((int)newdisp[i][j]) != f2)
                    continue;

                bool brnode = (((int)newdisp[i][j+1]) == f1 || ((int)newdisp[i][j+1]) == f2);
                bool bdnode =  (((int)newdisp[i+1][j]) == f1 || ((int)newdisp[i+1][j]) == f2);
                bool bunode = (((int)newdisp[i-1][j]) == f1 || ((int)newdisp[i-1][j]) == f2);
                bool blnode =  (((int)newdisp[i][j-1]) == f1 || ((int)newdisp[i][j-1]) == f2);

                GraphType::node_id nid = node_id_map[i][j];
                if(nid == -1)
                {
                    nid = g->add_node();
                    node_id_map[i][j] = nid;
                }


                double t1 = D(i,j,f1);
                t1 += (brnode?0 :V(i,j,f1, i,j+1,disparity[i][j+1]))
                     +(blnode?0:V(i,j,f1, i,j-1,disparity[i][j-1]))
                     +(bunode?0 :V(i,j,f1, i+1,j,disparity[i+1][j]))
                     +(bdnode?0 :V(i,j,f1, i-1,j,disparity[i-1][j]));

                double t2 = D(i,j,f2);
                t2 += (brnode?0 :V(i,j,f2, i,j+1,disparity[i][j+1]))
                     +(blnode?0:V(i,j,f2, i,j-1,disparity[i][j-1]))
                     +(bunode?0 :V(i,j,f2, i+1,j,disparity[i+1][j]))
                     +(bdnode?0 :V(i,j,f2, i-1,j,disparity[i-1][j]));


                g->add_tweights(nid, t1, t2);


                GraphType::node_id lnode = nid;
                GraphType::node_id rnode = node_id_map[i][j+1];
                GraphType::node_id dnode = node_id_map[i+1][j];



                if(rnode == -1 && brnode)
                {
                    rnode = g->add_node();
                    node_id_map[i][j+1] = rnode;
                }

                if(dnode == -1 && bdnode)
                {
                    dnode = g->add_node();
                    node_id_map[i+1][j] = dnode;
                }

                if(brnode)
                {
                  g->add_edge(lnode, rnode, V(i,j,f1, i,j+1,f2),0);
                }

                if(bdnode)
                {
                  g->add_edge(lnode, dnode, V(i,j,f1, i+1,j,f2),0);
                }


//                GraphType::node_id anid = g->add_node();

//                g->add_tweights(anid, V(i,j,disparity[i][j], i,j+1,disparity[i][j+1]), 0);

//                g->add_edge(lnode, anid, V(i,j,disparity[i][j], i,j+1,f),0);
//                g->add_edge(anid, rnode, V(i,j,f, i,j+1,disparity[i][j+1]), 0);

//                GraphType::node_id adnid = g->add_node();
//                g->add_tweights(adnid, V(i,j,disparity[i][j], i+1,j,disparity[i+1][j]), 0);
//                g->add_edge(lnode, adnid, V(i,j,disparity[i][j], i+1,j,f),0);
//                g->add_edge(adnid, dnode, V(i,j,f, i+1,j,disparity[i+1][j]),0);
            }

//        for(int i=0; i<size.y-1; i++)
//            for(int j=0; j<size.x-1; j++)
//            {
////                std::pair<int, int> pxl;
////                pxl.first = i;
////                pxl.second = j;
////                std::pair<int, int> pxr;
////                pxr.first = i;
////                pxr.second = j+1;

////                std::pair<int, int> pxd;
////                pxr.first = i+1;
////                pxr.second = j;

////                GraphType::node_id lnode = nodeidMap[pxl];
////                GraphType::node_id rnode = nodeidMap[pxr];
////                GraphType::node_id dnode = nodeidMap[pxd];

//                GraphType::node_id lnode = node_id_map[i][j];
//                GraphType::node_id rnode = node_id_map[i][j+1];
//                GraphType::node_id dnode = node_id_map[i+1][j];

//                GraphType::node_id nid = g->add_node();

//                g->add_tweights(nid, V(i,j,disparity[i][j], i,j+1,disparity[i][j+1]), 0);
//                g->add_edge(lnode, nid, V(i,j,disparity[i][j], i,j+1,f),0);
//                g->add_edge(nid, rnode, V(i,j,f, i,j+1,disparity[i][j+1]), 0);

//                GraphType::node_id dnid = g->add_node();
//                g->add_tweights(dnid, V(i,j,disparity[i][j], i+1,j,disparity[i+1][j]), 0);
//                g->add_edge(lnode, dnid, V(i,j,disparity[i][j], i+1,j,f),0);
//                g->add_edge(dnid, dnode, V(i,j,f, i+1,j,disparity[i+1][j]),0);

//            }

        timeval t22;
        timeval t12;


       gettimeofday(&t12, NULL);

       double elapsedTime3 = (-t1.tv_sec + t12.tv_sec) * 1000.0;      // sec to ms
       elapsedTime3 += (-t1.tv_usec + t12.tv_usec) / 1000.0;   // us to ms

       //printf("Setup Time: %f \n", elapsedTime3/1000.0);

        double flow = g->maxflow();
       // printf("flow:%f\n", flow);
        gettimeofday(&t22, NULL);

        double elapsedTime2 = (-t12.tv_sec + t22.tv_sec) * 1000.0;      // sec to ms
        elapsedTime2 += (-t12.tv_usec + t22.tv_usec) / 1000.0;   // us to ms

       // printf("Flow Time: %f \n", elapsedTime2/1000.0);

        for(int i=0; i<size.y; i++)
            for(int j=0; j<size.x; j++)
            {
//                std::pair<int, int> px;
//                px.first = i;
//                px.second = j;
//                GraphType::node_id nodeid = nodeidMap[px];
                GraphType::node_id nodeid = node_id_map[i][j];
                if(nodeid!= -1)
                {
                    if(g->what_segment(nodeid) == GraphType::SOURCE)
                    {
                        //printf("In source ..\n");
                        newdisp[i][j] = f2;
                    }
                    else
                    {
                        newdisp[i][j] = f1;
                    }
                }
                else
                {
                    newdisp[i][j] = disparity[i][j];
                }
            }

        double newE = E(newdisp);
        if( newE < lastE)
        {
           //round = 0;
           printf("flow reduced from:%f to:%f Time:%f\n", lastE, newE,elapsedTime2/1000.0);
            lastE = newE;
            disparity.copy_from(newdisp);
            disparity4x4 = CVD::halfSample(disparity);
        }

        //nodeidMap.clear();
        //delete g;


        gettimeofday(&t2, NULL);

        double elapsedTime = (-t1.tv_sec + t2.tv_sec) * 1000.0;      // sec to ms
        elapsedTime += (-t1.tv_usec + t2.tv_usec) / 1000.0;   // us to ms

       // printf("DTime: %f \n", elapsedTime/1000.0);
    }

    return true;

}

bool GraphcutStereo::AlphaExpansion(int f)
{
    static int round = 0;
    round++;
    if(round > nF+1)
        return false;

    ImageRef size = image[0].size();

    static double lastE=99999999999;

    Image<float> newdisp(disparity.copy_from_me());
    //std::map<std::pair<int,int>, GraphType::node_id> nodeidMap;

    int initF = (f==-1)?0:f;
    int endF = (f==-1)?nF:(f+1);

    for(int k=initF; k<endF; k++)
    {
        g->reset();
      //  nodeidMap.clear();

        timeval t2;
        timeval t1;
       gettimeofday(&t1, NULL);

        double f = ((double)(F[k]))*0.5;
        printf("alpha = %f\n", f);

        for(int i=0; i<size.y; i++)
            for(int j=0; j<size.x; j++)
            {
                node_id_map[i][j] = -1;
            }

        for(int i=0; i<size.y; i++)
            for(int j=0; j<size.x; j++)
            {
                newdisp[i][j] = disparity[i][j];

                //std::pair<int, int> px;
                //px.first = i;
                //px.second = j;
                GraphType::node_id nid = node_id_map[i][j];
                if(nid == -1)
                {
                    nid = g->add_node();
                    node_id_map[i][j] = nid;
                }


                g->add_tweights(nid, D(i,j,disparity[i][j]), D(i,j,f));


                GraphType::node_id lnode = nid;
                GraphType::node_id rnode = node_id_map[i][j+1];
                GraphType::node_id dnode = node_id_map[i+1][j];

                if(rnode == -1)
                {
                    rnode = g->add_node();
                    node_id_map[i][j+1] = rnode;
                }

                if(dnode == -1)
                {
                    dnode = g->add_node();
                    node_id_map[i+1][j] = dnode;
                }

                GraphType::node_id anid = g->add_node();

                g->add_tweights(anid, V(i,j,disparity[i][j], i,j+1,disparity[i][j+1]), 0);
                g->add_edge(lnode, anid, V(i,j,disparity[i][j], i,j+1,f),0);
                g->add_edge(anid, rnode, V(i,j,f, i,j+1,disparity[i][j+1]), 0);

                GraphType::node_id adnid = g->add_node();
                g->add_tweights(adnid, V(i,j,disparity[i][j], i+1,j,disparity[i+1][j]), 0);
                g->add_edge(lnode, adnid, V(i,j,disparity[i][j], i+1,j,f),0);
                g->add_edge(adnid, dnode, V(i,j,f, i+1,j,disparity[i+1][j]),0);
            }

//        for(int i=0; i<size.y-1; i++)
//            for(int j=0; j<size.x-1; j++)
//            {
////                std::pair<int, int> pxl;
////                pxl.first = i;
////                pxl.second = j;
////                std::pair<int, int> pxr;
////                pxr.first = i;
////                pxr.second = j+1;

////                std::pair<int, int> pxd;
////                pxr.first = i+1;
////                pxr.second = j;

////                GraphType::node_id lnode = nodeidMap[pxl];
////                GraphType::node_id rnode = nodeidMap[pxr];
////                GraphType::node_id dnode = nodeidMap[pxd];

//                GraphType::node_id lnode = node_id_map[i][j];
//                GraphType::node_id rnode = node_id_map[i][j+1];
//                GraphType::node_id dnode = node_id_map[i+1][j];

//                GraphType::node_id nid = g->add_node();

//                g->add_tweights(nid, V(i,j,disparity[i][j], i,j+1,disparity[i][j+1]), 0);
//                g->add_edge(lnode, nid, V(i,j,disparity[i][j], i,j+1,f),0);
//                g->add_edge(nid, rnode, V(i,j,f, i,j+1,disparity[i][j+1]), 0);

//                GraphType::node_id dnid = g->add_node();
//                g->add_tweights(dnid, V(i,j,disparity[i][j], i+1,j,disparity[i+1][j]), 0);
//                g->add_edge(lnode, dnid, V(i,j,disparity[i][j], i+1,j,f),0);
//                g->add_edge(dnid, dnode, V(i,j,f, i+1,j,disparity[i+1][j]),0);

//            }

        timeval t22;
        timeval t12;


       gettimeofday(&t12, NULL);

       double elapsedTime3 = (-t1.tv_sec + t12.tv_sec) * 1000.0;      // sec to ms
       elapsedTime3 += (-t1.tv_usec + t12.tv_usec) / 1000.0;   // us to ms

       //printf("Setup Time: %f \n", elapsedTime3/1000.0);

        double flow = g->maxflow();
       // printf("flow:%f\n", flow);
        gettimeofday(&t22, NULL);

        double elapsedTime2 = (-t12.tv_sec + t22.tv_sec) * 1000.0;      // sec to ms
        elapsedTime2 += (-t12.tv_usec + t22.tv_usec) / 1000.0;   // us to ms

        //printf("Flow Time: %f \n", elapsedTime2/1000.0);

        for(int i=0; i<size.y; i++)
            for(int j=0; j<size.x; j++)
            {
//                std::pair<int, int> px;
//                px.first = i;
//                px.second = j;
//                GraphType::node_id nodeid = nodeidMap[px];
                GraphType::node_id nodeid = node_id_map[i][j];
                if(g->what_segment(nodeid) == GraphType::SOURCE)
                {
                    //printf("In source ..\n");
                    newdisp[i][j] = f;
                }
            }

        double newE = E(newdisp);
        if( newE < lastE)
        {
            round = 0;
            printf("flow reduced from:%f to:%f\n", lastE, newE);
            lastE = newE;
            disparity.copy_from(newdisp);
            disparity4x4 = CVD::halfSample(disparity);
        }

        //nodeidMap.clear();
        //delete g;


        gettimeofday(&t2, NULL);

        double elapsedTime = (-t1.tv_sec + t2.tv_sec) * 1000.0;      // sec to ms
        elapsedTime += (-t1.tv_usec + t2.tv_usec) / 1000.0;   // us to ms

       // printf("DTime: %f \n", elapsedTime/1000.0);
    }


    return true;
}


