sub EVENT_COMBAT {
  if ($combat_state == 0) {
    quest::depop(); 
  }

}
