##########################################
# ZONE: Dawnshroud Peaks (dawnshroud)
#
# Rockhopper/Sambata Cycle
# Simulate chance that the cycle doesn't change
# even if Garn or Age Old is killed.
#
############################################# 

sub EVEN_SPAWN {
    quest::settimer("RGT", 1800);
    if(!defined($qglobal{"Rockhopper_Sambata"})) {
        quest::setglobal("Rockhopper_Sambata", 1, 3, "F");
    }
}

sub EVENT_TIMER {
    if($timer eq "RGT") {
        if(defined($qglobal{"Rockhopper_Sambata"})) {
            quest::delglobal("Rockhopper_Sambata");
        }
        quest::depop();
    }
}

# END of FILE Zone:dawnshroud  ID:174240 -- #Rock_Garn_Timer


