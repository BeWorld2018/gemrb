/* GemRB - Infinity Engine Emulator
 * Copyright (C) 2003 The GemRB Project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Id$
 *
 */

#include "../../includes/win32def.h"
#include "../../includes/strrefs.h"
#include "../Core/Actor.h"
#include "../Core/EffectQueue.h"
#include "../Core/Interface.h"
#include "../Core/Game.h"
#include "../Core/damages.h"
#include "../Core/GSUtils.h"  //needs for displaystringcore
#include "IWDOpc.h"

static ieResRef *iwd_spell_hits = NULL;
static int shcount = -1;

#define PI_PROTFROMEVIL 9
#define PI_FREEACTION 19
#define PI_HOPELESSNESS 47

static int fx_fade_rgb (Actor* Owner, Actor* target, Effect* fx);//e8
static int fx_iwd_visual_spell_hit (Actor* Owner, Actor* target, Effect* fx);//e9
static int fx_cold_damage (Actor* Owner, Actor* target, Effect* fx);//ea
//int fx_iwd_casting_glow (Actor* Owner, Actor* target, Effect* fx);//eb
static int fx_chill_touch (Actor* Owner, Actor* target, Effect* fx);//ec (how)
static int fx_chill_touch_panic (Actor* Owner, Actor* target, Effect* fx);//ec (iwd2)
static int fx_crushing_damage (Actor* Owner, Actor* target, Effect* fx);//ed
static int fx_save_bonus (Actor* Owner, Actor* target, Effect* fx); //ee
static int fx_slow_poison (Actor* Owner, Actor* target, Effect* fx); //ef
static int fx_iwd_monster_summoning (Actor* Owner, Actor* target, Effect* fx); //f0
static int fx_vampiric_touch (Actor* Owner, Actor* target, Effect* fx); //f1
// int fx_overlay f2 (same as PST)
static int fx_animate_dead (Actor* Owner, Actor* target, Effect* fx);//f3
//int fx_prayer (Actor* Owner, Actor* target, Effect* fx); //f4 pst
//int fx_prayer_bad (Actor* Owner, Actor* target, Effect* fx); //f5 pst
static int fx_summon_monster2 (Actor* Owner, Actor* target, Effect* fx); //f6
static int fx_burning_blood (Actor* Owner, Actor* target, Effect* fx); //f7 iwd
static int fx_burning_blood2 (Actor* Owner, Actor* target, Effect* fx); //f7 how, iwd2
static int fx_summon_shadow_monster (Actor* Owner, Actor* target, Effect* fx); //f8
static int fx_recitation (Actor* Owner, Actor* target, Effect* fx); //f9
static int fx_recitation_bad (Actor* Owner, Actor* target, Effect* fx); //fa
static int fx_lich_touch (Actor* Owner, Actor* target, Effect* fx); //fb
static int fx_blinding_orb (Actor* Owner, Actor* target, Effect* fx); //fc
// ac vs damage //fd
static int fx_remove_effects (Actor* Owner, Actor* target, Effect* fx); //fe
static int fx_salamander_aura (Actor* Owner, Actor* target, Effect* fx); //ff
static int fx_umberhulk_gaze (Actor* Owner, Actor* target, Effect* fx); //100
static int fx_zombielord_aura (Actor* Owner, Actor* target, Effect* fx); //101
//int fx_resist_spell (Actor* Owner, Actor* target, Effect* fx); //102
static int fx_summon_creature2 (Actor* Owner, Actor* target, Effect* fx); //103
//int fx_avatar_removal (Actor* Owner, Actor* target, Effect* fx); //104
//int fx_immunity_effect2 (Actor* Owner, Actor* target, Effect* fx); //105
static int fx_summon_pomab (Actor* Owner, Actor* target, Effect* fx); //106
static int fx_control_undead (Actor* Owner, Actor* target, Effect* fx); //107
static int fx_static_charge (Actor* Owner, Actor* target, Effect* fx); //108
static int fx_cloak_of_fear (Actor* Owner, Actor* target, Effect* fx); //109
//int fx_movement_modifier (Actor* Owner, Actor* target, Effect* fx); //10a
static int fx_remove_confusion (Actor* Owner, Actor* target, Effect* fx);//10b
static int fx_eye_of_the_mind (Actor* Owner, Actor* target, Effect* fx);//10c
static int fx_eye_of_the_sword (Actor* Owner, Actor* target, Effect* fx);//10d
static int fx_eye_of_the_mage (Actor* Owner, Actor* target, Effect* fx);//10e
static int fx_eye_of_venom (Actor* Owner, Actor* target, Effect* fx);//10f
static int fx_eye_of_the_spirit (Actor* Owner, Actor* target, Effect* fx);//110
static int fx_eye_of_fortitude (Actor* Owner, Actor* target, Effect* fx);//111
static int fx_eye_of_stone (Actor* Owner, Actor* target, Effect* fx);//112
static int fx_remove_seven_eyes (Actor* Owner, Actor* target, Effect* fx);//113
static int fx_remove_effect (Actor* Owner, Actor* target, Effect* fx);//114
static int fx_soul_eater (Actor* Owner, Actor* target, Effect* fx);//115
static int fx_shroud_of_flame (Actor* Owner, Actor* target, Effect* fx);//116
static int fx_shroud_of_flame2 (Actor* Owner, Actor* target, Effect* fx);//116
static int fx_animal_rage (Actor* Owner, Actor* target, Effect* fx);//117
static int fx_turn_undead (Actor* Owner, Actor* target, Effect* fx);//118
static int fx_turn_undead2 (Actor* Owner, Actor* target, Effect* fx);//118
static int fx_vitriolic_sphere (Actor* Owner, Actor* target, Effect* fx);//119
static int fx_suppress_hp (Actor* Owner, Actor* target, Effect* fx);//11a
static int fx_floattext (Actor* Owner, Actor* target, Effect* fx);//11b
static int fx_mace_of_disruption (Actor* Owner, Actor* target, Effect* fx);//11c
//0x11d Sleep2 ??? power word sleep?
//0x11e Reveal:Tracks (same as bg2)
//0x11f Protection:Backstab (same as bg2)
static int fx_set_state (Actor* Owner, Actor* target, Effect* fx); //120
static int fx_cutscene (Actor* Owner, Actor* target, Effect* fx);//121
static int fx_resist_spell (Actor* Owner, Actor* target, Effect *fx);//ce
static int fx_resist_spell_and_message (Actor* Owner, Actor* target, Effect *fx);//122
static int fx_rod_of_smithing (Actor* Owner, Actor* target, Effect* fx); //123
//0x124 MagicalRest (same as bg2)
//0x125 BeholderDispelMagic (???)
static int fx_harpy_wail (Actor* Owner, Actor* target, Effect* fx); //126
static int fx_jackalwere_gaze (Actor* Owner, Actor* target, Effect* fx); //127
//0x128 ModifyGlobalVariable (same as bg2)
//0x129 HideInShadows (same as bg2)
static int fx_use_magic_device_modifier (Actor* Owner, Actor* target, Effect* fx);//12a

