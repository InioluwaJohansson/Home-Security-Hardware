#include "Window.h"

WindowManager::WindowManager(
    IHardwareController& hardware)
    : _hardware(hardware)
{
}

void WindowManager::Initialize()
{
    for (int i = 0; i < MAX_WINDOWS; ++i)
    {
        _cache[i].WindowId = i;
        _cache[i].IsLocked = false;
        _cache[i].IsOpen = false;
    }
}

WindowDeviceState WindowManager::GetCurrentState(int windowId)
{
    if (windowId < 0 || windowId >= MAX_WINDOWS)
        return {};

    return _cache[windowId];
}

void WindowManager::Apply(const WindowDeviceState& state)
{
    if (state.WindowId < 0 || state.WindowId >= MAX_WINDOWS)
        return;

    WindowDeviceState current = _cache[state.WindowId];

    if (current.IsLocked != state.IsLocked)
    {
        if (state.IsLocked)
        {
            // HardwareController performs close-then-lock when necessary.
            _hardware.LockWindow(state.WindowId);
        }
        else
        {
            _hardware.UnlockWindow(state.WindowId);
        }
    }

    if (current.IsOpen != state.IsOpen)
    {
        if (state.IsOpen)
        {
            if (!current.IsLocked && !state.IsLocked)
                _hardware.OpenWindow(state.WindowId);
        }
        else
        {
            _hardware.CloseWindow(state.WindowId);
        }
    }

    _cache[state.WindowId] = state;

    if (StateChanged)
    {
        GenericDeviceUpdateState update;

        update.Id = state.WindowId;
        update.Type = FacilityType::Window;
        update.IsLocked = state.IsLocked;
        update.IsOpen = state.IsOpen;

        StateChanged(update);
    }
}

void WindowManager::Update(const GenericDeviceUpdateState& update)
{
    if (update.Id < 0 || update.Id >= MAX_WINDOWS)
        return;

    WindowDeviceState state;

    state.WindowId = update.Id;
    state.IsLocked = update.IsLocked;
    state.IsOpen = update.IsOpen;

    Apply(state);
}
