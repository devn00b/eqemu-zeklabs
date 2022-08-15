
sub EVENT_SPAWN {
#chance to get a drop
my $random_result = int(rand(350));
###### BETA
###
#my $random_result=1;
###
######

#items to drop
$rnd = quest::ChooseRandom(1001,1002);
$rnd2 = quest::ChooseRandom(1001,1002);

$devpet = $npc->GetPetType();

if($devpet < 3 && $client->Admin() <1 )
{
#just exit script so they have no effect
return;
}

#trigers npc loot chance.
##########
if($random_result <5){
$npc->AddItem($rnd,1,0);
$npc->AddItem($rnd2,1,0);
quest::shout("Woot I got $rnd and $rnd2");
return;
                      }
##########					  
if($random_result <30){
$npc->AddItem($rnd,1,0);
quest::shout("Yay I got $rnd");
return;
                      }
###########					  
quest::say("No items for You");
return;
}




