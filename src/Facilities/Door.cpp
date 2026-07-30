#include "Door.h"
#include "../Hardware/Hardware.h"

DoorManager::DoorManager(
    IHardwareController& hardware)
    : _hardware(hardware)
{
}
void DoorManager::Initialize()
{
    for (int i = 0; i < MAX_DOORS; i++)
    {
        _cache[i].DoorId = -1;
        _cache[i].IsLocked = false;
        _cache[i].IsOpen = false;
    }
}

DoorDeviceState DoorManager::GetCurrentState(int doorId)
{
    return _cache[doorId];
}

void DoorManager::Apply(const DoorDeviceState& state)
{
    auto current = GetCurrentState(state.DoorId);

    if (current.IsLocked == state.IsLocked &&
        current.IsOpen == state.IsOpen)
        return;

    if (current.IsLocked != state.IsLocked)
    {
        if (state.IsLocked)
            _hardware.LockDoor(state.DoorId);
        else
            _hardware.UnlockDoor(state.DoorId);
    }

    if (current.IsOpen != state.IsOpen)
    {
        if (state.IsOpen)
            _hardware.OpenDoor(state.DoorId);
        else
            _hardware.CloseDoor(state.DoorId);
    }

    _cache[state.DoorId] = state;

    if(StateChanged)
    {
        GenericDeviceUpdateState update;

        update.Id = state.DoorId;
        update.Type = FacilityType::Door;
        update.IsLocked = state.IsLocked;
        update.IsOpen = state.IsOpen;

        StateChanged(update);
    }
}

void DoorManager::Update(const GenericDeviceUpdateState& update)
{
    DoorDeviceState state;

    state.DoorId = update.Id;
    state.IsLocked = update.IsLocked;
    state.IsOpen = update.IsOpen;

    Apply(state);
}