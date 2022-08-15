sub EVENT_DEATH{
my $rando = int(rand(100));
my $bp = '7249';
my $es = '11677';

if ($rando <= '40'){
$npc->AddItem($bp,1,0);
}

if ($rando eq '0'){
$npc->AddItem($es,1,0);
}

}
#END of FILE Zone:permafrost  ID:Not_Found -- High_Priest_Zaharn 

