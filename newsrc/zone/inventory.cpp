/*  EQEMu:  Everquest Server Emulator
Copyright (C) 2001-2003  EQEMu Development Team (http://eqemulator.net)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY except by those people which sell it, which
are required to give you total support for your newly bought product;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "../common/debug.h"
#include "masterentity.h"
#include "worldserver.h"
#include "net.h"
#include "zonedb.h"
#include "spdat.h"
#include "../common/packet_dump.h"
#include "../common/packet_functions.h"
#include "petitions.h"
#include "../common/serverinfo.h"
#include "../common/ZoneNumbers.h"
#include "../common/moremath.h"
#include "../common/guilds.h"
#include "../common/logsys.h"
#include "StringIDs.h"
#include "NpcAI.h"


// @merth: this needs to be touched up
uint32 Client::NukeItem(uint32 itemnum, uint32 count) {
	if (itemnum == 0)
		return 0;
	uint32 x = 0;

	int i;
	for (i=0; i<=30; i++) { // Equipped, personal inventory, and cursor
		if (GetItemIDAt(i) == itemnum || (itemnum == 0xFFFE && GetItemIDAt(i) != INVALID_ID)) {
			DeleteItemInInventory(i, 0, true);
			x++;
			if ( count > 0 && x >= count )
				return x;
		}
	}
	for (i=251; i<=339; i++) { // Main inventory's and cursor's containers
		if (GetItemIDAt(i) == itemnum || (itemnum == 0xFFFE && GetItemIDAt(i) != INVALID_ID)) {
			DeleteItemInInventory(i, 0, true);
			x++;
			
			if ( count > 0 && x >= count )
				return x;
		}
	}
	for (i=2000; i<=2023; i++) { // Bank slots
		if (GetItemIDAt(i) == itemnum || (itemnum == 0xFFFE && GetItemIDAt(i) != INVALID_ID)) {
			DeleteItemInInventory(i, 0, true);
			x++;
			
			if ( count > 0 && x >= count )
				return x;
		}
	}
	for (i=2031; i<=2270; i++) { // Bank's containers
		if (GetItemIDAt(i) == itemnum || (itemnum == 0xFFFE && GetItemIDAt(i) != INVALID_ID)) {
			DeleteItemInInventory(i, 0, true);
			x++;
			
			if ( count > 0 && x >= count )
				return x;
		}
	}
	for (i=2500; i<=2501; i++) { // Shared bank
		if (GetItemIDAt(i) == itemnum || (itemnum == 0xFFFE && GetItemIDAt(i) != INVALID_ID)) {
			DeleteItemInInventory(i, 0, true);
			x++;
			
			if ( count > 0 && x >= count )
				return x;
		}
	}
	for (i=2531; i<=2550; i++) { // Shared bank's containers
		if (GetItemIDAt(i) == itemnum || (itemnum == 0xFFFE && GetItemIDAt(i) != INVALID_ID)) {
			DeleteItemInInventory(i, 0, true);
			x++;
			
			if ( count > 0 && x >= count )
				return x;
		}
	}
	// Power Source Slot
	if (GetItemIDAt(9999) == itemnum || (itemnum == 0xFFFE && GetItemIDAt(9999) != INVALID_ID)) {
		if (GetClientVersion() >= EQClientSoF)
			DeleteItemInInventory(9999, 0, true);
		else
			DeleteItemInInventory(9999, 0, false);	// Prevents Titanium crash
		x = 1;
	}	
	return x;
}

void Client::NukeInventory() {
	int i;
	for (i=0; i<=30; i++) { // Equipped, personal inventory, and cursor
		if (GetItemIDAt(i) != INVALID_ID) {
			DeleteItemInInventory(i, 0, true);
		}
	}
	for (i=251; i<=339; i++) { // Main inventory's and cursor's containers
		if (GetItemIDAt(i) != INVALID_ID) {
			DeleteItemInInventory(i, 0, true);
		}
	}
	for (i=2000; i<=2023; i++) { // Bank slots
		if (GetItemIDAt(i) != INVALID_ID) {
			DeleteItemInInventory(i, 0, true);
		}
	}
	for (i=2031; i<=2270; i++) { // Bank's containers
		if (GetItemIDAt(i) != INVALID_ID) {
			DeleteItemInInventory(i, 0, true);
		}
	}
	for (i=2500; i<=2501; i++) { // Shared bank
		if (GetItemIDAt(i) != INVALID_ID) {
			DeleteItemInInventory(i, 0, true);
		}
	}
	for (i=2531; i<=2550; i++) { // Shared bank's containers
		if (GetItemIDAt(i) != INVALID_ID) {
			DeleteItemInInventory(i, 0, true);
		}
	}
	// Power Source Slot
	if (GetItemIDAt(i) != INVALID_ID) {
		if (GetClientVersion() >= EQClientSoF)
			DeleteItemInInventory(9999, 0, true);
		else
			DeleteItemInInventory(9999, 0, false);	// Prevents Titanium crash
	}
}


// 9/21/2015 Image: - fixed lore groups in bags for trading/player corpses
bool Client::CheckLoreConflict(uint32 itemID) {
	const Item_Struct* item = database.GetItem(itemID);

	if (!item)
		return false;
	if (!(item->LoreFlag))
		return false;

	if (item->LoreGroup == -1)	// Standard lore items; look everywhere except the shared bank, return the result
		return (m_inv.HasItem(item->ID, 0, ~invWhereSharedBank) != SLOT_INVALID);	

	//If the item has a lore group, we check for other items with the same group and return the result
	return (m_inv.HasItemByLoreGroup(item->LoreGroup, ~invWhereSharedBank) != SLOT_INVALID);
}

bool Client::CheckLoreConflict(const Item_Struct* item) {
	if (!item)
		return false;
	if (!(item->LoreFlag))
		return false;

	if (item->LoreGroup == -1)	// Standard lore items; look everywhere except the shared bank, return the result
		return (m_inv.HasItem(item->ID, 0, ~invWhereSharedBank) != SLOT_INVALID);	

	//If the item has a lore group, we check for other items with the same group and return the result
	return (m_inv.HasItemByLoreGroup(item->LoreGroup, ~invWhereSharedBank) != SLOT_INVALID);
}

void Client::SummonItem(uint32 item_id, sint8 charges, uint32 aug1, uint32 aug2, uint32 aug3, uint32 aug4, uint32 aug5, bool attuned, uint16 to_slot, bool suppressLore) {
	const Item_Struct* item = database.GetItem(item_id);

	if (item == NULL) {
		Message(0, "No such item: %i", item_id);
		return;
	}
	// Checking to see if the Item is lore or not.
	bool foundlore = CheckLoreConflict(item);

	//TODO: check for lore conflict on augments

	// Checking to see if it is a GM only Item or not.
	//bool foundgm = (item->gm && (this->Admin() < 100));
	bool foundgm = false;

	if (!foundlore && !foundgm) { // Okay, It isn't LORE, or if it is, it is not in player's inventory.
		ItemInst* inst = database.CreateItem(item, charges);
		if (inst) {
			// Custom logic for SummonItem
			if ((inst->GetCharges()==0))// && inst->IsStackable())
				inst->SetCharges(1);
			if ((inst->GetCharges()>0))
				inst->SetCharges(inst->GetCharges());
			if (aug1) {
				const Item_Struct* augitem1 = database.GetItem(aug1);
				if (augitem1) {
					if (!CheckLoreConflict(augitem1)) {
						inst->PutAugment(&database, 0, aug1);
					}
					else {
						Message(0, "You already have a %s (%i) in your inventory!", item->Name, item_id);
					}
				}
			}
			if (aug2) {
				const Item_Struct* augitem2 = database.GetItem(aug2);
				if (augitem2) {
					if (!CheckLoreConflict(augitem2)) {
						inst->PutAugment(&database, 1, aug2);
					}
					else {
						Message(0, "You already have a %s (%i) in your inventory!", item->Name, item_id);
					}
				}
			}
			if (aug3) {
				const Item_Struct* augitem3 = database.GetItem(aug3);
				if (augitem3) {
					if (!CheckLoreConflict(augitem3)) {
						inst->PutAugment(&database, 2, aug3);
					}
					else {
						Message(0, "You already have a %s (%i) in your inventory!", item->Name, item_id);
					}
				}
			}
			if (aug4) {
				const Item_Struct* augitem4 = database.GetItem(aug4);
				if (augitem4) {
					if (!CheckLoreConflict(augitem4)) {
						inst->PutAugment(&database, 3, aug4);
					}
					else {
						Message(0, "You already have a %s (%i) in your inventory!", item->Name, item_id);
					}
				}
			}
			if (aug5) {
				const Item_Struct* augitem5 = database.GetItem(aug5);
				if (augitem5) {
					if (!CheckLoreConflict(augitem5)) {
						inst->PutAugment(&database, 4, aug5);
					}
					else {
						Message(0, "You already have a %s (%i) in your inventory!", item->Name, item_id);
					}
				}
			}
			if (attuned) {
				if (inst->GetItem()->Attuneable) {
					inst->SetInstNoDrop(true);
				}
			}
			if (to_slot == SLOT_CURSOR)
			{
				//inst->SetCharges(
				PushItemOnCursor(*inst);
				// Send item packet to user
				SendItemPacket(SLOT_CURSOR, inst, ItemPacketSummonItem);
			}
			else
			{
				PutItemInInventory(to_slot, *inst, true);
			}

			// Kings & Bandits - discovery inventory check
			bool discovery = database.UpdateItemDiscovery(item->ID,(char*)GetName(),GetGM());

			if ( discovery )
				Message(4,"Congratulations you have found a new item <<%s>>!!",item->Name);

			safe_delete(inst);
		}
	}
	else { // Item was already in inventory & is a LORE item or was a GM only item.  Give them a message about it.
		if (foundlore){
			if ( !suppressLore )
				Message_StringID(0,PICK_LORE);
			//Message(0, "You already have a %s (%i) in your inventory!", item->Name, item_id);
		}
		else if (foundgm)
			Message(0, "You are not a GM to summon this item");
	}
}

// Drop item from inventory to ground (generally only dropped from SLOT_CURSOR)
void Client::DropItem(sint16 slot_id)
{
	ItemInst* inst = m_inv.GetItem(slot_id);
			
	if (!inst) {
		// Item doesn't exist in inventory!
		Message(13, "Error: Item not found in slot %i", slot_id);
		return;
	}

	char logMsg[256];
	bool isAugmented = false;
	for(int i = 0; i < MAX_AUGMENT_SLOTS; ++i)
	{
		if (inst->GetAugmentItemID(i))
		{
			isAugmented = true;
			break;
		}
	}
	if (isAugmented || GetInv().CheckNoDrop(slot_id)) {
		Message(0, "No Drop Exploit: Items Destroyed.");

		_snprintf(logMsg,256,"Player attempted to drop a NODROP item to the ground %d %s (slot id: %i)",inst->GetItem()->ID,inst->GetItem()->Name, slot_id);
		LogPlayerMessage(15,"DropItem Attempt (NODROP)", logMsg);

		GetInv().DeleteItem(slot_id);
		return;
	}
	
	if (inst && inst->IsType(ItemClassContainer)) {
		for (int16 i=0; i<10; i++) {
			int16 bagslot_id = Inventory::CalcSlotId(slot_id, i);
			const ItemInst* bagitem = GetInv().GetItem(bagslot_id);
			
			if ( bagitem == NULL )
				continue;

			isAugmented = false;
			for(int i = 0; i < MAX_AUGMENT_SLOTS; ++i)
			{
				if (bagitem->GetAugmentItemID(i))
				{
					isAugmented = true;
					break;
				}
			}
				if (isAugmented || bagitem->GetItem()->NoDrop == 0)
				{
					_snprintf(logMsg,256,"Player attempted to drop a NODROP item to the ground %d %s (slot id: %i)",bagitem->GetItem()->ID,bagitem->GetItem()->Name, bagslot_id);
					LogPlayerMessage(15,"DropItem Attempt (NODROP)", logMsg);
					return;
				}
		}
	}

	// Take control of item in client inventory
	inst = m_inv.PopItem(slot_id);
	if (!inst) {
		// Item doesn't exist in inventory!
		Message(13, "Error: Item not found in slot %i", slot_id);
		return;
	}

	// Save client inventory change to database
	if (slot_id==SLOT_CURSOR) {
		list<ItemInst*>::const_iterator s=m_inv.cursor_begin(),e=m_inv.cursor_end();
		database.SaveCursor(CharacterID(), s, e);
	} else
		database.SaveInventory(CharacterID(), NULL, slot_id, &m_inv, zone->GetZoneID());

	// Package as zone object
	Object* object = new Object(this, inst);
	entity_list.AddObject(object, true);
	object->StartDecay();
	object->Save(); // KaB / Red69 / Zek - Saves to DB the item(s)
	
	_snprintf(logMsg,256,"Player Dropped item %d %s (slot id: %i)",inst->GetItem()->ID,inst->GetItem()->Name, slot_id);
	LogPlayerMessage(20,"Player Dropped Item", logMsg);

	safe_delete(inst);
}

// Drop inst
void Client::DropInst(const ItemInst* inst)
{
	if (!inst) {
		// Item doesn't exist in inventory!
		Message(13, "Error: Item not found");
		return;
	}


	if (inst->GetItem()->NoDrop == 0)
	{
		Message(13, "This item is NODROP. Deleting.");
		return;
	}

	// Package as zone object
	Object* object = new Object(this, inst);
	entity_list.AddObject(object, true);
	object->StartDecay();
}

// Returns a slot's item ID (returns INVALID_ID if not found)
uint32 Client::GetItemIDAt(sint16 slot_id) {
	const ItemInst* inst = m_inv[slot_id];
	if (inst)
		return inst->GetItem()->ID;

	// None found
	return INVALID_ID;
}

// Returns an augment's ID that's in an item (returns INVALID_ID if not found)
// Pass in the slot ID of the item and which augslot you want to check (0-4)
uint32 Client::GetAugmentIDAt(sint16 slot_id, uint8 augslot) {
	const ItemInst* inst = m_inv[slot_id];
	if (inst)
		if (inst->GetAugmentItemID(augslot))
			return inst->GetAugmentItemID(augslot);

	// None found
	return INVALID_ID;
}

// Remove item from inventory
void Client::DeleteItemInInventory(sint16 slot_id, sint8 quantity, bool client_update, bool update_db) {
#if (EQDEBUG >= 5)
	LogFile->write(EQEMuLog::Debug, "DeleteItemInInventory(%i, %i, %s)", slot_id, quantity, (client_update) ? "true":"false");
#endif

	if(!m_inv[slot_id]) {
		// Make sure the client deletes anything in this slot to match the server.
		if(client_update) {
			EQApplicationPacket* outapp;
			outapp = new EQApplicationPacket(OP_DeleteItem, sizeof(DeleteItem_Struct));
			DeleteItem_Struct* delitem	= (DeleteItem_Struct*)outapp->pBuffer;
			delitem->from_slot			= slot_id;
			delitem->to_slot			= 0xFFFFFFFF;
			delitem->number_in_stack	= 0xFFFFFFFF;
			QueuePacket(outapp);
			safe_delete(outapp);
		}
		return;
	}

	bool isDeleted = m_inv.DeleteItem(slot_id, quantity);

	const ItemInst* inst=NULL;
	if (slot_id==SLOT_CURSOR) {
		list<ItemInst*>::const_iterator s=m_inv.cursor_begin(),e=m_inv.cursor_end();
		if(update_db)
			database.SaveCursor(character_id, s, e);
	}
	else {
		// Save change to database
		inst = m_inv[slot_id];
		if(update_db)
			database.SaveInventory(character_id, inst, slot_id, &m_inv, zone->GetZoneID());
	}

	if(client_update) {
		EQApplicationPacket* outapp;
		if(inst) {
			if(!inst->IsStackable() && !isDeleted) 
				// Non stackable item with charges = Item with clicky spell effect ? Delete a charge.
				outapp = new EQApplicationPacket(OP_DeleteCharge, sizeof(MoveItem_Struct));
			else
				// Stackable, arrows, etc ? Delete one from the stack
				outapp = new EQApplicationPacket(OP_DeleteItem, sizeof(MoveItem_Struct));

			DeleteItem_Struct* delitem	= (DeleteItem_Struct*)outapp->pBuffer;
			delitem->from_slot			= slot_id;
			delitem->to_slot			= 0xFFFFFFFF;
			delitem->number_in_stack	= 0xFFFFFFFF;
			for(int loop=0;loop<quantity;loop++)
				QueuePacket(outapp);
			safe_delete(outapp);
		}
		else {
			outapp = new EQApplicationPacket(OP_MoveItem, sizeof(MoveItem_Struct));
			MoveItem_Struct* delitem	= (MoveItem_Struct*)outapp->pBuffer;
			delitem->from_slot			= slot_id;
			delitem->to_slot			= 0xFFFFFFFF;
			delitem->number_in_stack	= 0xFFFFFFFF;
			QueuePacket(outapp);
			safe_delete(outapp);
		}
	}

}

// Puts an item into the person's inventory
// Any items already there will be removed from user's inventory
// (Also saves changes back to the database: this may be optimized in the future)
// client_update: Sends packet to client
bool Client::PushItemOnCursor(const ItemInst& inst, bool client_update)
{
	mlog(INVENTORY__SLOTS, "Putting item %s (%d) on the cursor", inst.GetItem()->Name, inst.GetItem()->ID);
	m_inv.PushCursor(inst);

	if (client_update) {
		SendItemPacket(SLOT_CURSOR, &inst, ItemPacketSummonItem);
	}
	
	// Kings & Bandits - discovery inventory check
	bool discovery = database.UpdateItemDiscovery(inst.GetItem()->ID,(char*)GetName(),GetGM());

	if ( discovery )
		Message(4,"Congratulations you have found a new item <<%s>>!!",inst.GetItem()->Name);

	list<ItemInst*>::const_iterator s=m_inv.cursor_begin(),e=m_inv.cursor_end();
	return database.SaveCursor(CharacterID(), s, e);
}

bool Client::PutItemInInventory(sint16 slot_id, const ItemInst& inst, bool client_update)
{
	mlog(INVENTORY__SLOTS, "Putting item %s (%d) into slot %d", inst.GetItem()->Name, inst.GetItem()->ID, slot_id);
	if (slot_id==SLOT_CURSOR) 
	{
		return PushItemOnCursor(inst,client_update);
	}
	else
		m_inv.PutItem(slot_id, inst);

	if (client_update) {
		SendItemPacket(slot_id, &inst, (slot_id==SLOT_CURSOR)?ItemPacketSummonItem:ItemPacketTrade);
	}

	if (slot_id==SLOT_CURSOR) {
		list<ItemInst*>::const_iterator s=m_inv.cursor_begin(),e=m_inv.cursor_end();
		return database.SaveCursor(this->CharacterID(), s, e);
	} else
		return database.SaveInventory(this->CharacterID(), &inst, slot_id, &m_inv, zone->GetZoneID());

	CalcBonuses();
	
	// Kings & Bandits - discovery inventory check
	bool discovery = database.UpdateItemDiscovery(inst.GetItem()->ID,(char*)GetName(),GetGM());

	if ( discovery )
		Message(4,"Congratulations you have found a new item <<%s>>!!",inst.GetItem()->Name);
}

void Client::PutLootInInventory(sint16 slot_id, const ItemInst &inst, ServerLootItem_Struct** bag_item_data)
{
	if ( inst.GetItem() == NULL )
		return;

	mlog(INVENTORY__SLOTS, "Putting loot item %s (%d) into slot %d - charges %i", inst.GetItem()->Name, inst.GetItem()->ID, slot_id,inst.GetCharges());
	m_inv.PutItem(slot_id, inst);

	SendLootItemInPacket(&inst, slot_id);

	if (slot_id==SLOT_CURSOR) {
		list<ItemInst*>::const_iterator s=m_inv.cursor_begin(),e=m_inv.cursor_end();
		database.SaveCursor(this->CharacterID(), s, e);
	} else
		database.SaveInventory(this->CharacterID(), &inst, slot_id, &m_inv, zone->GetZoneID());

	if(bag_item_data)	// bag contents
	{
		sint16 interior_slot;
		// solar: our bag went into slot_id, now let's pack the contents in
		for(int i = 0; i < 10; i++)
		{
			if(bag_item_data[i] == NULL)
				continue;
			const ItemInst *bagitem = database.CreateItem(bag_item_data[i]->item_id, bag_item_data[i]->charges, bag_item_data[i]->aug1, bag_item_data[i]->aug2, bag_item_data[i]->aug3, bag_item_data[i]->aug4, bag_item_data[i]->aug5);
			interior_slot = Inventory::CalcSlotId(slot_id, i);
			
			mlog(INVENTORY__SLOTS, "Putting bag loot item %s (%d) into slot %d (bag slot %d) - charges %i", bagitem->GetItem()->Name, bagitem->GetItem()->ID, interior_slot, i, bag_item_data[i]->charges);
			PutLootInInventory(interior_slot, *bagitem);
			safe_delete(bagitem);
		}
	}

	CalcBonuses();
	
	// Kings & Bandits - discovery inventory check
	bool discovery = database.UpdateItemDiscovery(inst.GetItem()->ID,(char*)GetName(),GetGM());

	if ( discovery )
		Message(4,"Congratulations you have found a new item <<%s>>!!",inst.GetItem()->Name);

}
bool Client::TryStacking(ItemInst* item, int8 type, bool try_worn, bool try_cursor){
	if(!item || !item->IsStackable() || item->GetCharges()>=item->GetItem()->StackSize)
		return false;
	sint16 i;
	int32 item_id = item->GetItem()->ID;
	for (i = 22; i <= 29; i++)
	{
		ItemInst* tmp_inst = m_inv.GetItem(i);	
		if(tmp_inst && tmp_inst->GetItem()->ID == item_id && tmp_inst->GetCharges() < tmp_inst->GetItem()->StackSize){
			MoveItemCharges(*item, i, type);
			CalcBonuses();
			if(item->GetCharges())	// we didn't get them all
				return AutoPutLootInInventory(*item, try_worn, try_cursor, 0);
			return true;
		}
	}
	for (i = 22; i <= 29; i++)
	{
		for (uint8 j = 0; j < 10; j++)
		{
			int16 slotid = Inventory::CalcSlotId(i, j);
			ItemInst* tmp_inst = m_inv.GetItem(slotid);

			if(tmp_inst && tmp_inst->GetItem()->ID == item_id && tmp_inst->GetCharges() < tmp_inst->GetItem()->StackSize){
				MoveItemCharges(*item, slotid, type);
				CalcBonuses();
				if(item->GetCharges())	// we didn't get them all
					return AutoPutLootInInventory(*item, try_worn, try_cursor, 0);
				return true;
			}
		}
	}
	return false;
}
// Locate an available space in inventory to place an item
// and then put the item there
// The change will be saved to the database
bool Client::AutoPutLootInInventory(ItemInst& inst, bool try_worn, bool try_cursor, ServerLootItem_Struct** bag_item_data)
{
	// #1: Try to auto equip
	if (try_worn && inst.IsEquipable(GetBaseRace(), GetClass()) && inst.GetItem()->ReqLevel<=level && !inst.GetItem()->Attuneable && inst.GetItem()->ItemType != ItemTypeAugment)
	{
		for (sint16 i = 0; i < 22; i++)
		{
			if (!m_inv[i])
			{
				// KaB / Red69 / Zek - CHANGED THIS CODE TO SWITCH STATEMENTS AND HANDLE BARDS
				// BARDS IN CLASSIC CANNOT HAVE PRIMARY OR SECONDARY WHEN ALREADY EQUIPPED A INSTRUMENT

				/*Posted @ Fri, Sep 13th 11:46 AM 2002
					That is how all instruments work.  You must remove items from primary and secondary before you can hold the instrument in your secondary.
					http://web.archive.org/web/20040913225634/http://everquest.allakhazam.com/db/item.html?item=2462
				*/
				if( i == SLOT_PRIMARY && inst.IsWeapon() || GetClass() == BARD ) // If item is primary slot weapon
				{
					int8 use = inst.GetItem()->ItemType;
						switch(use)
						{
							case ItemType2HS:
							case ItemType2HB:
							case ItemType2HPierce: // 2 handers cannot wield a secondary

							case ItemTypeWindInstr: // instrument to primary, check if secondary slot is filled
							case ItemTypeStringInstr:
							case ItemTypeBrassInstr:
							case ItemTypeDrumInstr:
							case ItemUseAllInstruments:
								{
									if( m_inv[SLOT_SECONDARY] ) // and if secondary slot is not empty
									{
										continue; // Can't auto-equip
									}
									break;
								}
						}
				}
				if( i== SLOT_SECONDARY && m_inv[SLOT_PRIMARY]) // check to see if primary slot is a two hander
				{
					int8 use = m_inv[SLOT_PRIMARY]->GetItem()->ItemType;
						switch(use)
						{
							case ItemType2HS:
							case ItemType2HB:
							case ItemType2HPierce: // 2 handers cannot wield a secondary
								{
									continue;
									break;
								}

						}

						switch(inst.GetItem()->ItemType)
						{
							case ItemTypeWindInstr: // instrument attempting to be equipped, however primary slot is filled
							case ItemTypeStringInstr:
							case ItemTypeBrassInstr:
							case ItemTypeDrumInstr:
							case ItemUseAllInstruments:
								{
									continue;
									break;
								}
						}
				}
				if
					(
					i == SLOT_SECONDARY &&
					inst.IsWeapon() &&
					!CanThisClassDualWield()
					)
				{
					continue;
				}

				if (inst.IsEquipable(i))	// Equippable at this slot?
				{
					//send worn to everyone...
					PutLootInInventory(i, inst);
					int8 worn_slot_material = Inventory::CalcMaterialFromSlot(i);
					if(worn_slot_material != 0xFF)
					{
						SendWearChange(worn_slot_material);
					}
					return true;
				}
			}
		}
	}

	// #2: Stackable item?
	if (inst.IsStackable())
	{
		if(TryStacking(&inst, ItemPacketTrade, try_worn, try_cursor))
			return true;
	}

	// #3: put it in inventory
	bool is_arrow = (inst.GetItem()->ItemType == ItemTypeArrow) ? true : false;
	sint16 slot_id = m_inv.FindFreeSlot(inst.IsType(ItemClassContainer), try_cursor, inst.GetItem()->Size, is_arrow);
	if (slot_id != SLOT_INVALID)
	{
		PutLootInInventory(slot_id, inst, bag_item_data);
		return true;
	}

	return false;
}