static int fx_hopelessness (Actor* /*Owner*/, Actor* target, Effect* fx);//400
static int fx_protection_from_evil (Actor* /*Owner*/, Actor* target, Effect* fx);//401
static int fx_free_action_iwd2 (Actor* /*Owner*/, Actor* target, Effect* fx); //418

//No need to make these ordered, they will be ordered by EffectQueue
static EffectRef effectnames[] = {
	{ "Color:FadeRGB", fx_fade_rgb, 0}, //e8
	{ "IWDVisualSpellHit", fx_iwd_visual_spell_hit, 0}, //e9
	{ "ColdDamage", fx_cold_damage, 0}, //ea
	{ "ChillTouch", fx_chill_touch, 0}, //ec (how)
	{ "ChillTouchPanic", fx_chill_touch_panic, 0}, //ec (iwd2)
	{ "CrushingDamage", fx_crushing_damage, 0}, //ed
	{ "SaveBonus", fx_save_bonus, 0}, //ee
	{ "SlowPoison", fx_slow_poison, 0}, //ef
	{ "IWDMonsterSummoning", fx_iwd_monster_summoning, 0}, //f0
	{ "VampiricTouch", fx_vampiric_touch, 0}, //f1
	{ "AnimateDead", fx_animate_dead, 0}, //f3
	{ "SummonMonster2", fx_summon_monster2, 0}, //f6
	{ "BurningBlood", fx_burning_blood, 0}, //f7
	{ "BurningBlood2", fx_burning_blood2, 0}, //f7
	{ "SummonShadowMonster", fx_summon_shadow_monster, 0}, //f8
	{ "Recitation", fx_recitation, 0}, //f9
	{ "RecitationBad", fx_recitation_bad, 0},//fa
	{ "LichTouch", fx_lich_touch, 0},//fb
	{ "BlindingOrb", fx_blinding_orb, 0}, //fc
	{ "RemoveEffects", fx_remove_effects, 0}, //fe
	{ "SalamanderAura", fx_salamander_aura, 0}, //ff
	{ "UmberHulkGaze", fx_umberhulk_gaze, 0}, //100
	{ "ZombieLordAura", fx_zombielord_aura, 0},//101
	{ "SummonCreature2", fx_summon_creature2, 0}, //103
	{ "SummonPomab", fx_summon_pomab, 0}, //106
	{ "ControlUndead", fx_control_undead, 0}, //107
	{ "StaticCharge", fx_static_charge, 0}, //108
	{ "CloakOfFear", fx_cloak_of_fear, 0}, //109
	{ "RemoveConfusion", fx_remove_confusion, 0},//10b
	{ "EyeOfTheMind", fx_eye_of_the_mind, 0}, //10c
	{ "EyeOfTheSword", fx_eye_of_the_sword, 0}, //10d
	{ "EyeOfTheMage", fx_eye_of_the_mage, 0}, //10e
	{ "EyeOfVenom", fx_eye_of_venom, 0}, //10f
	{ "EyeOfTheSpirit", fx_eye_of_the_spirit, 0}, //110
	{ "EyeOfFortitude", fx_eye_of_fortitude, 0}, //111
	{ "EyeOfStone", fx_eye_of_stone, 0}, //112
	{ "RemoveSevenEyes", fx_remove_seven_eyes, 0}, //113
	{ "RemoveEffect", fx_remove_effect, 0}, //114
	{ "SoulEater", fx_soul_eater, 0}, //115
	{ "ShroudOfFlame", fx_shroud_of_flame, 0},//116
	{ "ShroudOfFlame2", fx_shroud_of_flame2, 0},//116
	{ "AnimalRage", fx_animal_rage, 0}, //117 - berserk?
	{ "TurnUndead", fx_turn_undead, 0}, //118 how
	{ "TurnUndead2", fx_turn_undead2, 0}, //118 iwd2
	{ "VitriolicSphere", fx_vitriolic_sphere, 0}, //119
	{ "SuppressHP", fx_suppress_hp, 0}, //11a -- some stat???
	{ "FloatText", fx_floattext, 0}, //11b
	{ "MaceOfDisruption", fx_mace_of_disruption, 0}, //11c
	{ "State:Set", fx_set_state, 0}, //120
	{ "CutScene", fx_cutscene, 0}, //121
	{ "Protection:Spell2", fx_resist_spell, 0}, //ce
	{ "Protection:Spell3", fx_resist_spell_and_message, 0}, //122
	{ "RodOfSmithing", fx_rod_of_smithing, 0}, //123
	{ "HarpyWail", fx_harpy_wail, 0}, //126
	{ "JackalWereGaze", fx_jackalwere_gaze, 0}, //127
	{ "UseMagicDeviceModifier", fx_use_magic_device_modifier, 0}, //12a
	//iwd2 effects
	{ "Hopelessness", fx_hopelessness, 0}, //400
	{ "ProtectionFromEvil", fx_protection_from_evil, 0}, //401
	{ "FreeAction2", fx_free_action_iwd2, 0}, //418
	{ "ControlUndead2", fx_control_undead, 0}, //425
	{ NULL, NULL, 0 },
};


IWDOpc::IWDOpc(void)
{
	core->RegisterOpcodes( sizeof( effectnames ) / sizeof( EffectRef ) - 1, effectnames );
}

IWDOpc::~IWDOpc(void)
{
}

//iwd got a weird targeting system
//the opcode parameters are:
//param1 - optional value used only rarely
//param2 - a specific condition mostly based on target's stats
//this is partly superior, partly inferior to the bioware
//ids targeting.
//superior because it can handle other stats and conditions
//inferior because it is not moddable
//The hardcoded conditions are simulated via the IWDIDSEntry
//structure.
//stat is usually a stat, but for special conditions it is a
//function code (>=0x100).
//If value is -1, then GemRB will use Param1, otherwise it is
//compared to the target's stat using the relation function.
//The relation function is exactly the same as the extended 
//diffmode for gemrb. (Thus scripts can use the very same relation
//functions).

