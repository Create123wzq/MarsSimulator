#include <iostream>
#include <string>
#include <sys/time.h>

#include "MarsWithCameraModel.h"
#include "GeoTIFFReader.h"
#include "csphere.h"
#include "perspectiveCamera.h"
#include "gvector3.h"
#include "cray.h"

float nextAngle(float angle, float d) {
    float where = angle + d;
    if (where > 2 * PI) {
        where = angle + d - 2 * PI;
    }
    return where;
}

/*
 * 将绘图功能从程序中分离出来
 * mars_path 火星数据存储路径
 * use_mem_buffer 是否使用缓存
 * height,width 生成图片的高宽
 * x,y,z 相机的位置
 * fov 视野大小
 * count 图片计数
 */
void draw(std::string mars_path, bool use_mem_buffer, size_t width, size_t height, float x, float y, float z, float fov, int count){
    MarsWithCameraModel marsWithCamera(mars_path, use_mem_buffer);
    marsWithCamera.setPhotoParameters(width, height);
    cv::Mat Image(width, height, CV_8UC3, cv::Scalar::all(0));
    marsWithCamera.setCameraParameters(GVector3(x, y, z), fov);

    // 平行光源 参数：图像、光源方向向量x坐标，y坐标，z坐标、是否有阴影（场景里只有一个球，所以加不加无所谓）
    marsWithCamera.snapshotMarsByDirectLight(Image, 1, 1, 1, true);

    //点光源 参数：图像、点光源位置x坐标，y坐标，z坐标、是否有阴影
    // marsWithCamera.snapshotMarsByPointLight(Image, 1.75*2*R, 2*2*R, 1.5*2*R, true);
    // 无光源
    //marsWithCamera.snapshotMars(Image);

    cv::imwrite(to_string(count) +".png", Image);
}
/*
 * 测试绘图和参数生成两部分分离后是否能成功生成图像
 */
void run2(std::string mars_path, bool use_mem_buffer){
    double timeuse;
    struct timeval t1,t2;
    // size of photo
    size_t width = 2048, height = 2048;
    // paras of the orbit
    // angle: 每个时刻相机位置， dAngle： 每次变化量， a， b， c： x, y, z 方向轴长， offset： x方向偏移量，为了让火星在焦点上
    double angle = 0, dAngle = 2 * PI / 100, a = 80 * R, b = 35 * R, c = 20 * R, offset = 40 * R;
    int count = 0;
    while (true) {
        cout << " ---------------------- " << endl;
        draw(mars_path, use_mem_buffer, width, height, a * cos(angle) + offset, b * sin(angle), c * cos(angle), 10, count++);
        gettimeofday(&t2,NULL);
        timeuse = t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) / 1000000.0;
        std::cout << " time: " << timeuse << std::endl;

        angle = nextAngle(angle, dAngle);
        angle += dAngle;
    }
}

void run(std::string mars_path, bool use_mem_buffer)
{
    double timeuse;
    struct timeval t1,t2;
    // size of photo
    size_t width = 2048, height = 2048;
    // paras of the orbit
    // angle: 每个时刻相机位置， dAngle： 每次变化量， a， b， c： x, y, z 方向轴长， offset： x方向偏移量，为了让火星在焦点上
    double angle = 0, dAngle = 2 * PI / 100, a = 80 * R, b = 35 * R, c = 20 * R, offset = 40 * R;
    MarsWithCameraModel marsWithCamera(mars_path, use_mem_buffer);
    marsWithCamera.setPhotoParameters(width, height);
    int count = 0;
    while (true) {
        cout << " ---------------------- " << endl;
        cv::Mat Image(width, height, CV_8UC3, cv::Scalar::all(0));
        marsWithCamera.setCameraParameters(GVector3(a * cos(angle) + offset, b * sin(angle), c * cos(angle)), 10);

        // count time for begin
        gettimeofday(&t1,NULL);

        // 平行光源 参数：图像、光源方向向量x坐标，y坐标，z坐标、是否有阴影（场景里只有一个球，所以加不加无所谓）
        marsWithCamera.snapshotMarsByDirectLight(Image, 1, 1, 1, true);

        //点光源 参数：图像、点光源位置x坐标，y坐标，z坐标、是否有阴影
        // marsWithCamera.snapshotMarsByPointLight(Image, 1.75*2*R, 2*2*R, 1.5*2*R, true);
        // 无光源
        //marsWithCamera.snapshotMars(Image);

        // count time for end
        gettimeofday(&t2,NULL);
        timeuse = t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) / 1000000.0;
        std::cout << " time: " << timeuse << std::endl;

        angle = nextAngle(angle, dAngle);
        angle += dAngle;

        cv::imwrite(to_string(count++) +".png", Image);
    }
}

int main() {
    bool use_mem_buffer = false;
    std::string mars_path = "/Users/apple/Desktop/test.tif";
    run2(mars_path, use_mem_buffer);
    return 0;
}
