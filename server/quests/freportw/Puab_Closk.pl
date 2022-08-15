############################################
# ZONE: West Freeport (freportw)
# DATABASE: PEQ-Velious
# LAST EDIT DATE: May 6, 2010
# VERSION: 1.1
# BASE QUEST BY: PEQ Team
# DEVELOPER: MWMDRAGON, Congdar
#
# *** NPC INFORMATION ***
#
# NAME: Puab_Closk
# ID: 9086
# TYPE: Monk Guild Master
# RACE: Human
# LEVEL: 61
#
# *** ITEMS GIVEN OR TAKEN ***
#
# Puab's Token ID-12369
# A tattered note ID-18746
# Torn Cloth Tunic ID-13507
# Tattered Parchment ID-28055
# Treant Fists ID-12344
#
# *** QUESTS INVOLVED IN ***
#
# Newbie Note Hand-in
# Treant Fists
#
# *** QUESTS AVAILABLE TO ***
#
# New Freeport Monks
# Monks with good Ashen Order Faction
#
############################################

sub EVENT_SPAWN {
    $x = $npc->GetX();
    $y = $npc->GetY();
    quest::set_proximity($x - 20, $x + 20, $y - 20, $y + 20);
}

sub EVENT_ENTER {
    if ($ulevel == 1) {
        quest::echo(15, "As you get your bearings, a lightly armored man that appears to be in incredible shape turns to greet you. 'Welcome traveller. I am Puab Closk. If you wish to learn the ways of the Ashen Order, read the note in your inventory and hand it to me to begin your training.'");
    }
}

sub EVENT_SAY
{
    if($text=~/Hail/i)
    {
        quest::say("Greetings. I am Puab Closk, Master of the Ashen Order.  Our home is your home, friend. Feel free to stay as long as you like.  Learn our ways as many have done in the past.  To fight like the tiger and strike like the cobra are your goals.");
    }
}

sub EVENT_ITEM
{
    # A tattered note ID-18746
    if (plugin::check_handin(\%itemcount, 18746 => 1))
    {
        quest::say("Welcome to the house of the Ashen Order. We are a small guild of monks who have devoted our lives to refining our minds, souls, and physical beings to their maximum potential. Please see Brother Torresk as soon as you get a chance. He is in charge of helping our newer members begin their training. Good luck, $name.");
        # Torn Cloth Tunic ID-13507
        quest::summonitem("13507");
        # Ashen Order Faction ID-12
        quest::faction("12","300");
        # Knights of Truth Faction ID-184
        quest::faction("184","2");
        # Silent Fist Clan Faction ID-300
        quest::faction("300","2");
        quest::exp("100");
    }

    #do all other handins first with plugin, then let it do disciplines
    plugin::try_tome_handins(\%itemcount, $class, 'Monk');
    plugin::return_items(\%itemcount);
}
#END of FILE Zone:freportw  ID:9086 -- Puab_Closk


