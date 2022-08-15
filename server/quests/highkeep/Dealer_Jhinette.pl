#sub delay{
#$delayOver = (time + @_[0]);
#while (time < $delayOver){}
#1;
#}

sub EVENT_SAY { 
if($text=~/Hail/i){
quest::say("Greetings. $name! This table is for 100 Gold players. If your interested in trying your hand give me 100 Gold. Or if you want more info you can [ask] how it works"); 

}
if($text=~/ask/i){
quest::say("It is pretty simple, for plat, gold, silver we random 1000, if you roll greater than 500 its a loss. Each coin type is its own roll. For copper, we roll 2000 if you roll over 1000 we set it to 1000. You can win upto 100plat, 500gold, 500silver, and 1000copper. That's it. Pretty Simple I think.");
quest::say("If you are interested in playing please just hand me 100 Gold.");
}
}

##start devnoob addons
## Gold Casino
## Plat rand 1000
## Plat if <100 they win
## Gold rand 1000
## Gold if <500 they win
## etc etc etc

sub EVENT_ITEM {
if($gold == 100)
{
$winp = int(rand (1000));
$wing = int(rand (1000));
$winx = int(rand (1000));
$winc = int(rand (2000));
#LOL encumber them!
if($client->GetGlobal("Gamble") >500)
{
#if they have gone over 500 gamble from a mixture of plat/gold rolls
#weighted more towards the plat winnings atm
$winp = int(rand (2800));
$wing = int(rand (2500));
$winx = int(rand (2000));
$winc = int(rand (6000));
}

quest::say("Badda, badda, badda, badda, booom, bang, clang, ching, chang.");
#sleep(3);
#delay(5);
##lets make it harder
#gold rolls can win you 10p
if($winp >10) {
quest::say("You rolled $winp which is >10 so you lose on the platinum roll.");
$winp = 0;
quest::setglobal("Gamble", ($client->GetGlobal("Gamble") - 75), 5, "H12");
# $winp = 0;
}
if($wing >500) {
quest::say("You rolled $wing which is >500 so you lose on the gold roll.");
quest::setglobal("Gamble", ($client->GetGlobal("Gamble") - 100), 5, "H12");
$wing = 0;
}

if($winx >500) {
quest::say("You rolled $winx which is >500 so you lose on the silver roll.");
$winx = 0;
}

if($winc >1000) {
quest::say("You rolled too high. $winc is > 1000. So you get 1000 copper.");
$winc = 1000;
}
if (!defined $qglobals{"Gamble"}) {
quest::setglobal("Gamble", 1, 5, "H12");
}
if ($client->GetGlobal("Gamble") < 1)
{
quest::setglobal("Gamble", 0, 5, "H12");
}

quest::say("Congrats, $name you won Platinum: $winp, Gold: $wing, Silver: $winx, Copper: $winc");
quest::givecash($winc, $winx, $wing, $winp);
#this permits fine tuning of the win/loss ratio. and how quick they hit the cap
$stf = $winp * 10; #max plat is 10, so we need to heavly weigh it for gold casino
$stx = $wing * 2;  #max gold is 500, so they can max out pretty fast.

#weight this more towards plat winnings so counter doesnt go up too fast. need to feel like they are making progress..
#removing this give them full gold count
#if($wing >75)
#{
#$stx = $wing / 4;
#}

#if they arent winning much, lets help them out a bit (less than 5 pp lets not even count it)
#max plat wins are 10, so if they get 1plat let it slide
if($winp <2)
{
$stf = 0;
}

#set their global
$winnings = $stf + $stx;
quest::setglobal("Gamble", ($client->GetGlobal("Gamble") + $winnings), 5, "H12");

return;
}else {
quest::say("You gave me $platinum Plat, $gold Gold, $silver Silver, $copper Copper and thats not 100 GOLD, so giving you back Copper: $copper, Silver: $silver, Gold: $gold, Platinum $platinum");
quest::givecash($copper, $silver, $gold, $platinum);
return;
}

}

#END of FILE Zone:highkeep  ID:6064 -- Dealer_Shonta 

