#include "BitmapLoader.h"
#include "Logger.h"
#include <fstream>
#include <stdexcept>

// BMP headers must be tightly packed to match file layout
#pragma pack(push, 1)

// Represents the 14-byte BMP file header
struct BmpFileHeader {
    uint16_t bfType;       // Signature, must be 'BM' (0x4D42)
    uint32_t bfSize;       // Total file size in bytes
    uint16_t bfReserved1;  // Reserved, unused
    uint16_t bfReserved2;  // Reserved, unused
    uint32_t bfOffBits;    // Offset from start of file to pixel data
};

// Represents the 40-byte BMP info header (BITMAPINFOHEADER)
struct BmpInfoHeader {
    uint32_t biSize;          // Size of this header (should be 40)
    int32_t  biWidth;         // Image width in pixels
    int32_t  biHeight;        // Image height in pixels (positive = bottom-up)
    uint16_t biPlanes;        // Number of color planes (must be 1)
    uint16_t biBitCount;      // Bits per pixel (24 or 32)
    uint32_t biCompression;   // Compression type (0 = uncompressed)
    uint32_t biSizeImage;     // Size of pixel data (can be 0 for BI_RGB)
    int32_t  biXPelsPerMeter; // Horizontal resolution (pixels per meter)
    int32_t  biYPelsPerMeter; // Vertical resolution (pixels per meter)
    uint32_t biClrUsed;       // Number of colors in palette (0 = all)
    uint32_t biClrImportant;  // Number of important colors (0 = all)
};

bool utilities::imageio::BitmapFileLoader::Load(const std::wstring& filepath, BitmapImageData& data)
{
    // Open the BMP file in binary mode
    std::ifstream file(filepath, std::ios::binary);
    if (!file)
    {
        // Log error if file cannot be opened
        LOGERROR("Failed to open BMP file");
        return false;
    }

    BmpFileHeader fileHeader = {};
    BmpInfoHeader infoHeader = {};

    // Read file and info headers from the BMP file
    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    // Validate BMP signature ('BM' = 0x4D42)
    if (fileHeader.bfType != 0x4D42)
    {
        LOGERROR(filepath.c_str() << " is not a BMP file");
        return false;
    }

    // Reject compressed BMP formats (only BI_RGB supported)
    if (infoHeader.biCompression != 0)
    {
        LOGERROR("Compressed BMP not supported");
        return false;
    }

    // Extract image dimensions and determine vertical orientation
    data.width = infoHeader.biWidth;
    data.height = std::abs(infoHeader.biHeight);
    bool flipY = infoHeader.biHeight > 0; // BMP stores bottom-up if height > 0

    // Calculate bytes per pixel and row pitch (aligned to 4 bytes)
    int bytesPerPixel = infoHeader.biBitCount / 8;
    int rowPitch = ((data.width * bytesPerPixel + 3) & ~3); // Align to 4-byte boundary

    // Allocate buffer for raw BMP pixel data
    std::vector<uint8_t> rawData(rowPitch * data.height);

    // Seek to pixel data offset and read into buffer
    file.seekg(fileHeader.bfOffBits);
    file.read(reinterpret_cast<char*>(rawData.data()), rawData.size());

    // Allocate RGBA output buffer (4 bytes per pixel)
    data.pixels.resize(data.width * data.height * 4);

    // Convert BMP pixel data to RGBA format
    for (unsigned int y = 0; y < data.height; ++y) 
    {
        // Flip vertically if BMP is bottom-up
        int srcY = flipY ? (data.height - 1 - y) : y;

        const uint8_t* srcRow = rawData.data() + srcY * rowPitch;
        uint8_t* dstRow = data.pixels.data() + y * data.width * 4;

        for (unsigned int x = 0; x < data.width; ++x) 
        {
            const uint8_t* srcPixel = srcRow + x * bytesPerPixel;

            // BMP stores pixels as BGR or BGRA
            dstRow[x * 4 + 0] = srcPixel[2];                            // Red
            dstRow[x * 4 + 1] = srcPixel[1];                            // Green
            dstRow[x * 4 + 2] = srcPixel[0];                            // Blue
            dstRow[x * 4 + 3] = (bytesPerPixel == 4)? srcPixel[3]:255;  // Alpha (255 if no alpha channel)
        }
    }

    // Successfully loaded and converted BMP data
    return true;
}
