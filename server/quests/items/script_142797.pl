sub EVENT_ITEM_CLICK_CAST {
$casttime = 10000;

if ( $client->GetPetID() > 0 )
{
$targetid = $client->GetPetID();
$client->Message(15,"The pendant begins to let off a mystical blue glow..."); 
if ( $client->GetLevel() < 18 )
{
$client->Message(15,"You must be at least level 18 before using this effect..."); 
$client->CastSpell(8558, $targetid, 10, 1000, 0); 
}
elsif ( $client->GetLevel() < 28 )
{
#spirit of the blizzard lvl 18 - spell 2636
$client->CastSpell(2636, $targetid, 10, $casttime/2, 0); 
}
elsif ( $client->GetLevel() < 38 )
{
#spirit of the inferno lvl 28 - spell 2637
$client->CastSpell(2637, $targetid, 10, $casttime/2, 0); 
}
elsif ( $client->GetLevel() < 46 )
{
#spirit of the scorpion lvl 38 - spell 2638
$client->CastSpell(2638, $targetid, 10, $casttime/2, 0); 
}
elsif ( $client->GetLevel() < 51 )
{
#spirit of the vermin lvl 46 - spell 2639
$client->CastSpell(2639, $targetid, 10, $casttime/2, 0); 
}
elsif ( $client->GetLevel() < 53 )
{
#spirit of the wind lvl 51 - spell 2640
$client->CastSpell(2640, $targetid, 10, $casttime/2, 0); 
}
elsif ( $client->GetLevel() < 54 )
{
#spirit of the storm lvl 53 - spell 2641
$client->CastSpell(2641, $targetid, 10, $casttime/2, 0); 
}
elsif ( $client->GetLevel() >= 54 )
{
#spirit of the snow lvl 54 - spell 2890
$client->CastSpell(2890, $targetid, 10, $casttime/2, 0); 
}
return; #end of has pet
}

#no pet
if ( $client->GetLevel() < 15 )
{
	    $client->Message(15,"[Spirit of Sharik] A warder presents itself!"); 
	    $client->CastSpell(2612, 0, 10, $casttime, 0); 
}
elsif ( $client->GetLevel() < 21 )
{
	    $client->Message(15,"[Spirit of Khaliz] A warder presents itself!"); 
	    $client->CastSpell(2633, 0, 10, $casttime, 0); 
}
elsif ( $client->GetLevel() < 30 )
{
	    $client->Message(15,"[Spirit of Keshuval] A warder presents itself!"); 
	    $client->CastSpell(2614, 0, 10, $casttime, 0); 
}
elsif ( $client->GetLevel() < 39 )
{
	    $client->Message(15,"[Spirit of Herikol] A warder presents itself!"); 
	    $client->CastSpell(2616, 0, 10, $casttime, 0); 
}
elsif ( $client->GetLevel() < 46 )
{
	    $client->Message(15,"[Spirit of Yekan] A warder presents itself!"); 
	    $client->CastSpell(2618, 0, 10, $casttime, 0); 
}
elsif ( $client->GetLevel() < 54 )
{
	    $client->Message(15,"[Spirit of Kashek] A warder presents itself!"); 
	    $client->CastSpell(2621, 0, 10, $casttime, 0); 
}
elsif ( $client->GetLevel() < 56 )
{
	    $client->Message(15,"[Spirit of Omakin] A warder presents itself!"); 
	    $client->CastSpell(2623, 0, 10, $casttime, 0); 
}
elsif ( $client->GetLevel() < 58 )
{
	    $client->Message(15,"[Spirit of Zehkes] A warder presents itself!"); 
	    $client->CastSpell(2626, 0, 10, $casttime, 0); 
}
elsif ( $client->GetLevel() < 60 )
{
	    $client->Message(15,"[Spirit of Khurenz] A warder presents itself!"); 
	    $client->CastSpell(2627, 0, 10, $casttime, 0); 
}
elsif ( $client->GetLevel() >= 60 )
{
	    $client->Message(15,"[Spirit of Khati Sha] A warder presents itself!"); 
	    $client->CastSpell(2631, 0, 10, $casttime, 0); 
}
}