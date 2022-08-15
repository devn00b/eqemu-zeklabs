


#ifndef RULE_CATEGORY
#define RULE_CATEGORY(name)
#endif
#ifndef RULE_INT
#define RULE_INT(cat, rule, default_value)
#endif
#ifndef RULE_REAL
#define RULE_REAL(cat, rule, default_value)
#endif
#ifndef RULE_BOOL
#define RULE_BOOL(cat, rule, default_value)
#endif
#ifndef RULE_CATEGORY_END
#define RULE_CATEGORY_END()
#endif




RULE_CATEGORY( Character )
RULE_INT ( Character, MaxLevel, 65 )
RULE_INT ( Character, MaxExpLevel, 0 ) //Sets the Max Level attainable via Experience
RULE_INT ( Character, DeathExpLossLevel, 10 )	// Any level greater than this will lose exp on death
RULE_INT ( Character, DeathExpLossMaxLevel, 255 )	// Any level greater than this will no longer lose exp on death
RULE_INT ( Character, DeathItemLossLevel, 10 )
RULE_INT ( Character, DeathExpLossMultiplier, 3) //Adjust how much exp is lost
RULE_BOOL( Character, UseDeathExpLossMult, false ) //Adjust to use the above multiplier or to use code default.
RULE_INT ( Character, CorpseDecayTimeMS, 10800000 )
RULE_INT ( Character, CorpseResTimeMS, 10800000 ) // time before cant res corpse(3 hours)
RULE_BOOL( Character, LeaveCorpses, true )
RULE_BOOL( Character, LeaveNakedCorpses, false )
RULE_INT ( Character, MaxDraggedCorpses, 2 )
RULE_REAL( Character, DragCorpseDistance, 400) // If the corpse is <= this distance from the player, it won't move
RULE_REAL( Character, ExpMultiplier, 0.5 )
RULE_REAL( Character, AAExpMultiplier, 0.5 )
RULE_REAL( Character, GroupExpMultiplier, 0.5 )
RULE_REAL( Character, RaidExpMultiplier, 0.2 )
RULE_BOOL( Character, UseXPConScaling, true )
RULE_INT ( Character, LightBlueModifier, 40 ) 
RULE_INT ( Character, BlueModifier, 90 )
RULE_INT ( Character, WhiteModifier, 100 )
RULE_INT ( Character, YellowModifier, 125 )
RULE_INT ( Character, RedModifier, 150 ) 
RULE_INT ( Character, AutosaveIntervalS, 300 )	//0=disabled
RULE_INT ( Character, HPRegenMultiplier, 100)
RULE_INT ( Character, ManaRegenMultiplier, 100)
RULE_INT ( Character, EnduranceRegenMultiplier, 100)
RULE_INT ( Character, ConsumptionMultiplier, 100) //item's hunger restored = this value * item's food level, 100 = normal, 50 = people eat 2x as fast, 200 = people eat 2x as slow
RULE_BOOL( Character, HealOnLevel, false)
RULE_BOOL( Character, FeignKillsPet, false)
RULE_INT ( Character, ItemManaRegenCap, 15)
RULE_INT ( Character, ItemHealthRegenCap, 35)
RULE_INT ( Character, ItemDamageShieldCap, 30)
RULE_INT ( Character, ItemAccuracyCap, 150)
RULE_INT ( Character, ItemAvoidanceCap, 100)
RULE_INT ( Character, ItemCombatEffectsCap, 100)
RULE_INT ( Character, ItemShieldingCap, 35)
RULE_INT ( Character, ItemSpellShieldingCap, 35)
RULE_INT ( Character, ItemDoTShieldingCap, 35)
RULE_INT ( Character, ItemStunResistCap, 35)
RULE_INT ( Character, ItemStrikethroughCap, 35)
RULE_INT ( Character, ItemATKCap, 250)
RULE_INT ( Character, ItemHealAmtCap, 250)
RULE_INT ( Character, ItemSpellDmgCap, 250)
RULE_INT ( Character, ItemClairvoyanceCap, 250)
RULE_INT ( Character, ItemDSMitigationCap, 50)
RULE_INT ( Character, ItemEnduranceRegenCap, 15)
RULE_INT ( Character, HasteCap, 100) // Haste cap for non-v3(overhaste) haste.
RULE_INT ( Character, SkillUpModifier, 100) //skill ups are at 100%
RULE_BOOL ( Character, SharedBankPlat, false) //off by default to prevent duping for now
RULE_BOOL ( Character, BindAnywhere, false)
RULE_INT ( Character, RestRegenPercent, 0) // Set to >0 to enable rest state bonus HP and mana regen.
RULE_INT ( Character, RestRegenTimeToActivate, 30) // Time in seconds for rest state regen to kick in.
RULE_BOOL ( Character, RestRegenEndurance, false) // Whether rest regen will work for endurance or not.
RULE_INT ( Character, KillsPerGroupLeadershipAA, 250) // Number of dark blues or above per Group Leadership AA
RULE_INT ( Character, KillsPerRaidLeadershipAA, 250) // Number of dark blues or above per Raid Leadership AA
RULE_INT ( Character, MaxFearDurationForPlayerCharacter, 4) //4 tics, each tic calculates every 6 seconds.
RULE_INT ( Character, MaxCharmDurationForPlayerCharacter, 15)
RULE_INT ( Character, BaseHPRegenBonusRaces, 4352)	//a bitmask of race(s) that receive the regen bonus. Iksar (4096) & Troll (256) = 4352. see common/races.h for the bitmask values
RULE_BOOL ( Character, SoDClientUseSoDHPManaEnd, false)	// Setting this to true will allow SoD clients to use the SoD HP/Mana/End formulas and previous clients will use the old formulas
RULE_BOOL ( Character, UseRaceClassExpBonuses, true)	// Setting this to true will enable Class and Racial experience rate bonuses
RULE_BOOL ( Character, RespawnFromHover, false)		// Use Respawn window, or not.
RULE_INT ( Character, RespawnFromHoverTimer, 300)	// Respawn Window countdown timer, in SECONDS
RULE_BOOL ( Character, UseNewStatsWindow, true) 	// New stats window shows everything
RULE_BOOL ( Character, ItemCastsUseFocus, false) // If true, this allows item clickies to use focuses that have limited max levels on them
RULE_BOOL ( Character, ClientDeathToNPCLeaveCorpse, false) // leave a corpse on a npc death
RULE_INT ( Character, ZoneInTimer, 1000) // time for invul
RULE_BOOL ( Character, AllowBindAtZone, true)
RULE_REAL( Character, ReqBindDistanceFromZonePoint, 200.0 )
RULE_BOOL ( Character, ZoneInLinkdead, false)
RULE_INT ( Character, ZoneInLinkdeadTimer, 30000) // keep at 30000 otherwise this will be the max time the client will stay in
RULE_BOOL ( Character, ZoneInLDShowFlag, false)
RULE_BOOL ( Character, AutoAssignSpells, false)
RULE_INT ( Character, AutoAssignSpellMaxLevel, 50)
RULE_INT ( Character, AutoAssignSkillLevel, 5)
RULE_BOOL ( Character, AutoAssignSpellCost, false)
RULE_BOOL( Character, BlockSecondarySlotItemSwap, true )
RULE_BOOL( Character, LogBandolierActivity, false )
RULE_CATEGORY_END()

RULE_CATEGORY( Guild )
RULE_INT ( Guild, MaxMembers, 2048 )
RULE_INT ( Guild, MinimumTimeInGuildSec, 1209600 ) // 14 days
RULE_BOOL ( Guild, ShowGuildMembersZone, true )
RULE_CATEGORY_END()

RULE_CATEGORY( Skills )
RULE_INT ( Skills, MaxTrainTradeskills, 21 )
RULE_BOOL ( Skills, UseLimitTradeskillSearchSkillDiff, true )
RULE_INT ( Skills, MaxTradeskillSearchSkillDiff, 50 )
RULE_CATEGORY_END()

