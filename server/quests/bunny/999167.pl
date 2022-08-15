sub EVENT_SAY
{
if ($text=~/hail/i) {
$interestedLink = quest::saylink("interested");

quest::say("Greetings $name, would you be $interestedLink in assisting me by getting me some grub?");
if ($ulevel > 2 )
{
	$prisonKeyLink = quest::saylink("prison keys");
	quest::say("If you want a bigger challenge I have some orcs that need slaying for their $prisonKeyLink.");
}
}
if ($text=~/interested/i) {
quest::say("Excellent!  I would gladly help you in return once you have collected my food, collect meat from two rabbits outside the main entrance return it to me.");
}

if ( $ulevel > 2 )
{
if ($text=~/prison keys/i) {
quest::say("There are Bunny Slave and Bunny Prisoner Orc's running around in the snow, some have an important [forked prison key] that I need, go get it!");
}
}

}

sub EVENT_ITEM
{
my $var = $itemcount{13048};

if ( $var > 1 )
{
quest::scribespells(3,3);
quest::exp(150);
quest::givecash(0,1,0,0);
quest::say("Thank you very much!! I have provided you some spells in return.");
}

if ( $ulevel > 2 )
{
my $var2 = $itemcount{5976};
if ( $var2 > 0 )
{
quest::scribespells(4,4);
quest::exp(300);
quest::givecash(0,5,0,0);
quest::say("Good job, here is some additional spells and cash for you.");
}
}

}