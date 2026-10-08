/**********************************************************************************************
*
*   rcore_nx - Functions to manage window, graphics device and inputs
*
*   PLATFORM: NX
*       - Switch (LibNX)
*
*   CONFIGURATION:
*       #define NX_DISABLE_GAMEPAD_EMULATION
*           Enabling this flag disables gamepad to set keyboard and mouse states
*       #define NX_USB_DEBUGGER
*           Enabling this flag allows debugging by USB, the application will wait for an USB connection to start
*
*   DEPENDENCIES:
*       - LibNX: Provides C API to access Switch functionality
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2021-2023 Luiz Pestana (@luizpestana) and contributors
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include <switch.h>
#include <EGL/egl.h>    // EGL library
#include <EGL/eglext.h> // EGL extensions
#include <GLES2/gl2.h>  // OpenGL ES 2.0 library
#if defined(NX_USB_DEBUGGER)
    #include "nxusb.h"
#endif

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct {
    NWindow *gbmSurface;                // GBM surface

    long int touchDeltaTime[MAX_TOUCH_POINTS];
    s32 prevTouchCount;
    double lastInputTime;               // Time of the previous input poll for cursor movement

    PadState nxPad[MAX_GAMEPADS];
    HidNpadStyleTag nxPadStyle[MAX_GAMEPADS];

    // Display data
    EGLDisplay device;                  // Native display device (physical screen connection)
    EGLSurface surface;                 // Surface to draw on, framebuffers (connected to context)
    EGLContext context;                 // Graphic context, mode in which drawing can be done
    EGLConfig config;                   // Graphic config
    bool swapFailed;                    // Suppress repeated errors until presentation recovers
} PlatformData;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
extern CoreData CORE;                   // Global CORE state context

static PlatformData platform = { 0 };   // Platform specific data

//----------------------------------------------------------------------------------
// Module Internal Functions Declaration
//----------------------------------------------------------------------------------
int InitPlatform(void);          // Initialize platform (graphics, inputs and more)
static bool SetVSync(bool enabled); // Apply the swap interval and update the window flag on success

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
// NOTE: Functions declaration is provided by raylib.h

//----------------------------------------------------------------------------------
// Module Functions Definition: Window and Graphics Device
//----------------------------------------------------------------------------------

// Check if application should close
bool WindowShouldClose(void)
{
    if (!appletMainLoop()) return true;
    if (CORE.Window.ready) return CORE.Window.shouldClose;
    else return true;
}

// Toggle fullscreen mode
void ToggleFullscreen(void)
{
    TRACELOG(LOG_WARNING, "ToggleFullscreen() not available on target platform");
}

// Toggle borderless windowed mode
void ToggleBorderlessWindowed(void)
{
    TRACELOG(LOG_WARNING, "ToggleBorderlessWindowed() not available on target platform");
}

// Set window state: maximized, if resizable
void MaximizeWindow(void)
{
    TRACELOG(LOG_WARNING, "MaximizeWindow() not available on target platform");
}

// Set window state: minimized
void MinimizeWindow(void)
{
    TRACELOG(LOG_WARNING, "MinimizeWindow() not available on target platform");
}

// Set window state: not minimized/maximized
void RestoreWindow(void)
{
    TRACELOG(LOG_WARNING, "RestoreWindow() not available on target platform");
}

// Set window configuration state using flags
void SetWindowState(unsigned int flags)
{
    if (!CORE.Window.ready)
    {
        TRACELOG(LOG_WARNING, "WINDOW: Use SetConfigFlags() before window initialization");
        return;
    }

    if (FLAG_IS_SET(flags, FLAG_VSYNC_HINT)) SetVSync(true);
    if (flags & ~(FLAG_VSYNC_HINT | FLAG_FULLSCREEN_MODE))
        TRACELOG(LOG_WARNING, "WINDOW: Requested flags other than VSync and fullscreen are not supported on NX");
}

// Clear window configuration state flags
void ClearWindowState(unsigned int flags)
{
    if (!CORE.Window.ready)
    {
        TRACELOG(LOG_WARNING, "WINDOW: ClearWindowState() requires an initialized window");
        return;
    }

    if (FLAG_IS_SET(flags, FLAG_VSYNC_HINT)) SetVSync(false);
    if (flags & ~FLAG_VSYNC_HINT)
        TRACELOG(LOG_WARNING, "WINDOW: Only VSync can be cleared on NX (fullscreen is required)");
}

// Set icon for window
void SetWindowIcon(Image image)
{
    TRACELOG(LOG_WARNING, "SetWindowIcon() not available on target platform");
}

// Set icon for window
void SetWindowIcons(Image *images, int count)
{
    TRACELOG(LOG_WARNING, "SetWindowIcons() not available on target platform");
}

// Set title for window
void SetWindowTitle(const char *title)
{
    CORE.Window.title = title;
}

// Set window position on screen (windowed mode)
void SetWindowPosition(int x, int y)
{
    TRACELOG(LOG_WARNING, "SetWindowPosition() not available on target platform");
}

// Set monitor for the current window
void SetWindowMonitor(int monitor)
{
    TRACELOG(LOG_WARNING, "SetWindowMonitor() not available on target platform");
}

// Set window minimum dimensions (FLAG_WINDOW_RESIZABLE)
void SetWindowMinSize(int width, int height)
{
    CORE.Window.screenMin.width = width;
    CORE.Window.screenMin.height = height;
}

// Set window maximum dimensions (FLAG_WINDOW_RESIZABLE)
void SetWindowMaxSize(int width, int height)
{
    CORE.Window.screenMax.width = width;
    CORE.Window.screenMax.height = height;
}

// Set window dimensions
void SetWindowSize(int width, int height)
{
    TRACELOG(LOG_WARNING, "WINDOW: NX buffer dimensions must be configured with InitWindow(), runtime resizing is not supported");
}

// Set window opacity, value opacity is between 0.0 and 1.0
void SetWindowOpacity(float opacity)
{
    TRACELOG(LOG_WARNING, "SetWindowOpacity() not available on target platform");
}

// Set window focused
void SetWindowFocused(void)
{
    TRACELOG(LOG_WARNING, "SetWindowFocused() not available on target platform");
}

// Get native window handle
void *GetWindowHandle(void)
{
    TRACELOG(LOG_WARNING, "GetWindowHandle() not implemented on target platform");
    return NULL;
}

// Get number of monitors
int GetMonitorCount(void)
{
    TRACELOG(LOG_WARNING, "GetMonitorCount() not implemented on target platform");
    return 1;
}

// Get number of monitors
int GetCurrentMonitor(void)
{
    TRACELOG(LOG_WARNING, "GetCurrentMonitor() not implemented on target platform");
    return 0;
}

// Get selected monitor position
Vector2 GetMonitorPosition(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorPosition() not implemented on target platform");
    return (Vector2){ 0, 0 };
}

// Get selected monitor width (currently used by monitor)
int GetMonitorWidth(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorWidth() not implemented on target platform");
    return 0;
}

// Get selected monitor height (currently used by monitor)
int GetMonitorHeight(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorHeight() not implemented on target platform");
    return 0;
}

// Get selected monitor physical width in millimetres
int GetMonitorPhysicalWidth(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorPhysicalWidth() not implemented on target platform");
    return 0;
}

// Get selected monitor physical height in millimetres
int GetMonitorPhysicalHeight(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorPhysicalHeight() not implemented on target platform");
    return 0;
}

// Get selected monitor refresh rate
int GetMonitorRefreshRate(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorRefreshRate() not implemented on target platform");
    return 0;
}

// Get the human-readable, UTF-8 encoded name of the selected monitor
const char *GetMonitorName(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorName() not implemented on target platform");
    return "";
}

// Get window position XY on monitor
Vector2 GetWindowPosition(void)
{
    TRACELOG(LOG_WARNING, "GetWindowPosition() not implemented on target platform");
    return (Vector2){ 0, 0 };
}

// Get window scale DPI factor for current monitor
Vector2 GetWindowScaleDPI(void)
{
    TRACELOG(LOG_WARNING, "GetWindowScaleDPI() not implemented on target platform");
    return (Vector2){ 1.0f, 1.0f };
}

// Set clipboard text content
void SetClipboardText(const char *text)
{
    TRACELOG(LOG_WARNING, "SetClipboardText() not implemented on target platform");
}

// Get clipboard text content
// NOTE: returned string is allocated and freed by GLFW
const char *GetClipboardText(void)
{
    TRACELOG(LOG_WARNING, "GetClipboardText() not implemented on target platform");
    return NULL;
}

// Get clipboard image
Image GetClipboardImage(void)
{
    Image image = { 0 };

    TRACELOG(LOG_WARNING, "GetClipboardImage() not implemented on target platform");

    return image;
}

// Show mouse cursor
void ShowCursor(void)
{
    CORE.Input.Mouse.cursorHidden = false;
}

// Hides mouse cursor
void HideCursor(void)
{
    CORE.Input.Mouse.cursorHidden = true;
}

// Enables cursor (unlock cursor)
void EnableCursor(void)
{
    // Set cursor position in the middle
    SetMousePosition(CORE.Window.screen.width/2, CORE.Window.screen.height/2);

    CORE.Input.Mouse.cursorHidden = false;
}

// Disables cursor (lock cursor)
void DisableCursor(void)
{
    // Set cursor position in the middle
    SetMousePosition(CORE.Window.screen.width/2, CORE.Window.screen.height/2);

    CORE.Input.Mouse.cursorHidden = true;
}

// Swap back buffer with front buffer (screen drawing)
void SwapScreenBuffer(void)
{
    if (eglSwapBuffers(platform.device, platform.surface) == EGL_FALSE)
    {
        EGLint error = eglGetError();
        if (!platform.swapFailed)
        {
            TRACELOG(LOG_WARNING, "DISPLAY: eglSwapBuffers() failed (EGL error: 0x%04x); further errors suppressed until presentation recovers", error);
        }
        platform.swapFailed = true;
    }
    else platform.swapFailed = false;
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Misc
//----------------------------------------------------------------------------------

// Get elapsed time measure in seconds since InitTimer()
double GetTime(void)
{
    double time = 0.0;
    struct timespec ts = { 0 };
    clock_gettime(CLOCK_MONOTONIC, &ts);
    unsigned long long int nanoSeconds = (unsigned long long int)ts.tv_sec*1000000000LLU + (unsigned long long int)ts.tv_nsec;

    time = (double)(nanoSeconds - CORE.Time.base)*1e-9;  // Elapsed time since InitTimer()

    return time;
}

// Open URL with default system browser (if available)
// NOTE: This function is only safe to use if you control the URL given.
// A user could craft a malicious string performing another action.
// Only call this function yourself not with user input or make sure to check the string yourself.
// Ref: https://github.com/raysan5/raylib/issues/686
void OpenURL(const char *url)
{
    // Security check to (partially) avoid malicious code on target platform
    if (strchr(url, '\'') != NULL) TRACELOG(LOG_WARNING, "SYSTEM: Provided URL could be potentially malicious, avoid [\'] character");
    else
    {
        // TODO:
    }
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Inputs
//----------------------------------------------------------------------------------

const char *GetNxGamePadName(int gamepad)
{
    switch(platform.nxPadStyle[gamepad]) {
        case HidNpadStyleTag_NpadFullKey:       return "Nintendo Switch Pro Controller";
        case HidNpadStyleTag_NpadHandheld:      return "Handheld Joy-Con controller";
        case HidNpadStyleTag_NpadJoyDual:       return "Dual Joy-Con controller";
        case HidNpadStyleTag_NpadJoyLeft:       return "Single Joy-Con left controller";
        case HidNpadStyleTag_NpadJoyRight:      return "Single Joy-Con right controller";
        case HidNpadStyleTag_NpadGc:            return "GameCube controller";
        case HidNpadStyleTag_NpadPalma:         return "Poké Ball Plus controller";
        case HidNpadStyleTag_NpadLark:          return "NES/Famicom controller";
        case HidNpadStyleTag_NpadHandheldLark:  return "Handheld NES/Famicom controller";
        case HidNpadStyleTag_NpadLucia:         return "SNES controller";
        case HidNpadStyleTag_NpadLagon:         return "N64 controller";
        case HidNpadStyleTag_NpadLager:         return "Sega Genesis controller";
        case HidNpadStyleTag_NpadSystemExt:     return "Generic external controller";
        default:                                return "Generic controller";
    }
}

// Set gamepad vibration
void SetGamepadVibration(int gamepad, float leftMotor, float rightMotor, float duration)
{
    TRACELOG(LOG_WARNING, "SetGamepadVibration() not implemented on target platform");
}

// Set internal gamepad mappings
int SetGamepadMappings(const char *mappings)
{
    TRACELOG(LOG_WARNING, "SetGamepadMappings() not implemented on target platform");
    return 0;
}

// Set mouse position XY
void SetMousePosition(int x, int y)
{
    CORE.Input.Mouse.currentPosition = (Vector2){ (float)x, (float)y };
    CORE.Input.Mouse.previousPosition = CORE.Input.Mouse.currentPosition;
}

// Set mouse cursor
void SetMouseCursor(int cursor)
{
    TRACELOG(LOG_WARNING, "SetMouseCursor() not implemented on target platform");
}

// NX keyboard emulation uses a fixed US QWERTY layout.
const char *GetKeyName(int key)
{
    static const char keyNames[KEY_GRAVE + 1][2] = {
        [KEY_SPACE] = " ", [KEY_APOSTROPHE] = "'", [KEY_COMMA] = ",",
        [KEY_MINUS] = "-", [KEY_PERIOD] = ".", [KEY_SLASH] = "/",
        [KEY_ZERO] = "0", [KEY_ONE] = "1", [KEY_TWO] = "2", [KEY_THREE] = "3",
        [KEY_FOUR] = "4", [KEY_FIVE] = "5", [KEY_SIX] = "6", [KEY_SEVEN] = "7",
        [KEY_EIGHT] = "8", [KEY_NINE] = "9", [KEY_SEMICOLON] = ";", [KEY_EQUAL] = "=",
        [KEY_A] = "a", [KEY_B] = "b", [KEY_C] = "c", [KEY_D] = "d",
        [KEY_E] = "e", [KEY_F] = "f", [KEY_G] = "g", [KEY_H] = "h",
        [KEY_I] = "i", [KEY_J] = "j", [KEY_K] = "k", [KEY_L] = "l",
        [KEY_M] = "m", [KEY_N] = "n", [KEY_O] = "o", [KEY_P] = "p",
        [KEY_Q] = "q", [KEY_R] = "r", [KEY_S] = "s", [KEY_T] = "t",
        [KEY_U] = "u", [KEY_V] = "v", [KEY_W] = "w", [KEY_X] = "x",
        [KEY_Y] = "y", [KEY_Z] = "z", [KEY_LEFT_BRACKET] = "[",
        [KEY_BACKSLASH] = "\\", [KEY_RIGHT_BRACKET] = "]", [KEY_GRAVE] = "\x60"
    };

    if ((key >= KEY_KP_0) && (key <= KEY_KP_9)) return keyNames[KEY_ZERO + key - KEY_KP_0];
    if ((key >= 0) && (key <= KEY_GRAVE) && (keyNames[key][0] != '\0')) return keyNames[key];

    switch (key)
    {
        case KEY_KP_DECIMAL: return ".";
        case KEY_KP_DIVIDE: return "/";
        case KEY_KP_MULTIPLY: return "*";
        case KEY_KP_SUBTRACT: return "-";
        case KEY_KP_ADD: return "+";
        case KEY_KP_EQUAL: return "=";
        default: return NULL;
    }
}

// Register all input events
void PollInputEvents(void)
{
#if SUPPORT_GESTURES_SYSTEM
    UpdateGestures();
    int previousTouchIds[MAX_TOUCH_POINTS];
#endif

    CORE.Input.Keyboard.keyPressedQueueCount = 0;
    CORE.Input.Keyboard.charPressedQueueCount = 0;
    CORE.Input.Gamepad.lastButtonPressed = GAMEPAD_BUTTON_UNKNOWN;

    // Snapshot each input source once, before rebuilding this frame's state.
    for (int i = 0; i < MAX_KEYBOARD_KEYS; i++)
    {
        CORE.Input.Keyboard.previousKeyState[i] = CORE.Input.Keyboard.currentKeyState[i];
        CORE.Input.Keyboard.currentKeyState[i] = 0;
        CORE.Input.Keyboard.keyRepeatInFrame[i] = 0;
    }

    for (int i = 0; i < MAX_MOUSE_BUTTONS; i++)
    {
        CORE.Input.Mouse.previousButtonState[i] = CORE.Input.Mouse.currentButtonState[i];
        CORE.Input.Mouse.currentButtonState[i] = 0;
    }
    CORE.Input.Mouse.previousPosition = CORE.Input.Mouse.currentPosition;
    CORE.Input.Mouse.previousWheelMove = CORE.Input.Mouse.currentWheelMove;
    CORE.Input.Mouse.currentWheelMove = (Vector2){ 0.0f, 0.0f };

    for (int i = 0; i < MAX_TOUCH_POINTS; i++)
    {
        CORE.Input.Touch.previousTouchState[i] = CORE.Input.Touch.currentTouchState[i];
        CORE.Input.Touch.previousPosition[i] = CORE.Input.Touch.position[i];
#if SUPPORT_GESTURES_SYSTEM
        previousTouchIds[i] = CORE.Input.Touch.pointId[i];
#endif
    }

    HidTouchScreenState state = { 0 };
    int touchCount = hidGetTouchScreenStates(&state, 1)? (int)state.count : 0;
    if (touchCount > MAX_TOUCH_POINTS) touchCount = MAX_TOUCH_POINTS;
#if SUPPORT_GESTURES_SYSTEM
    int previousTouchCount = platform.prevTouchCount;
#endif
    CORE.Input.Touch.pointCount = touchCount;

    for (int i = 0; i < touchCount; i++)
    {
        // HID coordinates use the handheld's 1280x720 space, regardless of buffer size.
        CORE.Input.Touch.position[i].x = (float)state.touches[i].x*CORE.Window.screen.width/1280.0f;
        CORE.Input.Touch.position[i].y = (float)state.touches[i].y*CORE.Window.screen.height/720.0f;
        CORE.Input.Touch.pointId[i] = state.touches[i].finger_id;
        CORE.Input.Touch.currentTouchState[i] = 1;
        platform.touchDeltaTime[i] = state.touches[i].delta_time;
    }

    for (int i = touchCount; i < MAX_TOUCH_POINTS; i++)
    {
        CORE.Input.Touch.currentTouchState[i] = 0;
        CORE.Input.Touch.pointId[i] = -1;
        CORE.Input.Touch.position[i] = (Vector2){ 0.0f, 0.0f };
        platform.touchDeltaTime[i] = 0;
    }

#if SUPPORT_GESTURES_SYSTEM
    int touchAction = -1;
    int gesturePointCount = touchCount;
    if (touchCount > previousTouchCount) touchAction = TOUCH_ACTION_DOWN;
    else if (touchCount < previousTouchCount)
    {
        touchAction = TOUCH_ACTION_UP;
        gesturePointCount = previousTouchCount;
    }
    else if (touchCount > 0) touchAction = TOUCH_ACTION_MOVE;

    if (touchAction >= 0)
    {
        GestureEvent gestureEvent = { 0 };
        gestureEvent.touchAction = touchAction;
        gestureEvent.pointCount = gesturePointCount;
        for (int i = 0; i < gestureEvent.pointCount; i++)
        {
            gestureEvent.pointId[i] = (touchAction == TOUCH_ACTION_UP)? previousTouchIds[i] : CORE.Input.Touch.pointId[i];
            gestureEvent.position[i] = (touchAction == TOUCH_ACTION_UP)? CORE.Input.Touch.previousPosition[i] : CORE.Input.Touch.position[i];
            gestureEvent.position[i].x /= (float)GetScreenWidth();
            gestureEvent.position[i].y /= (float)GetScreenHeight();
        }
        ProcessGestureEvent(gestureEvent);
    }
#endif

#if !defined(NX_DISABLE_GAMEPAD_EMULATION)
    u64 emulatedButtons = 0;
    bool escapePressed = false;
    Vector2 cursorAxis = { 0.0f, 0.0f };
    double inputTime = GetTime();
    float cursorDelta = Clamp((float)(inputTime - platform.lastInputTime), 0.0f, 0.1f);
    platform.lastInputTime = inputTime;
#endif

    for (int i = 0; i < MAX_GAMEPADS; i++)
    {
        for (int k = 0; k < MAX_GAMEPAD_BUTTONS; k++)
        {
            CORE.Input.Gamepad.previousButtonState[i][k] = CORE.Input.Gamepad.currentButtonState[i][k];
            CORE.Input.Gamepad.currentButtonState[i][k] = 0;
        }
        for (int k = 0; k < MAX_GAMEPAD_AXES; k++) CORE.Input.Gamepad.axisState[i][k] = 0.0f;
        CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_LEFT_TRIGGER] = -1.0f;
        CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_RIGHT_TRIGGER] = -1.0f;
        CORE.Input.Gamepad.axisCount[i] = 0;

        padUpdate(&platform.nxPad[i]);
        CORE.Input.Gamepad.ready[i] = padIsConnected(&platform.nxPad[i]);
        if (!CORE.Input.Gamepad.ready[i]) continue;

        HidNpadStyleTag styleTag = padGetStyleSet(&platform.nxPad[i]);
        if (styleTag != platform.nxPadStyle[i])
        {
            platform.nxPadStyle[i] = styleTag;
            strcpy(CORE.Input.Gamepad.name[i], GetNxGamePadName(i));
        }
        CORE.Input.Gamepad.axisCount[i] = 6;

        u64 kHeld = padGetButtons(&platform.nxPad[i]);
        for (int k = 0; k < MAX_GAMEPAD_BUTTONS; k++)
        {
            u64 kButton = 0;
            switch (k)
            {
                case GAMEPAD_BUTTON_LEFT_FACE_UP: kButton = HidNpadButton_Up; break;
                case GAMEPAD_BUTTON_LEFT_FACE_RIGHT: kButton = HidNpadButton_Right; break;
                case GAMEPAD_BUTTON_LEFT_FACE_DOWN: kButton = HidNpadButton_Down; break;
                case GAMEPAD_BUTTON_LEFT_FACE_LEFT: kButton = HidNpadButton_Left; break;
                case GAMEPAD_BUTTON_RIGHT_FACE_UP: kButton = HidNpadButton_X; break;
                case GAMEPAD_BUTTON_RIGHT_FACE_RIGHT: kButton = HidNpadButton_A; break;
                case GAMEPAD_BUTTON_RIGHT_FACE_DOWN: kButton = HidNpadButton_B; break;
                case GAMEPAD_BUTTON_RIGHT_FACE_LEFT: kButton = HidNpadButton_Y; break;
                case GAMEPAD_BUTTON_LEFT_TRIGGER_1: kButton = HidNpadButton_L; break;
                case GAMEPAD_BUTTON_LEFT_TRIGGER_2: kButton = HidNpadButton_ZL; break;
                case GAMEPAD_BUTTON_RIGHT_TRIGGER_1: kButton = HidNpadButton_R; break;
                case GAMEPAD_BUTTON_RIGHT_TRIGGER_2: kButton = HidNpadButton_ZR; break;
                case GAMEPAD_BUTTON_MIDDLE_LEFT: kButton = HidNpadButton_Minus; break;
                case GAMEPAD_BUTTON_MIDDLE_RIGHT: kButton = HidNpadButton_Plus; break;
                case GAMEPAD_BUTTON_LEFT_THUMB: kButton = HidNpadButton_StickL; break;
                case GAMEPAD_BUTTON_RIGHT_THUMB: kButton = HidNpadButton_StickR; break;
                default: break;
            }
            if (kHeld & kButton)
            {
                CORE.Input.Gamepad.currentButtonState[i][k] = 1;
                CORE.Input.Gamepad.lastButtonPressed = k;
            }
        }

        HidAnalogStickState kAxisL = padGetStickPos(&platform.nxPad[i], 0);
        HidAnalogStickState kAxisR = padGetStickPos(&platform.nxPad[i], 1);
        CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_LEFT_X] = Clamp((float)kAxisL.x/32767.0f, -1.0f, 1.0f);
        CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_LEFT_Y] = Clamp(-(float)kAxisL.y/32767.0f, -1.0f, 1.0f);
        CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_RIGHT_X] = Clamp((float)kAxisR.x/32767.0f, -1.0f, 1.0f);
        CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_RIGHT_Y] = Clamp(-(float)kAxisR.y/32767.0f, -1.0f, 1.0f);
        CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_LEFT_TRIGGER] = (kHeld & HidNpadButton_ZL)? 1.0f : -1.0f;
        CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_RIGHT_TRIGGER] = (kHeld & HidNpadButton_ZR)? 1.0f : -1.0f;

#if !defined(NX_DISABLE_GAMEPAD_EMULATION)
        // Evaluate the exit chord per controller, then combine the remaining input.
        if ((kHeld & (HidNpadButton_Plus | HidNpadButton_Minus)) == (HidNpadButton_Plus | HidNpadButton_Minus))
        {
            escapePressed = true;
            emulatedButtons |= kHeld & ~(HidNpadButton_Plus | HidNpadButton_Minus);
        }
        else emulatedButtons |= kHeld;

        float axisX = CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_RIGHT_X];
        float axisY = CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_RIGHT_Y];
        if (fabsf(axisX) > 0.1f) cursorAxis.x += axisX;
        if (fabsf(axisY) > 0.1f) cursorAxis.y += axisY;
#endif
    }

#if !defined(NX_DISABLE_GAMEPAD_EMULATION)
    static const struct { int key; u64 buttons; } keyMappings[] = {
        { KEY_RIGHT, HidNpadButton_Right | HidNpadButton_StickLRight },
        { KEY_D, HidNpadButton_Right | HidNpadButton_StickLRight },
        { KEY_LEFT, HidNpadButton_Left | HidNpadButton_StickLLeft },
        { KEY_A, HidNpadButton_Left | HidNpadButton_StickLLeft },
        { KEY_DOWN, HidNpadButton_Down | HidNpadButton_StickLDown },
        { KEY_S, HidNpadButton_Down | HidNpadButton_StickLDown },
        { KEY_UP, HidNpadButton_Up | HidNpadButton_StickLUp },
        { KEY_W, HidNpadButton_Up | HidNpadButton_StickLUp },
        { KEY_Q, HidNpadButton_Y }, { KEY_E, HidNpadButton_A },
        { KEY_R, HidNpadButton_X }, { KEY_F, HidNpadButton_B },
        { KEY_ENTER, HidNpadButton_Plus }, { KEY_SPACE, HidNpadButton_Minus },
        { KEY_LEFT_SHIFT, HidNpadButton_StickL }
    };
    for (unsigned int i = 0; i < sizeof(keyMappings)/sizeof(keyMappings[0]); i++)
    {
        if (keyMappings[i].key < MAX_KEYBOARD_KEYS)
            CORE.Input.Keyboard.currentKeyState[keyMappings[i].key] = ((emulatedButtons & keyMappings[i].buttons) != 0);
    }
    if (KEY_ESCAPE < MAX_KEYBOARD_KEYS) CORE.Input.Keyboard.currentKeyState[KEY_ESCAPE] = escapePressed;

    CORE.Input.Mouse.currentButtonState[MOUSE_BUTTON_LEFT] = ((emulatedButtons & HidNpadButton_ZR) != 0);
    CORE.Input.Mouse.currentButtonState[MOUSE_BUTTON_RIGHT] = ((emulatedButtons & HidNpadButton_ZL) != 0);
    CORE.Input.Mouse.currentButtonState[MOUSE_BUTTON_MIDDLE] = ((emulatedButtons & HidNpadButton_StickR) != 0);
    CORE.Input.Mouse.currentWheelMove.y = ((emulatedButtons & HidNpadButton_R) != 0) - ((emulatedButtons & HidNpadButton_L) != 0);

    if (touchCount == 0)
    {
        CORE.Input.Mouse.currentPosition.x += Clamp(cursorAxis.x, -1.0f, 1.0f)*600.0f*cursorDelta;
        CORE.Input.Mouse.currentPosition.y += Clamp(cursorAxis.y, -1.0f, 1.0f)*600.0f*cursorDelta;

        // Clamp in mouse coordinates, accounting for the application's offset and scale.
        if (CORE.Input.Mouse.scale.x != 0.0f)
        {
            float start = -CORE.Input.Mouse.offset.x;
            float end = CORE.Window.screen.width/CORE.Input.Mouse.scale.x - CORE.Input.Mouse.offset.x;
            CORE.Input.Mouse.currentPosition.x = Clamp(CORE.Input.Mouse.currentPosition.x, fminf(start, end), fmaxf(start, end));
        }
        if (CORE.Input.Mouse.scale.y != 0.0f)
        {
            float start = -CORE.Input.Mouse.offset.y;
            float end = CORE.Window.screen.height/CORE.Input.Mouse.scale.y - CORE.Input.Mouse.offset.y;
            CORE.Input.Mouse.currentPosition.y = Clamp(CORE.Input.Mouse.currentPosition.y, fminf(start, end), fmaxf(start, end));
        }
    }
#endif

    // Touch owns the cursor while active; controller clicks are combined with it.
    if (touchCount > 0)
    {
        CORE.Input.Mouse.currentButtonState[MOUSE_BUTTON_LEFT] = 1;
        CORE.Input.Mouse.currentPosition = CORE.Input.Touch.position[0];
        if (platform.prevTouchCount == 0) CORE.Input.Mouse.previousPosition = CORE.Input.Mouse.currentPosition;
    }
    platform.prevTouchCount = touchCount;

    for (int i = 1; i < MAX_KEYBOARD_KEYS; i++)
    {
        if (CORE.Input.Keyboard.currentKeyState[i] && !CORE.Input.Keyboard.previousKeyState[i] &&
            (CORE.Input.Keyboard.keyPressedQueueCount < MAX_KEY_PRESSED_QUEUE))
        {
            CORE.Input.Keyboard.keyPressedQueue[CORE.Input.Keyboard.keyPressedQueueCount++] = i;
        }
    }

    int exitKey = CORE.Input.Keyboard.exitKey;
    if ((exitKey > KEY_NULL) && (exitKey < MAX_KEYBOARD_KEYS) &&
        CORE.Input.Keyboard.currentKeyState[exitKey] && !CORE.Input.Keyboard.previousKeyState[exitKey])
    {
        CORE.Window.shouldClose = true;
    }
}


//----------------------------------------------------------------------------------
// Module Internal Functions Definition
//----------------------------------------------------------------------------------

static bool SetVSync(bool enabled)
{
    if (eglSwapInterval(platform.device, enabled? 1 : 0) == EGL_FALSE)
    {
        TRACELOG(LOG_WARNING, "DISPLAY: eglSwapInterval() failed (EGL error: 0x%04x)", eglGetError());
        return false;
    }

    if (enabled) FLAG_SET(CORE.Window.flags, FLAG_VSYNC_HINT);
    else FLAG_CLEAR(CORE.Window.flags, FLAG_VSYNC_HINT);
    return true;
}

// Initialize platform: graphics, inputs and more
int InitPlatform(void)
{
    bool eglInitialized = false;
    u32 nativeWidth = 0;
    u32 nativeHeight = 0;
    CORE.Window.ready = false;
    platform.device = EGL_NO_DISPLAY;
    platform.surface = EGL_NO_SURFACE;
    platform.context = EGL_NO_CONTEXT;
    platform.config = NULL;

    platform.swapFailed = false;
    platform.prevTouchCount = 0;

#if defined(NX_USB_DEBUGGER)
    NxUsbDebuggerInit();
#endif
    romfsInit();

    platform.gbmSurface = nwindowGetDefault();
    Result nativeResult = nwindowGetDimensions(platform.gbmSurface, &nativeWidth, &nativeHeight);
    if (R_FAILED(nativeResult) || (nativeWidth == 0) || (nativeHeight == 0))
    {
        TRACELOG(LOG_WARNING, "DISPLAY: Failed to get NX window dimensions (result: 0x%08x)", nativeResult);
        goto eglFailure;
    }

    CORE.Window.display.width = (int)nativeWidth;
    CORE.Window.display.height = (int)nativeHeight;
    if (CORE.Window.screen.width == 0) CORE.Window.screen.width = CORE.Window.display.width;
    if (CORE.Window.screen.height == 0) CORE.Window.screen.height = CORE.Window.display.height;
    if ((CORE.Window.screen.width > UINT16_MAX) || (CORE.Window.screen.height > UINT16_MAX))
    {
        TRACELOG(LOG_WARNING, "DISPLAY: Requested dimensions exceed the NX framebuffer size range");
        goto eglFailure;
    }

    // EGL registers buffers during surface creation; dimensions must be set beforehand.
    nativeResult = nwindowSetDimensions(platform.gbmSurface, CORE.Window.screen.width, CORE.Window.screen.height);
    if (R_FAILED(nativeResult))
    {
        TRACELOG(LOG_WARNING, "DISPLAY: Failed to set NX buffer dimensions (result: 0x%08x)", nativeResult);
        goto eglFailure;
    }
    FLAG_SET(CORE.Window.flags, FLAG_FULLSCREEN_MODE);

    const EGLint framebufferAttribs[] =
    {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, (rlGetVersion() == RL_OPENGL_ES_30)? EGL_OPENGL_ES3_BIT : EGL_OPENGL_ES2_BIT,      // Type of context support
        EGL_RED_SIZE, 8,            // RED color bit depth (alternative: 5)
        EGL_GREEN_SIZE, 8,          // GREEN color bit depth (alternative: 6)
        EGL_BLUE_SIZE, 8,           // BLUE color bit depth (alternative: 5)
        //EGL_TRANSPARENT_TYPE, EGL_NONE, // Request transparent framebuffer (EGL_TRANSPARENT_RGB does not work on RPI)
        EGL_DEPTH_SIZE, 16,         // Depth buffer size (Required to use Depth testing!)
        //EGL_STENCIL_SIZE, 8,      // Stencil buffer size
        EGL_NONE
    };

    const EGLint contextAttribs[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLint numConfigs = 0;

    // Get an EGL device connection
    platform.device = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (platform.device == EGL_NO_DISPLAY)
    {
        TRACELOG(LOG_WARNING, "DISPLAY: eglGetDisplay() failed (EGL error: 0x%04x)", eglGetError());
        goto eglFailure;
    }

    // Initialize the EGL device connection
    if (eglInitialize(platform.device, NULL, NULL) == EGL_FALSE)
    {
        TRACELOG(LOG_WARNING, "DISPLAY: eglInitialize() failed (EGL error: 0x%04x)", eglGetError());
        goto eglFailure;
    }
    eglInitialized = true;

    // Get an appropriate EGL framebuffer configuration
    if (eglChooseConfig(platform.device, framebufferAttribs, &platform.config, 1, &numConfigs) == EGL_FALSE)
    {
        TRACELOG(LOG_WARNING, "DISPLAY: eglChooseConfig() failed (EGL error: 0x%04x)", eglGetError());
        goto eglFailure;
    }
    if (numConfigs == 0)
    {
        TRACELOG(LOG_WARNING, "DISPLAY: No matching EGL window configuration");
        goto eglFailure;
    }

    // Set rendering API
    if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE)
    {
        TRACELOG(LOG_WARNING, "DISPLAY: eglBindAPI() failed (EGL error: 0x%04x)", eglGetError());
        goto eglFailure;
    }

    // Create an EGL rendering context
    platform.context = eglCreateContext(platform.device, platform.config, EGL_NO_CONTEXT, contextAttribs);
    if (platform.context == EGL_NO_CONTEXT)
    {
        TRACELOG(LOG_WARNING, "DISPLAY: eglCreateContext() failed (EGL error: 0x%04x)", eglGetError());
        goto eglFailure;
    }

    platform.surface = eglCreateWindowSurface(platform.device, platform.config, (EGLNativeWindowType)platform.gbmSurface, NULL);
    if (platform.surface == EGL_NO_SURFACE)
    {
        TRACELOG(LOG_WARNING, "DISPLAY: eglCreateWindowSurface() failed (EGL error: 0x%04x)", eglGetError());
        goto eglFailure;
    }
    if (eglMakeCurrent(platform.device, platform.surface, platform.surface, platform.context) == EGL_FALSE)
    {
        TRACELOG(LOG_WARNING, "DISPLAY: eglMakeCurrent() failed (EGL error: 0x%04x)", eglGetError());
        goto eglFailure;
    }

    // Set the swap interval after the rendering surface is current.
    if (!SetVSync(FLAG_IS_SET(CORE.Window.flags, FLAG_VSYNC_HINT))) goto eglFailure;

    // Read dimensions from libnx, matching the buffers allocated by Switch Mesa.
    nativeResult = nwindowGetDimensions(platform.gbmSurface, &nativeWidth, &nativeHeight);
    if (R_FAILED(nativeResult) || (nativeWidth == 0) || (nativeHeight == 0))
    {
        TRACELOG(LOG_WARNING, "DISPLAY: Failed to get NX buffer dimensions (result: 0x%08x)", nativeResult);
        goto eglFailure;
    }

    CORE.Window.ready = true;
    CORE.Window.render.width = (int)nativeWidth;
    CORE.Window.render.height = (int)nativeHeight;
    CORE.Window.renderOffset = (Point){ 0, 0 };
    CORE.Window.screenScale = MatrixScale((float)nativeWidth/CORE.Window.screen.width,
                                         (float)nativeHeight/CORE.Window.screen.height, 1.0f);
    CORE.Window.currentFbo.width = CORE.Window.render.width;
    CORE.Window.currentFbo.height = CORE.Window.render.height;

    TRACELOG(LOG_INFO, "DISPLAY: Device initialized successfully");
    TRACELOG(LOG_INFO, "    > Display size: %i x %i", CORE.Window.display.width, CORE.Window.display.height);
    TRACELOG(LOG_INFO, "    > Screen size:  %i x %i", CORE.Window.screen.width, CORE.Window.screen.height);
    TRACELOG(LOG_INFO, "    > Render size:  %i x %i", CORE.Window.render.width, CORE.Window.render.height);
    TRACELOG(LOG_INFO, "    > Viewport offsets: %i, %i", CORE.Window.renderOffset.x, CORE.Window.renderOffset.y);

    // NOTE: GL procedures address loader is required to load extensions
    rlLoadExtensions(eglGetProcAddress);

    // Configure our supported input layout
    padConfigureInput(MAX_GAMEPADS, HidNpadStyleSet_NpadStandard);
    // Initialize the gamepads
    for (int i = 0; i < MAX_GAMEPADS; i++)
    {
        if (i == 0)
        {
            padInitializeDefault(&platform.nxPad[i]);
        }
        else
        {
            padInitialize(&platform.nxPad[i], HidNpadIdType_No1 + i);
        }
        padUpdate(&platform.nxPad[i]);
        platform.nxPadStyle[i] = padGetStyleSet(&platform.nxPad[i]);
        strcpy(CORE.Input.Gamepad.name[i], GetNxGamePadName(i));
    }
    // Initialize the touchscreen
    hidInitializeTouchScreen();

    // Initialize hi-res timer
    InitTimer();
    platform.lastInputTime = GetTime();

    // Initialize storage system
    CORE.Storage.basePath = GetWorkingDirectory();

    TRACELOG(LOG_INFO, "PLATFORM: NX: Initialized successfully");

    return 0;

eglFailure:
    // Only release EGL resources after the display has initialized successfully.
    if (eglInitialized)
    {
        eglMakeCurrent(platform.device, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (platform.surface != EGL_NO_SURFACE) eglDestroySurface(platform.device, platform.surface);
        if (platform.context != EGL_NO_CONTEXT) eglDestroyContext(platform.device, platform.context);
        eglTerminate(platform.device);
    }
    platform.device = EGL_NO_DISPLAY;
    platform.surface = EGL_NO_SURFACE;
    platform.context = EGL_NO_CONTEXT;
    platform.config = NULL;
    platform.gbmSurface = NULL;
    romfsExit();
#if defined(NX_USB_DEBUGGER)
    NxUsbDebuggerEnd();
#endif
    return -1;
}

// Close platform
void ClosePlatform(void)
{
    // Close surface, context and display
    if (platform.device != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(platform.device, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (platform.surface != EGL_NO_SURFACE)
        {
            eglDestroySurface(platform.device, platform.surface);
            platform.surface = EGL_NO_SURFACE;
        }

        if (platform.context != EGL_NO_CONTEXT)
        {
            eglDestroyContext(platform.device, platform.context);
            platform.context = EGL_NO_CONTEXT;
        }

        eglTerminate(platform.device);
        platform.device = EGL_NO_DISPLAY;
    }
    romfsExit();
#if defined(NX_USB_DEBUGGER)
    NxUsbDebuggerEnd();
#endif
}
