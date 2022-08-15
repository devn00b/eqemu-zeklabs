sub EVENT_SAY {
#disable for beta bash make <2 for off or more than 2 for on
$betabash = 0;

if($betabash < 2){
quest::say("Discord is disabled. No discord for you!");

#$playflag = $client->GetPVPFlag();
#if($playflag eq '5'){
#my $oldlastn = $client->GetGlobal("gonediscord");
#quest::surname("$oldlastn");
#$client->SetTitleSuffix("$meh");
#quest::removetitle(420);
#quest::delglobal(gonediscord);
#quest::say("I have removed your discord, since its disabled.");
#$client->SetPVPValue(2);
return;
}
return;
}

if ($ulevel <10){
quest::say("Due to some players (Genkai) doing shady shit with the gnome race, We have moved min level for corpses to level 10. So you must be level 10 before you can go discord. Thanks Genkai");
return;
}

if ( defined $qglobals{akanon_racestarted} && defined $qglobals{akanon_race_playerflag})
{
quest::say("Sorry, Gnome Racers are Discord Exempt");
return;
}

if(($text=~/hail/i) ){
$playflag = $client->GetPVPFlag();
if($playflag eq '5')
{
$retard = quest::saylink("willing");
$retardtwo = quest::saylink("supply");
#quest::say("Sorry $name, I have nothing further to say to you!");
quest::say("Hello $name, I see you are a discord player. Either by choice, or because your fucking stupid. I can help you with that if you are [$retard].");
#if(!plugin::check_hasitem($client, $prze)){
quest::say("Alternativly, I can [$retardtwo] you with another staff if you lost your last one.");
return;
}
my $link = quest::saylink("interested");
my $notdsc = quest::saylink("revert");

quest::say("Hello $name, I am the High Priestess of Discord. I am tasked with finding the best, most hardcore of people and empowering them with the power of Discord");
quest::say("Beware however, that just because you think you are hardcore you will quickly find out, to your own detriment, there are others that might be better than you.");
quest::say("If you are [$link] in learning more please, just ask. If not, please leave, I have better things to do than waste time on you.");
quest::say("Also, if you are no longer discord, and wish to [$notdsc] your lastname and title , Just let me know and I'll see what I can do");

#return;
  }

if($text=~/willing/i){
my $itemz = quest::varlink(145624);
quest::say("All you have to do is bring me one $itemz. The last I saw of them they were being sold by Albedo in the bazaar. Once you have one come see me again.");
return;
}

if($text=~/supply/i){
$playflag = $client->GetPVPFlag();
if($playflag eq '5')
{
if(plugin::check_hasitem($client, "146595")){
quest::say("You already have this item $name Fuckoff!");
return;
}
quest::summonitem(146595);
my $itemzz = quest::varlink(146595);
quest::say("Here you go, one shiny new $itemzz. Please hold onto this one!?");
return;
} 
return;
}

if(($text=~/revert/i) and $playflag ne '5'){
my $ln = $client->GetLastName();
if( $ln ne "Discord Player")
{
quest::say("Stop wasting my time $name");
return;
}
my $meh = " ";
my $oldlastn = $client->GetGlobal("gonediscord");	
quest::surname("$oldlastn");
$client->SetTitleSuffix("$meh");
quest::removetitle(420);
quest::shout2("$name, has failed the path of Discord. They are now just a normal plebeian. Please join us in mocking them!");
quest::delglobal(gonediscord);
return;
}

if(($text=~/interested/i) ){
$playflag = $client->GetPVPFlag();

if($playflag eq '5')
{
quest::say("Sorry $name, I have nothing further to say to you!");
return;
}

$hardcore = quest::saylink("hardcore");
quest::say("So, you think you have what it takes? All you have to do now is tell me that you are [$hardcore] enough to become one of us.");
quest::say("***** BEWARE CONTINUING WILL RESULT IN YOU BECOMING A DISCORD PLAYER. DEATH WILL RESULT IN LOSS OF ALL ITEMS AND YOU BEING RESET TO LEVEL ONE *****");
}

