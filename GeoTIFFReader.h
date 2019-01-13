//
// Created by z on 9/23/18.
//

#ifndef MARSSIMULATOR_GEOTIFFREADER_H
#define MARSSIMULATOR_GEOTIFFREADER_H

#include <string>
#include <cmath>
#include <gdal_priv.h>
#include <opencv2/opencv.hpp>

// radius of mars
const double R = 3396190; //m

class GeoTIFFReader {
public:

    GeoTIFFReader() {}
    explicit GeoTIFFReader(std::string name, bool membuff=false, GDALAccess access=GA_ReadOnly);

    ~GeoTIFFReader() {
        if (geoData)
           GDALClose(geoData);
        if (buffer)
            delete[] buffer;
    }
    bool PixelLocation2Projection(int offset_x, int offset_y, double &x, double &y);

    bool Projection2GeoLocation(double x, double y, double &lon, double &lat);

    bool GeoLocation2Space(double lon, double lat, double &x, double &y, double &z);

    bool SpaceLocation2Geo(double x, double y, double z, double &lon, double &lat);

    bool GeoLocation2Proj(double lon, double lat, double &x, double &y);

    bool Projcection2Pixel(double x, double y, int &offset_x, int &offset_y);

    bool GetPixelValueByXYZ1(float x, float y, float z, float rgb[3]);

    bool GetPixelValueByXYZ2(float x, float y, float z, float rgb[3]);

    bool GetGrayPixelValueByXYZ2(float x, float y, float z, float *value);

    inline double getRadius() {
        return (geoTiffPixelWidth * SinglePixelWidth) / (2 * M_PI);
    }
private:
    inline bool PixelIsLegal(int x, int y) {
        return (x <= geoTiffPixelWidth) && (y <= geoTiffPixelHeight);
    }

private:
    bool useMemBuff = false;
    std::string geoFile;
    GDALDataset *geoData = nullptr;
    // memory buffer
    GByte *buffer = nullptr;

    // coordinate system
    OGRSpatialReference *GEO, PROJ;
    // tool for coordinate transformation
    OGRCoordinateTransformation *GEO2PROJ = nullptr, *PROJ2GEO = nullptr;
    // raster for reading gray value
    GDALRasterBand *RasterBandGray = nullptr;
    // rasters for reading rgb values
    GDALRasterBand *RasterBandR = nullptr, *RasterBandG = nullptr, *RasterBandB = nullptr;

    // for Quadratic linear interpolation
    float pafScanline1[4], pafScanline2[4], pafScanline3[4];

    double adfGeoTransform[6];

    int geoTiffPixelWidth = 0;
    int geoTiffPixelHeight = 0;
    double SinglePixelWidth = 0;
};


#endif //MARSSIMULATOR_GEOTIFFREADER_H