// solar: helper function for AutoPutLootInInventory
void Client::MoveItemCharges(ItemInst &from, sint16 to_slot, int8 type)
{
	ItemInst *tmp_inst = m_inv.GetItem(to_slot);

	if(tmp_inst && tmp_inst->GetCharges() < tmp_inst->GetItem()->StackSize)
	{
		// this is how much room is left on the item we're stacking onto
		int charge_slots_left = tmp_inst->GetItem()->StackSize - tmp_inst->GetCharges();
		// this is how many charges we can move from the looted item to
		// the item in the inventory
		int charges_to_move =
			from.GetCharges() < charge_slots_left ?
			from.GetCharges() :
		charge_slots_left;

		tmp_inst->SetCharges(tmp_inst->GetCharges() + charges_to_move);
		from.SetCharges(from.GetCharges() - charges_to_move);
		SendLootItemInPacket(tmp_inst, to_slot);
		if (to_slot==SLOT_CURSOR){
			list<ItemInst*>::const_iterator s=m_inv.cursor_begin(),e=m_inv.cursor_end();
			database.SaveCursor(this->CharacterID(), s, e);
		} else
			database.SaveInventory(this->CharacterID(), tmp_inst, to_slot, &m_inv, zone->GetZoneID());
	}
}

bool Client::MakeItemLink(char* &ret_link, const ItemInst *inst) {
	//we're sending back the entire "link", minus the null characters & item name
	//that way, we can use it for regular links & Task links
	//note: initiator needs to pass us ret_link

	/*
	--- Usage ---
	Chat: "%c" "%s" "%s" "%c", 0x12, ret_link, inst->GetItem()->name, 0x12
	Task: "<a WndNotify=\"27," "%s" "\">" "%s" "</a>", ret_link, inst->GetItem()->name
	<a WndNotify="27,00960F000000000000000000000000000000000000000">Master's Book of Wood Elven Culture</a>
	http://eqitems.13th-floor.org/phpBB2/viewtopic.php?p=510#510
	*/

	if (!inst) //have to have an item to make the link
		return false;

	const Item_Struct* item = inst->GetItem();
	//format:
	//0	itemid	aug1	aug2	aug3	aug4	aug5	evolving?	loregroup	evolved level	hash
	//0	00000	00000	00000	00000	00000	00000	0			0000		0				00000000
	//length:
	//1	5		5		5		5		5		5		1			4			1				8		= 45
	//evolving item info: http://eqitems.13th-floor.org/phpBB2/viewtopic.php?t=145#558
	int8 evolving = 0;
	uint16 loregroup = 0;
	int8 evolvedlevel = 0;
	int hash = 0;
	//int hash = GetItemLinkHash(inst);	//eventually this will work (currently crashes zone), but for now we'll skip the extra overhead
	if (GetClientVersion() >= EQClientSoF)
	{
		MakeAnyLenString(&ret_link, "%1X" "%05X" "%05X" "%05X" "%05X" "%05X" "%05X" "%1X" "%04X" "%1X" "%05X" "%08X", 
			0,
			item->ID, 
			inst->GetAugmentItemID(0), 
			inst->GetAugmentItemID(1), 
			inst->GetAugmentItemID(2), 
			inst->GetAugmentItemID(3), 
			inst->GetAugmentItemID(4), 
			evolving, 
			loregroup, 
			evolvedlevel,
			0,
			hash
			);
	}
	else
	{
		MakeAnyLenString(&ret_link, "%1X" "%05X" "%05X" "%05X" "%05X" "%05X" "%05X" "%1X" "%04X" "%1X" "%08X", 
			0,
			item->ID, 
			inst->GetAugmentItemID(0), 
			inst->GetAugmentItemID(1), 
			inst->GetAugmentItemID(2), 
			inst->GetAugmentItemID(3), 
			inst->GetAugmentItemID(4), 
			evolving, 
			loregroup, 
			evolvedlevel, 
			hash
			);
	}

	return true;
}

