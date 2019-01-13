//
// Created by z on 9/24/18.
//

#ifndef MARSSIMULATOR_MARSMODEL_H
#define MARSSIMULATOR_MARSMODEL_H


#include <iostream>
#include <string>
#include <cmath>

#include "GeoTIFFReader.h"
#include "csphere.h"
#include "perspectiveCamera.h"
#include "gvector3.h"
#include "cray.h"
#include "color.h"
#include "directlight.h"
#include "pointlight.h"

class MarsWithCameraModel {
public:
    explicit MarsWithCameraModel(std::string _file, bool _buff=false);
    ~MarsWithCameraModel() {
        delete GeoData;
    }
    void setCameraParameters(GVector3 location, float fov);
    inline void setPhotoParameters(size_t w, size_t h) {
        width = w;
        height = h;
    }
    void snapshotMars(cv::Mat &Image);
    void snapshotMarsByDirectLight(cv::Mat &Image, float x, float y ,float z, bool isShadow);
    void snapshotMarsByPointLight(cv::Mat &Image, float x, float y ,float z, bool isShadow);
private:
    // Mars
    std::string file;
    GeoTIFFReader *GeoData;
    GVector3 origin = GVector3(0,0,0);
    CSphere* sphereModel;
    // Camera
    perspectiveCamera camera;
    // photo
    size_t width = 1024;
    size_t height = 1024;
    vector<vector<cv::Vec3f>> pixelLocation; // location of every pixel on the image
};

#endif //MARSSIMULATOR_MARSMODEL_H
