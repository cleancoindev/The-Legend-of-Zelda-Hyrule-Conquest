#include "j1Player.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Pathfinding.h"
#include <algorithm>
#include"j1Collision.h"
#include "O_ChangeHeight.h"
#include"j1Enemy.h"

bool j1Player::Awake(pugi::xml_node& config)
{
	
	Link = new P_Link();
	Zelda = new P_Zelda();
	cooperative = true;
	Link->movement_direction = move_idle;
	Zelda->movement_direction = move_idle;
	Link->sprites_vector = new std::vector<Animation>;
	Zelda->sprites_vector = new std::vector<Animation>;
	Link->sprites_folder.create(config.child("folder").child_value());
	Zelda->sprites_folder.create(config.child("folder").child_value());
	Link->actual_event = player_event::idle;
	Zelda->actual_event = player_event::idle;
	Link->character_direction = direction::down;
	Zelda->character_direction = direction::down;

	Link->collision = App->collision->AddCollider({ Link->pos.x,Link->pos.y,32,32 }, collider_link, Link, this);
	Link->front_collider = App->collision->AddCollider({ Link->tilepos.x*8,Link->tilepos.y*8 + 32,32,16 }, front_link, Link, this);
	Zelda->collision = App->collision->AddCollider({ Zelda->pos.x,Zelda->pos.y,32,32 }, collider_zelda, Zelda, this);
	Zelda->front_collider = App->collision->AddCollider({ Zelda->pos.x,Zelda->pos.y + 32,32,16}, front_zelda, Zelda, this);
	return true;
}

bool j1Player::Start()
{


	//Change this for link spritesheet
	Link->character_texture = App->tex->Load("textures/map.png");

	//Change this for the zelda spritesheet
	Zelda->character_texture = Link->character_texture;
	

	Link->LoadAnimation("sprites/Link_Sprites_trim.xml");
	Zelda->LoadAnimation("sprites/Zelda_Sprites.xml");

	Link->actual_animation = Link->sprites_vector[0][0];
	Zelda->actual_animation = Zelda->sprites_vector[0][0];
	selected_character = Link;
	other_character = Zelda;
	change = false;
	cooperative = true;
	return true;
}

bool j1Player::PreUpdate()
{
	return true;
}

bool j1Player::Update(float dt)
{
	//Change the tile_pos
	Link->tilepos.x = (Link->pos.x + 8) / 16;
	Link->tilepos.y = (Link->pos.y + 8) / 16;
	Zelda->tilepos.x = (Zelda->pos.x + 8) / 16;
	Zelda->tilepos.y = (Zelda->pos.y + 8) / 16;

	Link->GetAdjacents();
	Zelda->GetAdjacents();
	int asd = Link->GetLogicHeightPlayer();
	
		
	//2 Players
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
	//	cooperative = !cooperative;
		selected_character = Link;
		other_character = Zelda;
	}

	if (cooperative == true) {
		Link->GetEvent();
		Zelda->GetEvent();
		Link->ExecuteEvent(dt);
		Zelda->ExecuteEvent(dt);

	}

	//Draw the two characters
	Draw();

	//Change the positions of player colliders
	Link->collision->SetPos(Link->pos.x, Link->pos.y, Link->GetLogicHeightPlayer());
	Zelda->collision->SetPos(Zelda->pos.x, Zelda->pos.y, Zelda->GetLogicHeightPlayer());
	Link->UpdateColliderFront();
	Zelda->UpdateColliderFront();

	return true;
}

bool j1Player::PostUpdate()
{
	return true;
}

void j1Player::Draw()
{
	SDL_Rect rect;
	rect = { Link->tilepos.x*16, Link->tilepos.y*16, 32, 32 };
	App->render->Blit(Link->character_texture, Link->pos.x - 3 , Link->pos.y - 12, &Link->actual_animation.GetCurrentFrame());
	App->render->Blit(Zelda->character_texture, Zelda->pos.x - 3, Zelda->pos.y - 12 , &Zelda->actual_animation.GetCurrentFrame());
	//App->render->DrawQuad(rect, 0, 0, 255, 255, true, true);
	
}