int Client::GetItemLinkHash(const ItemInst* inst) {
	//pre-Titanium: http://eqitems.13th-floor.org/phpBB2/viewtopic.php?t=70&postdays=0&postorder=asc
	//Titanium: http://eqitems.13th-floor.org/phpBB2/viewtopic.php?t=145
	if (!inst)	//have to have an item to make the hash
		return 0;

	const Item_Struct* item = inst->GetItem();
	char* hash_str = 0;
	/*register */int hash = 0;

	//now the fun part, since different types of items use different hashes...
	if (item->ItemClass == 0 && item->CharmFileID) {	//charm
		MakeAnyLenString(&hash_str, "%d%s-1-1-1-1-1%d %d %d %d %d %d %d %d %d",
			item->ID,
			item->Name,
			item->Light,
			item->Icon,
			item->Price,
			item->Size,
			item->Weight,
			item->ItemClass,
			item->ItemType,
			item->Favor,
			item->GuildFavor);
	} else if (item->ItemClass == 2) {	//book
		MakeAnyLenString(&hash_str, "%d%s%d%d%09X",
			item->ID,
			item->Name,
			item->Weight,
			item->BookType,
			item->Price);
	} else if (item->ItemClass == 1) {	//bag
		MakeAnyLenString(&hash_str, "%d%s%x%d%09X%d",
			item->ID,
			item->Name,
			item->BagSlots,
			item->BagWR,
			item->Price,
			item->Weight);
	} else {	//everything else
		MakeAnyLenString(&hash_str, "%d%s-1-1-1-1-1%d %d %d %d %d %d %d %d %d %d %d %d %d",
			item->ID,
			item->Name,
			item->Mana,
			item->HP,
			item->Favor,
			item->Light,
			item->Icon,
			item->Price,
			item->Weight,
			item->ReqLevel,
			item->Size,
			item->ItemClass,
			item->ItemType,
			item->AC,
			item->GuildFavor);
	}

	//this currently crashes zone, so someone feel free to fix this so we can work with hashes:
	//*** glibc detected *** double free or corruption (out): 0xb2403470 ***

	/*
	while (*hash_str != '\0') {
	register int c = toupper(*hash_str);

	asm volatile("\
	imul $31, %1, %1;\
	movzx %%ax, %%edx;\
	addl %%edx, %1;\
	movl %1, %0;\
	"
	:"=r"(hash)
	:"D"(hash), "a"(c)
	:"%edx"
	);

	// This is what the inline asm is doing:
	// hash *= 0x1f;
	// hash += (int)c;

	hash_str++;
	}
	*/

	safe_delete_array(hash_str);
	return hash;
}

