sub EVENT_SPAWN
{
	plugin::worldmob_spawnhandler($npc,1800);
	quest::settimer("fear",1);
		quest::settimer("hpupdate",15);
		quest::settimer("hpupdatetotal",600);
	$hp = $npc->GetHP();
	quest::setglobal("treasuregoblin_hp",$hp,3,"M75");
	quest::setglobal("treasuregoblin_incombat",0,3,"M75");
	quest::setglobal("treasuregoblin_inc",0,3,"M90");
	$texture = quest::ChooseRandom(0,1,2,3,4);
	$npc->SetTexture($texture);
	if ( $texture == 0 )
	{
		quest::shout2("[$zoneln] Bad boys bad boys watcha gonna do?!?!");
	}
	elsif ( $texture == 1 )
	{
		quest::shout2("[$zoneln] No I didn't tell you to drop me off here, wait, get back here!!");
	}
	elsif ( $texture == 3 )
	{
		quest::shout2("[$zoneln] Lets get this show on!  BRING IT!");
	}
	elsif ( $texture == 4 )
	{
		quest::shout2("[$zoneln] Come and get me you fools! Mwahwahwha!");
	}
	#enable no level check
	quest::setzonepvp(6);
	quest::we(13,"[WORLD EVENT] WARNING!! $zoneln is now PVP with no level restrictions.  Zone will reset upon completion of event.");
}

sub EVENT_TIMER
{
	if ( $zonesn eq "lfaydark" && !$npc->IsMoving() )
	{
		#$npc->CastSpell(354,$npc->GetID(),10,0,0);
	}
	if ( $timer eq "fear" )
	{
		#make him run!
		$npc->CastSpell(229,$npc->GetID(),10,0,0);

      my @clientlist = $entity_list->GetClientList();
      foreach $ent (@clientlist) {
		if ( $ent->GetLevel() > 15 )
		{
		$countval = $ent->GetGlobal("treasuregoblin_drops");
		 if ($countval < 1 )
		{
	         $ent->SetGlobal("treasuregoblin_drops","1",5,"M10");
			$ent->Message(13,"You have been added to the treasure goblin drop list.");
		}
		}
	}
	} #end fear if
	elsif ( $timer eq "hpupdatetotal" )
	{
		$curhp = $npc->GetHP();
		quest::setglobal("treasuregoblin_hp",$curhp,3,"M30");
	}
	elsif ( $timer eq "hpupdate" )
	{
		$hp = $qglobals{treasuregoblin_hp};
		$state = $qglobals{treasuregoblin_incombat};
		$curhp = $npc->GetHP();

		$diffhp = $hp - $curhp;
		if ( $diffhp < 0 )
		{
			quest::setglobal("treasuregoblin_hp",$curhp,3,"M30");
		}
		elsif ( $diffhp > 0 ) #large gap
		{
			$lasthp = $qglobals{treasuregoblin_lasthp};
			quest::setglobal("treasuregoblin_lasthp",$curhp,3,"M30");
			if ( defined $lasthp && $curhp == $lasthp )
			{
				return;
			}

			$largePrize = 0;
			if ( $diffhp > 10000 ) { $largePrize = 1; }
			if ( $diffhp > 30000 ) { $largePrize = 2; }
			$curx = $qglobals{treasuregoblin_x};
			$cury = $qglobals{treasuregoblin_y};
			$prevtreasure = $qglobals{treasuregoblin_prevprize};
			$treasurecooldown = $qglobals{treasuregoblin_cooldown};
			if ( defined $prevtreasure && $prevtreasure > 0 && $largePrize > 0 )
			{
				$largePrize = $prevtreasure - 1;
			}
			if ( defined $treasurecooldown && $treasurecooldown == 1 && $largePrize > 0 )
			{
				$largePrize = 0;
			}
			plugin::treasureGoblinDrop($npc,$curx,$cury,$largePrize);
			quest::setglobal("treasuregoblin_x",int($npc->GetX()),3,"M1");
			quest::setglobal("treasuregoblin_y",int($npc->GetY()),3,"M1");
			quest::setglobal("treasuregoblin_prevprize",$largePrize,3,"M10");
			if ( $largePrize > 0 ) {
			quest::setglobal("treasuregoblin_cooldown","1",3,"M3"); }
			$rndsay = quest::ChooseRandom(0,1,2,3,4,5,6,7,8,9);
			if ( $rndsay == 1 )
			{
				quest::shout2("[$zoneln] Ahhh!! GET AWAY FROM ME!!");
			}
			elsif ( $rndsay == 2 )
			{
				quest::shout2("[$zoneln] Must run away... must get away..!!");
			}
		#do ground spawn here!
		}
		else
		{
			$rndsay = quest::ChooseRandom(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);
			if ( $rndsay == 0 )
			{
				quest::shout2("[$zoneln] Come and get me you fools, what are you waiting for?!?");
			}
			elsif ( $rndsay == 1 )
			{
				quest::shout2("[$zoneln] Everyones afraid of the little treasure goblin oh no!!");
			}
			elsif ( $rndsay == 2 )
			{
				quest::shout2("[$zoneln] Man if I were given such an opportunity I WOULD NOT REFUSE!!");
			}
			elsif ( $rndsay == 3 )
			{
				quest::shout2("[$zoneln] You all are boring me, I should take all my trinkets home I guess...");
			}
		}
	} #end hpupdate elsif
	elsif ( $timer eq "despawn" )
	{
		$count = $qglobals{treasuregoblin_inc};
		if ( $npc->IsEngaged() == 0 || ( defined $count && $count > 0 ) )
		{
			plugin::worldmob_resetzone();
			quest::shout2("You have had your fun!! Mwahahah now I run away!!");
			quest::depop();
		}
		else
		{
			quest::setglobal("treasuregoblin_inc",1,3,"M90");
		}
	}
}

sub EVENT_COMBAT
{
	$curhp = $npc->GetHP();
	quest::setglobal("treasuregoblin_hp",$curhp,3,"M30");
	quest::setglobal("treasuregoblin_incombat",$combat_state,3,"M30");
}

sub EVENT_DEATH
{
#plugin::handlenpc_epicv2loot($client,$npc,2);
$npc->AddItem(142841,1);
$npc->AddItem(142841,1);
$npc->AddItem(142841,1);
plugin::worldmob_deathhandler($npc);
}