RULE_CATEGORY( Pets )
RULE_REAL( Pets, AttackCommandRange, 150 )
RULE_REAL( Pets, AttackCommandRangeWithLOS, 400 )
RULE_BOOL( Pets, OwnerAttackLogic, true )
RULE_BOOL( Pets, AttackLogicClientOnly, false )
RULE_CATEGORY_END()

RULE_CATEGORY( GM )
RULE_INT ( GM, MinStatusToZoneAnywhere, 250 )
RULE_CATEGORY_END()

RULE_CATEGORY( World )
RULE_INT ( World, NoTransferZoneExemption, 183 )
RULE_INT ( World, ZoneAutobootTimeoutMS, 60000 )
RULE_INT ( World, ClientKeepaliveTimeoutMS, 65000 )
RULE_BOOL ( World, UseBannedIPsTable, false ) // Toggle whether or not to check incoming client connections against the Banned_IPs table. Set this value to false to disable this feature.
RULE_BOOL ( World, EnableTutorialButton, true)
RULE_BOOL ( World, EnableReturnHomeButton, true)
RULE_INT  ( World, MaxLevelForTutorial, 10)
RULE_INT  ( World, TutorialZoneID, 189)
RULE_INT  ( World, GuildBankZoneID, 345)
RULE_INT  ( World, MinOfflineTimeToReturnHome, 21600) // 21600 seconds is 6 Hours
RULE_INT ( World, MaxClientsPerIP, -1 ) // Maximum number of clients allowed to connect per IP address if account status is < AddMaxClientsStatus.  Default value: -1 (feature disabled)
RULE_INT ( World, ExemptMaxClientsStatus, -1 ) // Exempt accounts from the MaxClientsPerIP and AddMaxClientsStatus rules, if their status is >= this value.  Default value: -1 (feature disabled)
RULE_INT ( World, AddMaxClientsPerIP, -1 ) // Maximum number of clients allowed to connect per IP address if account status is < ExemptMaxClientsStatus.  Default value: -1 (feature disabled)
RULE_INT ( World, AddMaxClientsStatus, -1 ) // Accounts with status >= this rule will be allowed to use the amount of accounts defined in the AddMaxClientsPerIP.  Default value: -1 (feature disabled)
RULE_BOOL ( World, MaxClientsSetByStatus, false) // If True, IP Limiting will be set to the status on the account as long as the status is > MaxClientsPerIP
RULE_BOOL ( World, ClearTempMerchantlist, true) // Clears temp merchant items when world boots.
RULE_INT ( World, AccountSessionLimit, -1 )  //Max number of characters allowed on at once from a single account (-1 is disabled)
RULE_INT ( World, ExemptAccountLimitStatus, -1 )  //Min status required to be exempt from multi-session per account limiting (-1 is disabled)
RULE_BOOL ( World, GMAccountIPList, false) // Check ip list against GM Accounts, AntiHack GM Accounts.
RULE_INT ( World, MinGMAntiHackStatus, 1 ) //Minimum GM status to check against AntiHack list
RULE_INT ( World, SoFStartZoneID, -1 ) //Sets the Starting Zone for SoF Clients separate from Titanium Clients (-1 is disabled)
RULE_INT ( World, WhoAllZoneRequestMS, 60000 ) // limits the time for each zone request by a player with no admin status
RULE_BOOL ( World, ForceCRCCheck, true ) // forces on a specific CRC check with the client
RULE_INT ( World, CRCCheck, 3617432954) // CRC check in integer value
RULE_INT ( World, MinLevelCheckBypass, 0) // anything above this level will not be allowed on
RULE_INT ( World, BaseTitaniumCRC, 2652224660) // CRC check in integer value

RULE_INT ( World, MaxMessageQueue, 4) // max messages in tell queue (number+1)
RULE_BOOL ( World, Bounties, true) // bounties enabled
RULE_BOOL ( World, GiveReward, true) // give veteran reward
RULE_INT ( World, VeteranRewardID, 2) // reward id
RULE_INT ( World, VeteranRewardCount, 2) // reward count
RULE_INT ( World, ServerType, 0) // our default was originally '2' for colors
RULE_INT ( World, CharacterDefaultPVPPoints, 0)

RULE_BOOL ( World, AllowIksars, true)
RULE_BOOL ( World, AllowVahShir, false)
RULE_BOOL ( World, AllowFroglok, false)
RULE_BOOL ( World, AllowBeastlord, false)
RULE_BOOL ( World, AllowBerserker, false)

RULE_BOOL ( World, EarlyAccessRequired, false) // if enabled to true requires optionals account flag of '1'
RULE_CATEGORY_END()

RULE_CATEGORY( Zone )
RULE_INT ( Zone,  NPCPositonUpdateTicCount, 32 ) //ms between intervals of sending a position update to the entire zone.
RULE_INT ( Zone,  ClientLinkdeadMS, 180000) //the time a client remains link dead on the server after a sudden disconnection
RULE_INT ( Zone,  GraveyardTimeMS, 1200000) //ms time until a player corpse is moved to a zone's graveyard, if one is specified for the zone
RULE_BOOL ( Zone, EnableShadowrest, 1 ) // enables or disables the shadowrest zone feature for player corpses. Default is turned on.
RULE_INT ( Zone, MQWarpExemptStatus, -1 ) // Required status level to exempt the MQWarpDetector.  Set to -1 to disable this feature.
RULE_INT ( Zone, MQZoneExemptStatus, -1 ) // Required status level to exempt the MQZoneDetector.  Set to -1 to disable this feature.
RULE_INT ( Zone, MQGateExemptStatus, -1 ) // Required status level to exempt the MQGateDetector.  Set to -1 to disable this feature.
RULE_INT ( Zone, MQGhostExemptStatus, -1 ) // Required status level to exempt the MGhostDetector.  Set to -1 to disable this feature.
RULE_BOOL ( Zone, EnableMQWarpDetector, true ) // Enable the MQWarp Detector.  Set to False to disable this feature.
RULE_BOOL ( Zone, EnableMQZoneDetector, true ) // Enable the MQZone Detector.  Set to False to disable this feature.
RULE_BOOL ( Zone, EnableMQGateDetector, true ) // Enable the MQGate Detector.  Set to False to disable this feature.
RULE_BOOL ( Zone, EnableMQGhostDetector, true ) // Enable the MQGhost Detector.  Set to False to disable this feature.
RULE_REAL ( Zone, MQWarpDetectionDistanceFactor, 9.0) //clients move at 4.4 about if in a straight line but with movement and to acct for lag we raise it a bit
RULE_BOOL ( Zone, MarkMQWarpLT, false )
RULE_INT ( Zone, AutoShutdownDelay, 5000 ) //How long a dynamic zone stays loaded while empty
RULE_INT ( Zone, PEQZoneReuseTime, 900 )	//How long, in seconds, until you can reuse the #peqzone command.
RULE_INT ( Zone, PEQZoneDebuff1, 4454 )	    //First debuff casted by #peqzone Default is Cursed Keeper's Blight.
RULE_INT ( Zone, PEQZoneDebuff2, 2209  )	//Second debuff casted by #peqzone Default is Tendrils of Apathy.
RULE_BOOL ( Zone, UsePEQZoneDebuffs, true )	//Will determine if #peqzone will debuff players or not when used.
RULE_REAL ( Zone, HotZoneBonus, 0.75 )
RULE_INT ( Zone, ReservedInstances, 30 ) //Will reserve this many instance ids for globals... probably not a good idea to change this while a server is running.
RULE_BOOL ( Zone, RestrictPositionUpdates, false )	//Uses a more reserved position update for pvp servers.

RULE_BOOL ( Zone, DeathRespawnMod, true )	//used for speeding up respawn time
RULE_REAL ( Zone, DeathRespawnLogBase, 1.5 )
RULE_REAL ( Zone, DeathRespawnGraphBase, .25 )

