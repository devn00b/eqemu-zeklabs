sub EVENT_SAY
{
if ( !defined $qglobals{RaceChange} || $qglobals{RaceChange} == 0 )
{
if($text=~/Hail/i){
$link = quest::saylink("Race Change Token");
quest::say("Hello $name, if you would like to change your race you must give me a $link, If you need one I sell them. Low price of 300PVP Points.!");
}
elsif($text=~/Race Change Token/i){
quest::say("Yes a token! Ears are so...last year. I sell them as I said. Buy one or get lost.");
}
}
else
{
if($text=~/Hail/i){
$human = quest::saylink("Human");
$barbarian = quest::saylink("Barbarian");
$erudite = quest::saylink("Erudite");
$woodelf = quest::saylink("Wood Elf");
$highelf = quest::saylink("High Elf");
$darkelf = quest::saylink("Dark Elf");
$halfelf = quest::saylink("Half Elf");
$dwarf = quest::saylink("Dwarf");
$troll = quest::saylink("Troll");
$ogre  = quest::saylink("Ogre");
$halfling = quest::saylink("Halfling");
$gnome = quest::saylink("Gnome");
$iksar = quest::saylink("Iksar");
$vahshir = quest::saylink("Vah Shir");
$froglok = quest::saylink("Froglok");
# Added Froglok and Vah Shir - Devnoob. Also converted to token from ear.
quest::say("$name, you may select your new race, $human, $barbarian, $erudite, $woodelf, $highelf, $darkelf, $halfelf, $dwarf, $troll, $ogre, $halfling, $gnome, $iksar, $vahshir, or $froglok?");

}
elsif($text=~/Human/i){
quest::setglobal("SelectRace",1,5,"F");
quest::popup("Select Race Human","<c \"#479eab\">Selected Race: </c><br>  <c \"#4fd613\">Human</c><br> <c \"#4fd613\">ARE YOU SURE?</c>",1,1,120);
}
elsif($text=~/Barbarian/i){
quest::setglobal("SelectRace",2,5,"F");
quest::popup("Select Race Barbarian","<c \"#479eab\">Selected Race: </c><br>  <c \"#4fd613\">Barbarian</c><br> <c \"#4fd613\">ARE YOU SURE?</c>",1,1,120);
}
elsif($text=~/Erudite/i){
quest::setglobal("SelectRace",3,5,"F");
quest::popup("Select Race Erudite","<c \"#479eab\">Selected Race: </c><br>  <c \"#4fd613\">Erudite</c><br> <c \"#4fd613\">ARE YOU SURE?</c>",1,1,120);
}
elsif($text=~/Wood Elf/i){
quest::setglobal("SelectRace",4,5,"F");
quest::popup("Select Race Wood Elf","<c \"#479eab\">Selected Race: </c><br>  <c \"#4fd613\">Wood Elf</c><br> <c \"#4fd613\">ARE YOU SURE?</c>",1,1,120);
}
elsif($text=~/High Elf/i){
quest::setglobal("SelectRace",5,5,"F");
quest::popup("Select Race High Elf","<c \"#479eab\">Selected Race: </c><br>  <c \"#4fd613\">High Elf</c><br> <c \"#4fd613\">ARE YOU SURE?</c>",1,1,120);
}
elsif($text=~/Dark Elf/i){
quest::setglobal("SelectRace",6,5,"F");
quest::popup("Select Race Dark Elf","<c \"#479eab\">Selected Race: </c><br>  <c \"#4fd613\">Dark Elf</c><br> <c \"#4fd613\">ARE YOU SURE?</c>",1,1,120);
}
elsif($text=~/Half Elf/i){
quest::setglobal("SelectRace",7,5,"F");
quest::popup("Select Race Half Elf","<c \"#479eab\">Selected Race: </c><br>  <c \"#4fd613\">Half Elf</c><br> <c \"#4fd613\">ARE YOU SURE?</c>",1,1,120);
}
elsif($text=~/Dwarf/i){
quest::setglobal("SelectRace",8,5,"F");
quest::popup("Select Race Dwarf","<c \"#479eab\">Selected Race: </c><br>  <c \"#4fd613\">Dwarf</c><br> <c \"#4fd613\">ARE YOU SURE?</c>",1,1,120);
}
elsif($text=~/Troll/i){
quest::setglobal("SelectRace",9,5,"F");
quest::popup("Select Race Troll","<c \"#479eab\">Selected Race: </c><br>  <c \"#4fd613\">Troll</c><br> <c \"#4fd613\">ARE YOU SURE?</c>",1,1,120);
}
elsif($text=~/Ogre/i){
quest::setglobal("SelectRace",10,5,"F");
quest::popup("Select Race Ogre","<c \"#479eab\">Selected Race: </c><br>  <c \"#4fd613\">Ogre</c><br> <c \"#4fd613\">ARE YOU SURE?</c>",1,1,120);
}
elsif($text=~/Halfling/i){
quest::setglobal("SelectRace",11,5,"F");
quest::popup("Select Race Halfling","<c \"#479eab\">Selected Race: </c><br>  <c \"#4fd613\">Halfling</c><br> <c \"#4fd613\">ARE YOU SURE?</c>",1,1,120);
}
elsif($text=~/Gnome/i){
quest::setglobal("SelectRace",12,5,"F");
quest::popup("Select Race Gnome","<c \"#479eab\">Selected Race: </c><br>  <c \"#4fd613\">Gnome</c><br> <c \"#4fd613\">ARE YOU SURE?</c>",1,1,120);
}
elsif($text=~/Iksar/i){
quest::setglobal("SelectRace",128,5,"F");
quest::popup("Select Race Iksar","<c \"#479eab\">Selected Race: </c><br>  <c \"#4fd613\">Iksar</c><br> <c \"#4fd613\">ARE YOU SURE?</c>",1,1,120);
}
elsif($text=~/Vah Shir/i){
quest::setglobal("SelectRace",130,5,"F");
quest::popup("Select Race Vah Shir","<c \"#479eab\">Selected Race: </c><br>  <c \"#4fd613\">Vah Shir</c><br> <c \"#4fd613\">ARE YOU SURE?</c>",1,1,120);
}
elsif($text=~/Froglok/i){
quest::setglobal("SelectRace",330,5,"F");
quest::popup("Select Race Froglok","<c \"#479eab\">Selected Race: </c><br>  <c \"#4fd613\">Froglok</c><br> <c \"#4fd613\">ARE YOU SURE?</c>",1,1,120);
}


}
}

