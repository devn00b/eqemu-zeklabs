use feature qw/switch/; 

sub EVENT_SAY { 
if($text=~/Hail/i){
$link = quest::saylink("ears");
quest::say("Come one, come all!  Looking for a challenge? Bring me the severed [$link] of opponents you have slain, $name.  The more you bring the more challengers I shall provide!");
}
elsif($text=~/ears/i){
quest::say("Indeed! Everyone has them, just cut it right off and bring it to me, 1,2,3,4, I will take them all!");
}
}

sub EVENT_ITEM {
my $charLevel = $client->GetLevel();

if ( $charLevel < 30 )
{
plugin::return_items2(\%itemcount); 
quest::say("You are too young to take part in this battle!  Come back when you have grown a bit more.");
return;
}
if ( defined $qglobals{arenabosscooldown} && $qglobals{arenabosscooldown} == 1 && !$client->GetGM() )
{
plugin::return_items2(\%itemcount); 
quest::say("I am afraid at this time we are not accepting any challengers of your calibur at this time, please come back a bit later.");
return;
}
if ( defined($entity_list->GetNPCByNPCTypeID(999581)) || 
	defined($entity_list->GetNPCByNPCTypeID(999582))|| 
	defined($entity_list->GetNPCByNPCTypeID(999583))|| 
	defined($entity_list->GetNPCByNPCTypeID(999584))|| 
	defined($entity_list->GetNPCByNPCTypeID(999585))|| 
	defined($entity_list->GetNPCByNPCTypeID(999586))|| 
	defined($entity_list->GetNPCByNPCTypeID(999587))|| 
	defined($entity_list->GetNPCByNPCTypeID(999588))|| 
	defined($entity_list->GetNPCByNPCTypeID(999589))|| 
	defined($entity_list->GetNPCByNPCTypeID(999590))|| 
	defined($entity_list->GetNPCByNPCTypeID(999591))|| 
	defined($entity_list->GetNPCByNPCTypeID(999592))|| 
	defined($entity_list->GetNPCByNPCTypeID(999593))|| 
	defined($entity_list->GetNPCByNPCTypeID(999594))|| 
	defined($entity_list->GetNPCByNPCTypeID(999595))|| 
	defined($entity_list->GetNPCByNPCTypeID(999596))|| 
	defined($entity_list->GetNPCByNPCTypeID(999597))|| 
	defined($entity_list->GetNPCByNPCTypeID(999598))|| 
	defined($entity_list->GetNPCByNPCTypeID(999599)) )
{
plugin::return_items2(\%itemcount); 
quest::say("There is currently an arena boss at the ready, there cannot be more!");
return;
}

my $result = plugin::checkear_turnin_omitscarlet($item1,$item1_name, $name);
my $result2 = plugin::checkear_turnin_omitscarlet($item2,$item2_name, $name);
my $result3 = plugin::checkear_turnin_omitscarlet($item3,$item3_name, $name);
my $result4 = plugin::checkear_turnin_omitscarlet($item4,$item4_name, $name);

if(plugin::check_handin(\%itemcount, 1 => 1)) {
}
my $count = 0;

if ( $result > 0 )
{
 $count += 1;
}
if ( $result2 > 0 )
{
 $count += 1;
}
if ( $result3 > 0 )
{
 $count += 1;
}
if ( $result4 > 0 )
{
 $count += 1;
}

if ( $count < 1 )
{
#plugin::return_items2(\%itemcount); 
quest::say("What is this?? I don't want this!");
return;
}

quest::shout2("Lets get ready to rumble!!!!");
#disable mob for 10 min
quest::setglobal("arenabosscooldown","1",7,"M10");

my $available = $count;
for ($c=0;$c<$count;$c++) {
my $result_npc_id = 0;

if ( $charLevel < 31 )
{
#lvl 25 mobs
	$result_npc_id = quest::ChooseRandom(999581,999582,999583,999584);
}
else
{
#lvl 35 mobs
	$result_npc_id = quest::ChooseRandom(999581,999582,999583,999584);
	if ( $available > 1 )
	{
		#lvl 25-35 mobs
		$result_npc_id = quest::ChooseRandom(999581,999582,999583,999584,999585,999586,999587,999588);
		$available -= 2;
		$c += 2;

		if ( $result_npc_id > 999584 )
		{
			quest::say("We have a special something for your challenge today!");
		}
	}
	
}

if ( $result_npc_id > 0 )
{
@arrayLoc = undefined;

#update to the max when(x) if you add a new location!
$max = 16;
$rnd = int(rand($max));
given($rnd){
  when(0) {
	@arrayLoc = (588.1, -32.5, 3.1, 67.2);
}
  when(1) {
	@arrayLoc = (505.5, 387.8, 3.1, 129.2);
}
  when(2) {
	@arrayLoc = (508.2, -494.8, 3.1, 7.6);
}
  when(3) {
	@arrayLoc = (283, -820, 3.1, 13.1);
}
  when(4) {
	@arrayLoc = (-389.4, -19.3, 3.1, 65.1);
}
  when(5) {
	@arrayLoc = (449, 788, 3.1, 117.1);
}
  when(6) {
	@arrayLoc = (467.8,-39.7,22.8,64.0);
}
  when(7) {
	@arrayLoc = (473.7,-248.3,3.8);
}
  when(8) {
	@arrayLoc = (225.8,-21.8,3.8);
}
  when(9) {
	@arrayLoc = (1224.2,-47.4,3.8);
}
  when(10) {
	@arrayLoc = (582.9,744.0,3.8);
}
  when(11) {
	@arrayLoc = (181.8,694.8,3.8);
}
  when(12) {
	@arrayLoc = (-90.9,-416.5,3.8);
}
  when(13) {
	@arrayLoc = (477.5,-821.9,3.8);
}
  when(14) {
	@arrayLoc = (-62.4,-13.9,3.8);
}
  when(15) {
	@arrayLoc = (164.1,397.2,3.8);
}
  when(16) {
	@arrayLoc = (129.4,-444.8,3.8);
}
}
$x = $arrayLoc[0];
$y = $arrayLoc[1];
$z = $arrayLoc[2];
$h = $arrayLoc[3];

quest::spawn2($result_npc_id,0,0,$x,$y,$z,$h);
}
else
{
quest::say("Either you did not provide me a player's ear or I could not find an adversary ($result_npc_id / $count)");
}
}
}

}
