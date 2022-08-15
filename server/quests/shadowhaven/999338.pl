# quest pvp merchant for warriors / monks
sub EVENT_SAY
{
if($text=~/Hail/i){
$link = quest::saylink("tickets");
quest::say("Hello $name, do you happen to have any precious $link?");
}
elsif($text=~/tickets/i){
quest::say("I collect tickets -- its what I do!!  Provide me one gold ticket and I will reward you for your troubles.");
}
}

sub EVENT_ITEM {

if(plugin::check_handin(\%itemcount, 132487 => 4)) {
quest::say("Thank you kindly.  Here is some cash and I award you four PVP points for your trouble.");

my @randomGivenPlat = (6,7);
my $platGiven = ($randomGivenPlat[int(rand(scalar @randomGivenPlat))]);
my @randomGivenSilver = (0,1,2,3,4,5,6);
my $silverGiven = ($randomGivenSilver[int(rand(scalar @randomGivenSilver))]);
my @randomGivenGold = (0,1,2,3,4,5,6,7);
my $goldGiven = ($randomGivenGold[int(rand(scalar @randomGivenGold))]);
quest::givecash(0,$silverGiven,$goldGiven,$platGiven);
$client->AddPVPPoints(4);
}
elsif(plugin::check_handin(\%itemcount, 132487 => 3)) {
quest::say("Thank you kindly.  Here is some cash and I award you three PVP points for your trouble.");

my @randomGivenPlat = (4,4,5,5,6,7);
my $platGiven = ($randomGivenPlat[int(rand(scalar @randomGivenPlat))]);
my @randomGivenSilver = (0,1,2,3,4,5,6);
my $silverGiven = ($randomGivenSilver[int(rand(scalar @randomGivenSilver))]);
my @randomGivenGold = (0,1,2,3,4,5,6,7);
my $goldGiven = ($randomGivenGold[int(rand(scalar @randomGivenGold))]);
quest::givecash(0,$silverGiven,$goldGiven,$platGiven);
$client->AddPVPPoints(3);
}
elsif(plugin::check_handin(\%itemcount, 132487 => 2)) {
quest::say("Thank you kindly.  Here is some cash and I award you two PVP points for your trouble.");

my @randomGivenPlat = (2,2,3,3,4,4,5,6,7);
my $platGiven = ($randomGivenPlat[int(rand(scalar @randomGivenPlat))]);
my @randomGivenSilver = (0,1,2,3,4,5,6);
my $silverGiven = ($randomGivenSilver[int(rand(scalar @randomGivenSilver))]);
my @randomGivenGold = (0,1,2,3,4,5,6,7);
my $goldGiven = ($randomGivenGold[int(rand(scalar @randomGivenGold))]);
quest::givecash(0,$silverGiven,$goldGiven,$platGiven);
$client->AddPVPPoints(2);
}
elsif(plugin::check_handin(\%itemcount, 132487 => 1)) {
quest::say("Thank you kindly.  Here is some cash and I award you a PVP point for your trouble.");

my @randomGivenPlat = (1,1,2,3,3,4,4,5,6,7);
my $platGiven = ($randomGivenPlat[int(rand(scalar @randomGivenPlat))]);
my @randomGivenSilver = (0,1,2,3,4,5,6);
my $silverGiven = ($randomGivenSilver[int(rand(scalar @randomGivenSilver))]);
my @randomGivenGold = (0,1,2,3,4,5,6,7);
my $goldGiven = ($randomGivenGold[int(rand(scalar @randomGivenGold))]);
quest::givecash(0,$silverGiven,$goldGiven,$platGiven);
$client->AddPVPPoints(1);
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