void Client::SendItemLink(const ItemInst* inst, bool send_to_all)
{
	/*

	this stuff is old, live dosent do this anymore. they send a much smaller
	packet with the item number in it, but I cant seem to find it right now

	*/
	if (!inst)
		return;

	const Item_Struct* item = inst->GetItem();
	const char* name2 = &item->Name[0];
	EQApplicationPacket* outapp = new EQApplicationPacket(OP_ItemLinkText,strlen(name2)+68);
	char buffer2[135] = {0};
	char itemlink[135] = {0};
	sprintf(itemlink,"%c0%06u0%05u-%05u-%05u-%05u-%05u00000000%c",
		0x12,
		item->ID,
		inst->GetAugmentItemID(0),
		inst->GetAugmentItemID(1),
		inst->GetAugmentItemID(2),
		inst->GetAugmentItemID(3),
		inst->GetAugmentItemID(4),
		0x12);
	sprintf(buffer2,"%c%c%c%c%c%c%c%c%c%c%c%c%s",0x00,0x00,0x00,0x00,0xD3,0x01,0x00,0x00,0x1E,0x01,0x00,0x00,itemlink);
	memcpy(outapp->pBuffer,buffer2,outapp->size);
	QueuePacket(outapp);
	safe_delete(outapp);
	if (send_to_all==false)
		return;
	const char* charname = this->GetName();
	outapp = new EQApplicationPacket(OP_ItemLinkText,strlen(itemlink)+14+strlen(charname));
	char buffer3[150] = {0};
	sprintf(buffer3,"%c%c%c%c%c%c%c%c%c%c%c%c%6s%c%s",0x00,0x00,0x00,0x00,0xD2,0x01,0x00,0x00,0x00,0x00,0x00,0x00,charname,0x00,itemlink);
	memcpy(outapp->pBuffer,buffer3,outapp->size);
	entity_list.QueueCloseClients(this->CastToMob(),outapp,true,200,0,false);
	safe_delete(outapp);
}

void Client::SendLootItemInPacket(const ItemInst* inst, sint16 slot_id)
{
	SendItemPacket(slot_id,inst, ItemPacketTrade);
}

bool Client::IsValidSlot(uint32 slot)
{
	if((slot == (uint32)SLOT_INVALID) ||	// Destroying/Dropping item
		(slot >= 0 && slot <= 30) ||		// Worn inventory, normal inventory, and cursor
		(slot >= 251 && slot <= 340) ||		// Normal inventory bags and cursor bag
		(slot >= 400 && slot <= 404) ||		// Tribute
		(slot >= 2000 && slot <= 2023) ||	// Bank
		(slot >= 2031 && slot <= 2270) ||	// Bank bags
		(slot >= 2500 && slot <= 2501) ||	// Shared bank
		(slot >= 2531 && slot <= 2550) ||	// Shared bank bags
		(slot >= 3000 && slot <= 3007) ||	// Trade window
		(slot >= 4000 && slot <= 4009) ||	// Tradeskill container
		(slot == 9999))						// Power Source
	{
		return true;
	}
	else {
		return false;
	}
}

