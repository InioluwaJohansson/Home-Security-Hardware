#include "Door.h"

DoorManager::DoorManager(
    IHardwareController& hardware)
    : _hardware(hardware)
{
}

void DoorManager::Initialize()
{
    for (int i = 0; i < MAX_DOORS; ++i)
    {
        _cache[i].DoorId = i;
        _cache[i].IsLocked = false;
        _cache[i].IsOpen = false;
        _cache[i].Type = DoorType::Interior;
    }
}

DoorDeviceState DoorManager::GetCurrentState(int doorId)
{
    if (doorId < 0 || doorId >= MAX_DOORS)
        return {};

    return _cache[doorId];
}

void DoorManager::Apply(const DoorDeviceState& state)
{
    if (state.DoorId < 0 || state.DoorId >= MAX_DOORS)
        return;

    DoorDeviceState current = _cache[state.DoorId];

    // Keep the backend's door type as part of the local hardware state.
    _cache[state.DoorId].Type = state.Type;

    // A lock command must never attempt to lock an open door.
    // Close the door first; the hardware layer will only energize the
    // lock after the closed sensor is confirmed.
    if (current.IsLocked != state.IsLocked)
    {
        if (state.IsLocked)
        {
            if (current.IsOpen)
            {
                _hardware.CloseDoor(state.DoorId);
                _cache[state.DoorId].IsOpen = true;
            }
            else
            {
                _hardware.LockDoor(state.DoorId);
            }
        }
        else
        {
            _hardware.UnlockDoor(state.DoorId);
        }
    }

    if (current.IsOpen != state.IsOpen)
    {
        if (state.IsOpen)
        {
            // Opening an already locked door is not allowed.
            if (!current.IsLocked)
                _hardware.OpenDoor(state.DoorId);
        }
        else
        {
            _hardware.CloseDoor(state.DoorId);
        }
    }

    _cache[state.DoorId] = state;

    if (StateChanged)
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
    if (update.Id < 0 || update.Id >= MAX_DOORS)
        return;

    DoorDeviceState state;

    state.DoorId = update.Id;
    state.IsLocked = update.IsLocked;
    state.IsOpen = update.IsOpen;

    // FacilityType::Door updates may contain the backend DoorType.
    // The enum is encoded in the same command as the other fields.
    // Keep the current type when the command does not supply one.
    state.Type = _cache[update.Id].Type;

    Apply(state);
}