RULE_BOOL ( Zone, DeathSpawnNPCTypeMod, true )	// used for rarer npc types to spawn more frequently
RULE_REAL ( Zone, DeathSpawnNPCLogBase, 5 )
RULE_REAL ( Zone, DeathSpawnNPCGraphBase, .5 )
RULE_INT ( Zone, DeathSpawnChanceBelow, 40 ) // npc type probability to spawn less than this value we will boost

RULE_BOOL ( Zone, LootMod, true ) // used for allowing rare loots to drop more often
RULE_REAL ( Zone, LootModLogBase, 5 ) // base added to log(x+thisvalue)
RULE_REAL ( Zone, LootModGraphBase, .5 ) // end subtraction log(x) - thisvalue
RULE_INT ( Zone, LootModChanceBelow, 25 ) // loot item probability to spawn less than this value we will boost
RULE_INT ( Zone, LootModChanceAbove, 50 ) // loot item probability higher than what we want, reduce it
RULE_REAL ( Zone, LootModAboveLogBase, 10 ) // base added to log(x+thisvalue)
RULE_INT ( Zone, LootModMaxMultiplier, 4 ) // 4x the original chance is the max
RULE_REAL ( Zone, LootModMinMultiplier, .5 ) // 4x the original chance is the max

RULE_INT ( Zone, CorrectSpawnDropChance, 2 )
RULE_INT ( Zone, CorrectSpawnMaxRandom, 5 )

RULE_INT ( Zone, CorrectLootDropChance, 2 )
RULE_INT ( Zone, CorrectLootMaxRandom, 5 )

RULE_INT ( Zone, WipeSpawnDeathCountMinutes, 360 ) // 6 hrs

RULE_BOOL ( Zone, PositionUpdateDebug, false )
RULE_INT ( Zone, PositionUpdateIdleTimer, 3000 )
RULE_INT ( Zone, EyeOfZommNPCID, 644 ) // npc type 644 for SummonEye

RULE_INT ( Zone, PositionUpdateDistance, 1350 ) // standard distance check for position updates
RULE_INT ( Zone, ShadowStepPositionUpdateDistance, 2700 ) // distance check for shadow step position updates
RULE_BOOL ( Zone, AllowDisarm, false )
RULE_BOOL ( Zone, RestrictDamagePackets, false )
RULE_BOOL ( Zone, AllowClientZoning, false )
RULE_CATEGORY_END()

RULE_CATEGORY( Map )
//enable these to help prevent mob hopping when they are pathing
RULE_BOOL ( Map, FixPathingZWhenLoading, true )		//increases zone boot times a bit to reduce hopping.
RULE_BOOL ( Map, FixPathingZAtWaypoints, false )	//alternative to `WhenLoading`, accomplishes the same thing but does it at each waypoint instead of once at boot time.
RULE_BOOL ( Map, FixPathingZWhenMoving, false )		//very CPU intensive, but helps hopping with widely spaced waypoints.
RULE_BOOL ( Map, FixPathingZOnSendTo, false )		//try to repair Z coords in the SendTo routine as well.
RULE_REAL ( Map, FixPathingZMaxDeltaMoving, 20 )	//at runtime while pathing: max change in Z to allow the BestZ code to apply.
RULE_REAL ( Map, FixPathingZMaxDeltaWaypoint, 20 )	//at runtime at each waypoint: max change in Z to allow the BestZ code to apply.
RULE_REAL ( Map, FixPathingZMaxDeltaSendTo, 20 )	//at runtime in SendTo: max change in Z to allow the BestZ code to apply.
RULE_REAL ( Map, FixPathingZMaxDeltaLoading, 45 )	//while loading each waypoint: max change in Z to allow the BestZ code to apply.
RULE_BOOL ( Map, UseClosestZ, false)			// Move mobs to the nearest Z above or below, rather than just the nearest below.
							// Only set UseClosestZ true if all your .map files generated from EQGs were created
							// with azone2.
							//
RULE_INT  ( Map, FindBestZHeightAdjust, 1)		// Adds this to the current Z before seeking the best Z position
RULE_CATEGORY_END()

RULE_CATEGORY( Pathing )
// Some of these rules may benefit by being made into columns in the zone table,
// for instance, in dungeons, the min LOS distances could be substantially lowered.
RULE_BOOL ( Pathing, Aggro, true )		// Enable pathing for aggroed mobs.
RULE_BOOL ( Pathing, AggroReturnToGrid, true )	// Enable pathing for aggroed roaming mobs returning to their previous waypoint.
RULE_BOOL ( Pathing, Guard, true )		// Enable pathing for mobs moving to their guard point.
RULE_BOOL ( Pathing, Find, true )		// Enable pathing for FindPerson requests from the client.
RULE_BOOL ( Pathing, Fear, true )		// Enable pathing for fear
RULE_REAL ( Pathing, ZDiffThreshold, 10)	// If a mob las LOS to it's target, it will run to it if the Z difference is < this.
RULE_INT  ( Pathing, LOSCheckFrequency, 1000)	// A mob will check for LOS to it's target this often (milliseconds).
RULE_INT  ( Pathing, RouteUpdateFrequencyShort, 1000)	// How often a new route will be calculated if the target has moved.
RULE_INT  ( Pathing, RouteUpdateFrequencyLong, 5000)	// How often a new route will be calculated if the target has moved.
// When a path has a path node route and it's target changes position, if it has RouteUpdateFrequencyNodeCount or less nodes to go on it's
// current path, it will recalculate it's path based on the RouteUpdateFrequencyShort timer, otherwise it will use the
// RouteUpdateFrequencyLong timer.
RULE_INT  ( Pathing, RouteUpdateFrequencyNodeCount, 5) 
RULE_REAL ( Pathing, MinDistanceForLOSCheckShort, 40000) // (NoRoot). While following a path, only check for LOS to target within this distance.
RULE_REAL ( Pathing, MinDistanceForLOSCheckLong, 1000000) // (NoRoot). Min distance when initially attempting to acquire the target.
RULE_INT  ( Pathing, MinNodesLeftForLOSCheck, 4)	// Only check for LOS when we are down to this many path nodes left to run.
// This next rule was put in for situations where the mob and it's target may be on different sides of a 'hazard', e.g. a pit
// If the mob has LOS to it's target, even though there is a hazard in it's way, it may break off from the node path and run at
// the target, only to later detect the hazard and re-acquire a node path. Depending upon the placement of the path nodes, this
// can lead to the mob looping. The rule is intended to allow the mob to at least get closer to it's target each time before
// checking LOS and trying to head straight for it.
RULE_INT  ( Pathing, MinNodesTraversedForLOSCheck, 3)	// Only check for LOS after we have traversed this many path nodes.
RULE_INT  ( Pathing, CullNodesFromStart, 1)		// Checks LOS from Start point to second node for this many nodes and removes first node if there is LOS
RULE_INT  ( Pathing, CullNodesFromEnd, 1)		// Checks LOS from End point to second to last node for this many nodes and removes last node if there is LOS
RULE_REAL ( Pathing, CandidateNodeRangeXY, 400)		// When searching for path start/end nodes, only nodes within this range will be considered.
RULE_REAL ( Pathing, CandidateNodeRangeZ, 10)		// When searching for path start/end nodes, only nodes within this range will be considered.
RULE_BOOL ( Pathing, PrintPathing, false)
RULE_CATEGORY_END()

RULE_CATEGORY( Watermap )
// enable these to use the water detection code. Requires Water Maps generated by awater utility
RULE_BOOL ( Watermap, CheckWaypointsInWaterWhenLoading, false ) // Does not apply BestZ as waypoints are loaded if they are in water
RULE_BOOL ( Watermap, CheckForWaterAtWaypoints, false) 		// Check if a mob has moved into/out of water when at waypoints and sets flymode
RULE_BOOL ( Watermap, CheckForWaterWhenMoving, false)		// Checks if a mob has moved into/out of water each time it's loc is recalculated
RULE_BOOL ( Watermap, CheckForWaterOnSendTo, false)		// Checks if a mob has moved into/out of water on SendTo
RULE_BOOL ( Watermap, CheckForWaterWhenFishing, false)		// Only lets a player fish near water (if a water map exists for the zone)
RULE_REAL ( Watermap, FishingRodLength, 30)			// How far in front of player water must be for fishing to work
RULE_REAL ( Watermap, FishingLineLength, 40)			// If water is more than this far below the player, it is considered too far to fish
RULE_CATEGORY_END()