// Moves items around both internally and in the database
// In the future, this can be optimized by pushing all changes through one database REPLACE call
bool Client::SwapItem(MoveItem_Struct* move_in) {

	uint32 src_slot_check = move_in->from_slot;
	uint32 dst_slot_check = move_in->to_slot;
	uint32 stack_count_check = move_in->number_in_stack;

	if(!IsValidSlot(src_slot_check)){
		// SoF+ sends a Unix timestamp (should be sint32) for src and dst slots every 10 minutes for some reason.
		if(src_slot_check < 2147483647)
			Message(13, "Warning: Invalid slot move from slot %u to slot %u with %u charges!", src_slot_check, dst_slot_check, stack_count_check);
		mlog(INVENTORY__SLOTS, "Invalid slot move from slot %u to slot %u with %u charges!", src_slot_check, dst_slot_check, stack_count_check);
		return false;
	}

	if(!IsValidSlot(dst_slot_check)) {
		// SoF+ sends a Unix timestamp (should be sint32) for src and dst slots every 10 minutes for some reason.
		if(src_slot_check < 2147483647)
			Message(13, "Warning: Invalid slot move from slot %u to slot %u with %u charges!", src_slot_check, dst_slot_check, stack_count_check);
		mlog(INVENTORY__SLOTS, "Invalid slot move from slot %u to slot %u with %u charges!", src_slot_check, dst_slot_check, stack_count_check);
		return false;
	}

	if (move_in->from_slot == move_in->to_slot)
		return true; // Item summon, no further proccessing needed

	if (move_in->to_slot == (uint32)SLOT_INVALID) {
		if(move_in->from_slot == (uint32)SLOT_CURSOR) {
			mlog(INVENTORY__SLOTS, "Client destroyed item from cursor slot %d", move_in->from_slot);
			DeleteItemInInventory(move_in->from_slot);
			return true; // Item destroyed by client
		}
		else {
			mlog(INVENTORY__SLOTS, "Deleted item from slot %d as a result of an inventory container tradeskill combine.", move_in->from_slot);
			DeleteItemInInventory(move_in->from_slot);
			return true; // Item deletetion
		}
	}

	if(auto_attack && (move_in->from_slot == SLOT_PRIMARY || move_in->from_slot == SLOT_SECONDARY || move_in->from_slot == SLOT_RANGE))
		SetAttackTimer();
	else if(auto_attack && (move_in->to_slot == SLOT_PRIMARY || move_in->to_slot == SLOT_SECONDARY || move_in->to_slot == SLOT_RANGE))
		SetAttackTimer();

	// Step 1: Variables
	sint16 src_slot_id = (sint16)move_in->from_slot;
	sint16 dst_slot_id = (sint16)move_in->to_slot;

	if(SharedDatabase::IsBankSlot(src_slot_id) || 
		SharedDatabase::IsBankSlot(dst_slot_id) || 
		SharedDatabase::IsBankSlot(src_slot_check) ||
		SharedDatabase::IsBankSlot(dst_slot_check))
	{
		uint32 distance = 0;
		NPC *banker = entity_list.GetClosestBanker(this, distance);

		if(!banker || distance > USE_NPC_RANGE2)
		{
			char *hacked_string = NULL;
			MakeAnyLenString(&hacked_string, "Player tried to make use of a banker(items) but %s is non-existant or too far away (%u units).", 
				banker ? banker->GetName() : "UNKNOWN NPC", distance);
			database.SetMQDetectionFlag(AccountName(), GetName(), hacked_string, zone->GetShortName());
			safe_delete_array(hacked_string);
			Kick();	// Kicking player to avoid item loss do to client and server inventories not being sync'd
			return false;
		}
	}

	//Setup
	uint32 srcitemid = 0;
	uint32 dstitemid = 0;
	ItemInst* src_inst = m_inv.GetItem(src_slot_id);
	ItemInst* dst_inst = m_inv.GetItem(dst_slot_id);
	if (src_inst){
		mlog(INVENTORY__SLOTS, "Src slot %d has item %s (%d) with %d charges in it.", src_slot_id, src_inst->GetItem()->Name, src_inst->GetItem()->ID, src_inst->GetCharges());
		srcitemid = src_inst->GetItem()->ID;
		//SetTint(dst_slot_id,src_inst->GetColor());
		if (src_inst->GetCharges() > 0 && (src_inst->GetCharges() < (sint16)move_in->number_in_stack || move_in->number_in_stack > src_inst->GetItem()->StackSize))
		{
			Message(13,"Error: Insufficent number in stack.");
			return false;
		}
	}
	if (dst_inst) {
		mlog(INVENTORY__SLOTS, "Dest slot %d has item %s (%d) with %d charges in it.", dst_slot_id, dst_inst->GetItem()->Name, dst_inst->GetItem()->ID, dst_inst->GetCharges());
		dstitemid = dst_inst->GetItem()->ID;
	}
	if (Trader && srcitemid>0){
		ItemInst* srcbag;
		ItemInst* dstbag;
		uint32 srcbagid =0;
		uint32 dstbagid = 0;
		if (src_slot_id>=250 && src_slot_id<330){
			srcbag=m_inv.GetItem(((int)(src_slot_id/10))-3);
			if(srcbag)
				srcbagid=srcbag->GetItem()->ID;
		}
		if (dst_slot_id>=250 && dst_slot_id<330){
			dstbag=m_inv.GetItem(((int)(dst_slot_id/10))-3);
			if(dstbag)
				dstbagid=dstbag->GetItem()->ID;
		}
		if (srcitemid==17899 || srcbagid==17899 || dstitemid==17899 || dstbagid==17899){
			this->Trader_EndTrader();
			this->Message(13,"You cannot move your Trader Satchels, or items inside them, while Trading.");
		}
	}

	// Step 2: Validate item in from_slot
	// After this, we can assume src_inst is a valid ptr
	if (!src_inst && (src_slot_id<4000 || src_slot_id>4009)) {
		if (dst_inst) {
			// If there is no source item, but there is a destination item, 
			// move the slots around before deleting the invalid source slot item,
			// which is now in the destination slot.
			move_in->from_slot = dst_slot_check;
			move_in->to_slot = src_slot_check;
			move_in->number_in_stack = dst_inst->GetCharges();
			SwapItem(move_in);
		}
		Message(13, "Error: Server found no item in slot %i (->%i), Deleting Item!", src_slot_id, dst_slot_id);
		LogFile->write(EQEMuLog::Debug, "Error: Server found no item in slot %i (->%i), Deleting Item!", src_slot_id, dst_slot_id);
		this->DeleteItemInInventory(dst_slot_id,0,true);
		return false;
	}
	//verify shared bank transactions in the database
	if(src_inst && src_slot_id >= 2500 && src_slot_id <= 2550) {
		if(!database.VerifyInventory(account_id, src_slot_id, src_inst)) {
			LogFile->write(EQEMuLog::Error, "Player %s on account %s was found exploiting the shared bank.\n", account_name, GetName());
			DeleteItemInInventory(dst_slot_id,0,true);
			return(false);
		}
		if(src_slot_id >= 2500 && src_slot_id <= 2501 && src_inst->IsType(ItemClassContainer)){
			for (uint8 idx=0; idx<10; idx++) {
				const ItemInst* baginst = src_inst->GetItem(idx);
				if(baginst && !database.VerifyInventory(account_id, Inventory::CalcSlotId(src_slot_id, idx), baginst)){
					DeleteItemInInventory(Inventory::CalcSlotId(src_slot_id, idx),0,false);
				}
			}
		}
	}
	if(dst_inst && dst_slot_id >= 2500 && dst_slot_id <= 2550) {
		if(!database.VerifyInventory(account_id, dst_slot_id, dst_inst)) {
			LogFile->write(EQEMuLog::Error, "Player %s on account %s was found exploting the shared bank.\n", account_name, GetName());
			DeleteItemInInventory(src_slot_id,0,true);
			return(false);
		}
		if(dst_slot_id >= 2500 && dst_slot_id <= 2501 && dst_inst->IsType(ItemClassContainer)){
			for (uint8 idx=0; idx<10; idx++) {
				const ItemInst* baginst = dst_inst->GetItem(idx);
				if(baginst && !database.VerifyInventory(account_id, Inventory::CalcSlotId(dst_slot_id, idx), baginst)){
					DeleteItemInInventory(Inventory::CalcSlotId(dst_slot_id, idx),0,false);
				}
			}
		}
	}


	// Check for No Drop Hacks
	Mob* with = trade->With();
	if (((with && with->IsClient() && dst_slot_id>=3000 && dst_slot_id<=3007) || // Trade
		(dst_slot_id >= 2500 && dst_slot_id <= 2550)) // Shared Bank
		&& GetInv().CheckNoDrop(src_slot_id)) {
			DeleteItemInInventory(src_slot_id);
			WorldKick();
			return false;
	}

	// Step 3: Check for interaction with World Container (tradeskills)
	if(m_tradeskill_object != NULL) {
		if (src_slot_id>=4000 && src_slot_id<=4009) {
			// Picking up item from world container
			ItemInst* inst = m_tradeskill_object->PopItem(Inventory::CalcBagIdx(src_slot_id));
			if (inst) {
				PutItemInInventory(dst_slot_id, *inst, false);
				safe_delete(inst);
			}

			return true;
		}
		else if (dst_slot_id>=4000 && dst_slot_id<=4009) {
			// Putting item into world container, which may swap (or pile onto) with existing item
			uint8 world_idx = Inventory::CalcBagIdx(dst_slot_id);
			ItemInst* world_inst = m_tradeskill_object->PopItem(world_idx);

			// Case 1: No item in container, unidirectional "Put"
			if (world_inst == NULL) {
				m_tradeskill_object->PutItem(world_idx, src_inst);
				m_inv.DeleteItem(src_slot_id);
			}
			else {
				const Item_Struct* world_item = world_inst->GetItem();
				const Item_Struct* src_item = src_inst->GetItem();
				if (world_item && src_item) {
					// Case 2: Same item on cursor, stacks, transfer of charges needed
					if ((world_item->ID == src_item->ID) && src_inst->IsStackable()) {
						sint8 world_charges = world_inst->GetCharges();
						sint8 src_charges = src_inst->GetCharges();

						// Fill up destination stack as much as possible
						world_charges += src_charges;
						if (world_charges > world_inst->GetItem()->StackSize) {
							src_charges = world_charges - world_inst->GetItem()->StackSize;
							world_charges = world_inst->GetItem()->StackSize;
						}
						else {
							src_charges = 0;
						}

						world_inst->SetCharges(world_charges);
						m_tradeskill_object->Save();

						if (src_charges == 0) {
							m_inv.DeleteItem(src_slot_id); // DB remove will occur below
						}
						else {
							src_inst->SetCharges(src_charges);
						}
					}
					else {
						// Case 3: Swap the item on user with item in world container
						// World containers don't follow normal rules for swapping
						ItemInst* inv_inst = m_inv.PopItem(src_slot_id);
						m_tradeskill_object->PutItem(world_idx, inv_inst);
						m_inv.PutItem(src_slot_id, *world_inst);
						safe_delete(inv_inst);
					}
				}
			}

			safe_delete(world_inst);
			if (src_slot_id==SLOT_CURSOR) {
				list<ItemInst*>::const_iterator s=m_inv.cursor_begin(),e=m_inv.cursor_end();
				database.SaveCursor(character_id, s, e);
			} else
				database.SaveInventory(character_id, m_inv[src_slot_id], src_slot_id, &m_inv, zone->GetZoneID());
			return true;
		}
	}

	// Step 4: Check for entity trade
	if (dst_slot_id>=3000 && dst_slot_id<=3007) {
		if (src_slot_id != SLOT_CURSOR) {
			Kick();
			return false;
		}
		if (with) {
			mlog(INVENTORY__SLOTS, "Trade item move from slot %d to slot %d (trade with %s)", src_slot_id, dst_slot_id, with->GetName());
			// Fill Trade list with items from cursor
			if (!m_inv[SLOT_CURSOR]) {
				Message(13, "Error: Cursor item not located on server!");
				return false;
			}

			// Add cursor item to trade bucket
			// Also sends trade information to other client of trade session
			trade->AddEntity(src_slot_id, dst_slot_id);
			return true;
		} else {
			SummonItem(src_inst->GetID(), src_inst->GetCharges());
			DeleteItemInInventory(SLOT_CURSOR);
			return true;
		}
	}

	// Step 5: Swap (or stack) items
	if (move_in->number_in_stack > 0) {
		// Determine if charged items can stack
		if (dst_inst) {
			if(src_inst->GetID() != dst_inst->GetID()) {
				mlog(INVENTORY__ERROR, "Move from %d to %d with stack size %d. Incompatible item types: %d != %d", src_slot_id, dst_slot_id, move_in->number_in_stack, src_inst->GetID(), dst_inst->GetID());
				return(false);
			}
			if(dst_inst->GetCharges() < dst_inst->GetItem()->StackSize) {
				//we have a chance of stacking.
				mlog(INVENTORY__SLOTS, "Move from %d to %d with stack size %d. dest has %d/%d charges", src_slot_id, dst_slot_id, move_in->number_in_stack, dst_inst->GetCharges(), dst_inst->GetItem()->StackSize);
				// Charges can be emptied into dst
				uint8 usedcharges = dst_inst->GetItem()->StackSize - dst_inst->GetCharges();
				if (usedcharges > move_in->number_in_stack)
					usedcharges = move_in->number_in_stack;

				dst_inst->SetCharges(dst_inst->GetCharges() + usedcharges);
				src_inst->SetCharges(src_inst->GetCharges() - usedcharges);

				// Depleted all charges?
				if (src_inst->GetCharges() < 1)
				{
					mlog(INVENTORY__SLOTS, "Dest (%d) now has %d charges, source (%d) was entirely consumed. (%d moved)", dst_slot_id, dst_inst->GetCharges(), src_slot_id, usedcharges);
					database.SaveInventory(CharacterID(),NULL,src_slot_id, &m_inv, zone->GetZoneID());
					m_inv.DeleteItem(src_slot_id);
				} else {
					mlog(INVENTORY__SLOTS, "Dest (%d) now has %d charges, source (%d) has %d (%d moved)", dst_slot_id, dst_inst->GetCharges(), src_slot_id, src_inst->GetCharges(), usedcharges);
				}
			} else {
				//stack is full, so 
			}
		}
		else {
			// Nothing in destination slot: split stack into two
			if ((sint16)move_in->number_in_stack >= src_inst->GetCharges()) {
				mlog(INVENTORY__SLOTS, "Move entire stack from %d to %d with stack size %d. Dest empty.", src_slot_id, dst_slot_id, move_in->number_in_stack);
				// Move entire stack
				m_inv.SwapItem(src_slot_id, dst_slot_id);
			}
			else {
				// Split into two
				src_inst->SetCharges(src_inst->GetCharges() - move_in->number_in_stack);
				mlog(INVENTORY__SLOTS, "Split stack of %s (%d) from slot %d to %d with stack size %d. Src keeps %d.", src_inst->GetItem()->Name, src_inst->GetItem()->ID, src_slot_id, dst_slot_id, move_in->number_in_stack, src_inst->GetCharges());
				ItemInst* inst = database.CreateItem(src_inst->GetItem(), move_in->number_in_stack);
				m_inv.PutItem(dst_slot_id, *inst);
				safe_delete(inst);
			}
		}
	}
	else {
		// Not dealing with charges - just do direct swap
		if(src_inst && (dst_slot_id < 22 || dst_slot_id == 9999) && dst_slot_id >= 0) {
			if (src_inst->GetItem()->Attuneable) {
				src_inst->SetInstNoDrop(true);
			}
			SetMaterial(dst_slot_id,src_inst->GetItem()->ID);
		}
		mlog(INVENTORY__SLOTS, "Moving entire item from slot %d to slot %d", src_slot_id, dst_slot_id);
		m_inv.SwapItem(src_slot_id, dst_slot_id);
	}

	int matslot = SlotConvert2(dst_slot_id);
	if (dst_slot_id<22 && matslot != 0) {
		SendWearChange(matslot);
	}

	// Step 7: Save change to the database
	if (src_slot_id==SLOT_CURSOR){ 
		list<ItemInst*>::const_iterator s=m_inv.cursor_begin(),e=m_inv.cursor_end();
		database.SaveCursor(character_id, s, e);
	} else
		database.SaveInventory(character_id, m_inv.GetItem(src_slot_id), src_slot_id, &m_inv, zone->GetZoneID());
	if (dst_slot_id==SLOT_CURSOR) {
		list<ItemInst*>::const_iterator s=m_inv.cursor_begin(),e=m_inv.cursor_end();
		database.SaveCursor(character_id, s, e);
	} else
		database.SaveInventory(character_id, m_inv.GetItem(dst_slot_id), dst_slot_id, &m_inv, zone->GetZoneID());


	if(auto_attack && (move_in->from_slot == SLOT_PRIMARY || move_in->from_slot == SLOT_SECONDARY || move_in->from_slot == SLOT_RANGE))
		SetAttackTimer();
	else if(auto_attack && (move_in->to_slot == SLOT_PRIMARY || move_in->to_slot == SLOT_SECONDARY || move_in->to_slot == SLOT_RANGE))
		SetAttackTimer();

	// Step 8: Re-calc stats
	CalcBonuses();
	return true;
}

