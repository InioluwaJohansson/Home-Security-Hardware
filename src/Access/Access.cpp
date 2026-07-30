#include "Access.h"

Access::Access(DoorManager& door)
    : _door(door)
{
}

void Access::Grant(int doorId)
{
    GenericDeviceUpdateState update;

    update.Id = doorId;
    update.IsLocked = false;
    update.IsOpen = true;

    _door.Update(update);
}

void Access::Deny()
{
    
}