typedef struct {
	ieDword value;
	ieWord stat;
	ieWord relation;
} IWDIDSEntry;

static int spellrescnt = -1;
static IWDIDSEntry *spellres = NULL;

static void ReadSpellProtTable(const ieResRef tablename)
{
	TableMgr * tab;
	if (spellres) {
		free(spellres);
	}
	spellres = NULL;
	spellrescnt = 0;
	int table=core->LoadTable( tablename );

	if (table<0) {
		return;
	}
	tab = core->GetTable( table );
	if (!tab) {
		core->DelTable(table);
		return;
	}
	spellrescnt=tab->GetRowCount();
	spellres = (IWDIDSEntry *) malloc(sizeof(IWDIDSEntry) * spellrescnt);
	if (!spellres) {
		core->DelTable(table);
		return;
	}
	for( int i=0;i<spellrescnt;i++) {
		spellres[i].stat = (ieWord) strtol(tab->QueryField(i,0),NULL,0 );
		spellres[i].value = (ieDword) strtol(tab->QueryField(i,1),NULL,0 );
		spellres[i].relation = (ieWord) strtol(tab->QueryField(i,2),NULL,0 );
	}
	core->DelTable(table);
	return;
}

//unusual types which need hacking (fake stats)
#define STI_SOURCE_TARGET     0x100
#define STI_SOURCE_NOT_TARGET 0x101
#define STI_CIRCLESIZE        0x102
#define STI_TWO_ROWS          0x103
#define STI_NOT_TWO_ROWS      0x104
#define STI_MORAL_ALIGNMENT   0x105
#define STI_AREATYPE          0x106
#define STI_DAYTIME           0x107
#define STI_EA                0x108

//returns true if iwd ids targeting resists the spell
static int check_iwd_targeting(Actor* Owner, Actor* target, ieDword value, ieDword type)
{
	if (spellrescnt==-1) {
		ReadSpellProtTable("splprot");
	}
	if (type>=(ieDword) spellrescnt) {
		return 0; //not resisted
	}

	ieDword idx = spellres[type].stat;
	ieDword val = spellres[type].value;
	//if IDS value is 'anything' then the supplied value is in Parameter1
	if (val==0xffffffff) {
		val = value;
	}
	switch (idx) {
	case STI_EA:
		return DiffCore(EARelation(Owner, target), val, spellres[type].relation);
	case STI_DAYTIME:
		return (core->GetGame()->GameTime%7200/3600) != val;
	case STI_AREATYPE:
		return DiffCore((ieDword) target->GetCurrentArea()->AreaType, val, spellres[type].relation);
	case STI_MORAL_ALIGNMENT:
		return DiffCore(Owner->GetStat(IE_ALIGNMENT)&0x3,target->GetStat(IE_ALIGNMENT)&0x3, spellres[type].relation);
	case STI_TWO_ROWS:
		if (check_iwd_targeting(Owner, target, value, idx)) return 0;
		if (check_iwd_targeting(Owner, target, value, val)) return 0;
		return 1;
	case STI_NOT_TWO_ROWS:
		if (check_iwd_targeting(Owner, target, value, idx)) return 1;
		if (check_iwd_targeting(Owner, target, value, val)) return 1;
		return 0;
	case STI_SOURCE_TARGET:
		if (Owner==target) {
			return 0;
		}
		return 1;
	case STI_SOURCE_NOT_TARGET:
		if (Owner!=target) {
			return 0;
		}
		return 1;
	case STI_CIRCLESIZE:
		return DiffCore((ieDword) target->GetAnims()->GetCircleSize(), val, spellres[type].relation);
	default:
		return DiffCore(target->GetStat(idx), val, spellres[type].relation);
	}
}

//iwd got a hardcoded 'fireshield' system
//this effect applies damage on ALL nearby actors, except the center
EffectRef fx_damage_opcode_ref={"Damage",NULL,-1};

static void ApplyDamageNearby(Actor* Owner, Actor* target, Effect *fx, ieDword damagetype)
{
	Effect *newfx = new Effect;
	//fill with zeros
	memset(newfx,0,sizeof(Effect));
	newfx->Opcode = EffectQueue::ResolveEffect(fx_damage_opcode_ref);
	newfx->Target = FX_TARGET_SELF;
	newfx->Power = fx->Power;
	newfx->Parameter1 = fx->Parameter1;
	newfx->Parameter2 = damagetype;
	//newfx->Resistance = ?
	newfx->Probability2=100;
	newfx->DiceThrown = fx->DiceThrown;
	newfx->DiceSides = fx->DiceSides;
	memcpy(newfx->Resource, fx->Resource,sizeof(newfx->Resource) );
	//applyeffectcopy on everyone near us
	Map *area = target->GetCurrentArea();
	int i = area->GetActorCount(true);
	while(i--) {
		Actor *victim = area->GetActor(i,true);
		if (target!=victim) continue;
		if (PersonalDistance(target, victim)<20) {
			core->ApplyEffect(newfx, victim, Owner);
		}
	}
	//finally remove the master copy
	delete newfx;
}

// 0xe8 Colour:FadeRGB
int fx_fade_rgb (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_fade_rgb (%2d): \n", fx->Opcode  );

	int speed = (fx->Parameter2 >> 16) & 0xFF;
	target->SetColorMod(-1, RGBModifier::ADD, speed,
						fx->Parameter1 >> 8, fx->Parameter1 >> 16,
						fx->Parameter1 >> 24, speed);

	return FX_NOT_APPLIED;
}

//0xe9 IWDVisualSpellHit
int fx_iwd_visual_spell_hit (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_iwd_visual_spell_hit (%2d): Type: %d\n", fx->Opcode, fx->Parameter2 );
	if (shcount<0) {
		shcount = core->ReadResRefTable("iwdshtab",iwd_spell_hits);
	}
	//delay apply until map is loaded
	Map *map = target->GetCurrentArea();
	if (!map) {
		return FX_APPLIED;
	}
	if (fx->Parameter2<(ieDword) shcount) {
		ScriptedAnimation *sca = core->GetScriptedAnimation(iwd_spell_hits[fx->Parameter2]);
		if (!sca) {
			return FX_NOT_APPLIED;
		}
		if (fx->Parameter1) {
			sca->XPos+=target->Pos.x;
			sca->YPos+=target->Pos.y;
		} else {
			sca->XPos+=fx->PosX;
			sca->YPos+=fx->PosY;
		}
		if (fx->Parameter2<32) {
			int tmp = fx->Parameter2>>2;
			if (tmp) {
				sca->SetFullPalette(tmp);
			}
		}
		sca->SetBlend();
		sca->PlayOnce();
		map->AddVVCell(sca);
	}
	return FX_NOT_APPLIED;
}

