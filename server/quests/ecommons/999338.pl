# quest pvp merchant for warriors / monks
sub EVENT_SAY
{
if($text=~/Hail/i){
$link = quest::saylink("tickets");
quest::say("Hello $name, do you happen to have any Golden PVP $link?");
}
elsif($text=~/tickets/i){
quest::say("I collect tickets -- its what I do!!  Provide me one gold pvp ticket and I will reward you for your troubles.");
}
}

sub EVENT_ITEM {
my $multiplier = 2;

if(plugin::check_handin(\%itemcount, 132487 => 4)) {

my @randomGivenPlat = (6,7);
my $platGiven = ($randomGivenPlat[int(rand(scalar @randomGivenPlat))]);
my @randomGivenSilver = (0,1,2,3,4,5,6);
my $silverGiven = ($randomGivenSilver[int(rand(scalar @randomGivenSilver))]);
my @randomGivenGold = (0,1,2,3,4,5,6,7);
my $goldGiven = ($randomGivenGold[int(rand(scalar @randomGivenGold))]);
quest::givecash(0,$silverGiven,$goldGiven,$platGiven);
my $totalTickets = $multiplier * 4;
quest::say("Thank you kindly.  Here is some cash and I award you $totalTickets PVP points for your trouble.");
$client->AddPVPPoints($totalTickets);
}
elsif(plugin::check_handin(\%itemcount, 132487 => 3)) {
my @randomGivenPlat = (4,4,5,5,6,7);
my $platGiven = ($randomGivenPlat[int(rand(scalar @randomGivenPlat))]);
my @randomGivenSilver = (0,1,2,3,4,5,6);
my $silverGiven = ($randomGivenSilver[int(rand(scalar @randomGivenSilver))]);
my @randomGivenGold = (0,1,2,3,4,5,6,7);
my $goldGiven = ($randomGivenGold[int(rand(scalar @randomGivenGold))]);
quest::givecash(0,$silverGiven,$goldGiven,$platGiven);
my $totalTickets = $multiplier * 3;
quest::say("Thank you kindly.  Here is some cash and I award you $totalTickets PVP points for your trouble.");
$client->AddPVPPoints($totalTickets);
}
elsif(plugin::check_handin(\%itemcount, 132487 => 2)) {
my @randomGivenPlat = (2,2,3,3,4,4,5,6,7);
my $platGiven = ($randomGivenPlat[int(rand(scalar @randomGivenPlat))]);
my @randomGivenSilver = (0,1,2,3,4,5,6);
my $silverGiven = ($randomGivenSilver[int(rand(scalar @randomGivenSilver))]);
my @randomGivenGold = (0,1,2,3,4,5,6,7);
my $goldGiven = ($randomGivenGold[int(rand(scalar @randomGivenGold))]);
quest::givecash(0,$silverGiven,$goldGiven,$platGiven);
my $totalTickets = $multiplier * 2;
quest::say("Thank you kindly.  Here is some cash and I award you $totalTickets PVP points for your trouble.");
$client->AddPVPPoints($totalTickets);
}
elsif(plugin::check_handin(\%itemcount, 132487 => 1)) {
my @randomGivenPlat = (1,1,2,3,3,4,4,5,6,7);
my $platGiven = ($randomGivenPlat[int(rand(scalar @randomGivenPlat))]);
my @randomGivenSilver = (0,1,2,3,4,5,6);
my $silverGiven = ($randomGivenSilver[int(rand(scalar @randomGivenSilver))]);
my @randomGivenGold = (0,1,2,3,4,5,6,7);
my $goldGiven = ($randomGivenGold[int(rand(scalar @randomGivenGold))]);
quest::givecash(0,$silverGiven,$goldGiven,$platGiven);
my $totalTickets = $multiplier * 1;
quest::say("Thank you kindly.  Here is some cash and I award you $totalTickets PVP points for your trouble.");
$client->AddPVPPoints($totalTickets);
}

#palladium pvp tickets are worth 10 each
$multiplier = 10;
if(plugin::check_handin(\%itemcount, 142842 => 4)) {
my @randomGivenPlat = (34,34,35,36,36,37,37,38,39,40);
my $platGiven = ($randomGivenPlat[int(rand(scalar @randomGivenPlat))]);
my @randomGivenSilver = (0,1,2,3,4,5,6);
my $silverGiven = ($randomGivenSilver[int(rand(scalar @randomGivenSilver))]);
my @randomGivenGold = (0,1,2,3,4,5,6,7);
my $goldGiven = ($randomGivenGold[int(rand(scalar @randomGivenGold))]);
quest::givecash(0,$silverGiven,$goldGiven,$platGiven);
my $totalTickets = $multiplier * 4;
quest::say("Thank you kindly.  Here is some cash and I award you $totalTickets PVP points for your trouble.");
$client->AddPVPPoints($totalTickets);
}
elsif(plugin::check_handin(\%itemcount, 142842 => 3)) {
my @randomGivenPlat = (24,24,25,26,26,27,27,28,29,30);
my $platGiven = ($randomGivenPlat[int(rand(scalar @randomGivenPlat))]);
my @randomGivenSilver = (0,1,2,3,4,5,6);
my $silverGiven = ($randomGivenSilver[int(rand(scalar @randomGivenSilver))]);
my @randomGivenGold = (0,1,2,3,4,5,6,7);
my $goldGiven = ($randomGivenGold[int(rand(scalar @randomGivenGold))]);
quest::givecash(0,$silverGiven,$goldGiven,$platGiven);
my $totalTickets = $multiplier * 3;
quest::say("Thank you kindly.  Here is some cash and I award you $totalTickets PVP points for your trouble.");
$client->AddPVPPoints($totalTickets);
}
elsif(plugin::check_handin(\%itemcount, 142842 => 2)) {
my @randomGivenPlat = (14,14,15,16,16,17,17,18,19,20);
my $platGiven = ($randomGivenPlat[int(rand(scalar @randomGivenPlat))]);
my @randomGivenSilver = (0,1,2,3,4,5,6);
my $silverGiven = ($randomGivenSilver[int(rand(scalar @randomGivenSilver))]);
my @randomGivenGold = (0,1,2,3,4,5,6,7);
my $goldGiven = ($randomGivenGold[int(rand(scalar @randomGivenGold))]);
quest::givecash(0,$silverGiven,$goldGiven,$platGiven);
my $totalTickets = $multiplier * 2;
quest::say("Thank you kindly.  Here is some cash and I award you $totalTickets PVP points for your trouble.");
$client->AddPVPPoints($totalTickets);
}
elsif(plugin::check_handin(\%itemcount, 142842 => 1)) {
my @randomGivenPlat = (4,4,5,6,6,7,7,8,9,10);
my $platGiven = ($randomGivenPlat[int(rand(scalar @randomGivenPlat))]);
my @randomGivenSilver = (0,1,2,3,4,5,6);
my $silverGiven = ($randomGivenSilver[int(rand(scalar @randomGivenSilver))]);
my @randomGivenGold = (0,1,2,3,4,5,6,7);
my $goldGiven = ($randomGivenGold[int(rand(scalar @randomGivenGold))]);
quest::givecash(0,$silverGiven,$goldGiven,$platGiven);
my $totalTickets = $multiplier * 1;
quest::say("Thank you kindly.  Here is some cash and I award you $totalTickets PVP points for your trouble.");
$client->AddPVPPoints($totalTickets);
}

#platinum pvp tickets are worth 35 each
$multiplier = 35;
if(plugin::check_handin(\%itemcount, 142799 => 1)) {
my @randomGivenPlat = (11,11,12,13,13,14,14,15,16,17);
my $platGiven = ($randomGivenPlat[int(rand(scalar @randomGivenPlat))]);
my @randomGivenSilver = (0,1,2,3,4,5,6);
my $silverGiven = ($randomGivenSilver[int(rand(scalar @randomGivenSilver))]);
my @randomGivenGold = (0,1,2,3,4,5,6,7);
my $goldGiven = ($randomGivenGold[int(rand(scalar @randomGivenGold))]);
quest::givecash(0,$silverGiven,$goldGiven,$platGiven);
my $totalTickets = $multiplier * 1;
quest::say("Thank you kindly.  Here is some cash and I award you $totalTickets PVP points for your trouble.");
$client->AddPVPPoints($totalTickets);
}

#titanium pvp tickets are worth 75 each
$multiplier = 75;
if(plugin::check_handin(\%itemcount, 142798 => 1)) {
my @randomGivenPlat = (21,21,22,23,23,24,24,25,26,27);
my $platGiven = ($randomGivenPlat[int(rand(scalar @randomGivenPlat))]);
my @randomGivenSilver = (0,1,2,3,4,5,6);
my $silverGiven = ($randomGivenSilver[int(rand(scalar @randomGivenSilver))]);
my @randomGivenGold = (0,1,2,3,4,5,6,7);
my $goldGiven = ($randomGivenGold[int(rand(scalar @randomGivenGold))]);
quest::givecash(0,$silverGiven,$goldGiven,$platGiven);
my $totalTickets = $multiplier * 1;
quest::say("Thank you kindly.  Here is some cash and I award you $totalTickets PVP points for your trouble.");
$client->AddPVPPoints($totalTickets);
}

#ethereal pvp tickets are worth 300 each
$multiplier = 300;
if(plugin::check_handin(\%itemcount, 142841 => 1)) {
my @randomGivenPlat = (61,61,62,63,63,64,64,65,66,67);
my $platGiven = ($randomGivenPlat[int(rand(scalar @randomGivenPlat))]);
my @randomGivenSilver = (0,1,2,3,4,5,6);
my $silverGiven = ($randomGivenSilver[int(rand(scalar @randomGivenSilver))]);
my @randomGivenGold = (0,1,2,3,4,5,6,7);
my $goldGiven = ($randomGivenGold[int(rand(scalar @randomGivenGold))]);
quest::givecash(0,$silverGiven,$goldGiven,$platGiven);
my $totalTickets = $multiplier * 1;
quest::say("Thank you kindly.  Here is some cash and I award you $totalTickets PVP points for your trouble.");
$client->AddPVPPoints($totalTickets);
}

}

sub EVENT_SPAWN {
$npc->SetSlotTint(1, 0,25, 45); #1 for chest
$npc->SetSlotTint(2, 0,25, 45); #2 for arms
$npc->SetSlotTint(3, 0,25, 45); #3 for bracers
$npc->SetSlotTint(4, 0,25, 45); #4 for hands
$npc->SetSlotTint(5, 0,25, 45); #5 for legs
$npc->SetSlotTint(6, 0,25, 45); #6 for boots
$npc->SetSlotTint(0, 0,25, 45); #0 for helm
}