void j1Player::ActivatePathfinding()
{

	static bool pathfinding_active = true;
	if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN) {
		pathfinding_active = !pathfinding_active;
	}
	if (pathfinding_active == true) {
		App->pathfinding->CreatePath(other_character->tilepos, selected_character->tilepos);
		
	}

	int dist = 0;
	dist = sqrt((selected_character->tilepos.x - other_character->tilepos.x)* (selected_character->tilepos.x - other_character->tilepos.x) + (selected_character->tilepos.y - other_character->tilepos.y)*(selected_character->tilepos.y - other_character->tilepos.y));

	if (chase == true&& dist >3 && pathfinding_active == true) {
		App->pathfinding->Move(other_character, selected_character);

	}
	else {		
		other_character->actual_event = idle;
		
	}

		if (other_character->tilepos == selected_character->tilepos || App->pathfinding->GetLastPath()->Count() == 0) {
			App->pathfinding->DeletePath();
			chase = false;
		}
		else chase = true;
	
}

bool j1Player::Move_Camera()
{

	int mult = -1;
	static int i = 0;
	
	//Time between character change
	const int time = 20;

	static float rest_x;
	static float rest_y;
	int sum_x = 0;
	int sum_y = 0;
	int x = other_character->pos.x * App->win->GetScale() - selected_character->pos.x * App->win->GetScale();
	int y = other_character->pos.y * App->win->GetScale() - selected_character->pos.y * App->win->GetScale();

	if (x > 0)mult = 1;

	rest_x = rest_x + x % time;
	rest_y = rest_y + y % time;

	if (mult * rest_x >= time) {
		sum_x = mult;
		rest_x = rest_x - time * mult;
	}

	mult = -1;
	if (y > 0)mult = 1;

	if (mult * rest_y >= time) {
		sum_y = mult;
		rest_y = rest_y - time * mult;
	}
	App->render->camera.x = App->render->camera.x + x / time + sum_x;
	App->render->camera.y = App->render->camera.y + y / time + sum_y;
	if (i >= time) {
		change = false;
		i = 0;
		rest_x = 0;
		rest_y = 0;
	}
	i++;
	
	return false;
}

void j1Player::OnCollision(Collider * collider1, Collider * collider2)
{
	Link->can_pick_up = false;
	Character* character = nullptr;
	if (collider1->type == collider_link || collider2->type == collider_link)
		character = Link;	
	else if (collider1->type == collider_zelda || collider2->type == collider_zelda)
		character = Zelda;
	else if (collider1->type == front_zelda || collider2->type == front_zelda)
		character = Zelda;
	else if (collider1->type == front_link || collider2->type == front_link)
		character = Link;


	if (collider1->type == collider_button) {
		auto temp = (Object*)collider1->parent;
		if (App->input->GetKey(SDL_SCANCODE_T)==KEY_DOWN) {
			temp->Action();
		}
	}
	else if(collider2->type == collider_button) {
		auto temp = (Object*)collider2->parent;
		if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN) {
			temp->Action();
		}
	}

	else if (collider1->type == collider_change_height) {
		auto temp = (ChangeHeight*)collider1->parent;
		character->ChangeLogicHeightPlayer(temp->height);
	}
	else if (collider2->type == collider_change_height) {
		auto temp = (ChangeHeight*)collider2->parent;
		character->ChangeLogicHeightPlayer(temp->height);

	}
	else if (collider1->type == collider_jump) {
		if (character->can_move == false && character->doing_script == false) {
			character->can_jump = true;

		}
	}
	else if (collider2->type == collider_jump) {
		if (character->can_move == false && character->doing_script == false) {
			character->can_jump = true;

		}
	}
	else if (collider1->type == front_link) {
		if(collider2->type == collider_zelda && !Link->im_lifting)
		Link->can_pick_up = true;
		
	}
	else if (collider2->type == front_link ) {
		if (collider1->type == collider_zelda && !Link->im_lifting)
			Link->can_pick_up = true;
		
	}
	else if (collider1->type == COLLIDER_TYPE::collider_enemy) {
		
	}
	else if (collider2->type == COLLIDER_TYPE::collider_enemy) {
		/*Enemy* temp = (Enemy*)collider2->parent;
		for (std::) {

		}
		App->enemy->V_MyEnemies.erase();*/
		live--;
		LOG("Link life: %i", live);
	}



}

