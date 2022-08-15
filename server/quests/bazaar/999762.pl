sub EVENT_SAY { 
if (!defined $qglobals{"twerky"}) {
quest::setglobal("twerky", 1, 5, "H2");
}

 if($text=~/Hail/i){ 
$fuckingask = quest::saylink("ask");
quest::say("Greetings. $name! I'll Twerk for Twerky Day and You'll get a prize all you have to do is [$fuckingask]"); 
}

if($text=~/ask/i){
my $shit = $client->GetGlobal("twerky");
#quest::say("V: $shit");

#check if they have a reward already
if($client->GetGlobal("twerky") >=2)
{
quest::say("You can only get tweryky every 2 Hours");
return;
}

##give items and set the global higher tahn 2

quest::setglobal("twerky", 5, 5, "H2");
#quest::say("set your global..riight.");
#food
my $pfood = quest::ChooseRandom(64416,64421,96795,87317,87318,42981);
#drink
my $pdrink = quest::ChooseRandom(37993,57819,61993,57818,52884,46135);
#special
my $pspecial = quest::ChooseRandom(64193,135039,145345,145347,145405,146735,135038,144134,87562,43860);
$random_result = 0;
my $random_result = quest::ChooseRandom(1,2,3);
#quest::say("Your res: $random_result");

if($random_result eq '1'){
$prze = $pfood;
#quest::say("ID:$prze");
}

if($random_result eq '2'){
$prze = $pdrink;
#quest::say("ID:$prze");

}

if($random_result eq '3'){
$prze = $pspecial;
#quest::say("ID:$prze");

}


$winning =  quest::varlink($prze);
if(plugin::check_hasitem($client, $prze)){
#quest::summonitem(64193);
quest::say("Sorry $name, you already had that item ($winning), please try again");
return;
}else{
if($random_result <= '2'){
quest::summonitem($prze,10);
quest::say("Congrats $name on your $winning! You can come back in 2 hours and see what you get again! And hey, Happy TwerkyDay");
return;
}

if($random_result > '2'){
quest::summonitem($prze);
quest::say("Congrats $name on your $winning! You can come back in 2 hours and see what you get again! And hey, Happy TwerkyDay");
return;
}

}



}
}

