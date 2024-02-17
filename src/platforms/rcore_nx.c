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

    PadState nxPad[MAX_GAMEPADS];
    HidNpadStyleTag nxPadStyle[MAX_GAMEPADS];

    // Display data
    EGLDisplay device;                  // Native display device (physical screen connection)
    EGLSurface surface;                 // Surface to draw on, framebuffers (connected to context)
    EGLContext context;                 // Graphic context, mode in which drawing can be done
    EGLConfig config;                   // Graphic config
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
    TRACELOG(LOG_WARNING, "SetWindowState() not available on target platform");
}

// Clear window configuration state flags
void ClearWindowState(unsigned int flags)
{
    TRACELOG(LOG_WARNING, "ClearWindowState() not available on target platform");
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
    TRACELOG(LOG_WARNING, "SetWindowSize() not available on target platform");
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
    eglSwapBuffers(platform.device, platform.surface);
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

// Register all input events
void PollInputEvents(void)
{
#if defined(SUPPORT_GESTURES_SYSTEM)
    // NOTE: Gestures update must be called every frame to reset gestures correctly
    // because ProcessGestureEvent() is just called on an event, not every frame
    UpdateGestures();
#endif

    // Reset keys/chars pressed registered
    CORE.Input.Keyboard.keyPressedQueueCount = 0;
    CORE.Input.Keyboard.charPressedQueueCount = 0;

    // Reset key repeats
    for (int i = 0; i < MAX_KEYBOARD_KEYS; i++) CORE.Input.Keyboard.keyRepeatInFrame[i] = 0;

    // Reset last gamepad button/axis registered state
    CORE.Input.Gamepad.lastButtonPressed = 0; // GAMEPAD_BUTTON_UNKNOWN

    // Register previous touch states
    for (int i = 0; i < MAX_TOUCH_POINTS; i++) CORE.Input.Touch.previousTouchState[i] = CORE.Input.Touch.currentTouchState[i];

    // Reset touch positions
    //for (int i = 0; i < MAX_TOUCH_POINTS; i++) CORE.Input.Touch.position[i] = (Vector2){ 0, 0 };

    // Register previous keys states
    for (int i = 0; i < 260; i++)
    {
        CORE.Input.Keyboard.previousKeyState[i] = CORE.Input.Keyboard.currentKeyState[i];
        CORE.Input.Keyboard.keyRepeatInFrame[i] = 0;
    }

    HidTouchScreenState state = {0};
    if (hidGetTouchScreenStates(&state, 1))
    {
        if (state.count != platform.prevTouchCount)
        {
            platform.prevTouchCount = state.count;
        }
        for(int i = 0; i < state.count; i++)
        {
            CORE.Input.Touch.position[i].x = state.touches[i].x;
            CORE.Input.Touch.position[i].y = state.touches[i].y;
            CORE.Input.Touch.pointId[i] = state.touches[i].finger_id;

            platform.touchDeltaTime[i] = state.touches[i].delta_time;
        }
        CORE.Input.Touch.pointCount = state.count;
    }

    for (int i = 0; i < MAX_GAMEPADS; i++)
    {
        // Scan the gamepad. This should be done once for each frame
        padUpdate(&platform.nxPad[i]);
        CORE.Input.Gamepad.ready[i] = padIsConnected(&platform.nxPad[i]);
        if (CORE.Input.Gamepad.ready[i]) {
            // Get the style of the gamepad.
            HidNpadStyleTag styleTag = padGetStyleSet(&platform.nxPad[i]);
            if (styleTag != platform.nxPadStyle[i]) {
                platform.nxPadStyle[i] = styleTag;
                strcpy(CORE.Input.Gamepad.name[i], GetNxGamePadName(i));
            }

            // Set number of axis of the gamepad.
            CORE.Input.Gamepad.axisCount[i] = 6;

            // Returns the set of buttons that are currently pressed
            u64 kHeld = padGetButtons(&platform.nxPad[i]);
            u64 kButton;
            for (int k = 0; k < MAX_GAMEPAD_BUTTONS; k++)
            {
                // Register previous gamepad states
                CORE.Input.Gamepad.previousButtonState[i][k] = CORE.Input.Gamepad.currentButtonState[i][k];

                // Check digital buttons
                kButton = 0;
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
                }
                if (kHeld & kButton) {
                    CORE.Input.Gamepad.currentButtonState[i][k] = 1;
                    CORE.Input.Gamepad.lastButtonPressed = k;
                } else {
                    CORE.Input.Gamepad.currentButtonState[i][k] = 0;
                }
            }

            // Check analogic axis and buttons
            HidAnalogStickState kAxisL = padGetStickPos(&platform.nxPad[i], 0);
            HidAnalogStickState kAxisR = padGetStickPos(&platform.nxPad[i], 1);

            CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_LEFT_X] = (float)kAxisL.x / 32767.0f;
            CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_LEFT_Y] = (float)kAxisL.y / 32767.0f;
            CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_RIGHT_X] = (float)kAxisR.x / 32767.0f;
            CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_RIGHT_Y] = (float)kAxisR.y / 32767.0f;
            CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_LEFT_TRIGGER] = (kHeld & HidNpadButton_ZL) ? 1.0f : 0.0f;
            CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_RIGHT_TRIGGER] = (kHeld & HidNpadButton_ZR) ? 1.0f : 0.0f;

