sub EVENT_SAY
{

my $flagName = "flag_communityevent1";
my $flagValue = "1";
my $flagType = 5;
my $flagTime = "F";

$flag = $qglobals{$flagName};

if($text=~/Hail/i){
quest::say("Hello $name, welcome to the King of the Hill PVP Community Event! If you wish to [participate] just let me know!");
}
elsif($text=~/participate/i){
if(defined($flag) && $flag == $flagValue )
{
quest::say("You have already received your participatory introduction.");
}
else
{
quest::setglobal($flagName,$flagValue,$flagType,$flagTime);
quest::shout("$name shall participate in the challenge!");
quest::say("Fantastic! I have given you a few items, pvp points and cash thanks to your participation.  Prepare yourself for the match to begin!  This will be a free for all, no groups, no friends, just enemies!  Last man standing will receive a title and special item!");
my @randomGivenPlat = (100,100);
my $platGiven = ($randomGivenPlat[int(rand(scalar @randomGivenPlat))]);
my @randomGivenSilver = (0,1,2,3,4,5,6);
my $silverGiven = ($randomGivenSilver[int(rand(scalar @randomGivenSilver))]);
my @randomGivenGold = (0,1,2,3,4,5,6,7);
my $goldGiven = ($randomGivenGold[int(rand(scalar @randomGivenGold))]);
quest::givecash(0,$silverGiven,$goldGiven,$platGiven);

my @randSparkler1 = (54714,54713);
quest::summonitem($randSparkler1[int(rand(scalar @randSparkler1))],50);
my @randSparkler2 = (54716,54715);
quest::summonitem($randSparkler2[int(rand(scalar @randSparkler2))],50);

$client->AddPVPPoints(5);
}
}

}