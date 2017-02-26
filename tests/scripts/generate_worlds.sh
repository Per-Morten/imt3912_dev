!./bin/bash

printf "Create normal_sprite\n"
./create_world_with_n_actors.sh normal 10 SpriteActor > normal_sprite_10.json
./create_world_with_n_actors.sh normal 100 SpriteActor > normal_sprite_100.json
./create_world_with_n_actors.sh normal 1000 SpriteActor > normal_sprite_1000.json
./create_world_with_n_actors.sh normal 10000 SpriteActor > normal_sprite_10000.json
./create_world_with_n_actors.sh normal 100000 SpriteActor > normal_sprite_100000.json

printf "Create normal_physics\n"
./create_world_with_n_actors.sh normal 10 PhysicsActor > normal_physics_10.json
./create_world_with_n_actors.sh normal 100 PhysicsActor > normal_physics_100.json
./create_world_with_n_actors.sh normal 1000 PhysicsActor > normal_physics_1000.json
./create_world_with_n_actors.sh normal 10000 PhysicsActor > normal_physics_10000.json
./create_world_with_n_actors.sh normal 100000 PhysicsActor > normal_physics_100000.json

printf "Create normal_physics_sprite\n"
./create_world_with_n_actors.sh normal 5 PhysicsActor 5 SpriteActor > normal_physics_5_sprite_5.json
./create_world_with_n_actors.sh normal 50 PhysicsActor 50 SpriteActor > normal_physics_50_sprite_50.json
./create_world_with_n_actors.sh normal 500 PhysicsActor 500 SpriteActor > normal_physics_500_sprite_500.json
./create_world_with_n_actors.sh normal 5000 PhysicsActor 5000 SpriteActor > normal_physics_5000_sprite_5000.json
./create_world_with_n_actors.sh normal 50000 PhysicsActor 50000 SpriteActor > normal_physics_50000_sprite_50000.json

printf "Create shuffle_physics_sprite\n"
./create_world_with_n_actors.sh shuffle 5 PhysicsActor 5 SpriteActor > shuffle_physics_5_sprite_5.json
./create_world_with_n_actors.sh shuffle 50 PhysicsActor 50 SpriteActor > shuffle_physics_50_sprite_50.json
./create_world_with_n_actors.sh shuffle 500 PhysicsActor 500 SpriteActor > shuffle_physics_500_sprite_500.json
./create_world_with_n_actors.sh shuffle 5000 PhysicsActor 5000 SpriteActor > shuffle_physics_5000_sprite_5000.json
./create_world_with_n_actors.sh shuffle 50000 PhysicsActor 50000 SpriteActor > shuffle_physics_50000_sprite_50000.json

