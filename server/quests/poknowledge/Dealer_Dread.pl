#sub delay{
#$delayOver = (time + @_[0]);
#while (time < $delayOver){}
#1;
#}

sub EVENT_SAY { 
if($text=~/Hail/i){
quest::say("Greetings. $name! This table is for 100 silver players. If your interested in trying your hand give me 100 SILVER. Or if you want more info you can [ask] how it works"); 

}
if($text=~/ask/i){
quest::say("It is pretty simple. The current max roll for plat is 5, gold 50, silver 500 and copper 1000. Each coin type is its own roll. You can win upto 5plat, 50 gold, 500 silver, and 1000 copper. That's it. Pretty Simple I think.");
quest::say("If you are interested in playing please just hand me 100 Silver.");
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
if (!defined $qglobals{"Gamble"}) {
quest::setglobal("Gamble","1", "1", "D3");
}

if($silver == 100)
{
### handle their gamble value if negative it shouldn't be since we set at the end but lets be sure!
$fuckitglob = $client->GetGlobal("Gamble");

if($fuckitglob < 1)
{
quest::setglobal("Gamble","0", "1", "D3");
}

#setup their winnings/losses
$winp = int(rand (1000));
$wing = int(rand (1000));
$winx = int(rand (1000));
$winc = int(rand (2000));
#LOL encumber them!

if($fuckitglob > 500)
{
quest::say("you are weighted");
#if they have gone over 500 gamble from a mixture of plat/gold rolls
#weighted more towards the plat winnings atm
$winp = int(rand (2800));
$wing = int(rand (2500));
$winx = int(rand (2000));
$winc = int(rand (6000));
}

quest::say("Badda, badda, badda, badda, booom, bang, clang, ching, chang.");
##lets make it harder

if($winp >5) {
quest::say("You rolled $winp which is >5 so you lose on the platinum roll.");
quest::setglobal("Gamble", $client->GetGlobal("Gamble") - 25, "0", "D3");
$winp = 0;
}

if($wing >50) {
quest::say("You rolled $wing which is >50 so you lose on the gold roll.");
quest::setglobal("Gamble", $client->GetGlobal("Gamble") - 50, "0", "D3");
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
quest::setglobal("Gamble","1", "1", "D1");
}

#if ($client->GetGlobal("Gamble") < 1)
#{
#quest::setglobal("Gamble", 0, 5, "H12");
#}

quest::say("Congrats, $name you won Platinum: $winp, Gold: $wing, Silver: $winx, Copper: $winc");
quest::givecash($winc, $winx, $wing, $winp);

#this permits fine tuning of the win/loss ratio. and how quick they hit the cap
#since this is the silver dealer we want plat/gold winnings to increase the winning count by a LOT
#$stf = $winp * 3; #max plat is 5 we need to increase this a lot
$stf = 0;
$stx = 0;
#$stx = $wing * 3; #max gold is 50 so we need this to go up quickly too
#since this is silver merchant lets count it!
$sts = $winx + 50; #max is 500 so lets just add 50 to whatever they roll

#weight this more towards plat winnings so counter doesnt go up too fast. need to feel like they are making progress..
#dont need this for silver wins give them full boom for gold wins
#if($wing >75)
#{
#$stx = $wing * 2;
#}

#if they arent winning much, lets help them out a bit (less than 5 pp lets not even count it)
#dont need this for silver winnigs, because they only earn a max of 5. let them get full credit.
#if($winp <5)
#{
#$stf = 0;
#}

$winnings = $stf + $stx + $sts;
$crap = $client->GetGlobal("Gamble");

quest::setglobal("Gamble", ($client->GetGlobal("Gamble") + $winnings), "1", "D3");

$crap2 = $client->GetGlobal("Gamble");
quest::say("Gamble After Winings Calculated: $crap2. Should be $crap + $winnings");

#lets set their gamble value to be 0 if its negative
if ($client->GetGlobal("Gamble") < 1)
{
quest::setglobal("Gamble", "0", "1", "D2");
}

return;
}else {
quest::say("You gave me $platinum Plat, $gold Gold, $silver Silver, $copper Copper and thats not 100 GOLD, so giving you back Copper: $copper, Silver: $silver, Gold: $gold, Platinum $platinum");
quest::givecash($copper, $silver, $gold, $platinum);
return;
}

}

#END of FILE Zone:highkeep  ID:6064 -- Dealer_Shonta 

