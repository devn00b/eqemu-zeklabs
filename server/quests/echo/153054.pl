sub EVENT_SAY {
if(($text=~/hail/i)){
my $gonow = quest::saylink("GO");
quest::say("Greetings! I can send you to the other side of the zone. just let me know if you wish to [$gonow]");
}

if(($text=~/GO/i)){
quest::say("To the other side with you!!");
quest::movepc(153,133.3,-1040.0,-1.0,9.5);

}



}