//0xea ColdDamage (special)
int fx_cold_damage (Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_cold_damage (%2d): Damage %d\n", fx->Opcode, fx->Parameter1 );
	target->Damage(fx->Parameter1, DAMAGE_COLD, Owner);
	return FX_NOT_APPLIED;
}

//0xeb CastingGlow2 will be same as original casting glow

//0xec ChillTouch (how)
//this effect is to simulate the composite effects of chill touch
//it is the usual iwd/how style hack
int fx_chill_touch (Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_chill_touch (%2d)\n", fx->Opcode);
	target->Damage(fx->Parameter1, DAMAGE_COLD, Owner);
	if (target->GetStat(IE_GENERAL)==GEN_UNDEAD) {
		target->Panic();
	}
	return FX_NOT_APPLIED;
}

//0xec ChillTouchPanic (iwd2)
//the undead check is made by IDS targeting as it should be
int fx_chill_touch_panic (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_chill_touch_panic (%2d)\n", fx->Opcode);
	STAT_SET(IE_MORALE, target->GetStat(IE_MORALEBREAK));
	return FX_APPLIED;
}

//0xed CrushingDamage (special)
int fx_crushing_damage (Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_crushing_damage (%2d): Damage %d\n", fx->Opcode, fx->Parameter1 );
	target->Damage(fx->Parameter1, DAMAGE_CRUSHING, Owner);
	return FX_NOT_APPLIED;
}

//0xee SaveBonus
int fx_save_bonus (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_save_bonus (%2d): Bonus %d Type: %d\n", fx->Opcode, fx->Parameter1, fx->Parameter2 );
	STAT_MOD( IE_SAVEVSDEATH );
	STAT_MOD( IE_SAVEVSWANDS );
	STAT_MOD( IE_SAVEVSPOLY );
	STAT_MOD( IE_SAVEVSBREATH );
	STAT_MOD( IE_SAVEVSSPELL );
	return FX_APPLIED;
}

//0xef SlowPoison
int fx_slow_poison (Actor* /*Owner*/, Actor* /*target*/, Effect* fx)
{
	if (0) printf( "fx_slow_poison (%2d): Damage %d\n", fx->Opcode, fx->Parameter1 );
	return FX_NOT_APPLIED;
}

#define IWD_MSC  13
ieResRef iwd_monster_2da[IWD_MSC]={"MSUMMO1","MSUMMO2","MSUMMO3","MSUMMO4",
 "MSUMMO5","MSUMMO6","MSUMMO7","ASUMMO1","ASUMMO2","ASUMMO3","CDOOM","GINSECT",
 "MSUMMOM"};

//0xf0 IWDMonsterSummoning
int fx_iwd_monster_summoning (Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_iwd_monster_summoning (%2d): ResRef:%s Anim:%s Type: %d\n", fx->Opcode, fx->Resource, fx->Resource2, fx->Parameter2 );
	//check the summoning limit?
	
	ieResRef monster;
	ieResRef hit;
	ieResRef areahit;

	if (fx->Parameter2>=IWD_MSC) {
		fx->Parameter2=0;
	}
	core->GetResRefFrom2DA(iwd_monster_2da[fx->Parameter2], monster, hit, areahit);

	//the monster should appear near the effect position
	Point p(fx->PosX, fx->PosY);
	core->SummonCreature(monster, areahit, Owner, target, p, -1, fx->Parameter1);
	return FX_NOT_APPLIED;
}

//0xf1 VampiricTouch
int fx_vampiric_touch (Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_vampiric_touch (%2d): ResRef:%s Type: %d\n", fx->Opcode, fx->Resource, fx->Parameter2 );
	if (Owner==target) {
		return FX_NOT_APPLIED;
	}

	Actor *receiver;
	Actor *donor;

	switch(fx->Parameter2) {
		case 0: receiver = target; donor = Owner; break;
		case 1: receiver = Owner; donor = target; break;
		default:
			return FX_NOT_APPLIED;
	}
	int damage = donor->Damage(fx->Parameter1, fx->Parameter2, Owner);
	receiver->SetStat( IE_HITPOINTS, STAT_GET( IE_HITPOINTS ) + ( damage ), 0 );
	return FX_NOT_APPLIED;
}

#define IWD_AD  2
ieResRef animate_dead_2da[IWD_AD]={"ADEAD","ADEADL"};

//0xf3 AnimateDead
int fx_animate_dead (Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_animate_dead (%2d): ResRef:%s Type: %d\n", fx->Opcode, fx->Resource, fx->Parameter2 );
	//check the summoning limit?
	
	ieResRef monster;
	ieResRef hit;
	ieResRef areahit;

	if (fx->Parameter2>=IWD_AD) {
		fx->Parameter2=0;
	}
	core->GetResRefFrom2DA(animate_dead_2da[fx->Parameter2], monster, hit, areahit);

	//the monster should appear near the effect position
	Point p(fx->PosX, fx->PosY);
	core->SummonCreature(monster, areahit, Owner, target, p, -1, fx->Parameter1);
	return FX_NOT_APPLIED;
}
//f4,f5 (implemented in PST)

//0xf6 SummonMonster2 
#define IWD_SM2 11 
ieResRef summon_monster_2da[IWD_SM2]={"SLIZARD","STROLLS","SSHADOW","ISTALKE",
 "CFELEMW","CEELEMW","CWELEMW","CFELEMP","CEELEMP","CWELEMP","CEELEMM"};

int fx_summon_monster2 (Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_summon_monster2 (%2d): ResRef:%s Type: %d\n", fx->Opcode, fx->Resource, fx->Parameter2 );
	//check the summoning limit?
	
	ieResRef monster;
	ieResRef hit;
	ieResRef areahit;

	if (fx->Parameter2>=IWD_SM2) {
		fx->Parameter2=0;
	}
	core->GetResRefFrom2DA(summon_monster_2da[fx->Parameter2], monster, hit, areahit);

	//the monster should appear near the effect position
	Point p(fx->PosX, fx->PosY);
	core->SummonCreature(monster, areahit, Owner, target, p, -1, fx->Parameter1);
	return FX_NOT_APPLIED;
}

