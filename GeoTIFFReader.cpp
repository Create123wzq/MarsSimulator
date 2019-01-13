//
// Created by z on 9/23/18.
//

#include "GeoTIFFReader.h"

GeoTIFFReader::GeoTIFFReader(std::string name, bool membuff, GDALAccess access) {
    geoFile = name;
    useMemBuff = membuff;

    GDALAllRegister();
    // buffer
    if (useMemBuff) {
        FILE *pFile = fopen(name.c_str(), "rb");
        fseek(pFile, 0, SEEK_END);
        size_t size = ftell(pFile);
        rewind(pFile);
        buffer = new GByte[size];
        fread(buffer, 1, size, pFile);
        fclose(pFile);
        VSIFCloseL(VSIFileFromMemBuffer("/vsimem/work", buffer, size, FALSE));
        geoFile = "/vsimem/work";
        std::cout << " Buff created done!" << std::endl;
    }
    geoData = (GDALDataset*)GDALOpen(geoFile.c_str(), access);
    if (useMemBuff) {
        VSIUnlink("/vsimem/work");
        std::cout << " Unlink MemBuff done!" << std::endl;
    }
    if (!geoData) {
        std::cerr << "Error When Open the Tiff File: " << name << std::endl;
        exit(0);
    }
    // coordinate
    if (PROJ.importFromWkt(geoData->GetProjectionRef()) == OGRERR_CORRUPT_DATA)
        exit(0);

    GEO = PROJ.CloneGeogCS();
    GEO2PROJ = OGRCreateCoordinateTransformation(GEO, &PROJ);
    PROJ2GEO = OGRCreateCoordinateTransformation(&PROJ, GEO);

    // rasters
    RasterBandR = geoData->GetRasterBand(1);
    RasterBandG = geoData->GetRasterBand(2);
    RasterBandB = geoData->GetRasterBand(3);
    if (!RasterBandR || !RasterBandG || !RasterBandB)
        exit(0);
    // size
    geoTiffPixelWidth = geoData->GetRasterXSize();
    geoTiffPixelHeight = geoData->GetRasterYSize();
    if (geoData->GetGeoTransform(adfGeoTransform) == CE_Failure)
        exit(0);
    SinglePixelWidth = adfGeoTransform[1];
}
bool GeoTIFFReader::PixelLocation2Projection(int offset_x, int offset_y, double &x, double &y) {
    if (!PixelIsLegal(offset_x, offset_y))
        return false;
    x = adfGeoTransform[0] + offset_x * adfGeoTransform[1] + offset_y * adfGeoTransform[2];
    y = adfGeoTransform[3] + offset_x * adfGeoTransform[4] + offset_y * adfGeoTransform[5];
    return true;
}


bool GeoTIFFReader::Projection2GeoLocation(double x, double y, double &lon, double &lat) {
    auto status = PROJ2GEO->Transform(1, &x, &y);
    if (status == FALSE)
        return false;
    lon = x;
    lat = y;
    return true;
}

bool GeoTIFFReader::GeoLocation2Space(double lon, double lat, double &x, double &y, double &z) {
    x = R * cos(lat * M_PI / 180) * cos(lon * M_PI / 180);
    y = R * cos(lat * M_PI / 180) * sin(lon * M_PI / 180);
    z = R * sin(lat * M_PI / 180);
    return true;
}

bool GeoTIFFReader::SpaceLocation2Geo(double x, double y, double z, double &lon, double &lat) {
    lat = asin(z / R) *180 / M_PI;
    lon = atan2(y, x) * 180 / M_PI;
    return true;
}

bool GeoTIFFReader::GeoLocation2Proj(double lon, double lat, double &x, double &y) {
    if( GEO2PROJ == NULL || !GEO2PROJ->Transform(1, &lon, &lat))
        return false;
    x = lon;
    y = lat;
    return true;
}

bool GeoTIFFReader::Projcection2Pixel(double x, double y, int &offset_x, int &offset_y) {
    double n3 = adfGeoTransform[2] * (y-adfGeoTransform[3]) + adfGeoTransform[5] * (adfGeoTransform[0] - x);
    double n4 = adfGeoTransform[1] * (y-adfGeoTransform[3]) + adfGeoTransform[4] * (adfGeoTransform[0]- x);
    double n5 = adfGeoTransform[2] * adfGeoTransform[4]-adfGeoTransform[1] * adfGeoTransform[5];
    offset_x = (int)(n3 / n5);
    offset_y = (int)(n4 / -n5);
    return true;
}

bool GeoTIFFReader::GetPixelValueByXYZ1(float x, float y, float z, float *rgb) {
    double lat = asin(z / R) *180 / M_PI;
    double lon = atan2(y, x) * 180 / M_PI;
    if( !GEO2PROJ || !GEO2PROJ->Transform(1, &lon, &lat))
        return false;
    double n3 = adfGeoTransform[2] * (lat-adfGeoTransform[3]) + adfGeoTransform[5] * (adfGeoTransform[0] - lon);
    double n4 = adfGeoTransform[1] * (lat-adfGeoTransform[3]) + adfGeoTransform[4] * (adfGeoTransform[0]- lon);
    double n5 = adfGeoTransform[2] * adfGeoTransform[4]-adfGeoTransform[1] * adfGeoTransform[5];
    int offset_x = (int)(n3 / n5);
    int offset_y = (int)(n4 / -n5);
    if (offset_x >= geoTiffPixelWidth)
        offset_x = geoTiffPixelWidth - 1;
    if (offset_y >= geoTiffPixelHeight)
        offset_y = geoTiffPixelHeight - 1;
    auto R = RasterBandR->RasterIO(GF_Read, offset_x, offset_y, 1, 1, &rgb[0], 1, 1, GDT_Float32, 0, 0);
    auto G = RasterBandG->RasterIO(GF_Read, offset_x, offset_y, 1, 1, &rgb[1], 1, 1, GDT_Float32, 0, 0);
    auto B = RasterBandB->RasterIO(GF_Read, offset_x, offset_y, 1, 1, &rgb[2], 1, 1, GDT_Float32, 0, 0);
    if (R == CE_Failure || G == CE_Failure || B == CE_Failure)
        exit(0);
    return true;
}

