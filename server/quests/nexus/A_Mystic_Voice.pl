sub EVENT_SPAWN {
quest::settimer("nexus2min",120);
quest::say("In approximately ten minutes the portals will become active and will transport you back to Odus, Antonica, Faydwer, and Commonlands.  In five minutes the portal to South Ro will become active.");
quest::signalwith(152000,20,0);
}

sub EVENT_TIMER {
if ($timer eq "nexus2min") {
quest::stoptimer("nexus2min");
quest::say("The portal to South Ro will become active in three minutes.  Please begin gathering in the portal area.");
quest::settimer("velious2min",120);
quest::signalwith(152000,21,0);
}
if ($timer eq "velious2min") {
quest::stoptimer("velious2min");
quest::say("One minute till teleportation to South Ro.  Please be prepared to step onto the teleport pad.");
quest::settimer("velious1min",60);
quest::signalwith(152000,22,0);
}
if ($timer eq "velious1min") {
quest::stoptimer("velious1min");
quest::say("The portal to South Ro is now active.  Please stand on the pad and you will be teleported shortly.");
quest::say("The portals to Odus, Antonica, Faydwer, and Commonlands will become active in five minutes. Please begin gathering in the portal areas.");
quest::setglobal(velious_port,1,3,"S60"); #when the portal is open, global is set true for 1 min.
quest::settimer("ports2min",120);
quest::signalwith(152000,23,0);
}
if ($timer eq "ports2min") {
quest::stoptimer("ports2min");
quest::say("As a reminder, the portals to Odus, Antonica, Faydwer, and Commonlands will become active in three minutes.");
quest::settimer("nexustimer",120);
quest::signalwith(152000,24,0);
}
if ($timer eq "nexustimer") {
quest::stoptimer("nexustimer");
quest::say("One minute till teleportation to Odus, Antonica, Faydwer, and Commonlands. Please be prepared to step onto the teleport pad.");
quest::settimer("ports1min",60);
quest::signalwith(152000,25,0);
}
if ($timer eq "ports1min") {
quest::stoptimer("ports1min");
quest::say("The portals to Odus, Antonica, Faydwer, and Commonlands are now active. Please stand on the pad and you will be teleported shortly. Come back soon.");
quest::setglobal(odus_port,1,3,"S60"); #when the portal is open, global is set true for 1 min.
quest::setglobal(kunark_port,1,3,"S60"); #when the portal is open, global is set true for 1 min.
quest::setglobal(faydwer_port,1,3,"S60"); #when the portal is open, global is set true for 1 min.
quest::setglobal(antonica_port,1,3,"S60"); #when the portal is open, global is set true for 1 min.
quest::settimer("nexus2min",60);
quest::say("In approximately ten minutes the portals will become active and will transport you back to Odus, Antonica, Faydwer, and Commonlands.  In five minutes the portal to South Ro will become active.");
quest::signalwith(152000,26,0);
}
 }

