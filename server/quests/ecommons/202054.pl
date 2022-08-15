sub EVENT_SAY { 

if ($text=~/hail/i) { quest::say("Hello! If your a veteran please turn in your Ticket to me. I will grant you level 50!");  #If you want [default skills] I will set skills to 100 that you do not have!");
}
#if ($text=~/default skills/i) { 
#skillUps($client);
#}
}

#item 85062
sub EVENT_ITEM {
if ( $client->GetLevel() > 49 )
{
quest::say("You are past lvl 50.. why are you giving me anything?");
}
elsif(plugin::check_handin(\%itemcount, 85062 => 1) || plugin::check_handin(\%itemcount, 132488 => 1)) {
my $skillpts = ((50 - $client->GetLevel()) * 5) + $client->GetSkillPoints();
#quest::scribespells(3,3);
quest::level(50);
quest::ding();
quest::givecash(0,0,0,150); 
$client->SetSkillPoints($skillpts);

skillUps($client);

quest::say("Thank you very much!! You are now level 50! Ive given you some coin to help you on your way.  You must make your way outside of east commons (zone to north ro) for your skill points to be available.");
quest::say("Be sure to bind yourself in a safe area!! If you die and are bound to a city they may be hostile to you if guild owned!");
}
}

sub skillUps {
   $client = $_[0];
my $defaultSkillLimit = 100;

for (my $i=0; $i <= 73; $i++) {

if ( $client->GetSkill($i) >= $defaultSkillLimit ) {

}
elsif ( $client->GetClass() != 9 && ( $i == 8 || $i == 35 ) )
{
#rogue & backstab, picklock
}
elsif ( $client->GetClass() != 8 && ( $i == 41 || $i == 49 || $i == 54 || $i == 70 ) )
{
#bard & singing, string instruments, wind instruments, percussion instruments
}
elsif( $client->GetClass() != 7 && ( $i == 20 || $i == 21 || $i == 23 || $i == 26 || $i == 28 || $i == 31 || $i == 32 || $i == 38 || $i == 52 || $i == 71 )  )
{
#monk & double attack, dragon punch, eagle strike, flying kick, h2h, meditate, mend, round kick,tiger claw, intimidation
}
elsif ( $i == 11 || $i == 72 )
{
#BLOCK (11), BERSERKING (72)
}
else
{
$client->SetSkill($i,$defaultSkillLimit);
}
}
}