if(($text=~/hardcore/i) ){
$playflag = $client->GetPVPFlag();
if($playflag eq '5')
{
quest::say("Sorry $name, I have nothing further to say to you!");
return;
}

if (!defined $qglobals{"gonediscord"}) {
quest::setglobal("gonediscord", "tempset", 5, "F");
}

my $ln = $client->GetLastName();
quest::setglobal("gonediscord", "$ln", 5, "F");
$client->SetPVPValue(5);
$client->SetTitleSuffix("of Discord");
quest::enabletitle(420);
quest::surname("Discord Player");
quest::say("Congrats $name! You are now a discord player. Lets see how mighty you are. I don't think you'll make it a week before you lose all your gear. Prove me wrong!");
quest::say("Oh, and by the way I have saved your lastname. If you die come back and ask for a revert and I'll fix it for you!");
quest::shout2("A true player-killer has arisen. $name has chosen to walk the path of Discord!");
quest::say("");
quest::say("..I almost forgot here take this. It should restore some of your ability to gate faster. It will go way, of course, if and when you die to another player that walks the path of discord.");
quest::summonitem(146595);
##Buff them
  if ( $client->GetPVPFlag() == 5){

        #give discord players spell haste.
        #$selfbuff = 3531;

        $casterbuff  = 5193;
        $casterbuffb = 5194;

        #SK/PAL/WAR
        $tankbuff  = 5196;
        $tankbuffb = 5195;

        #MONK/ROG/RNG/BRD
        $dpsbuff  = 5191;
        $dpsbuffb = 5192;

        if (   defined $qglobals->{akanon_racestarted} && defined $qglobals->{akanon_race_playerflag} ) {
            $casterbuff  = 0;
            $casterbuffb = 0;

            #SK/PAL/WAR
            $tankbuff  = 0;
            $tankbuffb = 0;

            #MONK/ROG/RNG/BRD
            $dpsbuff  = 0;
            $dpsbuffb = 0;
        }
      if (   $class eq "Necromancer" or $class eq "Enchanter" or $class eq "Magician" or $class eq "Wizard" or $class eq "Druid" or $class eq "Shaman" or $class eq "Cleric" ) {

          #if less than 31 give them low buff. over 30 so 31, give them highbuff
            if ( $ulevel < 31 ) {
                quest::selfcast($casterbuff);
            }
            if ( $ulevel > 30 ) {
                quest::selfcast($casterbuffb);
            }
        }

        if (   $class eq "Warrior" or $class eq "Shadowknight" or $class eq "Paladin" ) {
          #if less than 31 give them low buff. over 30 so 31, give them highbuff
            if ( $ulevel < 31 ) {
                quest::selfcast($tankbuff);
            }
            if ( $ulevel > 30 ) {
                quest::selfcast($tankbuffb);
            }
        }

        if (   $class eq "Monk" or $class eq "Rogue" or $class eq "Ranger" or $class eq "Bard" ) {
          #if less than 31 give them low buff. over 30 so 31, give them highbuff
            if ( $ulevel < 31 ) {
                quest::selfcast($dpsbuff);
            }
            if ( $ulevel > 30 ) {
                quest::selfcast($dpsbuffb);
            }

        }

		}

return;
}

} 

sub EVENT_ITEM {
$checkHandIn = plugin::check_handin(\%itemcount, 145624=> 1);
if ( $checkHandIn ) {
quest::say("Ok $name, I shall grant you release from the ways of discord.");
$client->SetPVPValue(2);
my $meh = " ";
my $meh2 = "I Bought My Freedom";
quest::surname("$meh2");
$client->SetTitleSuffix("$meh");
quest::removetitle(420);
quest::shout2("$name, has bought their way out of discord. What a despicable little $race.");
return;
}
 quest::say("I do not want this...Crap...But thanks");

}
