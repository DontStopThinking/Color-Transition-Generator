#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma region(TIME MEASURE MACROS)

// Define macros for start and end measurement using QueryPerformanceCounter
#define startMeasure() \
    LARGE_INTEGER __frequency; \
    LARGE_INTEGER __start; \
    QueryPerformanceFrequency(&__frequency); \
    QueryPerformanceCounter(&__start);

#define endMeasure() \
    LARGE_INTEGER __end; \
    QueryPerformanceCounter(&__end); \
    float __elapsedTimeS = static_cast<float>(__end.QuadPart - __start.QuadPart) / __frequency.QuadPart; \
    float __elapsedTimeMS = __elapsedTimeS * 1000.0f; \
    printf("\n"); \
    printf("Took: %.4f ms\n", __elapsedTimeMS); \
    printf("Took: %.4f s\n", __elapsedTimeS); \

#pragma endregion

using u32 = unsigned int;

struct RGBColor
{
    int r;
    int g;
    int b;
};

constinit RGBColor g_BrightBlue = { 0, 168, 255 };
constinit RGBColor g_Purple = { 128, 0, 128 };
constinit RGBColor g_Pink = { 255, 105, 180 };
constinit RGBColor g_Red = { 255, 36, 0 };
constinit RGBColor g_Gold = { 255, 175, 0 };

constexpr u32 g_HexColorStringSize = 8; // NOTE(sbalse): 8 to accommodate for the # and the '\0'. E.g. "#RRGGBB\0"

static RGBColor HexToRGB(const char* hexColor)
{
    RGBColor result = {};
    if (sscanf_s(hexColor + 1, "%02X%02X%02X", &result.r, &result.g, &result.b) != 3)
    {
        result = { 0xFF, 0xFF, 0xFF };
    }
    return result;
}

static void RGBToHex(const RGBColor color, char* hexColor)
{
    // NOTE(sbalse): Casting to void to silence "unused return value" warning
    (void)sprintf_s(hexColor, g_HexColorStringSize, "#%02X%02X%02X", color.r, color.g, color.b);
}

static RGBColor InterpolateColors(const RGBColor color1, const RGBColor color2, const double t)
{
    return RGBColor
    {
        static_cast<int>(color1.r + (color2.r - color1.r) * t),
        static_cast<int>(color1.g + (color2.g - color1.g) * t),
        static_cast<int>(color1.b + (color2.b - color1.b) * t)
    };
}

static void GenerateColorGradient(const u32 numColors, char gradientColors[][g_HexColorStringSize])
{
    for (u32 i = 0; i < numColors; i++)
    {
        double t = static_cast<double>(i) / (numColors - 1);

        RGBColor color = {};

        if (t <= 0.3f)
        {
            // NOTE(sbalse): Bright blue to purple for the first 30%
            color = InterpolateColors(g_BrightBlue, g_Purple, t / 0.3f);
        }
        else if (t > 0.3f && t <= 0.5f)
        {
            // NOTE(sbalse): Purple to pink for the next 20%
            color = InterpolateColors(g_Purple, g_Pink, (t - 0.3f) / 0.2f);
        }
        else if (t > 0.5f && t <= 0.7f)
        {
            // NOTE(sbalse): Pink to red for the next 20%
            color = InterpolateColors(g_Pink, g_Red, (t - 0.5f) / 0.2f);
        }
        else if (t > 0.7f)
        {
            // NOTE(sbalse): Red to gold for the remaining
            color = InterpolateColors(g_Red, g_Gold, (t - 0.7f) / 0.3f);
        }

        RGBToHex(color, gradientColors[i]);
    }
}

static void PrintHexColor(const char* hexColor)
{
    const RGBColor rgb = HexToRGB(hexColor);
    printf("%s: \033[48;2;%d;%d;%dm   \033[0m\n", hexColor, rgb.r, rgb.g, rgb.b);
}

int main(int argc, char* argv[])
{
    constexpr u32 numColors = 20;

    char gradientColors[numColors][g_HexColorStringSize] = {};

    u32 width = static_cast<u32>(log10(numColors - 1)) + 1;

    startMeasure();

    GenerateColorGradient(numColors, gradientColors);

    for (u32 i = 0; i < numColors; i++)
    {
        printf("%*d. ", width, i);
        PrintHexColor(gradientColors[i]);
    }

    endMeasure();

    return 0;
}