RULE_CATEGORY( Spells )
RULE_BOOL ( Spells, SharedCounters, false)	// if we subtract from the counters or not
RULE_INT ( Spells, MinCastTimeReduction, 3000) // min cast time that we allow
RULE_INT ( Spells, MaxCastReduction, 30) // percentage of max cast reduction
RULE_INT ( Spells, AutoResistDiff, 15)
RULE_REAL ( Spells, ResistChance, 2.0) //chance to resist given no resists and same level
RULE_REAL ( Spells, ResistMod, 0.40) //multiplier, chance to resist = this * ResistAmount
RULE_REAL ( Spells, PartialHitChance, 0.7) //The chance when a spell is resisted that it will partial hit.
RULE_REAL ( Spells, PartialHitChanceFear, 0.25) //The chance when a fear spell is resisted that it will partial hit.
RULE_INT ( Spells, BaseCritChance, 0) //base % chance that everyone has to crit a spell
RULE_INT ( Spells, BaseCritRatio, 100) //base % bonus to damage on a successful spell crit. 100 = 2x damage
RULE_INT ( Spells, WizCritLevel, 12) //level wizards first get spell crits
RULE_INT ( Spells, WizCritChance, 7) //wiz's crit chance, on top of BaseCritChance
RULE_INT ( Spells, WizCritRatio, 0) //wiz's crit bonus, on top of BaseCritRatio (should be 0 for Live-like)
RULE_INT ( Spells, ResistPerLevelDiff, 85) //8.5 resist per level difference.
RULE_INT ( Spells, TranslocateTimeLimit, 0) // If not zero, time in seconds to accept a Translocate.
RULE_INT ( Spells, SacrificeMinLevel, 46)	//first level Sacrifice will work on
RULE_INT ( Spells, SacrificeMaxLevel, 69)	//last level Sacrifice will work on
RULE_INT ( Spells, SacrificeItemID, 9963)	//Item ID of the item Sacrifice will return (defaults to an EE)
RULE_BOOL ( Spells, EnableSpellGlobals, false)	// If Enabled, spells check the spell_globals table and compare character data from the quest globals before allowing that spell to scribe with scribespells
RULE_INT ( Spells, MaxBuffSlotsNPC, 25)
RULE_INT ( Spells, MaxSongSlotsNPC, 10)
RULE_INT ( Spells, MaxDiscSlotsNPC, 1)
RULE_INT ( Spells, MaxTotalSlotsNPC, 36)
RULE_BOOL (Spells, EnableBlockedBuffs, true)
RULE_INT ( Spells, ReflectType, 1) //0 = disabled, 1 = single target player spells only, 2 = all player spells, 3 = all single target spells, 4 = all spells
RULE_INT ( Spells, VirusSpreadDistance, 30) // The distance a viral spell will jump to its next victim
RULE_CATEGORY_END()

RULE_CATEGORY( Combat )
RULE_INT ( Combat, MeleeBaseCritChance, 0 ) //The base crit chance for non warriors, NOTE: This will apply to NPCs as well
RULE_INT ( Combat, WarBerBaseCritChance, 3 ) //The base crit chance for warriors and berserkers, only applies to clients
RULE_INT ( Combat, BerserkBaseCritChance, 6 ) //The bonus base crit chance you get when you're berserk
RULE_INT ( Combat, WarBaseCripplingBlowChance, 0 ) //the crippling blow chance (Added) as default to items/spells
RULE_INT ( Combat, WarBaseCripplingBlowChanceNPC, 0 ) //the crippling blow chance (Added) as default to items/spells
RULE_INT ( Combat, BerserkHPRatio, 30 ) //The hp ratio you are at when you go berzerk
RULE_INT ( Combat, NPCBashKickLevel, 6 ) //The level that npcs can KICK/BASH
RULE_INT ( Combat, NPCBashKickStunChance, 15 ) //Percent chance that a bash/kick will stun
RULE_INT ( Combat, ClientBaseCritChance, 0 ) //The base crit chance for all clients, this will stack with warrior's/zerker's crit chance.
RULE_BOOL ( Combat, UseIntervalAC, true)
RULE_INT ( Combat, PetAttackMagicLevel, 30)
RULE_BOOL ( Combat, EnableFearPathing, true)
RULE_INT ( Combat, FleeHPRatio, 25)
RULE_INT ( Combat, FleeSnareHPRatio, 11) // HP at which snare will halt movement of a fleeing NPC.
RULE_BOOL ( Combat, FleeIfNotAlone, false) // If false, mobs won't flee if other mobs are in combat with it.
RULE_BOOL ( Combat, AdjustProcPerMinute, true)
RULE_REAL ( Combat, AvgProcsPerMinute, 2.0)
RULE_REAL ( Combat, ProcPerMinDexContrib, 0.075)
RULE_REAL ( Combat, BaseProcChance, 0.035)
RULE_REAL ( Combat, ProcDexDivideBy, 11000)
RULE_REAL ( Combat, BaseHitChance, 69.0)
RULE_REAL ( Combat, NPCBonusHitChance, 26.0)
RULE_REAL ( Combat, HitFalloffMinor, 5.0) //hit will fall off up to 5% over the initial level range
RULE_REAL ( Combat, HitFalloffModerate, 7.0) //hit will fall off up to 7% over the three levels after the initial level range
RULE_REAL ( Combat, HitFalloffMajor, 50.0) //hit will fall off sharply if we're outside the minor and moderate range
RULE_REAL ( Combat, HitBonusPerLevel, 1.2) //You gain this % of hit for every level you are above your target
RULE_REAL ( Combat, WeaponSkillFalloff, 0.33) //For every weapon skill point that's not maxed you lose this % of hit
RULE_REAL ( Combat, ArcheryHitPenalty, 0.25) //Archery has a hit penalty to try to help balance it with the plethora of long term +hit modifiers for it
RULE_REAL ( Combat, AgiHitFactor, 0.01) 
RULE_INT ( Combat, MinRangedAttackDist, 25) //Minimum Distance to use Ranged Attacks
RULE_BOOL ( Combat, ArcheryBonusRequiresStationary, true) //does the 2x archery bonus chance require a stationary npc
RULE_REAL ( Combat, ArcheryBaseDamageBonus, 1) // % Modifier to Base Archery Damage (.5 = 50% base damage, 1 = 100%, 2 = 200%)
RULE_REAL ( Combat, ArcheryNPCMultiplier, 1.0) // this is multiplied by the regular dmg to get the archery dmg
RULE_BOOL ( Combat, AssistNoTargetSelf, true)  //when assisting a target that does not have a target: true = target self, false = leave target as was before assist (false = live like)
RULE_INT ( Combat, MaxRampageTargets, 3) //max number of people hit with rampage
RULE_INT ( Combat, MaxFlurryHits, 2) //max number of extra hits from flurry
RULE_INT ( Combat, MonkDamageTableBonus, 5) //% bonus monks get to their damage table calcs
RULE_INT ( Combat, FlyingKickBonus, 25) //% Modifier that this skill gets to str and skill bonuses
RULE_INT ( Combat, DragonPunchBonus, 20) //% Modifier that this skill gets to str and skill bonuses
RULE_INT ( Combat, EagleStrikeBonus, 15) //% Modifier that this skill gets to str and skill bonuses
RULE_INT ( Combat, TigerClawBonus, 10) //% Modifier that this skill gets to str and skill bonuses
RULE_INT ( Combat, RoundKickBonus, 5) //% Modifier that this skill gets to str and skill bonuses
RULE_BOOL ( Combat, ProcTargetOnly, true) //true = procs will only affect our target, false = procs will affect all of our targets
RULE_REAL ( Combat, NPCACFactor, 2.25)
RULE_INT ( Combat, ClothACSoftcap, 75)
RULE_INT ( Combat, LeatherACSoftcap, 100)
RULE_INT ( Combat, MonkACSoftcap, 120)
RULE_INT ( Combat, ChainACSoftcap, 200)
RULE_INT ( Combat, PlateACSoftcap, 300)
RULE_REAL ( Combat, AAMitigationACFactor, 3.0)
RULE_REAL ( Combat, WarriorACSoftcapReturn, 0.45)
RULE_REAL ( Combat, KnightACSoftcapReturn, 0.33)
RULE_REAL ( Combat, LowPlateChainACSoftcapReturn, 0.23)
RULE_REAL ( Combat, LowChainLeatherACSoftcapReturn, 0.17)
RULE_REAL ( Combat, CasterACSoftcapReturn, 0.06)
RULE_REAL ( Combat, MiscACSoftcapReturn, 0.3)
RULE_REAL ( Combat, ACthac0Factor, 0.55)
RULE_REAL ( Combat, ACthac20Factor, 0.55)
RULE_INT ( Combat, RandomInterruptSpellChance, 85)
RULE_INT ( Combat, RandomStunChance, 100)
RULE_BOOL ( Combat, EnableAA, true)
RULE_INT ( Combat, DisarmDivision, 35)

