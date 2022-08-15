sub EVENT_SAY {
if(($text=~/hail/i)){
my $gonow = quest::saylink("GO");
quest::say("Greetings! I can send you to the other side of the zone. just let me know if you wish to [$gonow]");
}

if(($text=~/go/i)){
quest::say("To the other side with you!!");
quest::movepc(153,907.9,1009.2,-126.4,130.1);

}



}