void Client::DyeArmor(DyeStruct* dye, bool skipItemRequirement){
	sint16 slot=0;
	for(int i=0;i<7;i++){
		if(m_pp.item_tint[i].rgb.blue!=dye->dye[i].rgb.blue ||
			m_pp.item_tint[i].rgb.red!=dye->dye[i].rgb.red ||
			m_pp.item_tint[i].rgb.green != dye->dye[i].rgb.green){
				slot = m_inv.HasItem(32557, 1, invWherePersonal);
				if(slot != SLOT_INVALID || skipItemRequirement){
					if ( !skipItemRequirement )
						DeleteItemInInventory(slot,1,true);
					int8 slot2=SlotConvert(i);
					ItemInst* inst = this->m_inv.GetItem(slot2);
					if(inst){
						inst->SetColor((dye->dye[i].rgb.red*65536)+(dye->dye[i].rgb.green*256)+(dye->dye[i].rgb.blue));
						database.SaveInventory(CharacterID(),inst,slot2, &m_inv, zone->GetZoneID());
						if(dye->dye[i].rgb.use_tint) 
							m_pp.item_tint[i].rgb.use_tint = 0xFF;
						else 
							m_pp.item_tint[i].rgb.use_tint=0x00;
					}
					m_pp.item_tint[i].rgb.blue=dye->dye[i].rgb.blue;
					m_pp.item_tint[i].rgb.red=dye->dye[i].rgb.red;
					m_pp.item_tint[i].rgb.green=dye->dye[i].rgb.green;
					SendWearChange(i);
				}
				else{
					Message(13,"Could not locate A Vial of Prismatic Dye.");
					return;
				}
		}
	}
	EQApplicationPacket* outapp=new EQApplicationPacket(OP_Dye,0);
	QueuePacket(outapp);
	safe_delete(outapp);
	Save();
}

/*bool Client::DecreaseByItemType(int32 type, int8 amt) {
const Item_Struct* TempItem = 0;
ItemInst* ins;
int x;
for(x=0; x <= 30; x++)
{
TempItem = 0;
ins = GetInv().GetItem(x);
if (ins)
TempItem = ins->GetItem();
if (TempItem && TempItem->ItemType == type)
{
if (ins->GetCharges() < amt)
{
amt -= ins->GetCharges();
DeleteItemInInventory(x,amt,true);
}
else
{
DeleteItemInInventory(x,amt,true);
amt = 0;
}
if (amt < 1)
return true;
}
}
for(x=251; x < 331; x++)
{
TempItem = 0;
ins = GetInv().GetItem(x);
if (ins)
TempItem = ins->GetItem();
if (TempItem && TempItem->ItemType == type)
{
if (ins->GetCharges() < amt)
{
amt -= ins->GetCharges();
DeleteItemInInventory(x,amt,true);
}
else
{
DeleteItemInInventory(x,amt,true);
amt = 0;
}
if (amt < 1)
return true;
}
}
return false;
}*/

bool Client::DecreaseByID(int32 type, int8 amt) {
	const Item_Struct* TempItem = 0;
	ItemInst* ins;
	int x;
	int num = 0;
	for(x=0; x < 331; x++)
	{
		if (x == 31)
			x = 251;
		TempItem = 0;
		ins = GetInv().GetItem(x);
		if (ins)
			TempItem = ins->GetItem();
		if (TempItem && TempItem->ID == type)
		{
			num += ins->GetCharges();
			if (num >= amt)
				break;
		}
	}
	if (num < amt)
		return false;
	for(x=0; x < 331; x++)
	{
		if (x == 31)
			x = 251;
		TempItem = 0;
		ins = GetInv().GetItem(x);
		if (ins)
			TempItem = ins->GetItem();
		if (TempItem && TempItem->ID == type)
		{
			if (ins->GetCharges() < amt)
			{
				amt -= ins->GetCharges();
				DeleteItemInInventory(x,amt,true);
			}
			else
			{
				DeleteItemInInventory(x,amt,true);
				amt = 0;
			}
			if (amt < 1)
				break;
		}
	}
	return true;
}


void Client::RemoveNoRent() {
	const Item_Struct* TempItem = 0;
	ItemInst* ins;
	int x;

	//personal inventory
	for(x=0; x <= 30; x++)
	{
		TempItem = 0;
		ins = GetInv().GetItem(x);
		if(!ins)
			continue;
		TempItem = ins->GetItem();
		if(!TempItem)
			continue;
		if(TempItem->NoRent == 0)
			DeleteItemInInventory(x,0,true);
	}

	//containers
	for(x=251; x <= 330; x++)
	{
		TempItem = 0;
		ins = GetInv().GetItem(x);
		if(!ins)
			continue;
		TempItem = ins->GetItem();
		if(!TempItem)
			continue;
		if(TempItem->NoRent == 0)
			DeleteItemInInventory(x,0,true);
	}
	for(x=2000; x <= 2023; x++)
	{
		TempItem = 0;
		ins = GetInv().GetItem(x);
		if(!ins)
			continue;
		TempItem = ins->GetItem();
		if(!TempItem)
			continue;
		if(TempItem->NoRent == 0)
			DeleteItemInInventory(x,0,true);
	}
	for(x=2031; x <= 2270; x++)
	{
		TempItem = 0;
		ins = GetInv().GetItem(x);
		if(!ins)
			continue;
		TempItem = ins->GetItem();
		if(!TempItem)
			continue;
		if(TempItem->NoRent == 0)
			DeleteItemInInventory(x,0,true);
	}
	for(x=2500; x <= 2501; x++)
	{
		TempItem = 0;
		ins = GetInv().GetItem(x);
		if(!ins)
			continue;
		TempItem = ins->GetItem();
		if(!TempItem)
			continue;
		if(TempItem->NoRent == 0)
			DeleteItemInInventory(x,0,true);
	}
	for(x=2531; x <= 2550; x++)
	{
		TempItem = 0;
		ins = GetInv().GetItem(x);
		if(!ins)
			continue;
		TempItem = ins->GetItem();
		if(!TempItem)
			continue;
		if(TempItem->NoRent == 0)
			DeleteItemInInventory(x,0,true);
	}
	if(GetClientVersion() >= EQClientSoF)
	{
		TempItem = 0;
		ins = GetInv().GetItem(9999);
		if(ins)	{
			TempItem = ins->GetItem();
			if(TempItem) {
				if(TempItem->NoRent == 0)
					DeleteItemInInventory(9999,0,true);
			}
		}
	}
}

// these functions operate with a material slot, which is from 0 to 8
int32 Client::GetEquipment(int8 material_slot) const
{
	int invslot;
	const ItemInst *item;

	if(material_slot > 8)
	{
		return 0;
	}

	invslot = Inventory::CalcSlotFromMaterial(material_slot);
	if(invslot == -1)
	{
		return 0;
	}

	item = m_inv.GetItem(invslot);

	if(item && item->GetItem())
	{
		return item->GetItem()->ID;
	}

	return 0;
}

/*
sint32 Client::GetEquipmentMaterial(int8 material_slot)
{
const Item_Struct *item;

item = database.GetItem(GetEquipment(material_slot));
if(item != 0)
{
return item->Material;
}

return 0;
}
*/

uint32 Client::GetEquipmentColor(int8 material_slot) const
{
	const Item_Struct *item;

	if(material_slot > 8)
	{
		return 0;
	}

	item = database.GetItem(GetEquipment(material_slot));
	if(item != 0)
	{
		return m_pp.item_tint[material_slot].rgb.use_tint ?
			m_pp.item_tint[material_slot].color :
		item->Color;
	}

	return 0;
}

bool Client::LootToStack(int32 itemid) {  //Loots stackable items to existing stacks - Wiz
	// @merth: Need to do loot code with new inventory struct
	/*
	const Item_Struct* item;
	int i;
	for (i=22; i<=29; i++) {
	item = GetItemAt(i);
	if (item) {
	if (m_pp.invitemproperties[i].charges < 20 && item->ID == itemid)
	{
	m_pp.invitemproperties[i].charges += 1;
	EQApplicationPacket* outapp = new EQApplicationPacket(OP_PlaceItem, sizeof(Item_Struct));
	memcpy(outapp->pBuffer, item, outapp->size);
	Item_Struct* outitem = (Item_Struct*) outapp->pBuffer;
	outitem->equipSlot = i;
	outitem->charges = m_pp.invitemproperties[i].charges;
	QueuePacket(outapp);
	safe_delete(outapp);
	return true;
	}
	}
	}
	for (i=0; i<=pp_containerinv_size; i++) {
	if (m_pp.containerinv[i] != 0xFFFF) {
	item = database.GetItem(m_pp.containerinv[i]);
	if (m_pp.bagitemproperties[i].charges < 20 && item->ID == itemid)
	{
	m_pp.bagitemproperties[i].charges += 1;

	EQApplicationPacket* outapp = new EQApplicationPacket(OP_PlaceItem, sizeof(Item_Struct));
	memcpy(outapp->pBuffer, item, outapp->size);
	Item_Struct* outitem = (Item_Struct*) outapp->pBuffer;
	outitem->equipSlot = 250+i;
	outitem->charges = m_pp.bagitemproperties[i].charges;
	QueuePacket(outapp);
	safe_delete(outapp);
	return true;
	}
	}
	}
	*/
	return false;
}