//0xf7 BurningBlood (iwd)
int fx_burning_blood (Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_burning_blood (%2d): Type: %d\n", fx->Opcode, fx->Parameter2 );
	//inflicts damage calculated by dice values+parameter1
	//creates damage opcode on everyone around. fx->Parameter2 - 0 fire, 1 - ice
	ieDword damage = DAMAGE_FIRE;

	if (fx->Parameter2==1) {
		damage = DAMAGE_COLD;
	}

	target->Damage(fx->Parameter1, DAMAGE_FIRE, Owner);
	STAT_SET(IE_CHECKFORBERSERK,1);
	return FX_NOT_APPLIED;
}
//0xf7 BurningBlood2 (how, iwd2)
int fx_burning_blood2 (Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_burning_blood2 (%2d): Count: %d Type: %d\n", fx->Opcode, fx->Parameter1, fx->Parameter2 );
	//timing
	if (core->GetGame()->GameTime%6) {
		return FX_APPLIED;
	}
	//inflicts damage calculated by dice values+parameter1
	if (!fx->Parameter1) {
		return FX_NOT_APPLIED;
	}
	fx->Parameter1--;

	ieDword damage = DAMAGE_FIRE;

	if (fx->Parameter2==1) {
		damage = DAMAGE_COLD;
	}

	//this effect doesn't use Parameter1 to modify damage, it is a counter instead
	target->Damage(DICE_ROLL(0), DAMAGE_FIRE, Owner);
	STAT_SET(IE_CHECKFORBERSERK,1);
	return FX_APPLIED;
}

//0xf8 SummonShadowMonster

#define IWD_SSM 3
ieResRef summon_shadow_monster_2da[IWD_SM2]={"SMONSTE","DSMONST","SHADES" };

int fx_summon_shadow_monster (Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_summon_shadow_monster (%2d): ResRef:%s Type: %d\n", fx->Opcode, fx->Resource, fx->Parameter2 );
	//check the summoning limit?
	
	ieResRef monster;
	ieResRef hit;
	ieResRef areahit;

	if (fx->Parameter2>=IWD_SSM) {
		fx->Parameter2=0;
	}
	core->GetResRefFrom2DA(summon_shadow_monster_2da[fx->Parameter2], monster, hit, areahit);

	//the monster should appear near the effect position
	Point p(fx->PosX, fx->PosY);
	core->SummonCreature(monster, areahit, Owner, target, p, -1, fx->Parameter1);
	return FX_NOT_APPLIED;
}
//f9 Recitation
int fx_recitation (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_recitation (%2d): Type: %d\n", fx->Opcode, fx->Parameter2 );
	if (EXTSTATE_GET(4)) return FX_NOT_APPLIED;
	EXTSTATE_SET(4);
	return FX_APPLIED;
}
//fa RecitationBad
int fx_recitation_bad (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_recitation (%2d): Type: %d\n", fx->Opcode, fx->Parameter2 );
	if (EXTSTATE_GET(8)) return FX_NOT_APPLIED;
	EXTSTATE_SET(8);
	return FX_APPLIED;
}
//fb LichTouch (how)
EffectRef fx_hold_creature_ref={"State:Hold",NULL,-1};

int fx_lich_touch (Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_lich_touch (%2d): Type: %d\n", fx->Opcode, fx->Parameter2 );
	if (target->GetStat(IE_GENERAL)==GEN_UNDEAD) {
		return FX_NOT_APPLIED;
	}
	target->Damage(DICE_ROLL(0), DAMAGE_COLD, Owner);
	///convert to hold creature
	///shall we check for immunity vs. #175?
	///if yes, then probably it is easier to apply the hold effect instead of converting to it
	fx->Opcode = EffectQueue::ResolveEffect(fx_hold_creature_ref);
	fx->Duration = fx->Parameter1;
	fx->TimingMode = FX_DURATION_INSTANT_LIMITED;
	ieDword GameTime = core->GetGame()->GameTime;
	PrepareDuration(fx);
	return FX_APPLIED;
}
//fc BlindingOrb (how)
EffectRef fx_state_blind_ref={"State:Blind",NULL,-1};

int fx_blinding_orb (Actor* Owner, Actor* target, Effect* fx)
{
	ieDword damage = fx->Parameter1;
	if (target->GetStat(IE_GENERAL)==GEN_UNDEAD) {
		damage *= 2;
	}
	//check saving throw
	bool st = target->GetSavingThrow(4,0); //spell
	if (st) {
		target->Damage(damage/2, DAMAGE_FIRE, Owner);
		return FX_NOT_APPLIED;
	}
	target->Damage(damage, DAMAGE_FIRE, Owner);
	fx->Opcode = EffectQueue::ResolveEffect(fx_state_blind_ref);
	fx->Duration = core->Roll(1,6,0);
	fx->TimingMode = FX_DURATION_INSTANT_LIMITED;
	ieDword GameTime = core->GetGame()->GameTime;
	PrepareDuration(fx);
	return FX_APPLIED;
}
//0xfe RemoveEffects
int fx_remove_effects (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_remove_effects (%2d): ResRef:%s Type: %d\n", fx->Opcode, fx->Resource, fx->Parameter2 );

	switch(fx->Parameter2) {
		case 1:
			target->fxqueue.RemoveAllEffects(fx->Resource, FX_DURATION_INSTANT_WHILE_EQUIPPED);
			break;
		case 2:
			target->fxqueue.RemoveAllEffects(fx->Resource, FX_DURATION_INSTANT_LIMITED);
			break;
		default:
		target->fxqueue.RemoveAllEffects(fx->Resource);
	}
	return FX_NOT_APPLIED;
}

//0xff SalamanderAura
int fx_salamander_aura (Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_salamander_aura (%2d): ResRef:%s Type: %d\n", fx->Opcode, fx->Resource, fx->Parameter2 );
	//inflicts damage calculated by dice values+parameter1
	//creates damage opcode on everyone around. fx->Parameter2 - 0 fire, 1 - ice
	ieDword damage = DAMAGE_FIRE;

	if (fx->Parameter2==1) {
		damage = DAMAGE_COLD;
	}

	ApplyDamageNearby(Owner, target, fx, damage);
	return FX_APPLIED;
}
//0x100 UmberHulkGaze (causes confusion or other stuff)

int fx_umberhulk_gaze (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_umberhulk_gaze (%2d): Mod: %d, Type: %d\n", fx->Opcode, fx->Parameter1, fx->Parameter2 );
	STATE_SET( STATE_CONFUSED );
	//timing
	if (core->GetGame()->GameTime%6) {
		return FX_APPLIED;
	}
	if (!fx->Parameter1) {
		return FX_NOT_APPLIED;
	}
	//random event???
	fx->Parameter1--;
	return FX_APPLIED;
}

