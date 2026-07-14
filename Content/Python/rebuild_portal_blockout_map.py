import unreal


FIRST_PERSON_LEVEL = "/Game/FirstPerson/Lvl_FirstPerson"
PORTAL_LEVEL = "/Game/PortalOcclusion/L_PortalOcclusion_Blockout"


def destroy_generated_environment_actors():
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        class_name = actor.get_class().get_name()
        label = actor.get_actor_label()
        if (
            class_name == "PortalBlockoutEnvironment"
            or label.startswith("Player Start - ")
            or label in {
                "Key Directional Light",
                "Soft Sky Light",
                "Light Atmosphere Fog",
            }
        ):
            unreal.EditorLevelLibrary.destroy_actor(actor)


def spawn_actor(actor_class, label, location, rotation=None):
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        actor_class,
        location,
        rotation or unreal.Rotator(0.0, 0.0, 0.0),
    )
    actor.set_actor_label(label)
    return actor


def main():
    unreal.EditorLevelLibrary.load_level(FIRST_PERSON_LEVEL)
    destroy_generated_environment_actors()
    unreal.EditorLevelLibrary.save_current_level()
    unreal.log("Removed generated portal blockout actors from Lvl_FirstPerson.")

    if unreal.EditorAssetLibrary.does_asset_exist(PORTAL_LEVEL):
        unreal.EditorLevelLibrary.load_level(PORTAL_LEVEL)
        destroy_generated_environment_actors()
    else:
        unreal.EditorLevelLibrary.new_level(PORTAL_LEVEL)

    environment = spawn_actor(
        unreal.PortalBlockoutEnvironment,
        "Portal Occlusion Blockout Environment",
        unreal.Vector(0.0, 0.0, 0.0),
    )
    environment.set_editor_property("bBuildLabels", False)
    environment.set_editor_property("WallHeight", 260.0)

    spawn_actor(
        unreal.PlayerStart,
        "Player Start - Control Room",
        unreal.Vector(-1280.0, -150.0, 92.0),
        unreal.Rotator(0.0, 0.0, 0.0),
    )

    sun = spawn_actor(
        unreal.DirectionalLight,
        "Key Directional Light",
        unreal.Vector(0.0, 0.0, 900.0),
        unreal.Rotator(-45.0, -35.0, 0.0),
    )
    sun.get_component_by_class(unreal.DirectionalLightComponent).set_editor_property("intensity", 2.2)

    sky = spawn_actor(
        unreal.SkyLight,
        "Soft Sky Light",
        unreal.Vector(0.0, 0.0, 400.0),
    )
    sky.get_component_by_class(unreal.SkyLightComponent).set_editor_property("intensity", 0.55)

    spawn_actor(
        unreal.ExponentialHeightFog,
        "Light Atmosphere Fog",
        unreal.Vector(0.0, 0.0, 120.0),
    )

    unreal.EditorLevelLibrary.save_current_level()
    unreal.log(f"Created clean portal occlusion blockout map at {PORTAL_LEVEL}.")


if __name__ == "__main__":
    main()
