import unreal


MAP_PATH = "/Game/PortalOcclusion/L_AcousticLab_Blockout"
SKY_LABEL = "Default_SkyAtmosphere"
SUN_LABEL = "Key_DirectionalLight"
SKYLIGHT_LABEL = "Soft_SkyLight"


def try_set_editor_property(obj, property_name, value):
    try:
        obj.set_editor_property(property_name, value)
    except Exception:
        unreal.log_warning(f"Could not set {property_name} on {obj.get_name()}.")


def actor_with_label(label):
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        if actor.get_actor_label() == label:
            return actor
    return None


def spawn_actor(actor_class, label, location, rotation=None):
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        actor_class,
        location,
        rotation or unreal.Rotator(0.0, 0.0, 0.0),
    )
    actor.set_actor_label(label)
    return actor


def ensure_default_sky():
    if not actor_with_label(SKY_LABEL):
        spawn_actor(unreal.SkyAtmosphere, SKY_LABEL, unreal.Vector(0.0, 0.0, 0.0))

    sun = actor_with_label(SUN_LABEL)
    if not sun:
        sun = spawn_actor(
            unreal.DirectionalLight,
            SUN_LABEL,
            unreal.Vector(0.0, 0.0, 700.0),
            unreal.Rotator(-45.0, -35.0, 0.0),
        )

    sun_component = sun.get_component_by_class(unreal.DirectionalLightComponent)
    if sun_component:
        sun_component.set_editor_property("intensity", 2.0)
        try_set_editor_property(sun_component, "atmosphere_sun_light", True)
        try_set_editor_property(sun_component, "atmosphere_sun_light_index", 0)

    sky_light = actor_with_label(SKYLIGHT_LABEL)
    if not sky_light:
        sky_light = spawn_actor(
            unreal.SkyLight,
            SKYLIGHT_LABEL,
            unreal.Vector(0.0, 0.0, 400.0),
        )

    sky_light_component = sky_light.get_component_by_class(unreal.SkyLightComponent)
    if sky_light_component:
        sky_light_component.set_editor_property("intensity", 0.65)


def main():
    unreal.EditorLevelLibrary.load_level(MAP_PATH)
    ensure_default_sky()
    unreal.EditorLevelLibrary.save_current_level()
    unreal.log(f"Added default sky atmosphere to {MAP_PATH}.")


if __name__ == "__main__":
    main()
