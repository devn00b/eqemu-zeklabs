sub EVENT_SIGNAL {
if ($signal == 20) {
quest::say("In approximately ten minutes the portals will become active and will transport you back to Odus, Antonica, Faydwer, and Commonlands.  In five minutes the portal to South Ro will become active.");
}
if ($signal == 21) {
quest::say("The portal to South Ro will become active in three minutes.  Please begin gathering in the portal area.");
}
if ($signal == 22) {
quest::say("One minute till teleportation to South Ro.  Please be prepared to step onto the teleport pad.");
}
if ($signal == 23) {
quest::say("The portal to South Ro is now active.  Please stand on the pad and you will be teleported shortly.");
quest::say("The portals to Odus, Antonica, Faydwer, and Commonlands will become active in five minutes. Please begin gathering in the portal areas.");
}
if ($signal == 24) {
quest::say("As a reminder, the portals to Odus, Antonica, Faydwer, and Commonlands will become active in three minutes.");
}
if ($signal == 25) {
quest::say("One minute till teleportation to Odus, Antonica, Faydwer, and Commonlands. Please be prepared to step onto the teleport pad.");
}
if ($signal == 26) {
quest::say("The portals to Odus, Antonica, Faydwer, and Commonlands are now active. Please stand on the pad and you will be teleported shortly. Come back soon.");
quest::say("In approximately ten minutes the portals will become active and will transport you back to Odus, Antonica, Faydwer, and Commonlands.  In five minutes the portal to South Ro will become active.");
}
  }