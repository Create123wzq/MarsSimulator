//
// Created by z on 9/24/18.
//


#include "MarsWithCameraModel.h"

MarsWithCameraModel::MarsWithCameraModel(std::string _file, bool _buff) {
    file = std::move(_file);
    GeoData = new GeoTIFFReader(file, _buff);
    sphereModel = new CSphere(origin, R);
}

void MarsWithCameraModel::setCameraParameters(GVector3 location, float fov) {
    GVector3 up, forward(-location.x, -location.y, -location.z);
    forward.normalize();

    if ((int)location.x == 0  && (int)location.y == 0) {
        up = location.z > 0 ? GVector3(1, 0, 0) : GVector3(-1, 0, 0);
    } else {
        up = GVector3(0, 0, 1);
    }
    up.normalize();
    cout << " location " <<  location.x << " " << location.y << " " << location.z << endl;
    cout << " forward  " <<  forward.x << " " << forward.y << " " << forward.z << endl;
    cout << " up       " <<  up.x << " " << up.y << " " << up.z << endl;
    camera = perspectiveCamera(location, forward, up, fov);
}

void MarsWithCameraModel::snapshotMars(cv::Mat &Image) {
    pixelLocation = vector<vector<cv::Vec3f>>(width, vector<cv::Vec3f>(height, cv::Vec3f(0, 0, 0)));
    float dx=1.0f/width;
    float dy=1.0f/height;
    float sy, sx;
    for (long y = 0; y < width; ++y) {
        sy = 1 - dy*y;
        for (long x = 0; x < height; ++x) {
            sx =dx*x;
            CRay ray(camera.generateRay(sx, sy));
            IntersectResult result = sphereModel->isIntersected(ray);
            if (result.isHit) {
                pixelLocation[y][x][0] = result.position.x;
                pixelLocation[y][x][1] = result.position.y;
                pixelLocation[y][x][2] = result.position.z;
            }
        }
    }
    auto center = pixelLocation[width / 2][height / 2];
    double golon, golat;
    GeoData->SpaceLocation2Geo(center[0], center[1], center[2], golon, golat);
    std::cout << " space coordinate of image center is " << center[0] << " " << center[1] << " " << center[2] << std::endl;
    std::cout << " geo   coordinate of image center is " << golon << " " << golat << std::endl;
    float rgb[3];
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            auto loc = pixelLocation[i][j];
            if (loc[0] == 0 && loc[1] == 0 && loc[2] == 0)
                continue;
             GeoData->GetPixelValueByXYZ2(loc[0], loc[1], loc[2], rgb);
            cv::Vec3b pixel;
            pixel[0] = static_cast<uchar>(lround(rgb[2]));
            pixel[1] = static_cast<uchar>(lround(rgb[1]));
            pixel[2] = static_cast<uchar>(lround(rgb[0]));
            Image.at<cv::Vec3b>(i, j) = pixel;
        }
    }
}
/*平行光源
光线追踪的过程就是：
从摄像机产生光线->投射场景->若与物体相交，从该点产生光线，方向为光源方向的饭方向->投射场景->若与场景中的物体相交，则属于阴影区域。
*/
void MarsWithCameraModel::snapshotMarsByDirectLight(cv::Mat &Image, float x, float y, float z, bool isShadow) {
    DirectLight light1(Color::white().multiply(0.5), GVector3(x, y, z), isShadow);
    pixelLocation = vector<vector<cv::Vec3f>>(width, vector<cv::Vec3f>(height, cv::Vec3f(0, 0, 0)));
    float dx=1.0f/width;
    float dy=1.0f/height;
    float sy, sx;
    float rgb[3];
    for (int y = 0; y < width; ++y) {
        sy = 1 - dy*y;
        for (int x = 0; x < height; ++x) {
            sx =dx*x;
            CRay ray(camera.generateRay(sx, sy));
            IntersectResult result = sphereModel->isIntersected(ray);
            if (result.isHit) {
                float p_x = result.position.x;
                float p_y = result.position.y;
                float p_z = result.position.z;
                if(p_x == 0 && p_y==0 && p_z==0){
                    continue;
                }
                GeoData->GetPixelValueByXYZ2(p_x, p_y, p_z, rgb);

                cv::Vec3b pixel;
                Color color=light1.intersect(*sphereModel,result, rgb);
                pixel[0] = static_cast<uchar>(lround(std::min(color.b*255,(float)255.0)));
                pixel[1] = static_cast<uchar>(lround(std::min(color.g*255,(float)255.0)));
                pixel[2] = static_cast<uchar>(lround(std::min(color.r*255,(float)255.0)));
                Image.at<cv::Vec3b>(y, x) = pixel;
            }
        }
    }
}
/*点光源
点光源/点光灯(point light)，又称全向光源/泛光源/泛光灯(omnidirectional light/omni light)，
是指一个无限小的点，向所有光向平均地散射光。
需要确定光源的位置，还有就是光的颜色。*/
void MarsWithCameraModel::snapshotMarsByPointLight(cv::Mat &Image, float x, float y, float z, bool isShadow) {
    PointLight light(Color::white().multiply(2), GVector3(x, y, z), isShadow);
    pixelLocation = vector<vector<cv::Vec3f>>(width, vector<cv::Vec3f>(height, cv::Vec3f(0, 0, 0)));
    float dx=1.0f/width;
    float dy=1.0f/height;
    float sy, sx;
    float rgb[3];
    for (int y = 0; y < width; ++y) {
        sy = 1 - dy*y;
        for (int x = 0; x < height; ++x) {
            sx =dx*x;
            CRay ray(camera.generateRay(sx, sy));
            IntersectResult result = sphereModel->isIntersected(ray);
            if (result.isHit) {
                float p_x = result.position.x;
                float p_y = result.position.y;
                float p_z = result.position.z;
                if(p_x == 0 && p_y==0 && p_z==0){
                    continue;
                }
                GeoData->GetPixelValueByXYZ2(p_x, p_y, p_z, rgb);

                cv::Vec3b pixel;
                Color color=light.intersect(*sphereModel,result, rgb);
                pixel[0] = static_cast<uchar>(lround(std::min(color.b*255,(float)255.0)));
                pixel[1] = static_cast<uchar>(lround(std::min(color.g*255,(float)255.0)));
                pixel[2] = static_cast<uchar>(lround(std::min(color.r*255,(float)255.0)));
                Image.at<cv::Vec3b>(y, x) = pixel;
            }
        }
    }
}