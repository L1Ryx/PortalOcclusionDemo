import unreal


def main():
    actor_class = unreal.PortalBlockoutEnvironment
    existing = [
        actor
        for actor in unreal.EditorLevelLibrary.get_all_level_actors()
        if actor.get_class().get_name() == "PortalBlockoutEnvironment"
    ]

    for actor in existing:
        unreal.EditorLevelLibrary.destroy_actor(actor)

    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        actor_class,
        unreal.Vector(0.0, 0.0, 0.0),
        unreal.Rotator(0.0, 0.0, 0.0),
    )
    actor.set_actor_label("Portal Occlusion Blockout Environment")

    unreal.EditorLevelLibrary.save_current_level()
    unreal.log("Spawned Portal Occlusion Blockout Environment.")


if __name__ == "__main__":
    main()
