sub EVENT_COMBAT {

if ( $combat_state == 1 ) 
my $max = 4;
my $min = 1;
my $rnd = int(rand($max)) + $min;
if ( $rnd > 2 ) { quest:;say("You are most certainly not welcome here stranger.  This chamber is sacred to our Master!"); }
}

}