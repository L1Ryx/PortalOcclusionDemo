import unreal


MAP_PATH = "/Game/PortalOcclusion/L_AcousticLab_Blockout"
DOOR_CONFIGS = [
    {
        "label": "DynamicDoor_P01_AtriumReverb",
        "static_label": "Door_P01_HalfOpen",
        "location": (0.0, 720.0, 140.0),
        "door_size": (560.0, 40.0, 280.0),
        "closed_offset": (-280.0, 0.0, -140.0),
        "travel_offset": (620.0, 0.0, 0.0),
        "phase_offset": 0.0,
    },
    {
        "label": "DynamicDoor_P03_ControlAtrium",
        "static_label": "Door_P03_Sliding",
        "location": (-920.0, 0.0, 140.0),
        "door_size": (40.0, 400.0, 280.0),
        "closed_offset": (0.0, -200.0, -140.0),
        "travel_offset": (0.0, 460.0, 0.0),
        "phase_offset": 1.6,
    },
    {
        "label": "DynamicDoor_P05_CorridorMachinery",
        "static_label": "Door_P05_Partial",
        "location": (1500.0, 320.0, 140.0),
        "door_size": (420.0, 40.0, 280.0),
        "closed_offset": (-210.0, 0.0, -140.0),
        "travel_offset": (470.0, 0.0, 0.0),
        "phase_offset": 3.0,
    },
]


def destroy_actor_with_label(label):
    for actor in list(unreal.EditorLevelLibrary.get_all_level_actors()):
        if actor.get_actor_label() == label:
            unreal.EditorLevelLibrary.destroy_actor(actor)


def spawn_dynamic_door(config):
    door_class = getattr(unreal, "PortalDemoMovingDoor", None)
    if not door_class:
        raise RuntimeError("PortalDemoMovingDoor is unavailable. Compile the C++ project, then rerun this script.")

    door = unreal.EditorLevelLibrary.spawn_actor_from_class(
        door_class,
        unreal.Vector(*config["location"]),
        unreal.Rotator(0.0, 0.0, 0.0),
    )
    door.set_actor_label(config["label"])
    door.set_editor_property("DoorSize", unreal.Vector(*config["door_size"]))
    door.set_editor_property("ClosedPanelOffset", unreal.Vector(*config["closed_offset"]))
    door.set_editor_property("DoorTravelOffset", unreal.Vector(*config["travel_offset"]))
    door.set_editor_property("ClosedHoldSeconds", 1.0)
    door.set_editor_property("TravelSeconds", 1.8)
    door.set_editor_property("OpenHoldSeconds", 1.35)
    door.set_editor_property("PhaseOffsetSeconds", config["phase_offset"])
    door.set_editor_property("PreviewOpenFraction", 0.0)
    return door


def main():
    unreal.EditorLevelLibrary.load_level(MAP_PATH)
    for config in DOOR_CONFIGS:
        destroy_actor_with_label(config["static_label"])
        destroy_actor_with_label(config["label"])
        spawn_dynamic_door(config)
    unreal.EditorLevelLibrary.save_current_level()
    unreal.log(f"Added dynamic showcase doors to {MAP_PATH}.")


if __name__ == "__main__":
    main()
