use feature qw/switch/; 


sub EVENT_SAY
{
quest::set_proximity($x - 50, $x + 50, $y - 50, $y + 50, $z - 20, $z + 20);
$charnum = $qglobals{arena_ball_owner};
quest::say("$charnum");
#if ( $client->GetGM() == 1 )
#{
#quest::creategroundobject(52760, $x, $y, $z, $h, 600000, 0);
#}
}

sub EVENT_SIGNAL
{
if ( $signal == 1 )
{
quest::settimer("dropitem",660);
quest::stoptimer("arena_dropball_center");
}
elsif ( $signal == 2 )
{
quest::settimer("dropitem",10);
quest::stoptimer("arena_dropball_center");
}
elsif ( $signal == 3 )
{
quest::stoptimer("arena_dropball_center");
}
else
{
quest::stoptimer("arena_dropball_center");
quest::stoptimer("dropitem");
}
}

sub EVENT_SPAWN
{
quest::set_proximity($x - 125, $x + 125, $y - 125, $y + 125, $z - 20, $z + 20);
$charnum = $qglobals{arena_ball_owner};
if ( !defined $charnum || $charnum == 0 )
{
quest::settimer("dropitem",5);
}
}

sub EVENT_ENTER {
$charnum = $qglobals{arena_ball_owner};
if ( defined $charnum && $charnum == $client->CharacterID())
{
quest::stoptimer("arena_dropball_center");
$client->Message(13,"Entered the Arena Hill, remain here for PVP tickets!");
}
quest::setglobal("arena_onhill","1",5,"F");
}

sub EVENT_EXIT {
$charnum = $qglobals{arena_ball_owner};
if ( defined $charnum && $charnum == $client->CharacterID())
{
quest::settimer("arena_dropball_center",30);
$client->Message(13,"Exited the Arena Hill, you have 30 seconds to return!");
}
quest::setglobal("arena_onhill","0",5,"F");
}

sub EVENT_TIMER
{
if ( $timer eq "arena_dropball_center" )
{
quest::stoptimer("arena_dropball_center");
$charnum = $qglobals{arena_ball_owner};
if ( defined $charnum && $charnum > 0 )
{
$newclient = $entity_list->GetClientByCharID($charnum);
if ( defined $newclient )
{
$newclient->SetGlobal("arena_ball_disable","1",5,"H6");
plugin::dropBall($newclient,$charnum,1);
$newclient->Message(13,"You stepped away from the center platform in Arena too long!");
}
}
}

if ( $timer eq "dropitem" )
{
quest::stoptimer("dropitem");
quest::settimer("dropitem",660);

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
quest::ze(13,"Ball has been dropped in a new location ($y, $x).");
quest::creategroundobject(52760, $x, $y, $z, $h, 600000, 0, "arena_hill_allowed");
}
}