//0x101 ZombieLordAura (causes fear?)
int fx_zombielord_aura (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_zombielord_aura (%2d): Mod: %d, Type: %d\n", fx->Opcode, fx->Parameter1, fx->Parameter2 );
	STATE_SET( STATE_CONFUSED );
	//timing
	if (core->GetGame()->GameTime%6) {
		return FX_APPLIED;
	}
	if (!fx->Parameter1) {
		return FX_NOT_APPLIED;
	}
	//random event???
	fx->Parameter1--;
	return FX_APPLIED;
}

//0x102 Protection:Spell (this is the same as in bg2?)
//0x103 SummonCreature2

static int eamods[]={EAM_DEFAULT,EAM_SOURCEALLY,EAM_SOURCEENEMY,EAM_NEUTRAL};

int fx_summon_creature2 (Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_summon_creature2 (%2d): ResRef:%s Anim:%s Type: %d\n", fx->Opcode, fx->Resource, fx->Resource2, fx->Parameter2 );

	//summon creature (resource), play vvc (resource2)
	//creature's lastsummoner is Owner
	//creature's target is target
	//position of appearance is target's pos
	int eamod = EAM_DEFAULT;
	if (fx->Parameter2<4){
		eamod = eamods[fx->Parameter2];
	}
	core->SummonCreature(fx->Resource, fx->Resource2, Owner, target, target->Pos, eamod, 0);
	return FX_NOT_APPLIED;
}

//0x104 AvatarRemovalModifier (same as bg2)
//0x105 immunity to effect (same as bg2?)
//0x106 SummonPomab
int fx_summon_pomab (Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_summon_pomab (%2d): ResRef:%s Anim:%s Type: %d\n", fx->Opcode, fx->Resource, fx->Resource2, fx->Parameter2 );
	if (!fx->Resource[0]) {
		strcpy(fx->Resource,"POMAB");
	}

	Point p(fx->PosX+core->Roll(1,20,0), fx->PosY+core->Roll(1,20,0) );
	core->SummonCreature(fx->Resource, fx->Resource2, Owner, target, p, -1,100);
	return FX_NOT_APPLIED;
}

//0x107 ControlUndead (like charm?)
int fx_control_undead (Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_control_undead (%2d): General: %d, Type: %d\n", fx->Opcode, fx->Parameter1, fx->Parameter2 );
	if (fx->Parameter1 && (STAT_GET(IE_GENERAL)!=fx->Parameter1)) {
		return FX_NOT_APPLIED;
	}
	bool enemyally = Owner->GetStat(IE_EA)>EA_GOODCUTOFF; //or evilcutoff?

	//do this only on first use
	switch (fx->Parameter2) {
	case 0: //charmed (target neutral after charm)
		core->DisplayConstantStringName(STR_CHARMED, 0xf0f0f0, target);
		break;
	case 1: //charmed (target hostile after charm)
		core->DisplayConstantStringName(STR_CHARMED, 0xf0f0f0, target);
		target->SetBase(IE_EA, EA_ENEMY);
		break;
	case 2: //controlled by cleric
		core->DisplayConstantStringName(STR_CONTROLLED, 0xf0f0f0, target);
		break;
	case 3: //controlled by cleric (hostile after charm)
		core->DisplayConstantStringName(STR_CONTROLLED, 0xf0f0f0, target);
		target->SetBase(IE_EA, EA_ENEMY);
		break;
	case 4: //turn undead
		core->DisplayConstantStringName(STR_CONTROLLED, 0xf0f0f0, target);
		target->SetBase(IE_EA, EA_ENEMY);
		target->SetStat(IE_MORALE, 0, 0);
		break;
	}

	STATE_SET( STATE_CHARMED );
	STAT_SET( IE_EA, enemyally?EA_ENEMY:EA_CHARMED );
	//don't stick if permanent
	return FX_PERMANENT;
}

//0x108 StaticCharge
int fx_static_charge(Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_static_charge (%2d): Count: %d \n", fx->Opcode, fx->Parameter1 );
	//timing
	if (core->GetGame()->GameTime%6) {
		return FX_APPLIED;
	}
	if (!fx->Parameter1) {
		return FX_NOT_APPLIED;
	}
	fx->Parameter1--;
	//
	target->Damage(fx->Parameter1, DAMAGE_ELECTRICITY, Owner);
	return FX_APPLIED;
}

//0x109
int fx_cloak_of_fear(Actor* /*Owner*/, Actor* /*target*/, Effect* fx)
{
	if (0) printf( "fx_cloak_of_fear (%2d): Count: %d \n", fx->Opcode, fx->Parameter1 );
	//timing
	if (core->GetGame()->GameTime%6) {
		return FX_APPLIED;
	}

	if (!fx->Parameter1) {
		return FX_NOT_APPLIED;
	}
	fx->Parameter1--;
	//
	return FX_APPLIED;
}
//0x10a MovementRateModifier3 (Like bg2)
//0x10b RemoveConfusion
EffectRef fx_confusion_ref={"State:Confused",NULL,-1};
EffectRef fx_umberhulk_gaze_ref={"UmberHulkGaze",NULL,-1};
int fx_remove_confusion (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_remove_confusion (%2d)\n", fx->Opcode );
	target->fxqueue.RemoveAllEffects(fx_confusion_ref);
	target->fxqueue.RemoveAllEffects(fx_umberhulk_gaze_ref);
	return FX_APPLIED;
}
//0x10c EyeOfTheMind
int fx_eye_of_the_mind (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_eye_of_the_mind (%2d)\n", fx->Opcode );
	target->add_animation("eyemind",-1,0,true);
	EXTSTATE_SET(0x00000010);
	return FX_APPLIED;
}
//0x10d EyeOfTheSword
int fx_eye_of_the_sword (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_eye_of_the_sword (%2d)\n", fx->Opcode );
	target->add_animation("eyesword",-1,0,true);
	EXTSTATE_SET(0x00000020);
	return FX_APPLIED;
}
//0x10e EyeOfTheMage
int fx_eye_of_the_mage (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_eye_of_the_mage (%2d)\n", fx->Opcode );
	target->add_animation("eyemage",-1,0,true);
	EXTSTATE_SET(0x00000040);
	return FX_APPLIED;
}
//0x10f EyeOfVenom
int fx_eye_of_venom (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_eye_of_venom (%2d)\n", fx->Opcode );
	target->add_animation("eyevenom",-1,0,true);
	EXTSTATE_SET(0x00000080);
	return FX_APPLIED;
}
//0x110 EyeOfTheSpirit
int fx_eye_of_the_spirit (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_eye_of_the_spirit (%2d)\n", fx->Opcode );
	target->add_animation("eyespir",-1,0,true);
	EXTSTATE_SET(0x00000100);
	return FX_APPLIED;
}
//0x111 EyeOfFortitude
int fx_eye_of_fortitude (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_eye_of_fortitude (%2d)\n", fx->Opcode );
	target->add_animation("eyefort",-1,0,true);
	EXTSTATE_SET(0x00000200);
	return FX_APPLIED;
}
//0x112 EyeOfStone
int fx_eye_of_stone (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_eye_of_stone (%2d)\n", fx->Opcode );
	target->add_animation("eyestone",-1,0,true);
	EXTSTATE_SET(0x00000400);
	return FX_APPLIED;
}
//0x113 RemoveSevenEyes
EffectRef fx_eye1_ref={"EyeOfTheMind",NULL,-1};
EffectRef fx_eye2_ref={"EyeOfTheSword",NULL,-1};
EffectRef fx_eye3_ref={"EyeOfTheMage",NULL,-1};
EffectRef fx_eye4_ref={"EyeOfVenom",NULL,-1};
EffectRef fx_eye5_ref={"EyeOfThespirit",NULL,-1};
EffectRef fx_eye6_ref={"EyeOfFortitude",NULL,-1};
EffectRef fx_eye7_ref={"EyeOfStone",NULL,-1};

