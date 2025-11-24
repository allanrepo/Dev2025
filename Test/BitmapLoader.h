#pragma once
#include <string>
#include <vector>

namespace utilities::imageio
{
    // Represents raw bitmap image data loaded from disk.
    // Stores pixel dimensions and RGBA pixel buffer. 
    // Intended for use with BitmapFileLoader.
    class BitmapImageData 
    {
    private:

        // bitmap image size
        unsigned int width = 0;
        unsigned int height = 0;

        // array of bytes, every 4 bytes is RGBA data
        std::vector<uint8_t> pixels; 

        friend class BitmapFileLoader;

    public:
        BitmapImageData() = default;

        inline int GetWidth() const 
        { 
            return width; 
        }
        inline int GetHeight() const 
        { 
            return height; 
        }
        inline const std::vector<uint8_t>& GetPixels() const 
        { 
            return pixels; 
        }
    };
    
    // Static loader for bitmap (.bmp) files.
    // Parses BMP headers and pixel data into BitmapImageData.
    class BitmapFileLoader
    {
    public:
        // Loads a bitmap file from disk.
        // filepath - path to the .bmp file.
        // data - output container for image dimensions and pixel buffer.
        // Returns true if loading succeeds, false otherwise.
        static bool Load(const std::wstring& filepath, BitmapImageData& data);
    };
}