RULE_REAL ( Combat, BackstabMaxHitIncrease, .25) // increase backstab max_hit (percentage) max_hit += maxhit * BackstabMaxHitIncrease
RULE_REAL ( Combat, DoubleBackstabDivision, 20000.0) // increase backstab max_hit (percentage) max_hit += maxhit * BackstabMaxHitIncrease

RULE_INT ( Combat, ItemMeleeMitigation, 130)
RULE_INT ( Combat, SpellMeleeMitigation, 100)


RULE_REAL ( Combat, WarriorArmorBonus, .25 ) // 20% bonus in their AC for PVP combat
RULE_REAL ( Combat, ClericArmorBonus, .10 ) // 10% bonus in their AC for PVP combat
RULE_REAL ( Combat, PaladinArmorBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( Combat, RangerArmorBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( Combat, ShadowKnightArmorBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( Combat, DruidArmorBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( Combat, MonkArmorBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( Combat, BardArmorBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( Combat, RogueArmorBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( Combat, ShamanArmorBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( Combat, NecromancerArmorBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( Combat, WizardArmorBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( Combat, MagicianArmorBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( Combat, EnchanterArmorBonus, 0 ) // x% bonus in their AC for PVP combat

RULE_REAL( Combat, IncrementInterruptGreen, 0.25 )
RULE_REAL( Combat, IncrementInterruptLightBlue, 0.50 )
RULE_REAL( Combat, IncrementInterruptBlue, 0.75 )
RULE_REAL( Combat, IncrementInterruptWhite, 1.0 )
RULE_REAL( Combat, IncrementInterruptYellow, 1.10 )
RULE_REAL( Combat, IncrementInterruptRed, 1.25 )
RULE_REAL( Combat, InterruptMaxAttacks, 10.0 )


RULE_REAL( Combat, SneakAggroReduction, .50 ) // 50% reduction

RULE_REAL ( Combat, ResistanceLogDivider, 25.0)
RULE_INT ( Combat, ResistanceCap, 225)
RULE_INT ( Combat, ResistanceMaxRoll, 300)

RULE_INT ( Combat, MeleeKnockbackAmount, 1)
RULE_INT ( Combat, MeleeKnockbackChance, 10)

RULE_INT ( Combat, WarriorKickStunLevel, 54)

RULE_REAL ( Combat, BackstabHitChanceBonusPVE, 0.0)

RULE_BOOL ( Combat, SpellResistDebugMessaging, false)

RULE_INT ( Combat, IncreaseNPCCounterSpells, 2) // factor of 2x, 1 is minimum, 0 will break cures

RULE_REAL( Combat, PetRangeReduction, 100.0 ) // 100% by default, reduce to require lower range for pet to attack a mob

RULE_REAL ( Combat, CreateSpawnNPCDistance, 200.0) // distance npcs are included at zone in versus adding after zoned in
RULE_INT ( Combat, MaxNPCQueueCount, 50) // max npcs we send per packet for larger distances beyond the spawn npc distance
RULE_INT ( Combat, MaxQueuePacketsSent, 5) // max npc queue packets sent every 3 seconds (default 3, 5*50 = 250)
RULE_CATEGORY_END()

RULE_CATEGORY( NPC )
RULE_INT ( NPC, MinorNPCCorpseDecayTimeMS, 450000 ) //level<55
RULE_INT ( NPC, MajorNPCCorpseDecayTimeMS, 1500000 ) //level>=55
RULE_INT ( NPC, CorpseUnlockTimer, 150000 )
RULE_INT ( NPC, EmptyNPCCorpseDecayTimeMS, 0 )
RULE_BOOL (NPC, UseItemBonusesForNonPets, true)
RULE_INT ( NPC, SayPauseTimeInSec, 5)
RULE_INT ( NPC, OOCRegen, 0)
RULE_BOOL ( NPC, BuffFriends, false )
RULE_BOOL ( NPC, EnableNPCQuestJournal, false)
RULE_INT ( NPC, LastFightingDelayMovingMin, 10000)
RULE_INT ( NPC, LastFightingDelayMovingMax, 20000)
RULE_BOOL ( NPC, SmartLastFightingDelayMoving, true)
RULE_BOOL ( NPC, ReturnNonQuestNoDropItems, false)	// Returns NO DROP items on NPCs that don't have an EVENT_ITEM sub in their script
RULE_CATEGORY_END()

RULE_CATEGORY ( Aggro )
RULE_BOOL ( Aggro, SmartAggroList, true )
RULE_INT ( Aggro, SittingAggroMod, 35 ) //35%
RULE_INT ( Aggro, MeleeRangeAggroMod, 10 ) //10%
RULE_INT ( Aggro, CurrentTargetAggroMod, 0 ) //0% --will prefer our current target to any other; makes it harder for our npcs to switch targets.
RULE_INT ( Aggro, CriticallyWoundedAggroMod, 100 ) //100%
RULE_INT ( Aggro, SpellAggroMod, 100 )
RULE_INT ( Aggro, SongAggroMod, 33 )
RULE_INT ( Aggro, PetSpellAggroMod, 10 )
RULE_CATEGORY_END()

RULE_CATEGORY ( TaskSystem)
RULE_BOOL ( TaskSystem, EnableTaskSystem, true) // Globally enable or disable the Task system
RULE_INT ( TaskSystem, PeriodicCheckTimer, 5) // Seconds between checks for failed tasks. Also used by the 'Touch' activity
RULE_BOOL ( TaskSystem, RecordCompletedTasks, true)
RULE_BOOL ( TaskSystem, RecordCompletedOptionalActivities, false)
RULE_BOOL ( TaskSystem, KeepOneRecordPerCompletedTask, true)
RULE_BOOL ( TaskSystem, EnableTaskProximity, true)
RULE_CATEGORY_END()

RULE_CATEGORY ( TributeSystem)
RULE_BOOL ( TributeSystem, EnableTributeSystem, false) // Globally enable or disable the tribute system
RULE_CATEGORY_END()

RULE_CATEGORY ( CheatSystem)
RULE_BOOL ( CheatSystem, DamageWarpingPlayers, false) // do some damage to players warping
RULE_INT ( CheatSystem, DamageWarpingAmount, 1 )
RULE_BOOL ( CheatSystem, ReportBuffStatus, false) 
RULE_INT ( CheatSystem, RootStunTriggerDelayMS, 500 ) // millisecond delay of triggering root/stun cheat detection system, sometimes UDP can delay packets so we want to give some leeway.
RULE_CATEGORY_END()

RULE_CATEGORY ( Performance )
RULE_BOOL ( Performance, LOSCacheEnabled, true )
RULE_INT ( Performance, LOSCacheEntryMS, 1000 )
RULE_CATEGORY_END()

#ifdef BOTS
RULE_CATEGORY ( Bots )
RULE_REAL ( Bots, BotManaRegen, 2.0 ) // Adjust mana regen for bots, 1 is fast and higher numbers slow it down 3 is about the same as players.
RULE_BOOL ( Bots, BotFinishBuffing, false ) // Allow for buffs to complete even if the bot caster is out of mana.  Only affects buffing out of combat.
RULE_INT ( Bots, CreateBotCount, 150 ) // Number of bots that each account can create
RULE_INT ( Bots, SpawnBotCount, 71 ) // Number of bots a character can have spawned at one time, You + 71 bots is a 12 group raid
RULE_BOOL ( Bots, BotQuest, false ) // Optional quest method to manage bot spawn limits using the quest_globals name bot_spawn_limit, see: /bazaar/Aediles_Thrall.pl
RULE_BOOL ( Bots, BotGroupBuffing, false ) // Bots will cast single target buffs as group buffs, default is false for single. Does not make single target buffs work for MGB.
RULE_BOOL ( Bots, BotSpellQuest, false ) // Anita Thrall's (Anita_Thrall.pl) Bot Spell Scriber quests.
RULE_CATEGORY_END()
#endif

RULE_CATEGORY ( Chat )
RULE_BOOL ( Chat, OOCReportZone, true)
RULE_BOOL ( Chat, ServerWideOOC, true)
RULE_BOOL ( Chat, ServerWideAuction, true)
RULE_BOOL ( Chat, EnableVoiceMacros, true)
RULE_BOOL ( Chat, EnableMailKeyIPVerification, true)
RULE_BOOL ( Chat, EnableAntiSpam, true)
RULE_INT ( Chat, MinStatusToBypassAntiSpam, 100)
RULE_INT ( Chat, MinimumMessagesPerInterval, 4)
RULE_INT ( Chat, MaximumMessagesPerInterval, 12)
RULE_INT ( Chat, MaxTellsPerInterval, 12)
RULE_INT ( Chat, MaxMessagesBeforeKick, 20)
RULE_INT ( Chat, IntervalDurationMS, 60000)
RULE_INT ( Chat, KarmaUpdateIntervalMS, 1200000)
RULE_INT ( Chat, KarmaGlobalChatLimit, 72) //amount of karma you need to be able to talk in ooc/auction/chat below the level limit
RULE_INT ( Chat, GlobalChatLevelLimit, 8) //level limit you need to of reached to talk in ooc/auction/chat if your karma is too low.
RULE_CATEGORY_END()

RULE_CATEGORY ( Merchant )
RULE_BOOL ( Merchant, UsePriceMod, true) // Use faction/charisma price modifiers.
RULE_REAL ( Merchant, SellCostMod, 1.05) // Modifier for NPC sell price.
RULE_REAL ( Merchant, BuyCostMod, 0.95) // Modifier for NPC buy price.
RULE_INT ( Merchant, PriceBonusPct, 4) // Determines maximum price bonus from having good faction/CHA. Value is a percent.
RULE_INT ( Merchant, PricePenaltyPct, 4) // Determines maximum price penalty from having bad faction/CHA. Value is a percent.
RULE_REAL( Merchant, ChaBonusMod, 3.45) // Determines CHA cap, from 104 CHA. 3.45 is 132 CHA at apprehensive. 0.34 is 400 CHA at apprehensive.
RULE_REAL ( Merchant, ChaPenaltyMod, 1.52) // Determines CHA bottom, up to 102 CHA. 1.52 is 37 CHA at apprehensive. 0.98 is 0 CHA at apprehensive.
RULE_CATEGORY_END()

RULE_CATEGORY ( Bazaar )
RULE_BOOL ( Bazaar, AuditTrail, false)
RULE_INT ( Bazaar, MaxSearchResults, 50)
RULE_BOOL ( Bazaar, EnableWarpToTrader, true)
RULE_INT ( Bazaar, MaxBarterSearchResults, 200) // The max results returned in the /barter search
RULE_CATEGORY_END()

RULE_CATEGORY ( Mail )
RULE_BOOL ( Mail, EnableMailSystem, true) // If false, client won't bring up the Mail window.
RULE_INT ( Mail, ExpireTrash, 0) // Time in seconds. 0 will delete all messages in the trash when the mailserver starts
RULE_INT ( Mail, ExpireRead, 31536000 ) // 1 Year. Set to -1 for never
RULE_INT ( Mail, ExpireUnread, 31536000 ) // 1 Year. Set to -1 for never
RULE_CATEGORY_END()

RULE_CATEGORY ( Channels )
RULE_INT ( Channels, RequiredStatusAdmin, 251) // Required status to administer chat channels
RULE_INT ( Channels, RequiredStatusListAll, 251) // Required status to list all chat channels
RULE_INT ( Channels, DeleteTimer, 1440) // Empty password protected channels will be deleted after this many minutes
RULE_CATEGORY_END()

RULE_CATEGORY ( EventLog )
RULE_BOOL ( EventLog, RecordSellToMerchant, false ) // Record sales from a player to an NPC merchant in eventlog table
RULE_BOOL ( EventLog, RecordBuyFromMerchant, false ) // Record purchases by a player from an NPC merchant in eventlog table
RULE_BOOL ( EventLog, LogCastTime, false ) 
RULE_BOOL ( EventLog, LogSpellDamage, false ) 
RULE_CATEGORY_END()

RULE_CATEGORY ( Adventure )
RULE_INT ( Adventure, MinNumberForGroup, 2 )
RULE_INT ( Adventure, MaxNumberForGroup, 6 )
RULE_INT ( Adventure, MinNumberForRaid, 18 )
RULE_INT ( Adventure, MaxNumberForRaid, 36 )
RULE_INT ( Adventure, MaxLevelRange, 9 )
RULE_INT ( Adventure, NumberKillsForBossSpawn, 45)
RULE_REAL ( Adventure, DistanceForRescueAccept, 10000.0)
RULE_REAL ( Adventure, DistanceForRescueComplete, 2500.0)
RULE_INT ( Adventure, ItemIDToEnablePorts, 41000 ) //0 to disable, otherwise using a LDoN portal will require the user to have this item.
RULE_INT ( Adventure, LDoNTrapDistanceUse, 625 )
RULE_REAL ( Adventure, LDoNBaseTrapDifficulty, 15.0 )
RULE_REAL ( Adventure, LDoNCriticalFailTrapThreshold, 10.0 )
RULE_INT ( Adventure, LDoNAdventureExpireTime, 1800) //30 minutes to expire
RULE_CATEGORY_END()

RULE_CATEGORY ( AA )
RULE_INT ( AA, ExpPerPoint, 23976503)	//Amount of exp per AA. Is the same as the amount of exp to go from level 51 to level 52.
RULE_CATEGORY_END()

RULE_CATEGORY( PVPCombat )

RULE_INT ( PVPCombat, MeleeCriticalReduction, 100) // higher is more reduction, must be greater than 0
RULE_INT ( PVPCombat, BardResistReductionPct, 50) // percentage of resistance reduction on movement spells against yourself, 100 is no reduction, 0 is no resist period
RULE_INT ( PVPCombat, MitigationDoT, 0) // PVP mitigation for damage over time spell, 0 = skip, set by class (SpellReductionBard) or SpellReductionOther
RULE_REAL ( PVPCombat, ReduceCritChanceBerserker, .50) // reduce crit chance on zerker by 50%
RULE_INT ( PVPCombat, MitigationBerserker, 50) // PVP mitigation by class, berserker reduced in half
RULE_INT ( PVPCombat, MitigationBySkill, 65) // PVP mitigation by skill, 1h slash/blunt and (1h) piercing for all classes will use this percentage value (pve damage * (x/100))
RULE_INT ( PVPCombat, Mitigation2HPiercing, 65) // PVP mitigation by skill, 2h piercing for all classes will use this percentage value (pve damage * (x/100))
RULE_INT ( PVPCombat, Mitigation2HSlashing, 75) // PVP mitigation by skill, 2h piercing for all classes will use this percentage value (pve damage * (x/100))
RULE_INT ( PVPCombat, Mitigation2HBlunt, 75) // PVP mitigation by skill, 2h piercing for all classes will use this percentage value (pve damage * (x/100))
RULE_INT ( PVPCombat, MitigationClassRangerBard, 60) // bard/ranger damage using non 1h/blunt/piercing weapons will apply this mitigation formula (pve damage * (x/100))
RULE_INT ( PVPCombat, MitigationClassPaladinShadowKnight, 55) // pal/sk damage using non 1h/blunt/piercing weapons will apply this mitigation formula (pve damage * (x/100))
RULE_INT ( PVPCombat, MitigationClassCleric, 60) // cleric damage using non 1h/blunt/piercing weapons will apply this mitigation formula (pve damage * (x/100))
RULE_INT ( PVPCombat, MitigationClassWarrior, 70) // all classes not described by their own mitigationbyclass are in 'other' (pve damage * (x/100))
RULE_INT ( PVPCombat, MitigationClassOther, 50) // all classes not described by their own mitigationbyclass are in 'other' (pve damage * (x/100))

RULE_INT ( PVPCombat, MitigationArchery, 70) // archery pvp mitigation reduction (pve damage * (x/100))
RULE_INT ( PVPCombat, RangerMitigationArchery, 85) // archery pvp mitigation reduction (pve damage * (x/100))

RULE_REAL ( PVPCombat, PVPBackstabMaxHitIncrease, .25) // increase backstab max_hit (percentage) max_hit += maxhit * BackstabMaxHitIncrease

RULE_INT ( PVPCombat, SpellReductionBard, 100) // bards have their own define because their spell reduction is less than other classes (or in this case none)  (pve damage * (x/100))
RULE_INT ( PVPCombat, SpellReductionOther, 67) // all other classes reduce by this much in their spell casting in pvp  (pve damage * (x/100))
RULE_REAL ( PVPCombat, SpellMaxHPDamageRatio, .80) // decimal percentage (eg .80) of max hp taken in pvp combat via spells

RULE_BOOL ( PVPCombat, TakeLocationPermitted, true)
RULE_BOOL ( PVPCombat, TakeCitiesDisabled, false)
RULE_INT ( PVPCombat, MinLevelTakeCity, 39) // must be lvl 40 to take a city

RULE_BOOL ( PVPCombat, DisablePVPIPCheck, false)
RULE_BOOL ( PVPCombat, DisableCreditLastAttacker, true)
RULE_BOOL ( PVPCombat, DisableGroupCredit, true)

RULE_BOOL ( PVPCombat, DropEars, true) // by default ear drops are disabled
RULE_INT ( PVPCombat, PVPEarDropLevel, 45)

RULE_BOOL ( PVPCombat, InfamySpawnColor, true) // set a different color for infamy players
RULE_INT ( PVPCombat, InfamySpawnValue, 31)
RULE_BOOL ( PVPCombat, TeamSpawnColor, true)
RULE_INT ( PVPCombat, HalasBodyType, 29)
RULE_INT ( PVPCombat, InfamyMaxAggroCount, 2)

RULE_BOOL ( PVPCombat, PVPRandomItemDrop, true)

RULE_REAL ( PVPCombat, ClientViewSpawnDistance, 200.0) // distance of spawns at zone to get correct zone location of other player

RULE_INT ( PVPCombat, DiscordDeathCorpseSpell, 0)
RULE_INT ( PVPCombat, TeamDeathCorpseSpell, 0)
RULE_BOOL ( PVPCombat, DiscordDeathPerm, false) // means when bandit (discord) players die on ZEK server types (3) their characters are WIPED

RULE_INT ( PVPCombat, InfamyAttackLevel, 1000) // infamy value at which they can be attacked
RULE_INT ( PVPCombat, InfamyDecayRate, 100) // rate per 10 minutes infamy will decay
RULE_INT ( PVPCombat, InfamyDecayTimer, 600) // rate per 10 minutes infamy will decay (600s)
RULE_INT ( PVPCombat, InfamyUpdateTimer, 10) // interval in seconds at which we update the user of their new infamy level
RULE_INT ( PVPCombat, PVPLevelRange, 0) // if not 0 sets level range (if greater than 0), it should be value+1, so if you want lvl range of 4, 4+1=5.
RULE_INT ( PVPCombat, PVPZekLevelRange, 0) // if not 0 sets level range (if greater than 0), it should be value+1, so if you want lvl range of 4, 4+1=5.
RULE_INT ( PVPCombat, PVPSpellLvlMitigationLow, -10) // set for limiting how much damage we do to clients
RULE_INT ( PVPCombat, PVPSpellLvlMitigationHigh, 10) // set for limiting how much damage we do to clients
RULE_INT ( PVPCombat, TrainLevelRange, 9) // if not 0 sets the level range at which we add infamy due to aggroing mobs higher than the user level by this range (moblvl-userlvl) > TrainLevelRange
RULE_INT ( PVPCombat, TrainLowerLevelRange, 14) // if not 0 sets the level range at which we add infamy due to aggroing mobs lower than the user level by this range (userlevel-moblevel) > TrainLevelRange
RULE_INT ( PVPCombat, HealLevelRange, 14) // if not 0 sets the level range at which we add infamy due to aggroing mobs higher than the user level by this range (moblvl-userlvl) > TrainLevelRange
RULE_INT ( PVPCombat, PVPHealLevelRange, 4) // if not 0 sets the level range at which we add infamy due to aggroing mobs higher than the user level by this range (moblvl-userlvl) > TrainLevelRange
RULE_INT ( PVPCombat, TrainInfamyIncrease, 100) // amount of infamy increased per mob aggroed
RULE_INT ( PVPCombat, TrainInfamyLowerLevelIncrease, 100) // amount of infamy increased per mob aggroed
RULE_BOOL ( PVPCombat, CanAttackInfamyNewbies, false)
RULE_BOOL ( PVPCombat, DisablePVPCreditNonPVPFlagged, true)
RULE_BOOL ( PVPCombat, DisablePVPCreditLevelRange, false)
RULE_BOOL ( PVPCombat, PVPPoints, true) // points enabled
RULE_BOOL ( PVPCombat, PVPCoins, true) // coins enabled
RULE_INT ( PVPCombat, CoinDropLevel, 19) // greater than this lvl coin drop
RULE_INT ( PVPCombat, CoinItem, 132487) // coins enabled
RULE_BOOL ( PVPCombat, PureRed, true) // red everyones on team 5
RULE_BOOL ( PVPCombat, PVPFlagForced, true) // everyone gets pvp flag on
RULE_BOOL ( PVPCombat, TeamGuildWars, false) // aggro rules based on teams not guilds
RULE_BOOL ( PVPCombat, ManaDrainDeathNoCombat, true) //manadrain to death is only out of combat when true
RULE_BOOL ( PVPCombat, GuardDrop, true)
RULE_INT ( PVPCombat, GuardItem, 132487) 
RULE_INT ( PVPCombat, GuardChanceDrop, 35) 
RULE_INT ( PVPCombat, GuardChanceLevel, 29) 
RULE_INT ( PVPCombat, GuardChanceLevelCap, 50)
RULE_BOOL ( PVPCombat, AllowAnon, true)

// pvp hot zone variables
RULE_INT ( PVPCombat, NPCChanceLevel, 29) 
RULE_INT ( PVPCombat, NPCChanceLevelCap, 50) 
RULE_INT ( PVPCombat, NPCItem, 132487) 
RULE_INT ( PVPCombat, PCChanceLevelCap, 50) 

RULE_BOOL ( PVPCombat, ForceNexusHubOnDeath, false) // require the hub location
RULE_INT ( PVPCombat, LevelPickTeam, 6) 
RULE_BOOL ( PVPCombat, AllowDisarmPVP, false )
RULE_BOOL ( PVPCombat, UpdateClientBuffZoning, true )
RULE_BOOL ( PVPCombat, UpdateClientBuffInZone, true )
RULE_BOOL ( PVPCombat, SameTeamCanAttackInfamyPlayers, true )

RULE_INT ( PVPCombat, EarnKillsDelay, 600) // 10 min 
RULE_INT ( PVPCombat, ZoneDeathDelay, 600) // 10 min 
RULE_INT ( PVPCombat, MinLastKillDelay, 43200) // 1/2 day
RULE_INT ( PVPCombat, MaxLastKillDelay, 86400) // 1 day

RULE_INT ( PVPCombat, PVPKillMaxLvlDiffPoints, 10) // max level difference (killer-attacker) for points
RULE_INT ( PVPCombat, MaxPointsMultiplier, 100) // max pts multiplier , killer lvl * multiplier

RULE_BOOL ( PVPCombat, CharmAllowed, true)
RULE_INT ( PVPCombat, CharmedMobDmgReduction, 70)
RULE_INT ( PVPCombat, PetMobDmgReduction, 50)

RULE_REAL ( PVPCombat, PVPPointReductionNonGroup, .40) // reduce the points earned by a single player by this decimal value (.25 = 25%)
RULE_INT ( PVPCombat, DefaultGroupPointEarning, 1) // if less than 1 point earned, default points per member in group
RULE_INT ( PVPCombat, DefaultNoGroupPointEarning, 1) // if less than 1 point earned, default points with no group

RULE_REAL ( PVPCombat, WarriorACBonus, .20 ) // 20% bonus in their AC for PVP combat
RULE_REAL ( PVPCombat, ClericACBonus, .15 ) // 10% bonus in their AC for PVP combat
RULE_REAL ( PVPCombat, PaladinACBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( PVPCombat, RangerACBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( PVPCombat, ShadowKnightACBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( PVPCombat, DruidACBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( PVPCombat, MonkACBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( PVPCombat, BardACBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( PVPCombat, RogueACBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( PVPCombat, ShamanACBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( PVPCombat, NecromancerACBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( PVPCombat, WizardACBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( PVPCombat, MagicianACBonus, 0 ) // x% bonus in their AC for PVP combat
RULE_REAL ( PVPCombat, EnchanterACBonus, 0 ) // x% bonus in their AC for PVP combat



RULE_REAL ( PVPCombat, PVPMerchantDiscount, 0.00 ) // x% discount
RULE_BOOL ( PVPCombat, DisablePVPItemsByReqLevel, true )

RULE_BOOL ( PVPCombat, GroupPVPPointsCrossZone, false )

RULE_REAL ( PVPCombat, BackstabHitChanceBonus, 0.00 )

RULE_REAL ( PVPCombat, PVPPointRatioMaxCap, 1.25 )
RULE_REAL ( PVPCombat, PVPPointRatioMinCap, .25 )

RULE_BOOL ( PVPCombat, LockoutLastKillOnly, true )

RULE_BOOL ( PVPCombat, DyeArmorOnDuel, true )
RULE_INT ( PVPCombat, DuelArmorDyeRed, 255)
RULE_INT ( PVPCombat, DuelArmorDyeGreen, 90)
RULE_INT ( PVPCombat, DuelArmorDyeBlue, 155)

RULE_BOOL ( PVPCombat, DisplayPlayerStealItem, true)
RULE_BOOL ( PVPCombat, DisplayCorpseDropItem, true)
RULE_BOOL ( PVPCombat, AllowItemSteal, true)
RULE_BOOL ( PVPCombat, AllowMoneySteal, true)
RULE_INT ( PVPCombat, ChanceStealItem, 50)
RULE_INT ( PVPCombat, ChanceStealMultiplier, 100)

RULE_BOOL ( PVPCombat, WorldPoints, true)

RULE_BOOL ( PVPCombat, BeneficialOtherTeams, true) // disables beneficial spells if you are not on the same pvp team
RULE_BOOL ( PVPCombat, BeneficialTeamsDiscord, true) // disables beneficial spells if you are on a pvp team, but not both discord

RULE_INT ( PVPCombat, PVPDefaultTeam, 2)
RULE_CATEGORY_END()


RULE_CATEGORY( PVPLeaderBoard )
RULE_BOOL ( PVPLeaderBoard, TrackDeaths, true) // Count deaths on the public leaderboard (if true)
RULE_CATEGORY_END()

RULE_CATEGORY( Kingdoms )
RULE_BOOL ( Kingdoms, NPCGuildNames, true) // show guild names (if true)
RULE_BOOL ( Kingdoms, GuildXP, true)
RULE_INT ( Kingdoms, GuardAttacksLevel, 30) // guards begin attacking people at this level
RULE_BOOL ( Kingdoms, HellExpansionLevels, true) // 50+ is half xp earning
RULE_INT ( Kingdoms, GuardRepopTimerMS, 5000) // milisecond delay for the guard repop
RULE_INT ( Kingdoms, MaxSpellClassUpgrades, 2) // milisecond delay for the guard repop
RULE_INT ( Kingdoms, MaxLevelGuardSpells, 45) // max level guards will give for buffs
RULE_INT ( Kingdoms, MaxTeamCities, 1) // max amount of cities +1 (eg. value of 1 means they can own 2 cities)
RULE_BOOL ( Kingdoms, CanAttackOwnGuards, false) // enables being able to attack your own city guards 
RULE_INT ( Kingdoms, RandomMaxBuffPercentage, 100)
RULE_INT ( Kingdoms, BuffChooseClientToCheck, 35)
RULE_INT ( Kingdoms, CityResetCityMode, 2) // 2 = restrictive, 1 = friendly, 0 = hostile
RULE_INT ( Kingdoms, CityTakenCityMode, 0) // 2 = restrictive, 1 = friendly, 0 = hostile
RULE_INT ( Kingdoms, GuardLevelLimit, 55) // max level for guards
RULE_INT ( Kingdoms, MinLevelBuff, 20) // min level guards will buff clients
RULE_INT ( Kingdoms, GuardBeneficialCheckMinMS, 750) // spell casting beneficial (AI checks), more frequent more cpu.
RULE_INT ( Kingdoms, GuardBeneficialCheckMaxMS, 1500) // spell casting beneficial (AI checks), more frequent more cpu.
RULE_BOOL ( Kingdoms, OldQuestMessageHandling, true)
RULE_BOOL ( Kingdoms, NewQuestMessagingForTeamNPCOnly, true)
RULE_REAL( Kingdoms, GuardAssistDistance, 90000) // squared value of distance for guards to assist
RULE_BOOL ( Kingdoms, ResetOnFailedTakeover, true)
RULE_CATEGORY_END()


RULE_CATEGORY( ExpanionSettings )
RULE_BOOL ( ExpanionSettings, EpicsEnabled, true) // enable availability of epics
RULE_INT ( ExpanionSettings, MobExpansion, 0) // make mobs available at higher numbers
RULE_CATEGORY_END()

RULE_CATEGORY( Replay )
RULE_INT ( Replay, SnapshotDelay, 5) // time in seconds that we wait before recording next snapshot
RULE_CATEGORY_END()

 RULE_CATEGORY( EQStream )
	RULE_INT ( EQStream, RetransmitTimeoutMax, 5000 ) // maximum retransmit timeout before retransmitting unacked packets
	RULE_INT ( EQStream, AverageDeltaMax, 2500 ) // maximum average rtt where we will still recalculate transmit rates
	RULE_REAL ( EQStream, RetransmitTimeoutMult, 3.0 ) // multiplier applied to rtt stats to generate a retransmit timeout value
	RULE_BOOL ( EQStream, RetransmitAckedPackets, true ) // should we restransmit packets that were already acked?
RULE_CATEGORY_END()

#undef RULE_CATEGORY
#undef RULE_INT
#undef RULE_REAL
#undef RULE_BOOL
#undef RULE_CATEGORY_END