int fx_remove_seven_eyes (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_remove_seven_eyes (%2d): Type: %d\n", fx->Opcode, fx->Parameter2 );
	target->fxqueue.RemoveAllEffects(fx_eye1_ref);
	target->fxqueue.RemoveAllEffects(fx_eye2_ref);
	target->fxqueue.RemoveAllEffects(fx_eye3_ref);
	target->fxqueue.RemoveAllEffects(fx_eye4_ref);
	target->fxqueue.RemoveAllEffects(fx_eye5_ref);
	target->fxqueue.RemoveAllEffects(fx_eye6_ref);
	target->fxqueue.RemoveAllEffects(fx_eye7_ref);
	return FX_NOT_APPLIED;
}

//0x114 RemoveEffect
int fx_remove_effect (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_remove_effect (%2d): Type: %d\n", fx->Opcode, fx->Parameter2 );
	target->fxqueue.RemoveAllEffects(fx->Parameter2);
	return FX_NOT_APPLIED;
}

//0x115 SoulEater
int fx_soul_eater (Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_soul_eater (%2d): Damage %d\n", fx->Opcode, fx->Parameter1 );
	target->Damage(fx->Parameter1, DAMAGE_SOULEATER, Owner);
	return FX_NOT_APPLIED;
}

//0x116 ShroudOfFlame (how)
//FIXME: maybe it is cheaper to port effsof1/2 to how than having an alternate effect
int fx_shroud_of_flame (Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_shroud_of_flame (%2d): Type: %d\n", fx->Opcode, fx->Parameter2 );
	//timing
	if (core->GetGame()->GameTime%6) {
		return FX_APPLIED;
	}
	//inflicts damage calculated by dice values+parameter1
	//creates damage opcode on everyone around. fx->Parameter2 - 0 fire, 1 - ice
	ieDword damage = DAMAGE_FIRE;

	if (fx->Parameter2==1) {
		damage = DAMAGE_COLD;
	}

	target->Damage(fx->Parameter1, DAMAGE_FIRE, Owner);
	ApplyDamageNearby(Owner, target, fx, DAMAGE_FIRE);
	return FX_APPLIED;
}

//0x116 ShroudOfFlame (iwd2)
int fx_shroud_of_flame2 (Actor* /*Owner*/, Actor* /*target*/, Effect* fx)
{
	if (0) printf( "fx_shroud_of_flame2 (%2d)\n", fx->Opcode );
	//timing
	if (core->GetGame()->GameTime%6) {
		return FX_APPLIED;
	}
	//apply effsof1 on target
	//apply effsof2 on nearby
	return FX_APPLIED;
}

//0x117 AnimalRage
int fx_animal_rage (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_animal_rage (%2d): Count %d\n", fx->Opcode, fx->Parameter1 );
	//timing
	if (core->GetGame()->GameTime%6) {
		return FX_APPLIED;
	}
	if (!fx->Parameter1) {
		return FX_NOT_APPLIED;
	}
	fx->Parameter1--;
	STAT_SET( IE_CHECKFORBERSERK, 1 );
	return FX_APPLIED;
}
//0x118 TurnUndead how
int fx_turn_undead (Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_turn_undead (%2d): Level %d\n", fx->Opcode, fx->Parameter1 );
	if (fx->Parameter1) {
		target->Turn(Owner, fx->Parameter1);
	} else {
		target->Turn(Owner, Owner->GetStat(IE_TURNUNDEADLEVEL));
	}
	return FX_APPLIED;
}

//0x118 TurnUndead2 iwd2
int fx_turn_undead2 (Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_turn_undead2 (%2d): Level: %d  Type %d\n", fx->Opcode, fx->Parameter1, fx->Parameter2 );
	switch (fx->Parameter2)
	{
	case 0: //command
		target->Panic();
		break;
	case 1://rebuke
		target->Panic();
		break;
	case 2://destroy
		target->Die(Owner);
		break;
	case 3://panic
		target->Panic();
		break;
	default://depends on caster
		if (fx->Parameter1) {
			target->Turn(Owner, fx->Parameter1);
		} else {
			target->Turn(Owner, Owner->GetStat(IE_TURNUNDEADLEVEL));
		}
		break;
	}
	return FX_APPLIED;
}
//0x119 VitriolicSphere
int fx_vitriolic_sphere (Actor* Owner, Actor* target, Effect* fx)
{
	if (0) printf( "fx_vitriolic_sphere (%2d): Damage %d\n", fx->Opcode, fx->Parameter1 );
	//timing
	if (core->GetGame()->GameTime%6) {
		return FX_APPLIED;
	}
	target->Damage(fx->Parameter1, DAMAGE_ACID, Owner);
	fx->DiceThrown-=2;
	if (fx->DiceThrown<1) {
		return FX_NOT_APPLIED;
	}
	//also damage people nearby?
//	ApplyDamageNearby(Owner, target, fx, DAMAGE_ACID);
	return FX_APPLIED;
}

//0x11a SuppressHP
int fx_suppress_hp (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_suppress_hp (%2d)\n", fx->Opcode);
	EXTSTATE_SET(0x00001000);
	return FX_APPLIED;
}

