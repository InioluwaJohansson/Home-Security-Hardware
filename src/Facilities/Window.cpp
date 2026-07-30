#include "Window.h"
#include "../Hardware/Hardware.h"

WindowManager::WindowManager(
    IHardwareController& hardware)
    : _hardware(hardware)
{
}

void WindowManager::Initialize()
{
    for (int i = 0; i < MAX_WINDOWS; i++)
    {
        _cache[i].WindowId = -1;
        _cache[i].IsLocked = false;
        _cache[i].IsOpen = false;
    }
}

WindowDeviceState WindowManager::GetCurrentState(int windowId)
{
    return _cache[windowId];
}

void WindowManager::Apply(const WindowDeviceState& state)
{
    auto current = GetCurrentState(state.WindowId);

    if (current.IsLocked == state.IsLocked &&
        current.IsOpen == state.IsOpen)
        return;

    if (current.IsLocked != state.IsLocked)
    {
        if (state.IsLocked)
            _hardware.LockWindow(state.WindowId);
        else
            _hardware.UnlockWindow(state.WindowId);
    }

    if (current.IsOpen != state.IsOpen)
    {
        if (state.IsOpen)
            _hardware.OpenWindow(state.WindowId);
        else
            _hardware.CloseWindow(state.WindowId);
    }

    _cache[state.WindowId] = state;

    if(StateChanged)
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
    WindowDeviceState state;

    state.WindowId = update.Id;
    state.IsLocked = update.IsLocked;
    state.IsOpen = update.IsOpen;

    Apply(state);
}