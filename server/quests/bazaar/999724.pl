sub EVENT_SAY {

if($text=~/hail/i){
if (
 $class eq 'Warrior' ||
 $class eq 'Monk' ||
 $class eq 'Berserker' ||
 $class eq 'Shadowkight' ||
 $class eq 'Paladin' ||
 $class eq 'Ranger' ||
 $class eq 'Bard' ||
 $class eq 'Beastlord' ||
 $class eq 'Cleric' ||
 $class eq 'Druid' ||
 $class eq 'Necromancer' ||
 $class eq 'Magician' ||
 $class eq 'Enchanter' ||
 $class eq 'Wizard' ||
 $class eq 'Shaman' ) {
quest::say("Sorry $name we dont want to talk to scum like you. I'm only interested in talking to Rogues.");
return;
}
$devone = quest::varlink(145644);
$devtwo = quest::varlink(145726);


quest::say("For this beta we are testing a new weapon if you are interested just hand me a $devone and I'll give you $devtwo !");
}

}

sub EVENT_ITEM {
if (
 $class eq 'Warrior' ||
 $class eq 'Monk' ||
 $class eq 'Berserker' ||
 $class eq 'Shadowkight' ||
 $class eq 'Paladin' ||
 $class eq 'Ranger' ||
 $class eq 'Bard' ||
 $class eq 'Beastlord' ||
 $class eq 'Cleric' ||
 $class eq 'Druid' ||
 $class eq 'Necromancer' ||
 $class eq 'Magician' ||
 $class eq 'Enchanter' ||
 $class eq 'Wizard' ||
 $class eq 'Shaman' ) {
quest::say("Nice try! Oh, and I'll keep that...thanks...");
return;
}

$checkHandIn = plugin::check_handin(\%itemcount,145644=> 1);
if ( $checkHandIn ) {

quest::say("Boom! Please let us know how it goes!");
quest::summonitem(145726);
return;
}

quest::say("Whats this crap? I guess I can wipe my ass with it.");
}