// Send an item packet (including all subitems of the item)
void Client::SendItemPacket(sint16 slot_id, const ItemInst* inst, ItemPacketType packet_type)
{
	if (!inst)
		return;

	// Serialize item into |-delimited string
	string packet = inst->Serialize(slot_id);

	EmuOpcode opcode = OP_Unknown;
	EQApplicationPacket* outapp = NULL;
	ItemPacket_Struct* itempacket = NULL;

	// Construct packet
	opcode = (packet_type==ItemPacketViewLink) ? OP_ItemLinkResponse : OP_ItemPacket;
	outapp = new EQApplicationPacket(opcode, packet.length()+sizeof(ItemPacket_Struct));
	itempacket = (ItemPacket_Struct*)outapp->pBuffer;
	memcpy(itempacket->SerializedItem, packet.c_str(), packet.length());
	itempacket->PacketType = packet_type;

#if EQDEBUG >= 9
	DumpPacket(outapp);
#endif
	//DumpPacket(outapp);
	FastQueuePacket(&outapp);
}

EQApplicationPacket* Client::ReturnItemPacket(sint16 slot_id, const ItemInst* inst, ItemPacketType packet_type)
{
	if (!inst)
		return 0;

	// Serialize item into |-delimited string
	string packet = inst->Serialize(slot_id);

	EmuOpcode opcode = OP_Unknown;
	EQApplicationPacket* outapp = NULL;
	BulkItemPacket_Struct* itempacket = NULL;

	// Construct packet
	opcode = OP_ItemPacket;
	outapp = new EQApplicationPacket(opcode, packet.length()+1);
	itempacket = (BulkItemPacket_Struct*)outapp->pBuffer;
	memcpy(itempacket->SerializedItem, packet.c_str(), packet.length());

#if EQDEBUG >= 9
	DumpPacket(outapp);
#endif

	return outapp;
}

static sint16 BandolierSlotToWeaponSlot(int BandolierSlot) {

	switch(BandolierSlot) {
	case bandolierMainHand:
		return SLOT_PRIMARY;
	case bandolierOffHand:
		return SLOT_SECONDARY;
	case bandolierRange:
		return SLOT_RANGE;
	default:
		return SLOT_AMMO;
	}
}

void Client::CreateBandolier(const EQApplicationPacket *app) {

	// Store bandolier set with the number and name passed by the client, along with the items that are currently
	// in the players weapon slots.

	BandolierCreate_Struct *bs = (BandolierCreate_Struct*)app->pBuffer;

	
	if ( bs->number >= MAX_PLAYER_BANDOLIER )
		return;
	
	char bandoName[32];
	int targetLength = sanitizeCharArray(bs->name, (char*)bandoName, 32);

	_log(INVENTORY__BANDOLIER, "Char: %s Creating Bandolier Set %i, Set Name: %s", GetName(), bs->number, bandoName);
	strcpy(m_pp.bandoliers[bs->number].name, bandoName);

	const ItemInst* InvItem = NULL;

	const Item_Struct *BaseItem = NULL;

	sint16 WeaponSlot = 0;

	for(int BandolierSlot = bandolierMainHand; BandolierSlot <= bandolierAmmo; BandolierSlot++) {
		WeaponSlot = BandolierSlotToWeaponSlot(BandolierSlot);
		InvItem = GetInv()[WeaponSlot];
		if(InvItem) {
			BaseItem = InvItem->GetItem();
			_log(INVENTORY__BANDOLIER, "Char: %s adding item %s to slot %i", GetName(),BaseItem->Name, WeaponSlot);
			m_pp.bandoliers[bs->number].items[BandolierSlot].item_id = BaseItem->ID;
			m_pp.bandoliers[bs->number].items[BandolierSlot].icon = BaseItem->Icon;
		}
		else {
			_log(INVENTORY__BANDOLIER, "Char: %s no item in slot %i", GetName(), WeaponSlot);
			m_pp.bandoliers[bs->number].items[BandolierSlot].item_id = 0;
			m_pp.bandoliers[bs->number].items[BandolierSlot].icon = 0;
		}
	}
	Save();
}

void	Client::RemoveBandolier(const EQApplicationPacket *app) {

	// Delete bandolier with the specified number

	BandolierDelete_Struct *bds = (BandolierDelete_Struct*)app->pBuffer;
	_log(INVENTORY__BANDOLIER, "Char: %s removing set", GetName(), bds->number);
	
	if ( bds->number >= MAX_PLAYER_BANDOLIER )
		return;
	
	memset(m_pp.bandoliers[bds->number].name, 0, 32);
	for(int i = bandolierMainHand; i <= bandolierAmmo; i++) {
		m_pp.bandoliers[bds->number].items[i].item_id = 0;
		m_pp.bandoliers[bds->number].items[i].icon = 0;
	}
	Save();
}


void	Client::SetBandolier(const EQApplicationPacket *app) {

	// Swap the weapons in the given bandolier set into the character's weapon slots and return
	// any items currently in the weapon slots to inventory.

	BandolierSet_Struct *bss = (BandolierSet_Struct*)app->pBuffer;
	_log(INVENTORY__BANDOLIER, "Char: %s activating set %i", GetName(), bss->number);
	sint16 slot;
	sint16 WeaponSlot;
	ItemInst *BandolierItems[4];  // Temporary holding area for the weapons we pull out of their inventory
	sint16	BandolierSlots[4];
	BandolierItems[0] = NULL;
	BandolierSlots[0] = SLOT_INVALID;
	BandolierItems[1] = NULL;
	BandolierSlots[1] = SLOT_INVALID;
	BandolierItems[2] = NULL;
	BandolierSlots[2] = SLOT_INVALID;
	BandolierItems[3] = NULL;
	BandolierSlots[3] = SLOT_INVALID;

	// First we pull the items for this bandolier set out of their inventory, this makes space to put the
	// currently equipped items back.
	for(int BandolierSlot = bandolierMainHand; BandolierSlot <= bandolierAmmo; BandolierSlot++) {
		// If this bandolier set has an item in this position
		if(m_pp.bandoliers[bss->number].items[BandolierSlot].item_id) {
			WeaponSlot = BandolierSlotToWeaponSlot(BandolierSlot);

			// Check if the player has the item specified in the bandolier set on them.
			//
			ItemInst* tmpItem = m_inv.GetItem(WeaponSlot);
			if ( tmpItem && tmpItem->GetItem()->ID == m_pp.bandoliers[bss->number].items[BandolierSlot].item_id )
			{
				if ( RuleB(Character,LogBandolierActivity) )
					LogFile->write(EQEMuLog::Error, "%s BANDOLIER: DEFAULT SELECTED %i with BandolierSlot %i, item id %i (%s).", GetName(), WeaponSlot, BandolierSlot, m_pp.bandoliers[bss->number].items[BandolierSlot].item_id, tmpItem->GetItem()->Name);

				BandolierSlots[BandolierSlot] = WeaponSlot;
				continue;
			}

			slot = m_inv.HasItem(m_pp.bandoliers[bss->number].items[BandolierSlot].item_id, 1, 
				invWhereWorn|invWherePersonal|invWhereCursor);
			
			if ( RuleB(Character,LogBandolierActivity) )
				LogFile->write(EQEMuLog::Error, "%s BANDOLIER: TEST %i with WeaponSlot %i, BandolierSlot %i, item id %i.", GetName(), slot, WeaponSlot, BandolierSlot, m_pp.bandoliers[bss->number].items[BandolierSlot].item_id);

			bool retry = false;
			int attempts = 0;
			do
			{
			retry = false;
			
			for(int i=0;i<4;i++)
			{
				if ( BandolierSlots[i] != SLOT_INVALID && BandolierSlots[i] == slot )
				{
					if ( RuleB(Character,LogBandolierActivity) )
						LogFile->write(EQEMuLog::Error, "%s BANDOLIER: CONFLICT slot %i with BandolierSlot %i, item id %i.", GetName(), slot, i, m_pp.bandoliers[bss->number].items[BandolierSlot].item_id);

					slot = m_inv.HasItem(m_pp.bandoliers[bss->number].items[BandolierSlot].item_id, 1, 
						invWherePersonal|invWhereCursor);

					if ( slot == SLOT_INVALID )
					{
						for(int z=3;z>attempts;z--)
						{
							sint16 testSlot = BandolierSlotToWeaponSlot(z);
							ItemInst* testItem = m_inv.GetItem(testSlot);
							if ( testItem && testItem->GetItem()->ID == m_pp.bandoliers[bss->number].items[BandolierSlot].item_id )
							{
								if ( RuleB(Character,LogBandolierActivity) )
									LogFile->write(EQEMuLog::Error, "%s BANDOLIER: TEST REVERSE slot %i with BandolierSlot %i, item id %i.", GetName(), slot, i, m_pp.bandoliers[bss->number].items[BandolierSlot].item_id);

								slot = testSlot;
								break;
							}
						}
					}

					retry = true;
					break;
				}
			}

			attempts++;
			if ( attempts > 2 )
			{
				if ( RuleB(Character,LogBandolierActivity) )
					LogFile->write(EQEMuLog::Error, "%s BANDOLIER: FAILED CONFLICT slot %i with WeaponSlot %i (bandolierslot %i), item id %i.", GetName(), slot, WeaponSlot, BandolierSlot, m_pp.bandoliers[bss->number].items[BandolierSlot].item_id);
				if ( retry )
					slot = SLOT_INVALID;
				retry = false;
				
				break;
			}
			}while(retry);

			if ( RuleB(Character,LogBandolierActivity) )
				LogFile->write(EQEMuLog::Error, "%s BANDOLIER: SELECTED %i with WeaponSlot %i, BandolierSlot %i, item id %i.", GetName(), slot, WeaponSlot, BandolierSlot, m_pp.bandoliers[bss->number].items[BandolierSlot].item_id);

			// if the player has this item in their inventory,
			if(slot != SLOT_INVALID) {
					// Pull the item out of the inventory
					BandolierSlots[BandolierSlot] = slot;
					BandolierItems[BandolierSlot] = m_inv.PopItem(slot);
					// If ammo with charges, only take one charge out to put in the range slot, that is what
					// the client does.

					if(((BandolierSlot == bandolierAmmo) || (BandolierSlot == bandolierRange)) && 
						BandolierItems[BandolierSlot] && BandolierItems[BandolierSlot]->IsStackable()){
							int Charges = BandolierItems[BandolierSlot]->GetCharges();
							// If there is more than one charge
							if(Charges > 1) {
								BandolierItems[BandolierSlot]->SetCharges(Charges-1);
								// Take one charge out and put the rest back
								m_inv.PutItem(slot, *BandolierItems[BandolierSlot]);
								database.SaveInventory(character_id, BandolierItems[BandolierSlot], slot, &m_inv, zone->GetZoneID());
								BandolierItems[BandolierSlot]->SetCharges(1);
							}
							else	// Remove the item from the inventory
								database.SaveInventory(character_id, 0, slot, &m_inv, zone->GetZoneID());
					}
					else	// Remove the item from the inventory
						database.SaveInventory(character_id, 0, slot, &m_inv, zone->GetZoneID());
			}
			else { // The player doesn't have the required weapon with them.
				BandolierItems[BandolierSlot] = 0;
				if(slot == SLOT_INVALID) {
					_log(INVENTORY__BANDOLIER, "Character does not have required bandolier item for slot %i", WeaponSlot);
					ItemInst *InvItem = m_inv.PopItem(WeaponSlot);
					if(InvItem) {
						// If there was an item in that weapon slot, put it in the inventory	
						_log(INVENTORY__BANDOLIER, "returning item %s in weapon slot %i to inventory",
							InvItem->GetItem()->Name, WeaponSlot);
					if(MoveItemToInventory(InvItem)) 
							database.SaveInventory(character_id, 0, WeaponSlot, &m_inv, zone->GetZoneID());
						else
							_log(INVENTORY__BANDOLIER, "Char: %s, ERROR returning %s to inventory", GetName(),
							InvItem->GetItem()->Name);
						safe_delete(InvItem);
					}

				}
			}
		}
	}

	// Now we move the required weapons into the character weapon slots, and return any items we are replacing
	// back to inventory.
	//
	for(int BandolierSlot = bandolierMainHand; BandolierSlot <= bandolierAmmo; BandolierSlot++) {

		// Find the inventory slot corresponding to this bandolier slot

		WeaponSlot = BandolierSlotToWeaponSlot(BandolierSlot);
		
		ItemInst* tmpItem = m_inv.GetItem(WeaponSlot);
		if ( tmpItem && tmpItem->GetItem()->ID == m_pp.bandoliers[bss->number].items[BandolierSlot].item_id )
			continue;

		if(m_pp.bandoliers[bss->number].items[BandolierSlot].item_id) {

			if ( BandolierItems[BandolierSlot] )
			{
			// if the player has this item in their inventory, and it is not already where it needs to be
				// Pull the item that we are going to replace
				ItemInst *InvItem = m_inv.PopItem(WeaponSlot);
				// Put the item specified in the bandolier where it needs to be
				m_inv.PutItem(WeaponSlot, *BandolierItems[BandolierSlot]);

				safe_delete(BandolierItems[BandolierSlot]);
				// Update the database, save the item now in the weapon slot
				database.SaveInventory(character_id, m_inv.GetItem(WeaponSlot), WeaponSlot, &m_inv, zone->GetZoneID());

				if(InvItem) {
					// If there was already an item in that weapon slot that we replaced, find a place to put it
					if(!MoveItemToInventory(InvItem)) 
						_log(INVENTORY__BANDOLIER, "Char: %s, ERROR returning %s to inventory", GetName(),
						InvItem->GetItem()->Name);
					safe_delete(InvItem);
				}
			}
		}
		else {
			// This bandolier set has no item for this slot, so take whatever is in the weapon slot and
			// put it in the player's inventory.
			ItemInst *InvItem = m_inv.PopItem(WeaponSlot);
			if(InvItem) {
				_log(INVENTORY__BANDOLIER, "Bandolier has no item for slot %i, returning item %s to inventory",
					WeaponSlot, InvItem->GetItem()->Name);
				// If there was an item in that weapon slot, put it in the inventory	
					if(MoveItemToInventory(InvItem)) 
					database.SaveInventory(character_id, 0, WeaponSlot, &m_inv, zone->GetZoneID());
				else
					_log(INVENTORY__BANDOLIER, "Char: %s, ERROR returning %s to inventory", GetName(),
					InvItem->GetItem()->Name);
				safe_delete(InvItem);
			}
		}
	}
	// finally, recalculate any stat bonuses from the item change
	SetAttackTimer();
	CalcBonuses();
}

