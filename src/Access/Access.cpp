#include "Access.h"

Access::Access(DoorManager& door)
    : _door(door)
{
}

bool Access::Initialize()
{
    return true;
}

void Access::Grant(int doorId)
{
    GenericDeviceUpdateState update;

    update.Type = FacilityType::Door;
    update.Id = doorId;
    update.IsLocked = false;
    update.IsOpen = true;

    _door.Update(update);
}

void Access::Deny()
{
}

void Access::Update()
{
}
