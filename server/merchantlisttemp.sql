
# Dumping structure for table kings.merchantlist_temp
CREATE TABLE IF NOT EXISTS `merchantlist_temp` (
  `npcid` int(10) unsigned NOT NULL DEFAULT '0',
  `slot` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `itemid` int(10) unsigned NOT NULL DEFAULT '0',
  `charges` int(10) signed NOT NULL DEFAULT '1',
  PRIMARY KEY (`npcid`,`slot`),
  KEY `npcid_2` (`npcid`,`itemid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

# Dumping data for table kings.merchantlist_temp: ~0 rows (approximately)
delete from merchantlist_temp where npcid=999166;
INSERT INTO `merchantlist_temp` (`npcid`, `slot`, `itemid`, `charges`) VALUES
	(999166, 16, 13073, -1),
	(999166, 17, 10015, -1),
        (999166, 18, 13080, -1);
#deleting the npc vs npc crime
delete from eventlog_combat where accountname="NA";
