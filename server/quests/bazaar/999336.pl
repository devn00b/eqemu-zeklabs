sub EVENT_SAY {
  if($text=~/hail/i) {
quest::say("If u got ears me want ears!  I want ears of all kinds! Give me one or more ears for a prize!");
}
}

sub EVENT_ITEM {
my $result = plugin::checkear_turnin($item1,$item1_name, $name);
my $result2 = plugin::checkear_turnin($item2,$item2_name, $name);
my $result3 = plugin::checkear_turnin($item3,$item3_name, $name);
my $result4 = plugin::checkear_turnin($item4,$item4_name, $name);
if(plugin::check_handin(\%itemcount, 1 => 1)) {
}
my $count = 0;

if ( $result > 0 )
{
 $count += 1;
}
if ( $result2 > 0 )
{
 $count += 1;
}
if ( $result3 > 0 )
{
 $count += 1;
}
if ( $result4 > 0 )
{
 $count += 1;
}
my $total = $count*2;


if ( $total < 1 )
{
	quest::say("Me keep dis but nothing for joo.");
}
else
{
   	for (my $i=1; $i <=  $total; $i++) {
    	quest::summonitem(132487);
    	}
	quest::say("Mmmm excellent!! Take this and talk to my buddy Tizzl he can give you some platinum and points for these tickets");

}

}