#if !defined(NX_DISABLE_GAMEPAD_EMULATION)
            CORE.Input.Keyboard.previousKeyState[KEY_RIGHT] = CORE.Input.Keyboard.currentKeyState[KEY_RIGHT];
            CORE.Input.Keyboard.previousKeyState[KEY_D] = CORE.Input.Keyboard.currentKeyState[KEY_D];
            if (kHeld & HidNpadButton_Right || kHeld & HidNpadButton_StickLRight) {
                CORE.Input.Keyboard.currentKeyState[KEY_RIGHT] = 1;
                CORE.Input.Keyboard.currentKeyState[KEY_D] = 1;
            } else {
                CORE.Input.Keyboard.currentKeyState[KEY_RIGHT] = 0;
                CORE.Input.Keyboard.currentKeyState[KEY_D] = 0;
            }

            CORE.Input.Keyboard.previousKeyState[KEY_LEFT] = CORE.Input.Keyboard.currentKeyState[KEY_LEFT];
            CORE.Input.Keyboard.previousKeyState[KEY_A] = CORE.Input.Keyboard.currentKeyState[KEY_A];
            if (kHeld & HidNpadButton_Left || kHeld & HidNpadButton_StickLLeft) {
                CORE.Input.Keyboard.currentKeyState[KEY_LEFT] = 1;
                CORE.Input.Keyboard.currentKeyState[KEY_A] = 1;
            } else {
                CORE.Input.Keyboard.currentKeyState[KEY_LEFT] = 0;
                CORE.Input.Keyboard.currentKeyState[KEY_A] = 0;
            }

            CORE.Input.Keyboard.previousKeyState[KEY_DOWN] = CORE.Input.Keyboard.currentKeyState[KEY_DOWN];
            CORE.Input.Keyboard.previousKeyState[KEY_S] = CORE.Input.Keyboard.currentKeyState[KEY_S];
            if (kHeld & HidNpadButton_Down || kHeld & HidNpadButton_StickLDown) {
                CORE.Input.Keyboard.currentKeyState[KEY_DOWN] = 1;
                CORE.Input.Keyboard.currentKeyState[KEY_S] = 1;
            } else {
                CORE.Input.Keyboard.currentKeyState[KEY_DOWN] = 0;
                CORE.Input.Keyboard.currentKeyState[KEY_S] = 0;
            }

            CORE.Input.Keyboard.previousKeyState[KEY_UP] = CORE.Input.Keyboard.currentKeyState[KEY_UP];
            CORE.Input.Keyboard.previousKeyState[KEY_W] = CORE.Input.Keyboard.currentKeyState[KEY_W];
            if (kHeld & HidNpadButton_Up || kHeld & HidNpadButton_StickLUp) {
                CORE.Input.Keyboard.currentKeyState[KEY_UP] = 1;
                CORE.Input.Keyboard.currentKeyState[KEY_W] = 1;
            } else {
                CORE.Input.Keyboard.currentKeyState[KEY_UP] = 0;
                CORE.Input.Keyboard.currentKeyState[KEY_W] = 0;
            }

            CORE.Input.Keyboard.previousKeyState[KEY_Q] = CORE.Input.Keyboard.currentKeyState[KEY_Q];
            if (kHeld & HidNpadButton_Y) {
                CORE.Input.Keyboard.currentKeyState[KEY_Q] = 1;
            } else {
                CORE.Input.Keyboard.currentKeyState[KEY_Q] = 0;
            }

            CORE.Input.Keyboard.previousKeyState[KEY_E] = CORE.Input.Keyboard.currentKeyState[KEY_E];
            if (kHeld & HidNpadButton_A) {
                CORE.Input.Keyboard.currentKeyState[KEY_E] = 1;
            } else {
                CORE.Input.Keyboard.currentKeyState[KEY_E] = 0;
            }

            CORE.Input.Keyboard.previousKeyState[KEY_R] = CORE.Input.Keyboard.currentKeyState[KEY_R];
            if (kHeld & HidNpadButton_X) {
                CORE.Input.Keyboard.currentKeyState[KEY_R] = 1;
            } else {
                CORE.Input.Keyboard.currentKeyState[KEY_R] = 0;
            }

            CORE.Input.Keyboard.previousKeyState[KEY_F] = CORE.Input.Keyboard.currentKeyState[KEY_F];
            if (kHeld & HidNpadButton_B) {
                CORE.Input.Keyboard.currentKeyState[KEY_F] = 1;
            } else {
                CORE.Input.Keyboard.currentKeyState[KEY_F] = 0;
            }

            CORE.Input.Keyboard.previousKeyState[KEY_ENTER] = CORE.Input.Keyboard.currentKeyState[KEY_ENTER];
            CORE.Input.Keyboard.previousKeyState[KEY_SPACE] = CORE.Input.Keyboard.currentKeyState[KEY_SPACE];
            CORE.Input.Keyboard.previousKeyState[KEY_ESCAPE] = CORE.Input.Keyboard.currentKeyState[KEY_ESCAPE];
            if (kHeld & HidNpadButton_Plus && kHeld & HidNpadButton_Minus) {
                CORE.Input.Keyboard.currentKeyState[KEY_ENTER] = 0;
                CORE.Input.Keyboard.currentKeyState[KEY_SPACE] = 0;
                CORE.Input.Keyboard.currentKeyState[KEY_ESCAPE] = 1;
            } else {
                if (kHeld & HidNpadButton_Plus) {
                    CORE.Input.Keyboard.currentKeyState[KEY_ENTER] = 1;
                } else {
                    CORE.Input.Keyboard.currentKeyState[KEY_ENTER] = 0;
                }
                if (kHeld & HidNpadButton_Minus) {
                    CORE.Input.Keyboard.currentKeyState[KEY_SPACE] = 1;
                } else {
                    CORE.Input.Keyboard.currentKeyState[KEY_SPACE] = 0;
                }
            }

            CORE.Input.Keyboard.previousKeyState[KEY_LEFT_SHIFT] = CORE.Input.Keyboard.currentKeyState[KEY_LEFT_SHIFT];
            if (kHeld & GAMEPAD_BUTTON_LEFT_THUMB) {
                CORE.Input.Keyboard.currentKeyState[KEY_LEFT_SHIFT] = 1;
            } else {
                CORE.Input.Keyboard.currentKeyState[KEY_LEFT_SHIFT] = 0;
            }

            CORE.Input.Mouse.previousButtonState[MOUSE_BUTTON_LEFT] = CORE.Input.Mouse.currentButtonState[MOUSE_BUTTON_LEFT];
            if (kHeld & HidNpadButton_ZL) {
                CORE.Input.Mouse.currentButtonState[MOUSE_BUTTON_RIGHT] = 1;
            } else {
                CORE.Input.Mouse.currentButtonState[MOUSE_BUTTON_RIGHT] = 0;
            }

            CORE.Input.Mouse.previousButtonState[MOUSE_BUTTON_MIDDLE] = CORE.Input.Mouse.currentButtonState[MOUSE_BUTTON_MIDDLE];
            if (kHeld & GAMEPAD_BUTTON_RIGHT_THUMB) {
                CORE.Input.Mouse.currentButtonState[MOUSE_BUTTON_MIDDLE] = 1;
            } else {
                CORE.Input.Mouse.currentButtonState[MOUSE_BUTTON_MIDDLE] = 0;
            }

            CORE.Input.Mouse.previousButtonState[MOUSE_BUTTON_RIGHT] = CORE.Input.Mouse.currentButtonState[MOUSE_BUTTON_RIGHT];
            if (kHeld & HidNpadButton_ZR) {
                CORE.Input.Mouse.currentButtonState[MOUSE_BUTTON_LEFT] = 1;
            } else {
                CORE.Input.Mouse.currentButtonState[MOUSE_BUTTON_LEFT] = 0;
            }

            CORE.Input.Mouse.previousWheelMove = CORE.Input.Mouse.currentWheelMove;
            if (kHeld & HidNpadButton_L) {
                CORE.Input.Mouse.currentWheelMove = (Vector2){ 0.0f, -1.0f };
            } else if (kHeld & HidNpadButton_R) {
                CORE.Input.Mouse.currentWheelMove = (Vector2){ 0.0f, 1.0f };
            } else {
                CORE.Input.Mouse.currentWheelMove = (Vector2){ 0.0f, 0.0f };
            }

            CORE.Input.Mouse.previousPosition.x = CORE.Input.Mouse.currentPosition.x;
            CORE.Input.Mouse.previousPosition.y = CORE.Input.Mouse.currentPosition.y;

            CORE.Input.Mouse.currentPosition.x += CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_RIGHT_X] * 10;
            CORE.Input.Mouse.currentPosition.y -= CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_RIGHT_Y] * 10;

            if (CORE.Input.Mouse.currentPosition.x < 0) CORE.Input.Mouse.currentPosition.x = 0;
            else if (CORE.Input.Mouse.currentPosition.x > CORE.Window.screen.width/CORE.Input.Mouse.scale.x) CORE.Input.Mouse.currentPosition.x = CORE.Window.screen.width/CORE.Input.Mouse.scale.x;

            if (CORE.Input.Mouse.currentPosition.y < 0) CORE.Input.Mouse.currentPosition.y = 0;
            else if (CORE.Input.Mouse.currentPosition.y > CORE.Window.screen.height/CORE.Input.Mouse.scale.y) CORE.Input.Mouse.currentPosition.y = CORE.Window.screen.height/CORE.Input.Mouse.scale.y;

            if (CORE.Input.Keyboard.currentKeyState[CORE.Input.Keyboard.exitKey] == 1) CORE.Window.shouldClose = true;
