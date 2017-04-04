#include "S_Campaign.h"
#include "j1Player.h"
#include "Gui.h"
#include "j1GameStartMenuBack.h"

S_Campaign::S_Campaign()
{
	scene_str = "Campaign";
}

S_Campaign::~S_Campaign()
{
}

bool S_Campaign::Awake(pugi::xml_node& conf)
{
	controllerlayout = App->gui->CreateImage(iPoint(1, 0), &controllerlayout_rec, false);
	((Gui*)controllerlayout)->SetListener(this);
	controllerlayout->SetVisible(false);
	newcampaign = App->gui->CreateButton(iPoint(920, 490), &std::string("New Campaign"), ButtonType::idle_hover_pressed, &idle_button_rect, &hover_button_rect, &pressed_button_rect, false);
	newcampaign->SetFont(App->font->Sherwood20);
	((Gui*)newcampaign)->SetListener(this);
	newcampaign->SetVisible(false);
	newcampaign->Focusable(true);
	back = App->gui->CreateButton(iPoint(920, 600), &std::string("Back"), ButtonType::idle_hover_pressed, &idle_button_rect, &hover_button_rect, &pressed_button_rect, false);
	back->SetFont(App->font->Sherwood20);
	((Gui*)back)->SetListener(this);
	back->SetVisible(false);
	back->Focusable(true);

	buttons.push_back(newcampaign);
	buttons.push_back(back);

	return true;
}

bool S_Campaign::Start()
{
	controllerlayout->SetVisible(true);
	newcampaign->SetVisible(true);
	back->SetVisible(true);

	App->gui->SetFocus(buttons.front());

	return true;
}

bool S_Campaign::Update()
{
	MenuInput(&buttons);
	return true;
}

bool S_Campaign::Clean()
{
	controllerlayout->SetVisible(false);
	newcampaign->SetVisible(false);
	back->SetVisible(false);
	return true;
}

void S_Campaign::OnGui(Gui* ui, GuiEvent event)
{
	if ((ui == (Gui*)newcampaign) && (event == GuiEvent::mouse_lclk_down))
	{
		App->scene->ChangeScene(Scene_ID::world);
		App->scene->Hide();
		App->startmenuback->Freeze(true);
	}
	if ((ui == (Gui*)back) && (event == GuiEvent::mouse_lclk_down))
	{
		App->scene->Show(Scene_ID::mainmenu);
	}
}