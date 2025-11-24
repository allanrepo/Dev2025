//#include "PNGLoader.h"
//#include <wrl/client.h>
//#include <vector>
//
//using Microsoft::WRL::ComPtr;
//
//namespace utilities::imageio
//{
//    HRESULT LoadTextureFromFile(
//        ID3D11Device* device,
//        ID3D11DeviceContext* context,
//        const wchar_t* filename,
//        ID3D11ShaderResourceView** textureView)
//    {
//        if (!device || !filename || !textureView)
//            return E_INVALIDARG;
//
//        ComPtr<IWICImagingFactory> factory;
//        HRESULT hr = CoCreateInstance(
//            CLSID_WICImagingFactory,
//            nullptr,
//            CLSCTX_INPROC_SERVER,
//            IID_PPV_ARGS(&factory));
//        if (FAILED(hr)) return hr;
//
//        ComPtr<IWICBitmapDecoder> decoder;
//        hr = factory->CreateDecoderFromFilename(
//            filename,
//            nullptr,
//            GENERIC_READ,
//            WICDecodeMetadataCacheOnDemand,
//            &decoder);
//        if (FAILED(hr)) return hr;
//
//        ComPtr<IWICBitmapFrameDecode> frame;
//        hr = decoder->GetFrame(0, &frame);
//        if (FAILED(hr)) return hr;
//
//        ComPtr<IWICFormatConverter> converter;
//        hr = factory->CreateFormatConverter(&converter);
//        if (FAILED(hr)) return hr;
//
//        hr = converter->Initialize(
//            frame.Get(),
//            GUID_WICPixelFormat32bppRGBA,
//            WICBitmapDitherTypeNone,
//            nullptr,
//            0.0f,
//            WICBitmapPaletteTypeCustom);
//        if (FAILED(hr)) return hr;
//
//        UINT width, height;
//        hr = converter->GetSize(&width, &height);
//        if (FAILED(hr)) return hr;
//
//        std::vector<uint8_t> pixels(width * height * 4);
//        hr = converter->CopyPixels(
//            nullptr,
//            width * 4,
//            static_cast<UINT>(pixels.size()),
//            pixels.data());
//        if (FAILED(hr)) return hr;
//
//        D3D11_TEXTURE2D_DESC texDesc = {};
//        texDesc.Width = width;
//        texDesc.Height = height;
//        texDesc.MipLevels = 1;
//        texDesc.ArraySize = 1;
//        texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//        texDesc.SampleDesc.Count = 1;
//        texDesc.Usage = D3D11_USAGE_DEFAULT;
//        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//        texDesc.CPUAccessFlags = 0;
//        texDesc.MiscFlags = 0;
//
//        D3D11_SUBRESOURCE_DATA initData = {};
//        initData.pSysMem = pixels.data();
//        initData.SysMemPitch = width * 4;
//
//        ComPtr<ID3D11Texture2D> texture;
//        hr = device->CreateTexture2D(&texDesc, &initData, &texture);
//        if (FAILED(hr)) return hr;
//
//        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
//        srvDesc.Format = texDesc.Format;
//        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//        srvDesc.Texture2D.MipLevels = 1;
//
//        hr = device->CreateShaderResourceView(texture.Get(), &srvDesc, textureView);
//        return hr;
//    }
//}