#endif
        }
    }
}


//----------------------------------------------------------------------------------
// Module Internal Functions Definition
//----------------------------------------------------------------------------------

// Initialize platform: graphics, inputs and more
int InitPlatform(void)
{
#if defined(NX_USB_DEBUGGER)
    NxUsbDebuggerInit();
#endif
    romfsInit();

    CORE.Window.screen.width = 1280;
    CORE.Window.screen.height = 720;
    CORE.Window.display.width = CORE.Window.screen.width;
    CORE.Window.display.height = CORE.Window.screen.height;
    CORE.Window.fullscreen = true;
    CORE.Window.flags |= FLAG_FULLSCREEN_MODE;

    const EGLint framebufferAttribs[] =
    {
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
        TRACELOG(LOG_WARNING, "DISPLAY: Failed to initialize EGL device");
        return false;
    }

    // Initialize the EGL device connection
    if (eglInitialize(platform.device, NULL, NULL) == EGL_FALSE)
    {
        // If all of the calls to eglInitialize returned EGL_FALSE then an error has occurred.
        TRACELOG(LOG_WARNING, "DISPLAY: Failed to initialize EGL device");
        return false;
    }

    // Get an appropriate EGL framebuffer configuration
    eglChooseConfig(platform.device, framebufferAttribs, &platform.config, 1, &numConfigs);

    // Set rendering API
    eglBindAPI(EGL_OPENGL_ES_API);

    // Create an EGL rendering context
    platform.context = eglCreateContext(platform.device, platform.config, EGL_NO_CONTEXT, contextAttribs);
    if (platform.context == EGL_NO_CONTEXT)
    {
        TRACELOG(LOG_WARNING, "DISPLAY: Failed to create EGL context");
        return -1;
    }

    platform.gbmSurface = nwindowGetDefault();
    platform.surface = eglCreateWindowSurface(platform.device, platform.config, (EGLNativeWindowType)platform.gbmSurface, NULL);

    // There must be at least one frame displayed before the buffers are swapped
    eglSwapInterval(platform.device, 1);

    EGLBoolean result = eglMakeCurrent(platform.device, platform.surface, platform.surface, platform.context);

    // Check surface and context activation
    if (result != EGL_FALSE)
    {
        CORE.Window.ready = true;

        CORE.Window.render.width = CORE.Window.screen.width;
        CORE.Window.render.height = CORE.Window.screen.height;
        CORE.Window.currentFbo.width = CORE.Window.render.width;
        CORE.Window.currentFbo.height = CORE.Window.render.height;

        TRACELOG(LOG_INFO, "DISPLAY: Device initialized successfully");
        TRACELOG(LOG_INFO, "    > Display size: %i x %i", CORE.Window.display.width, CORE.Window.display.height);
        TRACELOG(LOG_INFO, "    > Screen size:  %i x %i", CORE.Window.screen.width, CORE.Window.screen.height);
        TRACELOG(LOG_INFO, "    > Render size:  %i x %i", CORE.Window.render.width, CORE.Window.render.height);
        TRACELOG(LOG_INFO, "    > Viewport offsets: %i, %i", CORE.Window.renderOffset.x, CORE.Window.renderOffset.y);
    }
    else
    {
        TRACELOG(LOG_FATAL, "PLATFORM: Failed to initialize graphics device");
        return -1;
    }

    // If everything work as expected, we can continue
    CORE.Window.render.width = CORE.Window.screen.width;
    CORE.Window.render.height = CORE.Window.screen.height;
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

    // Initialize storage system
    CORE.Storage.basePath = GetWorkingDirectory();

    TRACELOG(LOG_INFO, "PLATFORM: NX: Initialized successfully");

    return 0;
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
