sub createBreadCrumb {
my $client = shift;
my $x = shift;
my $y = shift;

$rndItem = quest::ChooseRandom(142827,142827,142827,142827,142827,142827,132487,132487,142828,142829,142830,142828,142829,142830);

if ( !defined $x )
{
#$client->Message(14,"Something drops at your feet: $x, $y.");
quest::creategroundobject($rndItem, $client->GetX(), $client->GetY(), $client->GetZ(), $client->GetHeading(), 300000, 0, "akanon_race_playerflag");
}
else
{
$diffx = $x - int($client->GetX());
$diffy = $y - int($client->GetY());

if ( $diffx < 0 ) { $diffx = -$diffx; }
if ( $diffy < 0 ) { $diffy = -$diffy; }

if ( $diffx > 20 || $diffy > 20 )
{
#$client->Message(14,"Something drops at your feet: $x, $y.");
quest::creategroundobject($rndItem, $client->GetX(), $client->GetY(), $client->GetZ(), $client->GetHeading(), 300000, 0, "akanon_race_playerflag");
}
}
}

sub GnomeRaceEventTimer {
    my $client   = plugin::val('$client');
    my $qglobals = plugin::var('qglobals');
    my $timer    = shift;
    if ( $timer eq "drunk" ) {
        if ( defined $qglobals->{akanon_race_playerflag}
            && $client->GetIntoxication() > 0 )
        {
            $client->CastSpell( 8558, 0, 10, 0, 0 );
        }
    }
    elsif ( $timer eq "breadcrumb"
        && defined $qglobals->{akanon_race_playerflag} )
    {
        $guild = $client->GuildID();
        if ( $guild > 0 && $guild < 10000 ) {
            quest::setguild( -1, 0 );
            $client->Message( 13,
                "You cannot remain guilded during the event." );
        }
        $curx = $qglobals->{akanon_race_breadcrumbx};
        $cury = $qglobals->{akanon_race_breadcrumby};
        plugin::createBreadCrumb( $client, $curx, $cury );
        quest::setglobal( "akanon_race_breadcrumbx", int( $client->GetX() ),
            5, "M1" );
        quest::setglobal( "akanon_race_breadcrumby", int( $client->GetY() ),
            5, "M1" );
    }    #end breadcrumb
}

sub GnomeRaceEnterZone {
    my $client   = plugin::val('$client');
    my $qglobals = plugin::var('qglobals');

    if (   defined $qglobals->{akanon_race_notinterested}
        || defined $qglobals->{akanon_race_endtime}
        || !defined $qglobals->{akanon_racestarted}
        || !defined $qglobals->{akanon_race_playerflag} )
    {
        if ( $client->HasItem(142757) ) {
            $client->NukeItem(142757);
        }
        if ( $client->HasItem(142755) ) {
            $client->NukeItem(142755);
        }
    }
    if ( defined $qglobals->{akanon_racefinish_playerflag}
        && $qglobals->{akanon_racefinish_playerreset} != "3" )
    {
        quest::delglobal("notranslocateallowed");
        quest::delglobal("noteleportallowed");
        quest::delglobal("nogroupallowedflag");
        quest::delglobal("nobeneficialbuffsallowed");
        quest::delglobal("nobindallowed");
        quest::delglobal("notradeallowed");
        quest::delglobal("akanon_race_playerflag");
        quest::setglobal( "akanon_racefinish_playerreset", "3", 5, "F" );
        $client->Message( 14, "Reset player flags." );
        return;
    }
    my $zoneid = plugin::val('$zoneid');

    if ( $zoneid == 55 ) {
        quest::settimer( "akanonrace", 10 );
    }

    if ( defined( $qglobals->{akanon_race_playerflag} ) && $zoneid != 152 && $zoneid != 151 ) {
        quest::settimer( "drunk",      180 );
        quest::settimer( "breadcrumb", 5 );
    }
}