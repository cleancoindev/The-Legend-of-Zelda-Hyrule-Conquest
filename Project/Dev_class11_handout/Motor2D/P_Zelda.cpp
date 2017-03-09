#include "P_Zelda.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1FileSystem.h"
#include "j1Player.h"

void P_Zelda::Attack()
{
}

void P_Zelda::LoadAnimation(const char * path)
{
	
		p2SString tmp("%s%s", sprites_folder.GetString(), path);

		char* buf;
		int size = App->fs->Load(tmp.GetString(), &buf);
		pugi::xml_parse_result result = sprites_file.load_buffer(buf, size);

		if (result == NULL)
		{
			//LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
			//ret = false;
		}

		pugi::xml_node info = sprites_file.child("TextureAtlas");
		char* imagepath = (char*)info.attribute("imagePath").as_string();

		character_texture = App->tex->Load(imagepath);

		pugi::xml_node animations = info.first_child();

		char* last_name = nullptr;
		int x = animations.attribute("x").as_int();
		int y = animations.attribute("y").as_int();
		int w = animations.attribute("w").as_int();
		int h = animations.attribute("h").as_int();
		SDL_Rect anim = { x,y,w,h };
		Animation temp_animation;


		char* name = (char*)animations.attribute("name").as_string();
	
		auto temp = animations;
		last_name = name;
		while (animations) {
			auto temp = animations;
			name = (char*)animations.attribute("name").as_string();
			x = temp.attribute("x").as_int();
			y = temp.attribute("y").as_int();
			w = temp.attribute("w").as_int();
			h = temp.attribute("h").as_int();
			anim = { x,y,w,h };

			if (strcmp(name, last_name)) {

				temp_animation.speed = 0.2;
				sprites_vector->push_back(temp_animation);
				temp_animation.Reset();
				temp_animation.last_frame = 0;

				
				temp_animation.PushBack(anim);
				last_name = name;
			}
			else {
				temp_animation.PushBack(anim);
			}
			animations = animations.next_sibling();
		}

	
	

}

void P_Zelda::ChangeAnimation(movement_animation animation)
{
	static movement_animation last_animation = animation_idle_down;

	if (last_animation != animation) {
		this->actual_animation = this->sprites_vector[0][animation];
		last_animation = animation;
	}

}

SDL_Scancode UP = SDL_SCANCODE_W;

key_state P_Zelda::GetEvent()
{
	SDL_Scancode UP;
	SDL_Scancode DOWN;	
	SDL_Scancode LEFT;
	SDL_Scancode RIGHT;

	if (App->player->cooperative == false) {
		UP = SDL_SCANCODE_W;
		DOWN = SDL_SCANCODE_S;
		LEFT = SDL_SCANCODE_A;
		RIGHT = SDL_SCANCODE_D;

	}
	else {
		UP = SDL_SCANCODE_UP;
		DOWN = SDL_SCANCODE_DOWN;
		LEFT = SDL_SCANCODE_LEFT;
		RIGHT = SDL_SCANCODE_RIGHT;

	}
		key_state state = idle;
		movement_animation animation_state = animation_idle_down;
		static movement_animation last_state = animation_idle_down;
		if (App->input->GetKey(UP) == KEY_REPEAT) {
			if (App->input->GetKey(LEFT) == KEY_REPEAT) {
				state = left_up;
				animation_state = animation_up;
			}
			else if (App->input->GetKey(RIGHT) == KEY_REPEAT) {
				state = right_up;
				animation_state = animation_up;
			}
			else {
				state = up;
				animation_state = animation_up;

			}
		}

		else if (App->input->GetKey(DOWN) == KEY_REPEAT) {
			if (App->input->GetKey(LEFT) == KEY_REPEAT) {
				state = left_down;
				animation_state = animation_down;
			}
			else if (App->input->GetKey(RIGHT) == KEY_REPEAT) {
				state = right_down;
				animation_state = animation_down;
			}
			else {
				state = down;
				animation_state = animation_down;

			}
		}

		else if (App->input->GetKey(RIGHT) == KEY_REPEAT) {


			state = right;
			animation_state = animation_right;

		}
		else if (App->input->GetKey(LEFT) == KEY_REPEAT) {

			state = left;
			animation_state = animation_left;

		}
		else {
			state = idle;
			animation_state = animation_idle_down;
		}


		this->ChangeAnimation(animation_state);

		return state;
	}

	