bool GeoTIFFReader::GetPixelValueByXYZ2(float x, float y, float z, float *rgb) {
    double lat = asin(z / R) *180 / M_PI;
    double lon = atan2(y, x) * 180 / M_PI;
    if( !GEO2PROJ || !GEO2PROJ->Transform(1, &lon, &lat))
        return false;
    double n3 = adfGeoTransform[2] * (lat-adfGeoTransform[3]) + adfGeoTransform[5] * (adfGeoTransform[0] - lon);
    double n4 = adfGeoTransform[1] * (lat-adfGeoTransform[3]) + adfGeoTransform[4] * (adfGeoTransform[0]- lon);
    double n5 = adfGeoTransform[2] * adfGeoTransform[4]-adfGeoTransform[1] * adfGeoTransform[5];
    float offset_x = (float)(n3 / n5);
    float offset_y = (float)(n4 / -n5);

    // 二次线性插值
    int x1 = floorf(offset_x);
    int y1 = floorf(offset_y);
    if (x1+1 >= geoTiffPixelWidth)
        x1 = geoTiffPixelWidth - 2;
    if (y1+1 >= geoTiffPixelHeight)
        y1 = geoTiffPixelHeight - 2;

    int x2 = x1+1;
    int y2 = y1+1;

    auto R = RasterBandR->RasterIO(GF_Read, x1, y1, 2, 2, pafScanline1, 2, 2, GDT_Float32, 0, 0);
    auto G = RasterBandG->RasterIO(GF_Read, x1, y1, 2, 2, pafScanline2, 2, 2, GDT_Float32, 0, 0);
    auto B = RasterBandB->RasterIO(GF_Read, x1, y1, 2, 2, pafScanline3, 2, 2, GDT_Float32, 0, 0);
    if (R == CE_Failure || G == CE_Failure || B == CE_Failure)
        exit(0);

    float m1 = (x2-offset_x)*(y2-offset_y);
    float m2 = (offset_x-x1)*(y2-offset_y);
    float m3 = (x2-offset_x)*(offset_y-y1);
    float m4 = (offset_x-x1)*(offset_y-y1);

    rgb[0] = m1*pafScanline1[2] + m2*pafScanline1[3] + m3*pafScanline1[0] + m4*pafScanline1[1];
    rgb[1] = m1*pafScanline2[2] + m2*pafScanline2[3] + m3*pafScanline2[0] + m4*pafScanline2[1];
    rgb[2] = m1*pafScanline3[2] + m2*pafScanline3[3] + m3*pafScanline3[0] + m4*pafScanline3[1];

    return true;
}

bool GeoTIFFReader::GetGrayPixelValueByXYZ2(float x, float y, float z, float *value) {
    double lat = asin(z / R) *180 / M_PI;
    double lon = atan2(y, x) * 180 / M_PI;
    if( !GEO2PROJ || !GEO2PROJ->Transform(1, &lon, &lat))
        return false;
    double n3 = adfGeoTransform[2] * (lat-adfGeoTransform[3]) + adfGeoTransform[5] * (adfGeoTransform[0] - lon);
    double n4 = adfGeoTransform[1] * (lat-adfGeoTransform[3]) + adfGeoTransform[4] * (adfGeoTransform[0]- lon);
    double n5 = adfGeoTransform[2] * adfGeoTransform[4]-adfGeoTransform[1] * adfGeoTransform[5];

    float offset_x = (float)(n3 / n5);
    float offset_y = (float)(n4 / -n5);
    //二次线性插值
    int x1 = floorf(offset_x);
    int y1 = floorf(offset_y);
    if (x1+1 >= geoTiffPixelWidth)
        x1 = geoTiffPixelWidth - 2;
    if (y1+1 >= geoTiffPixelHeight)
        y1 = geoTiffPixelHeight - 2;

    int x2 = x1+1;
    int y2 = y1+1;

    float* pafScanline;
    pafScanline = (float*)CPLMalloc(sizeof(float) * 2 * 2);
    auto R = RasterBandGray->RasterIO(GF_Read, x1, y1, 2, 2, pafScanline, 2, 2, GDT_Float32, 0, 0);
    if (R == CE_Failure)
        exit(0);

    float m1 = (x2-offset_x)*(y2-offset_y);
    float m2 = (offset_x-x1)*(y2-offset_y);
    float m3 = (x2-offset_x)*(offset_y-y1);
    float m4 = (offset_x-x1)*(offset_y-y1);

    *value = m1*pafScanline[2] + m2*pafScanline[3] + m3*pafScanline[0] + m4*pafScanline[1];

    CPLFree(pafScanline);
    return true;
}
