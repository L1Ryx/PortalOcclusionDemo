import unreal


MAP_PATH = "/Game/PortalOcclusion/L_AcousticLab_Blockout"
CUBE_PATH = "/Engine/BasicShapes/Cube.Cube"
GRID_MAT_PATH = "/Game/LevelPrototyping/Materials/MI_PrototypeGrid_Gray.MI_PrototypeGrid_Gray"
DARK_MAT_PATH = "/Game/LevelPrototyping/Materials/MI_PrototypeGrid_TopDark.MI_PrototypeGrid_TopDark"
ACCENT_MAT_PATH = "/Game/FirstPerson/MI_FirstPersonColorway.MI_FirstPersonColorway"


def asset(path):
    loaded = unreal.EditorAssetLibrary.load_asset(path)
    if not loaded:
        raise RuntimeError(f"Could not load asset: {path}")
    return loaded


def clear_level():
    for actor in list(unreal.EditorLevelLibrary.get_all_level_actors()):
        unreal.EditorLevelLibrary.destroy_actor(actor)


def cube_actor(label, location, size, material, collision=True):
    mesh = asset(CUBE_PATH)
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.StaticMeshActor,
        unreal.Vector(*location),
        unreal.Rotator(0.0, 0.0, 0.0),
    )
    actor.set_actor_label(label)
    component = actor.static_mesh_component
    component.set_static_mesh(mesh)
    component.set_material(0, material)
    component.set_mobility(unreal.ComponentMobility.STATIC)
    component.set_collision_enabled(
        unreal.CollisionEnabled.QUERY_AND_PHYSICS
        if collision
        else unreal.CollisionEnabled.NO_COLLISION
    )
    actor.set_actor_scale3d(unreal.Vector(size[0] / 100.0, size[1] / 100.0, size[2] / 100.0))
    return actor


def wall_x(label, y, x_min, x_max, material, height=280.0, thickness=40.0):
    length = abs(x_max - x_min)
    center_x = (x_min + x_max) * 0.5
    cube_actor(label, (center_x, y, height * 0.5), (length, thickness, height), material)


def wall_y(label, x, y_min, y_max, material, height=280.0, thickness=40.0):
    length = abs(y_max - y_min)
    center_y = (y_min + y_max) * 0.5
    cube_actor(label, (x, center_y, height * 0.5), (thickness, length, height), material)


def threshold(label, location, size, material):
    cube_actor(label, location, size, material, collision=False)