//0x11b FloatText
int fx_floattext (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_floattext (%2d): StrRef:%d Type: %d\n", fx->Opcode, fx->Parameter1, fx->Parameter2 );
	switch (fx->Parameter2)
	{
	default:
		DisplayStringCore(target, fx->Parameter1, DS_HEAD);
		break;
	case 1:
		//in the original game this signified that a specific weapon is equipped
		EXTSTATE_SET(0x00008000);
		return FX_APPLIED;
	case 2: //gemrb extension, displays verbalconstant
		DisplayStringCore(target, fx->Parameter1, DS_CONST|DS_HEAD);
		break;
	}
	return FX_NOT_APPLIED;
}

//0x11c MaceOfDisruption
int fx_mace_of_disruption (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_mace_of_disruption (%2d): ResRef:%s Anim:%s Type: %d\n", fx->Opcode, fx->Resource, fx->Resource2, fx->Parameter2 );
	ieDword race = target->GetStat(IE_RACE);
	//golem / outer planar gets hit
	switch (race) {
		case 144: // golem
			break;
		default:;
	}
	return FX_NOT_APPLIED;
}
//0x11d Sleep2 ??? power word sleep?
//0x11e Reveal:Tracks (same as bg2)
//0x11f Protection:Backstab (same as bg2)

//0x120 State:Set
int fx_set_state (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_set_state (%2d): Type: %d\n", fx->Opcode, fx->Parameter2 );
	//in HoW this also sets the last bits of extstate (until it runs out of bits)
	EXTSTATE_SET(0x40000<<fx->Parameter2);
	return FX_APPLIED;
}

//0x121 Cutscene (this is a very ugly hack in iwd)
int fx_cutscene (Actor* /*Owner*/, Actor* /*target*/, Effect* fx)
{
	if (0) printf( "fx_cutscene (%2d)\n", fx->Opcode );
	Game *game = core->GetGame();
	game->locals->SetAt("GEM_ACTIVE", 1);
	return FX_NOT_APPLIED;
}

//0xce (same place as in bg2, but different)
int fx_resist_spell (Actor* Owner, Actor* target, Effect *fx)
{
	//check iwd ids targeting
	if (check_iwd_targeting(Owner, target, fx->Parameter1, fx->Parameter2) ) {
		return FX_NOT_APPLIED;
	}

	if (strnicmp(fx->Resource,fx->Source,sizeof(fx->Resource)) ) {
		return FX_APPLIED;
	}
	//this has effect only on first apply, it will stop applying the spell
	return FX_ABORT;
}
EffectRef fx_resist_spell_ref={"ResistSpell2",NULL,-1};

//0x122 Protection:Spell3 ??? IWD ids targeting
// this is a variant of resist spell, used in iwd2
int fx_resist_spell_and_message (Actor* Owner, Actor* target, Effect *fx)
{
	//check iwd ids targeting
	if (check_iwd_targeting(Owner, target, fx->Parameter1, fx->Parameter2) ) {
		return FX_NOT_APPLIED;
	}

	fx->Opcode=EffectQueue::ResolveEffect(fx_resist_spell_ref);
	//display message too

	if (strnicmp(fx->Resource,fx->Source,sizeof(fx->Resource)) ) {
		return FX_APPLIED;
	}
	//this has effect only on first apply, it will stop applying the spell
	return FX_ABORT;
}

//0x123 RodOfSmithing
int fx_rod_of_smithing (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_rod_of_smithing (%2d): ResRef:%s Anim:%s Type: %d\n", fx->Opcode, fx->Resource, fx->Resource2, fx->Parameter2 );
	ieDword race = target->GetStat(IE_RACE);
	//golem / outer planar gets hit
	switch (race) {
		case 144: // golem
			break;
		default:;
	}
	return FX_NOT_APPLIED;
}

//0x124 MagicalRest (same as bg2)
//0x125 BeholderDispelMagic (???)
//0x126 HarpyWail (???)
int fx_harpy_wail (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_harpy_wail (%2d): Mod: %d, Type: %d\n", fx->Opcode, fx->Parameter1, fx->Parameter2 );
	target->Panic();
	return FX_NOT_APPLIED;
}
//0x127 JackalWereGaze (Charm ?)
int fx_jackalwere_gaze (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_jackalwere_gaze (%2d): Mod: %d, Type: %d\n", fx->Opcode, fx->Parameter1, fx->Parameter2 );
	target->Panic();
	return FX_APPLIED;
}
//0x128 ModifyGlobalVariable (same as bg2)
//0x129 HideInShadows (same as bg2)
//0x12a UseMagicDevice
int fx_use_magic_device_modifier (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_use_magic_device_modifier (%2d): Mod: %d, Type: %d\n", fx->Opcode, fx->Parameter1, fx->Parameter2 );
	STAT_MOD( IE_MAGICDEVICE );
	return FX_APPLIED;
}

//IWD2 effects

//400 Hopelessness
int fx_hopelessness (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_hopelessness (%2d)\n", fx->Opcode);
	//what else we need
	target->AddPortraitIcon(PI_HOPELESSNESS);
	return FX_APPLIED;
}

//401 ProtectionFromEvil
int fx_protection_from_evil (Actor* /*Owner*/, Actor* target, Effect* fx)
{
	if (0) printf( "fx_protection_from_evil (%2d)\n", fx->Opcode);
	target->AddPortraitIcon(PI_PROTFROMEVIL);
	return FX_APPLIED;
}
//402 AddEffectsList
//403 ArmorOfFaith
//404 Nausea
//405 Enfeeblement
//406 FireShield
//407 DeathWard
//408 HolyPower
//409 RighteousWrath
//410 SummonAllyIWD2
//411 SummonEnemyIWD2
//412 Control2
//413 VisualEffectIWD2
//414 ResilientSphere
//415 Barkskin
//416 BleedingWounds
//417 AreaEffect
//418 FreeAction2
int fx_free_action_iwd2 (Actor* /*Owner*/, Actor* target, Effect* fx)
{
        if (0) printf( "fx_free_action_iwd2 (%2d)\n", fx->Opcode);
	target->AddPortraitIcon(PI_FREEACTION);
	return FX_APPLIED;	
}
//419 Unconsciousness
//420 Death2 (see in core effects)
//421 EntropyShield
//422
//423
//424
//425 ControlUndead2 (see above)
//426
//427
//428
//429
//430
//431
//432
//433
//434
//435
//436
//437
//438
//439
//440
//441
//442
//443
//444
//445
//446
//447
//448
//449
//450
//451
//452
//453
//454
//455
//456
//457