## Removed the ear check, leaving here in case we ever need it again
#sub EVENT_ITEM {
#my $result = plugin::checkear_turnin($item1,$item1_name, $name);
#my $result2 = plugin::checkear_turnin($item2,$item2_name, $name);
#my $result3 = plugin::checkear_turnin($item3,$item3_name, $name);
#my $result4 = plugin::checkear_turnin($item4,$item4_name, $name);
#if(plugin::check_handin(\%itemcount, 1 => 1)) {

#}

sub EVENT_ITEM
{
 if(plugin::check_handin(\%itemcount, 145166 => 1))
  {
$result  = 1;
$result2 = 1;
$result3 = 1;
$result4 = 1;

if ( $result > 0 || $result2 > 0 || $result3 > 0 || $result4 > 0 )
{
$human = quest::saylink("Human");
$barbarian = quest::saylink("Barbarian");
$erudite = quest::saylink("Erudite");
$woodelf = quest::saylink("Wood Elf");
$highelf = quest::saylink("High Elf");
$darkelf = quest::saylink("Dark Elf");
$halfelf = quest::saylink("Half Elf");
$dwarf = quest::saylink("Dwarf");
$troll = quest::saylink("Troll");
$ogre  = quest::saylink("Ogre");
$halfling = quest::saylink("Halfling");
$gnome = quest::saylink("Gnome");
$iksar = quest::saylink("Iksar");
#devn00b added vashir and froglok
$vahshir = quest::saylink("Vah Shir");
$froglok = quest::saylink("Froglok");
quest::say("$name, you may select your new race, $human, $barbarian, $erudite, $woodelf, $highelf, $darkelf, $halfelf, $dwarf, $troll, $ogre, $halfling, $gnome, $iksar, $vahshir, or $froglok?");
quest::setglobal("RaceChange",1,5,"F");
}
}

 }
#i dont give a fuck to return the item if its wrong. Thats on the player fuck them

sub EVENT_POPUPRESPONSE {
if ( !defined $qglobals{SelectRace} || $qglobals{SelectRace} == 0 )
{
quest::say("No race was selected!");
}
else
{
$client->SetRace($SelectRace);
$client->SetBaseRace($SelectRace);
$client->Save(1);
quest::delglobal("SelectRace");
quest::delglobal("RaceChange");
quest::setglobal("SelectRace",0,5,"F");
quest::setglobal("RaceChange",0,5,"F");
quest::say("Your race has been changed, you must zone for it to take effect.");
}
}
