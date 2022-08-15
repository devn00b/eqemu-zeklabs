sub EVENT_DEATH {


if ( defined $npc && $npc->GetOwnerID() > 0 )
{
return;
}
#setup and 0 out vars
$random_result = 0;
$random_res = 0;
$shoutrand = 0;

## For TESTING ONLY!! THIS CAUSES IT TO SPAWN EVERY TIME
#$random_res = 1;
#$random_result = 1;   # 1 - For image // 51 - for devnoob
## DISABLE FOR LIVE!! DISABLE FOR LIVE!! DISABLE FOR LIVE!! DISABLE FOR LIVE!! DISABLE FOR LIVE!! DISABLE FOR LIVE!!


##Who are we going to spawn? <50 image >50 devn00b
$random_result = int(rand(100));

##For Season 3 Starting with 10k Makes the chance to spawn rather low. Doing this to make people stay in zone for a while
##Also slows them down since they were doing whole zone pulls before. ** 700 spawns pretty fast increasing to 800 on 5.25.17
$random_res = int(rand(800));

##This picks the shout used
$shoutrand = int(rand(4));

if ( $random_res<10) {
  if( $random_result<50 ){  
    #image
    quest::unique_spawn(999476,999476,0,152.6,1952.3,419.5,0);

	if($shoutrand == 0) {
	quest::shout2("It appears Image's tea shop is open in Scarlet, but beware he beheaded his last customer!");
	}
	elsif($shoutrand == 1) {
	quest::shout2("All appears calm in scarlet. And there is a smell of tea in the air.");
	}
	elsif($shoutrand == 2) {
	quest::shout2("You idiot $name ! Now Image will get you! Cant you smell the tea???");
	}
	elsif($shoutrand == 3) {
	quest::shout2("Because of you ".$name.", Image has come, and will bring calmness to the land! ");
	}
	  else {
	    quest::shout2("Image has spawned");
	  }


   }
	    elsif( $random_result>50 ){  
	    #devn00b
    	       quest::unique_spawn(999477,999476,0,141.4,1944.6,420.1,0);

	if($shoutrand == 0) {
	quest::shout2("Devnoob has appeared in a cloud of reefer smoke! Scarlet will suffer!");
	}
	elsif($shoutrand == 1) {
	quest::shout2("There is a thick cloud forming in scarlet. It smells like, some kind of indica.");
	}
	elsif($shoutrand == 2) {
	quest::shout2("Well done $name now there is going to be some kind of hippy jam band festival. And I HATE HIPPIES");
	}
	elsif($shoutrand == 3) {
	quest::shout2("Let it be known that $name has caused scarlet to become...hahahah.hehehehhe..uhhh...nevermind....");
	}
	    else {
	    quest::shout2("Devnoob has spawned!");
            }


    }
 else {

   }
  }
}
