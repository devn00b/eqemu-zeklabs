sub EVENT_SAY { 
if($text=~/Hail/i){
quest::say("Hello! Have you been to Sleepers Tomb recently? If you have, there is an item I am looking for. Please let me know if you can [help] me."); 
}

if($text=~/help/i){
quest::say("Fantastic! Please just find me the spell scroll, Spiritual Strength. You'll find it quite useless to you, however it means much to my research. As a reward I will give you trinkets I found on my last expedition there.");
}
}


sub EVENT_ITEM {

if(plugin::check_handin(\%itemcount, 7730 => 1)) { 
my $NPC_ID = $npc->GetID();
quest::say("Oh my! What a fantastic research sample! Yes! I can feel the power, lets try this!");
$npc->CastSpell(2630, $NPC_ID);
quest::summonitem(quest::ChooseRandom(5835, 27320, 27321, 27322,27323, 27324, 27325, 27326, 27327, 27328));
quest::say("Absolutly amazing! Thank you so much!");
}
}
