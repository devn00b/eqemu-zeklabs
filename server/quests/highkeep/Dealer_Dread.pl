sub EVENT_SAY { 
 if($text=~/Hail/i){ 
quest::say("Greetings. $name! This table is for 100 Silver players. If your interested in trying your hand give me 100 plat.Or if you want more info you can [ask] how it works"); 
}
if($text=~/ask/i){
quest::say("It is pretty simple, for plat, gold, silver we random 1000, if you roll greater than 500 its a loss. Each coin type is its own roll. For copper, we roll 2000 if you roll over 1000 we set it to 1000. You can win upto 500plat, 500goold, 500silver, and 1000copper. That's it. Pretty Simple I think.");
quest::say("If you are interested in playing please just hand me 100 Silver..");
}
}

##start devnoob addons

sub EVENT_ITEM {
my $shit = $client->GetGlobal("Gamble");
#$shit = "meh";
#quest::say("V: $shit");
if($silver == 100)
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
quest::say("you are weighted");
$winp = int(rand (2800));
$wing = int(rand (2500));
$winx = int(rand (2000));
$winc = int(rand (6000));
}
quest::say("Badda, badda, badda, badda, booom, bang, clang, ching, chang.");

##lets make it harder
if($winp >5) {
quest::say("You rolled $winp which is >5 so you lose on the platinum roll.");
#quest::setglobal("Gamble", ($client->GetGlobal("Gamble") - 50), 5, "D3");
$winp = 0;
}
if($wing >50) {
quest::say("You rolled $wing which is >50 so you lose on the gold roll.");
quest::setglobal("Gamble", ($client->GetGlobal("Gamble") - 10), 5, "D3");
$wing = 0;
}
if($winx >500) {
quest::say("You rolled $winx which is >500 so you lose on the silver roll.");
quest::setglobal("Gamble", ($client->GetGlobal("Gamble") - 50), 5, "D3");
$winx = 0;
}
if($winc >1000) {
quest::say("You rolled too high. $winc is > 1000. So you get 1000 copper.");
$winc = 1000;
}

if (!defined $qglobals{"Gamble"}) {
quest::setglobal("Gamble", 1, 5, "D3");
}
#if their gamble o meter goes below 0 lets be sure it stays at 0 so its not too hard to climb back out. (if they are at 0 or less set to 0)
if ($client->GetGlobal("Gamble") < 1)
{
quest::setglobal("Gamble", 0, 5, "D3");
}

quest::say("Congrats, $name you won Platinum: $winp, Gold: $wing, Silver: $winx, Copper: $winc");
quest::givecash($winc, $winx, $wing, $winp);

#this permits fine tuning of the win/loss ratio. and how quick they hit the cap
$stf = $winp * 10;
$stx = $wing * 10;
if($winx > 0){
#quest::say("sts > 0 so did $winx + 50");
$sts = $winx + 50;
}else{
#quest::say("sts <= 0 so 0 you go");
$sts = 0;
}

#weight this more towards plat winnings so counter doesnt go up too fast. need to feel like they are making progress..
#if($wing >200)
#{
#$stx = $wing / 4;
#}

#if they arent winning much, lets help them out a bit (less than 100 pp lets not even count it)
#if($winp <100)
#{
#$stf = 0;
#}

$winnings = $stf + $stx;
$winnings = $winnings + $sts;
quest::setglobal("Gamble", ($client->GetGlobal("Gamble") + $winnings), 5, "D3");
$shitstorm = $client->GetGlobal("Gamble");
#quest::say("STF $stf STX $stx STS $sts WINNING $winnings Total: $shitstorm");
return;
}else {
quest::say("You gave me $platinum plat and thats not 100, so giving you back Copper: $copper, Silver: $silver, Gold: $gold, Platinum $platinum");
quest::givecash($copper, $silver, $gold, $platinum);
return;
}

}

#END of FILE Zone:highkeep  ID:6064 -- Dealer_Shonta 

