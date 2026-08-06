#include <d3d11.h>
#include <dxgi.h>
#include <cstdio>

int main() {
    // Juste vérifier que DirectX11 est accessible
    D3D_FEATURE_LEVEL featureLevel;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;

    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &device,
        &featureLevel,
        &context
    );

    if (SUCCEEDED(hr)) {
        printf("DirectX11 OK\n");
    } else {
        printf("DirectX11 FAILED\n");
    }

    return 0;
}
