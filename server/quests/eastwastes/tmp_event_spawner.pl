sub EVENT_SAY
{
$getx = $client->GetX();
$gety = $client->GetY();
$bestz = quest::GetBestZ($client->GetX(),$client->GetY(),$client->GetZ());
quest::say("BestZ: $bestz, XY: $getx, $gety");
}