def build_layout():
    grid = asset(GRID_MAT_PATH)
    dark = asset(DARK_MAT_PATH)
    accent = asset(ACCENT_MAT_PATH)

    # One continuous collision floor: centered on the demo, top face at z = 0.
    cube_actor("Walkable_Floor", (450.0, 0.0, -25.0), (5400.0, 3400.0, 50.0), grid)

    # Control room -> atrium -> corridor/listening bay with north/south branches.
    wall_y("Control_West", -2100.0, -500.0, 500.0, grid)
    wall_x("Control_North", 500.0, -2100.0, -900.0, grid)
    wall_x("Control_South", -500.0, -2100.0, -900.0, grid)

    wall_y("Atrium_West_S", -900.0, -700.0, -180.0, grid)
    wall_y("Atrium_West_N", -900.0, 180.0, 700.0, grid)
    wall_y("Atrium_East_S", 500.0, -700.0, -180.0, grid)
    wall_y("Atrium_East_N", 500.0, 180.0, 700.0, grid)
    wall_x("Atrium_North_W", 700.0, -900.0, -260.0, grid)
    wall_x("Atrium_North_E", 700.0, 260.0, 500.0, grid)
    wall_x("Atrium_South_W", -700.0, -900.0, -260.0, grid)
    wall_x("Atrium_South_E", -700.0, 260.0, 500.0, grid)

    wall_x("Corridor_South", -300.0, 500.0, 2100.0, grid)
    wall_x("Corridor_North_W", 300.0, 500.0, 900.0, grid)
    wall_x("Corridor_North_E", 300.0, 1650.0, 2100.0, grid)

    wall_y("Listening_West_S", 2100.0, -350.0, -140.0, grid)
    wall_y("Listening_West_N", 2100.0, 140.0, 350.0, grid)
    wall_y("Listening_East", 3000.0, -350.0, 350.0, grid)
    wall_x("Listening_North", 350.0, 2100.0, 3000.0, grid)
    wall_x("Listening_South", -350.0, 2100.0, 3000.0, grid)

    wall_y("Reverb_West", -700.0, 700.0, 1550.0, dark)
    wall_y("Reverb_East", 500.0, 700.0, 1550.0, dark)
    wall_x("Reverb_North", 1550.0, -700.0, 500.0, dark)

    wall_y("Machinery_West", 900.0, 300.0, 1250.0, grid)
    wall_y("Machinery_East", 2100.0, 300.0, 1250.0, grid)
    wall_x("Machinery_North", 1250.0, 900.0, 2100.0, grid)

    wall_y("Courtyard_West", -300.0, -1550.0, -700.0, dark)
    wall_y("Courtyard_East", 1500.0, -1550.0, -700.0, dark)
    wall_x("Courtyard_South", -1550.0, -300.0, 1500.0, dark)
    wall_x("Courtyard_North_W", -700.0, -300.0, -260.0, dark)
    wall_x("Courtyard_North_E", -700.0, 260.0, 1500.0, dark)

    # Preset-style orange wall accents. These sit slightly proud of the walls
    # to avoid z-fighting and make the portal test spaces easier to read.
    cube_actor("Accent_Control_BackWall", (-2101.0, 0.0, 150.0), (10.0, 520.0, 250.0), accent, collision=False)
    cube_actor("Accent_Atrium_North_Left", (-520.0, 701.0, 150.0), (360.0, 10.0, 250.0), accent, collision=False)
    cube_actor("Accent_Atrium_South_Right", (350.0, -701.0, 150.0), (260.0, 10.0, 250.0), accent, collision=False)
    cube_actor("Accent_Corridor_LongWall", (1280.0, -301.0, 150.0), (580.0, 10.0, 250.0), accent, collision=False)
    cube_actor("Accent_Reverb_BackWall", (-100.0, 1551.0, 150.0), (620.0, 10.0, 250.0), accent, collision=False)
    cube_actor("Accent_Machinery_RightWall", (2101.0, 780.0, 150.0), (10.0, 520.0, 250.0), accent, collision=False)
    cube_actor("Accent_Courtyard_BackWall", (580.0, -1551.0, 150.0), (700.0, 10.0, 250.0), accent, collision=False)
    cube_actor("Accent_Listening_BackWall", (3001.0, 0.0, 150.0), (10.0, 460.0, 250.0), accent, collision=False)

    # Orange markers are non-blocking, tiny, and live on top of the floor.
    threshold("P01_Atrium_Reverb", (0.0, 700.0, 5.0), (480.0, 26.0, 10.0), accent)
    threshold("P02_Atrium_Courtyard", (0.0, -700.0, 5.0), (480.0, 26.0, 10.0), accent)
    threshold("P03_Control_Atrium", (-900.0, 0.0, 5.0), (26.0, 360.0, 10.0), accent)
    threshold("P04_Atrium_Corridor", (500.0, 0.0, 5.0), (26.0, 360.0, 10.0), accent)
    threshold("P05_Corridor_Machinery", (1500.0, 300.0, 5.0), (360.0, 26.0, 10.0), accent)
    threshold("P06_Corridor_Listening", (2100.0, 0.0, 5.0), (26.0, 280.0, 10.0), accent)

    # Collision door samples for later open-fraction testing.
    cube_actor("Door_P01_HalfOpen", (135.0, 718.0, 118.0), (150.0, 18.0, 236.0), accent)
    cube_actor("Door_P03_Sliding", (-918.0, 105.0, 118.0), (18.0, 110.0, 236.0), accent)
    cube_actor("Door_P05_Partial", (1600.0, 318.0, 118.0), (105.0, 18.0, 236.0), accent)

    # Small markers only. No labels, no giant frames.
    threshold("Source_A_Courtyard", (600.0, -1250.0, 20.0), (80.0, 80.0, 40.0), accent)
    threshold("Source_B_Machinery", (1500.0, 940.0, 20.0), (80.0, 80.0, 40.0), accent)
    threshold("Listener_Start_Marker", (-1500.0, 0.0, 20.0), (90.0, 90.0, 40.0), accent)
    threshold("Listener_Compare_Marker", (2550.0, 0.0, 20.0), (90.0, 90.0, 40.0), accent)

    player = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.PlayerStart,
        unreal.Vector(-1500.0, 0.0, 95.0),
        unreal.Rotator(0.0, 0.0, 0.0),
    )
    player.set_actor_label("PlayerStart_ControlRoom")

    sun = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.DirectionalLight,
        unreal.Vector(0.0, 0.0, 700.0),
        unreal.Rotator(-45.0, -35.0, 0.0),
    )
    sun.set_actor_label("Key_DirectionalLight")
    sun.get_component_by_class(unreal.DirectionalLightComponent).set_editor_property("intensity", 2.0)

    sky = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.SkyLight,
        unreal.Vector(0.0, 0.0, 400.0),
    )
    sky.set_actor_label("Soft_SkyLight")
    sky.get_component_by_class(unreal.SkyLightComponent).set_editor_property("intensity", 0.65)


def main():
    if unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        unreal.EditorLevelLibrary.load_level(MAP_PATH)
    else:
        unreal.EditorLevelLibrary.new_level(MAP_PATH)

    clear_level()
    build_layout()
    unreal.EditorLevelLibrary.save_current_level()
    unreal.log(f"Built baked acoustic lab blockout at {MAP_PATH}.")


if __name__ == "__main__":
    main()
