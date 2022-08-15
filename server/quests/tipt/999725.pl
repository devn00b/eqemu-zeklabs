sub EVENT_DEATH {
$dome = 0;
my $dome = int(rand(500));

if($dome <10){
plugin::handlenpc_epicloot($client,$npc,1);
plugin::handlenpc_epicloot($client,$npc,1);
return;
}

if($dome >10 && $dome <250){
plugin::handlenpc_epicloot($client,$npc,1);
return;
}

if($dome >251) {
return;
}

}