bool Client::MoveItemToInventory(ItemInst *ItemToReturn, bool UpdateClient) {

	// This is a support function for Client::SetBandolier, however it can be used anywhere it's functionality is required.
	//
	// When the client moves items around as Bandolier sets are activated, it does not send details to the 
	// server of what item it has moved to which slot. It assumes the server knows what it will do.
	//
	// The standard EQEmu auto inventory routines do not behave as the client does when manipulating bandoliers. 
	// The client will look in each main inventory slot. If it finds a bag in a slot, it will then look inside
	// the bag for a free slot.
	//
	// This differs from the standard EQEmu method of looking in all 8 inventory slots first to find an empty slot, and
	// then going back and looking in bags. There are also other differences related to how it moves stackable items back
	// to inventory.
	//
	// Rather than alter the current auto inventory behaviour, just in case something
	// depends on current behaviour, this routine operates the same as the client when moving items back to inventory when
	// swapping bandolier sets.

	if(!ItemToReturn) return false;

	_log(INVENTORY__SLOTS,"Char: %s Returning %s to inventory", GetName(), ItemToReturn->GetItem()->Name);

	int32 ItemID = ItemToReturn->GetItem()->ID;

	// If the item is stackable (ammo in range slot), try stacking it with other items of the same type
	//
	if(ItemToReturn->IsStackable()) {

		for (sint16 i=22; i<=29; i++) {

			ItemInst* InvItem = m_inv.GetItem(i);

			if(InvItem && (InvItem->GetItem()->ID == ItemID) && (InvItem->GetCharges() < InvItem->GetItem()->StackSize)) {

				int ChargeSlotsLeft = InvItem->GetItem()->StackSize - InvItem->GetCharges();

				int ChargesToMove = ItemToReturn->GetCharges() < ChargeSlotsLeft ? ItemToReturn->GetCharges() : 
					ChargeSlotsLeft;

				InvItem->SetCharges(InvItem->GetCharges() + ChargesToMove);

				if(UpdateClient)
					SendItemPacket(i, InvItem, ItemPacketTrade);

				database.SaveInventory(character_id, m_inv.GetItem(i), i, &m_inv, zone->GetZoneID());

				ItemToReturn->SetCharges(ItemToReturn->GetCharges() - ChargesToMove);

				if(!ItemToReturn->GetCharges()) 
					return true;
			}
			// If there is a bag in this slot, look inside it.
			//
			if (InvItem && InvItem->IsType(ItemClassContainer)) {

				sint16 BaseSlotID = Inventory::CalcSlotId(i, 0);

				int8 BagSize=InvItem->GetItem()->BagSlots;

				uint8 BagSlot;
				for (BagSlot=0; BagSlot<BagSize; BagSlot++) {
					InvItem = m_inv.GetItem(BaseSlotID + BagSlot);
					if(InvItem && (InvItem->GetItem()->ID == ItemID) && 
						(InvItem->GetCharges() < InvItem->GetItem()->StackSize)) {

							int ChargeSlotsLeft = InvItem->GetItem()->StackSize - InvItem->GetCharges();

							int ChargesToMove = ItemToReturn->GetCharges() < ChargeSlotsLeft 
								? ItemToReturn->GetCharges() : ChargeSlotsLeft;

							InvItem->SetCharges(InvItem->GetCharges() + ChargesToMove);

							if(UpdateClient)
								SendItemPacket(BaseSlotID + BagSlot, m_inv.GetItem(BaseSlotID + BagSlot), 
								ItemPacketTrade);

							database.SaveInventory(character_id, m_inv.GetItem(BaseSlotID + BagSlot), 
								BaseSlotID + BagSlot, &m_inv, zone->GetZoneID());

							ItemToReturn->SetCharges(ItemToReturn->GetCharges() - ChargesToMove);

							if(!ItemToReturn->GetCharges()) 
								return true;
					}		
				}
			}
		}
	}

	// We have tried stacking items, now just try and find an empty slot.

	for (sint16 i=22; i<=29; i++) {

		ItemInst* InvItem = m_inv.GetItem(i);

		if (!InvItem) {
			// Found available slot in personal inventory
			m_inv.PutItem(i, *ItemToReturn);

			if(UpdateClient)
				SendItemPacket(i, ItemToReturn, ItemPacketTrade);

			database.SaveInventory(character_id, m_inv.GetItem(i), i, &m_inv, zone->GetZoneID());

			_log(INVENTORY__SLOTS, "Char: %s Storing in main inventory slot %i", GetName(), i);

			return true;
		}
		if(InvItem->IsType(ItemClassContainer) && Inventory::CanItemFitInContainer(ItemToReturn->GetItem(), InvItem->GetItem())) {

			sint16 BaseSlotID = Inventory::CalcSlotId(i, 0);

			int8 BagSize=InvItem->GetItem()->BagSlots;

			for (uint8 BagSlot=0; BagSlot<BagSize; BagSlot++) {

				InvItem = m_inv.GetItem(BaseSlotID + BagSlot);

				if (!InvItem) {
					// Found available slot within bag
					m_inv.PutItem(BaseSlotID + BagSlot, *ItemToReturn);

					if(UpdateClient)
						SendItemPacket(BaseSlotID + BagSlot, ItemToReturn, ItemPacketTrade);

					database.SaveInventory(character_id, m_inv.GetItem(BaseSlotID + BagSlot), BaseSlotID + BagSlot, &m_inv, zone->GetZoneID());

					_log(INVENTORY__SLOTS, "Char: %s Storing in bag slot %i", GetName(), BaseSlotID + BagSlot);

					return true;
				}
			}
		}
	}

	// Store on the cursor
	// 
	_log(INVENTORY__SLOTS, "Char: %s No space, putting on the cursor", GetName());

	PushItemOnCursor(*ItemToReturn, UpdateClient);

	return true;
}