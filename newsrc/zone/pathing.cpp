#include "../common/debug.h"
#include <string.h>
#include <math.h>
#include <list>
#include <algorithm>
#include <sstream>
#include <fstream>
#include "pathing.h"
#include "watermap.h"
#include "../common/files.h"
#include "../common/MiscFunctions.h"
#include "doors.h"
#include "client.h"
#include "zone.h"

#ifdef WIN32
#define snprintf        _snprintf
#endif

#define ABS(x) ((x)<0?-(x):(x))

extern Zone *zone;

float VertexDistance(VERTEX a, VERTEX b)
{
	_ZP(Pathing_VertexDistance);

	float xdist = a.x - b.x;
	float ydist = a.y - b.y;
	float zdist = a.z - b.z;
	return sqrtf(xdist * xdist + ydist * ydist + zdist * zdist);
}

float VertexDistanceNoRoot(VERTEX a, VERTEX b)
{
	_ZP(Pathing_VertexDistanceNoRoot);

	float xdist = a.x - b.x;
	float ydist = a.y - b.y;
	float zdist = a.z - b.z;
	return xdist * xdist + ydist * ydist + zdist * zdist;

}

PathManager* PathManager::LoadPathFile(const char* ZoneName)
{

	FILE *PathFile = NULL;

	char LowerCaseZoneName[64];

	char ZonePathFileName[256];

	PathManager* Ret = NULL;

	strn0cpy(LowerCaseZoneName, ZoneName, 64);
	
	strlwr(LowerCaseZoneName);

	snprintf(ZonePathFileName, 250, MAP_DIR "/%s.path", LowerCaseZoneName);

	if((PathFile = fopen(ZonePathFileName, "rb")))
	{
		Ret = new PathManager();

		if(Ret->loadPaths(PathFile))
		{
			LogFile->write(EQEMuLog::Status, "Path File %s loaded.", ZonePathFileName);

		}
		else
		{
			LogFile->write(EQEMuLog::Error, "Path File %s failed to load.", ZonePathFileName);
			safe_delete(Ret);
		}
		fclose(PathFile);
	}
	else
	{
		LogFile->write(EQEMuLog::Error, "Path File %s not found.", ZonePathFileName);
	}

	return Ret;
}

PathManager::PathManager()
{
	PathNodes = NULL;
	ClosedListFlag = NULL;
	Head.PathNodeCount = 0;
	Head.version = 2;
	QuickConnectTarget = -1;
}

PathManager::~PathManager()
{
	safe_delete_array(PathNodes);
	safe_delete_array(ClosedListFlag);
}

bool PathManager::loadPaths(FILE *PathFile)
{

	char Magic[10];

	fread(&Magic, 9, 1, PathFile);

	if(strncmp(Magic, "EQEMUPATH", 9))
	{
		LogFile->write(EQEMuLog::Error, "Bad Magic String in .path file.");
		return false;
	}

	fread(&Head, sizeof(Head), 1, PathFile);

	LogFile->write(EQEMuLog::Status, "Path File Header: Version %ld, PathNodes %ld",
		       (long)Head.version, (long)Head.PathNodeCount);

	if(Head.version != 2)
	{
		LogFile->write(EQEMuLog::Error, "Unsupported path file version.");
		return false;
	}

	PathNodes = new PathNode[Head.PathNodeCount];
	
	fread(PathNodes, sizeof(PathNode), Head.PathNodeCount, PathFile);

	ClosedListFlag = new int[Head.PathNodeCount];

	if ( RuleB(Pathing,PrintPathing) )
		PrintPathing();

	int MaxNodeID = Head.PathNodeCount - 1;

	bool PathFileValid = true;

	for(int32 i = 0; i < Head.PathNodeCount; ++i)
	{
		for(int32 j = 0; j < PATHNODENEIGHBOURS; ++j)
		{
			if(PathNodes[i].Neighbours[j].id > MaxNodeID)
			{
				LogFile->write(EQEMuLog::Error, "Path Node %i, Neighbour %i (%i) out of range.", i, j, PathNodes[i].Neighbours[j].id);

				PathFileValid = false;
			}
		}
	}

	if(!PathFileValid)
	{
		safe_delete_array(PathNodes);
	}

	return PathFileValid;
}

void PathManager::PrintPathing()
{

	for(int32 i = 0; i < Head.PathNodeCount; ++i)
	{
		printf("PathNode: %2d id %2d. (%8.3f, %8.3f, %8.3f), BestZ: %8.3f\n",
		       i, PathNodes[i].id, PathNodes[i].v.x, PathNodes[i].v.y, PathNodes[i].v.z, PathNodes[i].bestz);
		       

		if(PathNodes[i].Neighbours[0].id == -1)
		{
			printf("  NO NEIGHBOURS.\n");
			continue;
		}

		for(int j=0; j<PATHNODENEIGHBOURS; j++)
		{
			if(PathNodes[i].Neighbours[j].id == -1)
				break;

			printf("  Neighbour: %2d, Distance %8.3f", PathNodes[i].Neighbours[j].id,
			       PathNodes[i].Neighbours[j].distance);

			if(PathNodes[i].Neighbours[j].Teleport)
				printf("    ***** TELEPORT *****");

			if(PathNodes[i].Neighbours[j].DoorID >= 0)
				printf("    ***** via door %i *****", PathNodes[i].Neighbours[j].DoorID);

			printf("\n");
		}
	}
}

VERTEX PathManager::GetPathNodeCoordinates(int NodeNumber, bool BestZ)
{
	VERTEX Result;

	if(NodeNumber < Head.PathNodeCount)
	{
		Result = PathNodes[NodeNumber].v;

		if(!BestZ)
			return Result;

		Result.z = PathNodes[NodeNumber].bestz;
	}

	return Result;
	
}

list<int> PathManager::FindRoute(int startID, int endID)
{
	_ZP(Pathing_FindRoute_FromNodes);

	_log(PATHING__DEBUG, "FindRoute from node %i to %i", startID, endID);

	memset(ClosedListFlag, 0, sizeof(int) * Head.PathNodeCount);

	list<AStarNode> OpenList, ClosedList;

	list<int>Route;

	AStarNode AStarEntry, CurrentNode;

	AStarEntry.PathNodeID = startID;
	AStarEntry.Parent = -1;
	AStarEntry.HCost = 0;
	AStarEntry.GCost = 0;
	AStarEntry.Teleport = false;

	OpenList.push_back(AStarEntry);

	while(OpenList.size() > 0)
	{
		// The OpenList is maintained in sorted order, lowest to highest cost.

		CurrentNode = (*OpenList.begin());

		ClosedList.push_back(CurrentNode);
		
		ClosedListFlag[CurrentNode.PathNodeID] = true;

		OpenList.pop_front();

		for(int i = 0; i < PATHNODENEIGHBOURS; ++i)
		{
			if(PathNodes[CurrentNode.PathNodeID].Neighbours[i].id == -1)
				break;

			if(PathNodes[CurrentNode.PathNodeID].Neighbours[i].id == CurrentNode.Parent)
				continue;

			if(PathNodes[CurrentNode.PathNodeID].Neighbours[i].id == endID)
			{
				Route.push_back(CurrentNode.PathNodeID);

				Route.push_back(endID);
			
				list<AStarNode>::iterator RouteIterator;

				while(CurrentNode.PathNodeID != startID)
				{
					for(RouteIterator = ClosedList.begin(); RouteIterator != ClosedList.end(); ++RouteIterator)
					{
						if((*RouteIterator).PathNodeID == CurrentNode.Parent)
						{
							if(CurrentNode.Teleport)
								Route.insert(Route.begin(), -1);

							CurrentNode = (*RouteIterator);
							
							Route.insert(Route.begin(), CurrentNode.PathNodeID);

							break;
						}
					}
				}

				return Route;
			}
			if(ClosedListFlag[PathNodes[CurrentNode.PathNodeID].Neighbours[i].id])
				continue;

			AStarEntry.PathNodeID = PathNodes[CurrentNode.PathNodeID].Neighbours[i].id;

			AStarEntry.Parent = CurrentNode.PathNodeID;

			AStarEntry.Teleport = PathNodes[CurrentNode.PathNodeID].Neighbours[i].Teleport;

			// HCost is the estimated cost to get from this node to the end.
			AStarEntry.HCost = VertexDistance(PathNodes[PathNodes[CurrentNode.PathNodeID].Neighbours[i].id].v,
											PathNodes[endID].v);

			AStarEntry.GCost = CurrentNode.GCost + PathNodes[CurrentNode.PathNodeID].Neighbours[i].distance;

			float FCost = AStarEntry.HCost +  AStarEntry.GCost;

			if( RuleB(Pathing,PrintPathing) )
			{
			printf("Node: %i, Open Neighbour %i has HCost %8.3f, GCost %8.3f (Total Cost: %8.3f)\n",
					CurrentNode.PathNodeID, 
					PathNodes[CurrentNode.PathNodeID].Neighbours[i].id,
					AStarEntry.HCost,
					AStarEntry.GCost,
					AStarEntry.HCost + AStarEntry.GCost);
			}

			bool AlreadyInOpenList = false;

			list<AStarNode>::iterator OpenListIterator, InsertionPoint = OpenList.end();

			for(OpenListIterator = OpenList.begin(); OpenListIterator != OpenList.end(); ++OpenListIterator)
			{
				if((*OpenListIterator).PathNodeID == PathNodes[CurrentNode.PathNodeID].Neighbours[i].id)
				{
					AlreadyInOpenList = true;

					float GCostToNode = CurrentNode.GCost +  PathNodes[CurrentNode.PathNodeID].Neighbours[i].distance;

					if(GCostToNode < (*OpenListIterator).GCost)
					{
						(*OpenListIterator).Parent = CurrentNode.PathNodeID;

						(*OpenListIterator).GCost = GCostToNode;

						(*OpenListIterator).Teleport = PathNodes[CurrentNode.PathNodeID].Neighbours[i].Teleport;
					}
					break;
				}
				else if((InsertionPoint == OpenList.end()) && (((*OpenListIterator).HCost + (*OpenListIterator).GCost) > FCost))
				{
					InsertionPoint = OpenListIterator;
				}
			}
			if(!AlreadyInOpenList)
				OpenList.insert(InsertionPoint, AStarEntry);
		}

	}
	_log(PATHING__DEBUG, "Unable to find a route.");
	return Route;

}

bool CheckLOSBetweenPoints(VERTEX start, VERTEX end) {

	VERTEX hit;
	FACE *face;

	if((zone->zonemap) && (zone->zonemap->LineIntersectsZone(start, end, 1, &hit, &face))) return false;

	return true;
}

bool SortPathNodesByDistance(PathNodeSortStruct n1, PathNodeSortStruct n2)
{
	return n1.Distance < n2.Distance;
}

list<int> PathManager::FindRoute(VERTEX Start, VERTEX End)
{

	_ZP(Pathing_FindRoute_FromVertices);

	if(RuleB(Pathing,PrintPathing) )
		printf( "FindRoute(%8.3f, %8.3f, %8.3f,  %8.3f, %8.3f, %8.3f)", Start.x, Start.y, Start.z, End.x, End.y, End.z);

	list<int> noderoute;

	float CandidateNodeRangeXY = RuleR(Pathing, CandidateNodeRangeXY);

	float CandidateNodeRangeZ = RuleR(Pathing, CandidateNodeRangeZ);

	// Find the nearest PathNode the Start has LOS to.
	//
	//
	int ClosestPathNodeToStart = -1;

	list<PathNodeSortStruct> SortedByDistance;

	PathNodeSortStruct TempNode;

	for(int i = 0 ; i < Head.PathNodeCount; ++i)
	{
		if((ABS(Start.x - PathNodes[i].v.x) <= CandidateNodeRangeXY) &&
		   (ABS(Start.y - PathNodes[i].v.y) <= CandidateNodeRangeXY) &&
		   (ABS(Start.z - PathNodes[i].v.z) <= CandidateNodeRangeZ))
		{
		   	TempNode.id = i;
			TempNode.Distance = VertexDistanceNoRoot(Start, PathNodes[i].v);
			SortedByDistance.push_back(TempNode);

		}
	}
	
	SortedByDistance.sort(SortPathNodesByDistance);

	for(list<PathNodeSortStruct>::iterator Iterator = SortedByDistance.begin(); Iterator != SortedByDistance.end(); ++Iterator)
	{
		if(RuleB(Pathing,PrintPathing) )
			printf("Checking Reachability of Node %i from Start Position.\n", PathNodes[(*Iterator).id].id);

		if(!zone->zonemap->LineIntersectsZone(Start, PathNodes[(*Iterator).id].v, 1.0f, NULL, NULL))
		{
			ClosestPathNodeToStart = (*Iterator).id;
			break;
		}
	}

	if(ClosestPathNodeToStart <0 ) {
		_log(PATHING__DEBUG, "No LOS to any starting Path Node within range.\n");
		return noderoute;
	}
	
		if(RuleB(Pathing,PrintPathing) )
			printf("Closest Path Node To Start: %2d\n", ClosestPathNodeToStart);

	// Find the nearest PathNode the end point has LOS to

	int ClosestPathNodeToEnd = -1;

	SortedByDistance.clear();

	for(int i = 0 ; i < Head.PathNodeCount; ++i)
	{
		if((ABS(End.x - PathNodes[i].v.x) <= CandidateNodeRangeXY) &&
		   (ABS(End.y - PathNodes[i].v.y) <= CandidateNodeRangeXY) &&
		   (ABS(End.z - PathNodes[i].v.z) <= CandidateNodeRangeZ))
		{
		   	TempNode.id = i;
			TempNode.Distance = VertexDistanceNoRoot(End, PathNodes[i].v);
			SortedByDistance.push_back(TempNode);
		}
	}
	
	SortedByDistance.sort(SortPathNodesByDistance);

	for(list<PathNodeSortStruct>::iterator Iterator = SortedByDistance.begin(); Iterator != SortedByDistance.end(); ++Iterator)
	{
		if(RuleB(Pathing,PrintPathing) )
		{
			printf("Checking Reachability of Node %i from End Position.\n", PathNodes[(*Iterator).id].id);
			printf(" (%8.3f, %8.3f, %8.3f) to (%8.3f, %8.3f, %8.3f)\n",End.x, End.y, End.z,
			PathNodes[(*Iterator).id].v.x, PathNodes[(*Iterator).id].v.y, PathNodes[(*Iterator).id].v.z);
		}

		if(!zone->zonemap->LineIntersectsZone(End, PathNodes[(*Iterator).id].v, 1.0f, NULL, NULL))
		{
			ClosestPathNodeToEnd = (*Iterator).id;
			break;
		}
	}
	
	if(ClosestPathNodeToEnd < 0) {
		
		if(RuleB(Pathing,PrintPathing) )
		{
			printf("No LOS to any end Path Node within range.\n");
		}
		return noderoute;
	}

	
		if(RuleB(Pathing,PrintPathing) )
		{
			printf("Closest Path Node To End: %2d\n", ClosestPathNodeToEnd);
		}

	if(ClosestPathNodeToStart == ClosestPathNodeToEnd)
	{
		noderoute.push_back(ClosestPathNodeToStart);
		return noderoute;
	}
	noderoute = FindRoute(ClosestPathNodeToStart, ClosestPathNodeToEnd);

	int NodesToAttemptToCull = RuleI(Pathing, CullNodesFromStart);

	if(NodesToAttemptToCull > 0)
	{
		int CulledNodes = 0;
		
		list<int>::iterator First, Second;

		while((noderoute.size() >= 2) && (CulledNodes < NodesToAttemptToCull))
		{
			First = noderoute.begin();

			Second = First;

			++Second;

			if((*Second) < 0)
				break;

			if(!zone->zonemap->LineIntersectsZone(Start, PathNodes[(*Second)].v, 1.0f, NULL, NULL)
			   && zone->pathing->NoHazards(Start, PathNodes[(*Second)].v))
			{
				noderoute.erase(First);

				++CulledNodes;
			}
			else
				break;
		}
	}
				
	NodesToAttemptToCull = RuleI(Pathing, CullNodesFromEnd);

	if(NodesToAttemptToCull > 0)
	{
		int CulledNodes = 0;
		
		list<int>::iterator First, Second;

		while((noderoute.size() >= 2) && (CulledNodes < NodesToAttemptToCull))
		{
			First = noderoute.end();

			--First;

			Second = First;

			--Second;

			if((*Second) < 0)
				break;

			if(!zone->zonemap->LineIntersectsZone(End, PathNodes[(*Second)].v, 1.0f, NULL, NULL)
			   && zone->pathing->NoHazards(End, PathNodes[(*Second)].v))
			{
				noderoute.erase(First);

				++CulledNodes;
			}
			else
				break;
		}
	}

	return noderoute;
}

const char* DigitToWord(int i)
{
	switch(i) {
		case 0:
			return "zero";
		case 1:
			return "one";
		case 2:
			return "two";
		case 3:
			return "three";
		case 4:
			return "four";
		case 5:
			return "five";
		case 6:
			return "six";
		case 7:
			return "seven";
		case 8:
			return "eight";
		case 9:
			return "nine";
	}
	return "";
}

void PathManager::SpawnPathNodes()
{

	for(int i = 0; i < Head.PathNodeCount; ++i)
	{
		NPCType* npc_type = new NPCType;
		memset(npc_type, 0, sizeof(NPCType));

		if(PathNodes[i].id < 10)
			sprintf(npc_type->name, "%s", DigitToWord(PathNodes[i].id));
		else if(PathNodes[i].id < 100)
			sprintf(npc_type->name, "%s_%s", DigitToWord(PathNodes[i].id/10), DigitToWord(PathNodes[i].id % 10));
		else
			sprintf(npc_type->name, "%s_%s_%s", DigitToWord(PathNodes[i].id/100), DigitToWord((PathNodes[i].id % 100)/10), 
				DigitToWord(((PathNodes[i].id % 100) %10)));

		sprintf(npc_type->lastname, "%i", PathNodes[i].id);
		npc_type->cur_hp = 4000000;
		npc_type->max_hp = 4000000;
		npc_type->race = 151;
		npc_type->gender = 2;
		npc_type->class_ = 9;
		npc_type->deity= 1;
		npc_type->level = 75;
		npc_type->npc_id = 0;
		npc_type->loottable_id = 0;
		npc_type->texture = 1;
		npc_type->light = 0;
		npc_type->runspeed = 0;
		npc_type->d_meele_texture1 = 1;
		npc_type->d_meele_texture2 = 1;
		npc_type->merchanttype = 1;
		npc_type->MerchantMinLevel = 0;
		npc_type->bodytype = 1;
	
		npc_type->STR = 150;
		npc_type->STA = 150;
		npc_type->DEX = 150;
		npc_type->AGI = 150;
		npc_type->INT = 150;
		npc_type->WIS = 150;
		npc_type->CHA = 150;

		npc_type->findable = 1;
	
		NPC* npc = new NPC(npc_type, 0, PathNodes[i].v.x, PathNodes[i].v.y, PathNodes[i].v.z, 0, FlyMode1);
		npc->GiveNPCTypeData(npc_type);
	
		entity_list.AddNPC(npc, true, true);
	}
}

void PathManager::MeshTest()
{
	// This will test connectivity between all path nodes

	int TotalTests = 0;
	int NoConnections = 0;

	printf("Beginning Pathmanager connectivity tests.\n"); fflush(stdout);

	for(int i = 0; i < Head.PathNodeCount; ++i)
	{
		for(int j = 0; j < Head.PathNodeCount; ++j)
		{
			if(j == i)
				continue;
			
			list<int> Route = FindRoute(PathNodes[i].id, PathNodes[j].id);

			if(Route.size() == 0)
			{
				++NoConnections;
				printf("FindRoute(%i, %i) **** NO ROUTE FOUND ****\n", PathNodes[i].id, PathNodes[j].id);
			}
			++TotalTests;	
		}
	}
	printf("Executed %i route searches.\n", TotalTests);
	printf("Failed to find %i routes.\n", NoConnections);
	fflush(stdout);
}

void PathManager::SimpleMeshTest()
{
	// This will test connectivity between the first path node and all other nodes

	int TotalTests = 0;
	int NoConnections = 0;

	printf("Beginning Pathmanager connectivity tests.\n"); 
	fflush(stdout);

	for(int j = 1; j < Head.PathNodeCount; ++j)
	{
		list<int> Route = FindRoute(PathNodes[0].id, PathNodes[j].id);

		if(Route.size() == 0)
		{
			++NoConnections;
			printf("FindRoute(%i, %i) **** NO ROUTE FOUND ****\n", PathNodes[0].id, PathNodes[j].id);
		}
		++TotalTests;	
	}
	printf("Executed %i route searches.\n", TotalTests);
	printf("Failed to find %i routes.\n", NoConnections);
	fflush(stdout);
}

VERTEX Mob::UpdatePath(float ToX, float ToY, float ToZ, float Speed, bool &WaypointChanged, bool &NodeReached)
{
	_ZP(Pathing_UpdatePath);

	WaypointChanged = false;

	NodeReached = false;

	VERTEX NodeLoc;

	VERTEX From(GetX(), GetY(), GetZ());

	VERTEX HeadPosition(From.x, From.y,  From.z + (GetSize() < 6.0 ? 6 : GetSize()) * HEAD_POSITION);

	VERTEX To(ToX, ToY, ToZ);

	bool SameDestination = (To == PathingDestination);

	int NextNode;

	if(To == From)
		return To;

	mlog(PATHING__DEBUG, "UpdatePath. From(%8.3f, %8.3f, %8.3f) To(%8.3f, %8.3f, %8.3f)", From.x, From.y, From.z, To.x, To.y, To.z);

	if(From == PathingLastPosition)
	{
		++PathingLoopCount;

		if((PathingLoopCount > 5) && !IsRooted())
		{
			mlog(PATHING__DEBUG, "appears to be stuck. Teleporting them to next position.", GetName());

			if(Route.size() == 0)
			{
				Teleport(To);

				WaypointChanged = true;

				PathingLoopCount = 0;

				return To;
			}
			NodeLoc = zone->pathing->GetPathNodeCoordinates(Route.front());

			Route.pop_front();

			++PathingTraversedNodes;

			Teleport(NodeLoc);

			WaypointChanged = true;

			PathingLoopCount = 0;

			return NodeLoc;
		}
	}
	else
	{
		PathingLoopCount = 0;

		PathingLastPosition = From;
	}
		
	if(Route.size() > 0)
	{

		// If we are already pathing, and the destination is the same as before ...
		if(SameDestination)
		{
			mlog(PATHING__DEBUG, "  Still pathing to the same destination.");

			// Get the coordinates of the first path node we are going to.
			NextNode = Route.front();

			NodeLoc = zone->pathing->GetPathNodeCoordinates(NextNode);

			// May need to refine this as rounding errors may mean we never have equality
			// We have reached the path node.
			if(NodeLoc == From)
			{
				mlog(PATHING__DEBUG, "  Arrived at node %i", NextNode);	

				NodeReached = true;

				PathingLastNodeVisited = Route.front();
				// We only check for LOS again after traversing more than 1 node, otherwise we can get into
				// a loop where we have a hazard and so run to a path node far enough away from the hazard, and
				// then run right back towards the same hazard again.
				//
				// An exception is when we are about to head for the last node. We always check LOS then. This
				// is because we are seeking a path to the node nearest to our target. This node may be behind the
				// target, and we may run past the target if we don't check LOS at this point.
				int RouteSize = Route.size();

				mlog(PATHING__DEBUG, "Route size is %i", RouteSize);

				if((RouteSize == 2)
					|| ((PathingTraversedNodes >= RuleI(Pathing, MinNodesTraversedForLOSCheck))
				   && (RouteSize <= RuleI(Pathing, MinNodesLeftForLOSCheck))
				   && PathingLOSCheckTimer->Check()))
				{
					mlog(PATHING__DEBUG, "  Checking distance to target.");
					float Distance = VertexDistanceNoRoot(From, To);

					mlog(PATHING__DEBUG, "  Distance between From and To (NoRoot) is %8.3f", Distance);

					if((Distance <= RuleR(Pathing, MinDistanceForLOSCheckShort))
					   && (ABS(From.z - To.z) <= RuleR(Pathing, ZDiffThreshold)))
					{
						if(!zone->zonemap->LineIntersectsZone(HeadPosition, To, 1.0f, NULL, NULL))
							PathingLOSState = HaveLOS;
						else
							PathingLOSState = NoLOS;
						mlog(PATHING__DEBUG, "  LOS stats is %s", (PathingLOSState == HaveLOS) ? "HaveLOS" : "NoLOS");	

						if((PathingLOSState == HaveLOS) && zone->pathing->NoHazards(From, To))
						{
							mlog(PATHING__DEBUG, "  No hazards. Running directly to target.");
							Route.clear();

							return To;
						}
						else
						{
							mlog(PATHING__DEBUG, "  Continuing on node path.");	
						}
					}
					else
						PathingLOSState = UnknownLOS;
				}
				// We are on the same route, no LOS (or not checking this time, so pop off the node we just reached
				//
				Route.pop_front();

				++PathingTraversedNodes;

				WaypointChanged = true;

				// If there are more nodes on the route, return the coords of the next node
				if(Route.size() > 0)
				{
					NextNode = Route.front();

					if(NextNode == -1)
					{
						// -1 indicates a teleport to the next node
						Route.pop_front();

						if(Route.size() == 0)
						{
							mlog(PATHING__DEBUG, "Missing node after teleport.");
							return To;
						}

						NextNode = Route.front();

						NodeLoc = zone->pathing->GetPathNodeCoordinates(NextNode);

						Teleport(NodeLoc);

						mlog(PATHING__DEBUG, "  TELEPORTED to %8.3f, %8.3f, %8.3f\n", NodeLoc.x, NodeLoc.y, NodeLoc.z);

						Route.pop_front();

						if(Route.size() == 0)
							return To;

						NextNode = Route.front();
					}
					zone->pathing->OpenDoors(PathingLastNodeVisited, NextNode, this);

					mlog(PATHING__DEBUG, "  Now moving to node %i", NextNode);

					return  zone->pathing->GetPathNodeCoordinates(NextNode);
				}
				else
				{	
					// we have run all the nodes, all that is left is the direct path from the last node
					// to the destination
					mlog(PATHING__DEBUG, "  Reached end of node path, running direct to target.");

					return To;
				}
			}
			// At this point, we are still on the previous path, but not reached a node yet.
			// The route shouldn't be empty, but check anyway.
			//
			int RouteSize = Route.size();

			if((PathingTraversedNodes >= RuleI(Pathing, MinNodesTraversedForLOSCheck))
			   && (RouteSize <= RuleI(Pathing, MinNodesLeftForLOSCheck))
			   && PathingLOSCheckTimer->Check())
			{
				mlog(PATHING__DEBUG, "  Checking distance to target.");

				float Distance = VertexDistanceNoRoot(From, To);

				mlog(PATHING__DEBUG, "  Distance between From and To (NoRoot) is %8.3f", Distance);

				if((Distance <= RuleR(Pathing, MinDistanceForLOSCheckShort))
				   && (ABS(From.z - To.z) <= RuleR(Pathing, ZDiffThreshold)))
				{
					if(!zone->zonemap->LineIntersectsZone(HeadPosition, To, 1.0f, NULL, NULL))
						PathingLOSState = HaveLOS;
					else
						PathingLOSState = NoLOS;
					mlog(PATHING__DEBUG, "  LOS stats is %s", (PathingLOSState == HaveLOS) ? "HaveLOS" : "NoLOS");	

					if((PathingLOSState == HaveLOS) && zone->pathing->NoHazards(From, To))
					{
						mlog(PATHING__DEBUG, "  No hazards. Running directly to target.");
						Route.clear();

						return To;
					}
					else
					{
						mlog(PATHING__DEBUG, "  Continuing on node path.");	
					}
				}
				else
					PathingLOSState = UnknownLOS;
			}
			return NodeLoc;
		}
		else
		{
			// We get here if we were already pathing, but our destination has now changed.
			//
			mlog(PATHING__DEBUG, "  Target has changed position.");
			// Update our record of where we are going to.
			PathingDestination = To;
			// Check if we now have LOS etc to the new destination.	
			if(PathingLOSCheckTimer->Check())
			{
				float Distance = VertexDistanceNoRoot(From, To);

				if((Distance <= RuleR(Pathing, MinDistanceForLOSCheckShort))
				   && (ABS(From.z - To.z) <= RuleR(Pathing, ZDiffThreshold)))
				{
					mlog(PATHING__DEBUG, "  Checking for short LOS at distance %8.3f.", Distance);
					if(!zone->zonemap->LineIntersectsZone(HeadPosition, To, 1.0f, NULL, NULL))
						PathingLOSState = HaveLOS;
					else
						PathingLOSState = NoLOS;
	
					mlog(PATHING__DEBUG, "  LOS stats is %s", (PathingLOSState == HaveLOS) ? "HaveLOS" : "NoLOS");	
	
					if((PathingLOSState == HaveLOS) && zone->pathing->NoHazards(From, To))
					{
						mlog(PATHING__DEBUG, "  No hazards. Running directly to target.");
						Route.clear();
						return To;
					}
					else
					{
						mlog(PATHING__DEBUG, "  Continuing on node path.");	
					}
				}
			}

			// If the player is moving, we don't want to recalculate our route too frequently.
			//
			if(static_cast<int>(Route.size()) <= RuleI(Pathing, RouteUpdateFrequencyNodeCount))
			{
				if(!PathingRouteUpdateTimerShort->Check())
				{
					mlog(PATHING__DEBUG, "Short route update timer not yet expired.");
					return zone->pathing->GetPathNodeCoordinates(Route.front());
				}
				mlog(PATHING__DEBUG, "Short route update timer expired.");
			}
			else
			{
				if(!PathingRouteUpdateTimerLong->Check())
				{
					mlog(PATHING__DEBUG, "Long route update timer not yet expired.");
					return zone->pathing->GetPathNodeCoordinates(Route.front());
				}
				mlog(PATHING__DEBUG, "Long route update timer expired.");
			}

			// We are already pathing, destination changed, no LOS. Find the nearest node to our destination.
			int DestinationPathNode= zone->pathing->FindNearestPathNode(To);

			// Destination unreachable via pathing, return direct route.
			if(DestinationPathNode == -1)
			{
				mlog(PATHING__DEBUG, "  Unable to find path node for new destination. Running straight to target.");
				Route.clear();
				return To;
			}
			// If the nearest path node to our new destination is the same as for the previous
			// one, we will carry on on our path.
			if(DestinationPathNode == Route.back())
			{
				mlog(PATHING__DEBUG, "  Same destination Node (%i). Continue with current path.", DestinationPathNode);

				NodeLoc = zone->pathing->GetPathNodeCoordinates(Route.front());

				// May need to refine this as rounding errors may mean we never have equality
				// Check if we have reached a path node.
				if(NodeLoc == From)
				{	
					mlog(PATHING__DEBUG, "  Arrived at node %i, moving to next one.\n", Route.front());

					NodeReached = true;

					PathingLastNodeVisited = Route.front();
			
					Route.pop_front();

					++PathingTraversedNodes;

					WaypointChanged = true;

					if(Route.size() > 0)
					{
						NextNode = Route.front();

						if(NextNode == -1)
						{
							// -1 indicates a teleport to the next node
							Route.pop_front();

							if(Route.size() == 0)
							{
								mlog(PATHING__DEBUG, "Missing node after teleport.");
								return To;
							}
	
							NextNode = Route.front();

							NodeLoc = zone->pathing->GetPathNodeCoordinates(NextNode);

							Teleport(NodeLoc);

							mlog(PATHING__DEBUG, "  TELEPORTED to %8.3f, %8.3f, %8.3f\n", NodeLoc.x, NodeLoc.y, NodeLoc.z);

							Route.pop_front();

							if(Route.size() == 0)
								return To;

							NextNode = Route.front();
						}
						// Return the coords of our next path node on the route.
						mlog(PATHING__DEBUG, "  Now moving to node %i", NextNode);

						zone->pathing->OpenDoors(PathingLastNodeVisited, NextNode, this);

						return zone->pathing->GetPathNodeCoordinates(NextNode);
					}
					else
					{
						mlog(PATHING__DEBUG, "  Reached end of path grid. Running direct to target.");
						return To;
					}
				}
				return NodeLoc;
			}
			else 
			{
				mlog(PATHING__DEBUG, "  Target moved. End node is different. Clearing route.");

				Route.clear();
				// We will now fall through to get a new route.
			}

		}


	}
	mlog(PATHING__DEBUG, "  Our route list is empty.");

	if((SameDestination) && !PathingLOSCheckTimer->Check())
	{
		mlog(PATHING__DEBUG, "  Destination same as before, LOS check timer not reached. Returning To.");
		return To;
	}

	PathingLOSState = UnknownLOS;

	PathingDestination = To;

	WaypointChanged = true;

	float Distance = VertexDistanceNoRoot(From, To);

	if((Distance <= RuleR(Pathing, MinDistanceForLOSCheckLong))
	   && (ABS(From.z - To.z) <= RuleR(Pathing, ZDiffThreshold)))
	{	
		mlog(PATHING__DEBUG, "  Checking for long LOS at distance %8.3f.", Distance);
	
		if(!zone->zonemap->LineIntersectsZone(HeadPosition, To, 1.0f, NULL, NULL))
			PathingLOSState = HaveLOS;
		else
			PathingLOSState = NoLOS;

		mlog(PATHING__DEBUG, "  LOS stats is %s", (PathingLOSState == HaveLOS) ? "HaveLOS" : "NoLOS");	
		
		if((PathingLOSState == HaveLOS) && zone->pathing->NoHazards(From, To))
		{
			mlog(PATHING__DEBUG, "Target is reachable. Running directly there.");
			return To;
		}
	}
	mlog(PATHING__DEBUG, "  Calculating new route to target.");

	Route = zone->pathing->FindRoute(From, To);

	PathingTraversedNodes = 0;

	if(Route.size() == 0)
	{
		mlog(PATHING__DEBUG, "  No route available, running direct.");

		return To;
	}

	if(SameDestination && (Route.front() == PathingLastNodeVisited))
	{
		mlog(PATHING__DEBUG, "  Probable loop detected. Same destination and Route.front() == PathingLastNodeVisited.");

		Route.clear();

		return To;
	}
	NodeLoc = zone->pathing->GetPathNodeCoordinates(Route.front());

	mlog(PATHING__DEBUG, "  New route determined, heading for node %i", Route.front());

	PathingLoopCount = 0;

	return NodeLoc;

}

int PathManager::FindNearestPathNode(VERTEX Position)
{

	// Find the nearest PathNode we have LOS to.
	//
	//

	float CandidateNodeRangeXY = RuleR(Pathing, CandidateNodeRangeXY);

	float CandidateNodeRangeZ = RuleR(Pathing, CandidateNodeRangeZ);

	int ClosestPathNodeToStart = -1;

	list<PathNodeSortStruct> SortedByDistance;

	PathNodeSortStruct TempNode;

	for(int i = 0 ; i < Head.PathNodeCount; ++i)
	{
		if((ABS(Position.x - PathNodes[i].v.x) <= CandidateNodeRangeXY) &&
		   (ABS(Position.y - PathNodes[i].v.y) <= CandidateNodeRangeXY) &&
		   (ABS(Position.z - PathNodes[i].v.z) <= CandidateNodeRangeZ))
		{
		   	TempNode.id = i;
			TempNode.Distance = VertexDistanceNoRoot(Position, PathNodes[i].v);
			SortedByDistance.push_back(TempNode);

		}
	}
	
	SortedByDistance.sort(SortPathNodesByDistance);

	for(list<PathNodeSortStruct>::iterator Iterator = SortedByDistance.begin(); Iterator != SortedByDistance.end(); ++Iterator)
	{
		_log(PATHING__DEBUG, "Checking Reachability of Node %i from Start Position.", PathNodes[(*Iterator).id].id);

		if(!zone->zonemap->LineIntersectsZone(Position, PathNodes[(*Iterator).id].v, 1.0f, NULL, NULL))
		{
			ClosestPathNodeToStart = (*Iterator).id;
			break;
		}
	}

	if(ClosestPathNodeToStart <0 ) {
		_log(PATHING__DEBUG, "No LOS to any starting Path Node within range.");
		return -1;
	}
	return ClosestPathNodeToStart;
}

bool PathManager::NoHazards(VERTEX From, VERTEX To)
{
	_ZP(Pathing_NoHazards);

	// Test the Z coordinate at the mid point.
	//
	VERTEX MidPoint((From.x + To.x) / 2, (From.y + To.y) / 2, From.z);

	float NewZ = zone->zonemap->FindBestZ(MAP_ROOT_NODE, MidPoint, NULL, NULL);

	if(ABS(NewZ - From.z) > RuleR(Pathing, ZDiffThreshold))
	{
		_log(PATHING__DEBUG, "  HAZARD DETECTED moving from %8.3f, %8.3f, %8.3f to %8.3f, %8.3f, %8.3f. Z Change is %8.3f",
			From.x, From.y, From.z, MidPoint.x, MidPoint.y, MidPoint.z, NewZ - From.z);

		return false;
	}
	else
	{
		_log(PATHING__DEBUG, "No HAZARD DETECTED moving from %8.3f, %8.3f, %8.3f to %8.3f, %8.3f, %8.3f. Z Change is %8.3f",
			From.x, From.y, From.z, MidPoint.x, MidPoint.y, MidPoint.z, NewZ - From.z);
	}

	return true;
}

bool PathManager::NoHazardsAccurate(VERTEX From, VERTEX To)
{
	float stepx, stepy, stepz, curx, cury, curz;
	VERTEX cur = From;
	float last_z = From.z;
	float step_size = 1.0;

	curx = From.x;
	cury = From.y;
	curz = From.z;

	do 
	{
		stepx = (float)To.x - curx;
		stepy = (float)To.y - cury;
		stepz = (float)To.z - curz;
		float factor =  sqrt(stepx*stepx + stepy*stepy + stepz*stepz);
		stepx = (stepx/factor)*step_size;
		stepy = (stepy/factor)*step_size;
		stepz = (stepz/factor)*step_size;
			
		VERTEX TestPoint(curx, cury, curz);
		float NewZ = zone->zonemap->FindBestZ(MAP_ROOT_NODE, TestPoint, NULL, NULL);
		if(ABS(NewZ - last_z) > 5.0)
		{
			_log(PATHING__DEBUG, "  HAZARD DETECTED moving from %8.3f, %8.3f, %8.3f to %8.3f, %8.3f, %8.3f. Best Z %8.3f, Z Change is %8.3f",
				From.x, From.y, From.z, TestPoint.x, TestPoint.y, TestPoint.z, NewZ, NewZ - From.z);
			return false;
		}
		last_z = NewZ;

		if(zone->watermap)
		{
			NodeRef n = zone->zonemap->SeekNode( zone->zonemap->GetRoot(), TestPoint.x, TestPoint.y);
			if(n != NODE_NONE) 
			{
				if(zone->watermap->InLiquid(From.x, From.y, From.z) || zone->watermap->InLiquid(To.x, To.y, To.z))
				{
					break;
				}

				if(zone->watermap->InLiquid(TestPoint.x, TestPoint.y, NewZ))
				{
					VERTEX TestPointWater(TestPoint.x, TestPoint.y, NewZ-0.5);
					VERTEX TestPointWaterDest(TestPointWater);
					VERTEX hit;
					TestPointWaterDest.z -= 500;
					float best_z2 = -999990;
					if(zone->zonemap->LineIntersectsNode(n, TestPointWater, TestPointWaterDest, &hit, NULL)) 
					{
						best_z2 = hit.z;
					}
					if(best_z2 == -999990)
					{
						_log(PATHING__DEBUG, "  HAZARD DETECTED, really deep water/lava!");
						return false;
					}
					else
					{
						if(ABS(NewZ - best_z2) > RuleR(Pathing, ZDiffThreshold))
						{
							_log(PATHING__DEBUG, "  HAZARD DETECTED, water is fairly deep at %8.3f units deep", ABS(NewZ - best_z2));
							return false;
						}
						else
						{
							_log(PATHING__DEBUG, "  HAZARD NOT DETECTED, water is shallow at %8.3f units deep", ABS(NewZ - best_z2));
						}
					}
				}
				else
				{
					_log(PATHING__DEBUG, "Hazard point not in water or lava!");
				}
			}
		}
		else
		{
			_log(PATHING__DEBUG, "No water map loaded for hazards!");
		}

		curx += stepx;
		cury += stepy;
		curz += stepz;

		cur.x = curx;
		cur.y = cury;
		cur.z = curz;

		if(ABS(curx - To.x) < step_size) cur.x = To.x;
		if(ABS(cury - To.y) < step_size) cur.y = To.y;
		if(ABS(curz - To.z) < step_size) cur.z = To.z;

	} 
	while(cur.x != To.x || cur.y != To.y || cur.z != To.z);
	return true;
}

void Mob::PrintRoute()
{

	printf("Route is : ");
	
	list<int>::iterator Iterator;

	for(Iterator = Route.begin(); Iterator !=Route.end(); ++Iterator)
	{
		printf("%i, ", (*Iterator));
	}

	printf("\n");

}

void PathManager::OpenDoors(int Node1, int Node2, Mob *ForWho)
{

	_ZP(Pathing_OpenDoors);

	if(!ForWho || (Node1 >= Head.PathNodeCount) || (Node2 >= Head.PathNodeCount) || (Node1 < 0) || (Node2 < 0))
		return;

	for(int i = 0; i < PATHNODENEIGHBOURS; ++i)
	{
		if(PathNodes[Node1].Neighbours[i].id == -1)
			return;

		if(PathNodes[Node1].Neighbours[i].id != Node2)
			continue;

		if(PathNodes[Node1].Neighbours[i].DoorID >= 0)
		{
			Doors *d = entity_list.FindDoor(PathNodes[Node1].Neighbours[i].DoorID);

			if(d && !d->IsDoorOpen() )
			{
				_log(PATHING__DEBUG, "Opening door %i for %s", PathNodes[Node1].Neighbours[i].DoorID, ForWho->GetName());

				d->ForceOpen(ForWho);
			}
			return;
		}
	}
}

//this assumes that the first point in the list is the player's
//current position, I dont know how well it works if its not.
void Client::SendPathPacket(vector<FindPerson_Point> &points) {
	if(points.size() < 2) {
		//empty length packet == not found.
		EQApplicationPacket outapp(OP_FindPersonReply, 0);
		QueuePacket(&outapp);
		return;
	}
	
	int len = sizeof(FindPersonResult_Struct) + (points.size()+1) * sizeof(FindPerson_Point);
	EQApplicationPacket *outapp = new EQApplicationPacket(OP_FindPersonReply, len);
	FindPersonResult_Struct* fpr=(FindPersonResult_Struct*)outapp->pBuffer;
	
	vector<FindPerson_Point>::iterator cur, end;
	cur = points.begin();
	end = points.end();
	unsigned int r;
	for(r = 0; cur != end; cur++, r++) {
		fpr->path[r] = *cur;

	}
	//put the last element into the destination field
	cur--;
	fpr->path[r] = *cur;
	fpr->dest = *cur;

	FastQueuePacket(&outapp);
	
	
}

PathNode* PathManager::FindPathNodeByCoordinates(float x, float y, float z)
{
	for(int i = 0; i < Head.PathNodeCount; ++i)
		if((PathNodes[i].v.x == x) && (PathNodes[i].v.y == y) && (PathNodes[i].v.z == z))
			return &PathNodes[i];

	return NULL;
}

int PathManager::GetRandomPathNode()
{
	return MakeRandomInt(0, Head.PathNodeCount - 1);

}

void PathManager::ShowPathNodeNeighbours(Client *c)
{
	if(!c || !c->GetTarget())
		return;


	PathNode *Node = zone->pathing->FindPathNodeByCoordinates(c->GetTarget()->GetX(), c->GetTarget()->GetY(), c->GetTarget()->GetZ());

	if(!Node)
	{
		c->Message(0, "Unable to find path node.");
		return;
	}
	c->Message(0, "Path node %4i", Node->id);

	for(int i = 0; i < Head.PathNodeCount; ++i)
	{
		char Name[64];

		if(PathNodes[i].id < 10)
			sprintf(Name, "%s000", DigitToWord(PathNodes[i].id));
		else if(PathNodes[i].id < 100)
			sprintf(Name, "%s_%s000", DigitToWord(PathNodes[i].id / 10), DigitToWord(PathNodes[i].id % 10));
		else
			sprintf(Name, "%s_%s_%s000", DigitToWord(PathNodes[i].id/100), DigitToWord((PathNodes[i].id % 100)/10), 
				DigitToWord(((PathNodes[i].id % 100) %10)));

		Mob *m = entity_list.GetMob(Name);

		if(m)
			m->SendIllusionPacket(151);
	}

	std::stringstream Neighbours;

	for(int i = 0; i < PATHNODENEIGHBOURS; ++i)
	{
		if(Node->Neighbours[i].id == -1)
			break;
		Neighbours << Node->Neighbours[i].id << ", ";	

		char Name[64];

		if(Node->Neighbours[i].id < 10)
			sprintf(Name, "%s000", DigitToWord(Node->Neighbours[i].id));
		else if(Node->Neighbours[i].id < 100)
			sprintf(Name, "%s_%s000", DigitToWord(Node->Neighbours[i].id / 10), DigitToWord(Node->Neighbours[i].id % 10));
		else
			sprintf(Name, "%s_%s_%s000", DigitToWord(Node->Neighbours[i].id/100), DigitToWord((Node->Neighbours[i].id % 100)/10), 
				DigitToWord(((Node->Neighbours[i].id % 100) %10)));

		Mob *m = entity_list.GetMob(Name);

		if(m)
			m->SendIllusionPacket(46);
	}
	c->Message(0, "Neighbours: %s", Neighbours.str().c_str());
}

void PathManager::NodeInfo(Client *c)
{
	if(!c)
	{
		return;
	}

	if(!c->GetTarget())
	{
		c->Message(0, "You must target a node.");
		return;
	}

	PathNode *Node = zone->pathing->FindPathNodeByCoordinates(c->GetTarget()->GetX(), c->GetTarget()->GetY(), c->GetTarget()->GetZ());
	if(!Node)
	{
		return;
	}

	c->Message(0, "Pathing node: %i at (%.2f, %.2f, %.2f) with bestz %.2f", 
		Node->id, Node->v.x, Node->v.y, Node->v.z, Node->bestz);

	bool neighbour = false;
	for(int x = 0; x < 50; ++x)
	{
		if(Node->Neighbours[x].id != -1)
		{
			if(!neighbour)
			{
				c->Message(0, "Neighbours found:");
				neighbour = true;
			}
			c->Message(0, "id: %i, distance: %.2f, door id: %i, is teleport: %i", 
				Node->Neighbours[x].id, Node->Neighbours[x].distance,
				Node->Neighbours[x].DoorID, Node->Neighbours[x].Teleport);
		}
	}

	if(!neighbour)
	{
		c->Message(0, "No neighbours found!");
	}
	return;
}

void PathManager::DumpPath(string filename)
{
	ofstream o_file;
	o_file.open(filename.c_str(), ios_base::binary | ios_base::trunc | ios_base::out);
	o_file.write("EQEMUPATH", 9);
	o_file.write((const char*)&Head, sizeof(Head));
	o_file.write((const char*)PathNodes, (sizeof(PathNode)*Head.PathNodeCount));
	o_file.close();
}

sint32 PathManager::AddNode(float x, float y, float z, float best_z, sint32 requested_id)
{
	sint32 new_id = -1;
	if(requested_id != 0)
	{
		new_id = requested_id;
		for(int i = 0; i < Head.PathNodeCount; ++i)
		{
			if(PathNodes[i].id == requested_id)
			{
				new_id = -1;
				break;
			}
		}
	}
	
	if(new_id == -1)
	{	
		for(int i = 0; i < Head.PathNodeCount; ++i)
		{
			if(PathNodes[i].id > new_id)
				new_id = PathNodes[i].id;
		}
		new_id++;
	}
	
	PathNode new_node;
	new_node.v.x = x;
	new_node.v.y = y;
	new_node.v.z = z;
	new_node.bestz = best_z;
	new_node.id = (uint16)new_id;
	for(int x = 0; x < PATHNODENEIGHBOURS; ++x)
	{
		new_node.Neighbours[x].id = -1;
		new_node.Neighbours[x].distance = 0.0;
		new_node.Neighbours[x].DoorID = -1;
		new_node.Neighbours[x].Teleport = 0;
	}

	Head.PathNodeCount++;
	if(Head.PathNodeCount > 1)
	{
		PathNode *t_PathNodes = new PathNode[Head.PathNodeCount];
		for(int x = 0; x < (Head.PathNodeCount - 1); ++x)
		{
			t_PathNodes[x].v.x = PathNodes[x].v.x;
			t_PathNodes[x].v.y = PathNodes[x].v.y;
			t_PathNodes[x].v.z = PathNodes[x].v.z;
			t_PathNodes[x].bestz = PathNodes[x].bestz;
			t_PathNodes[x].id = PathNodes[x].id;
			for(int n = 0; n < PATHNODENEIGHBOURS; ++n)
			{
				t_PathNodes[x].Neighbours[n].distance = PathNodes[x].Neighbours[n].distance;
				t_PathNodes[x].Neighbours[n].DoorID = PathNodes[x].Neighbours[n].DoorID;
				t_PathNodes[x].Neighbours[n].id = PathNodes[x].Neighbours[n].id;
				t_PathNodes[x].Neighbours[n].Teleport = PathNodes[x].Neighbours[n].Teleport;
			}

		}

		sint32 index = (Head.PathNodeCount - 1);
		t_PathNodes[index].v.x = new_node.v.x;
		t_PathNodes[index].v.y = new_node.v.y;
		t_PathNodes[index].v.z = new_node.v.z;
		t_PathNodes[index].bestz = new_node.bestz;
		t_PathNodes[index].id = new_node.id;
		for(int n = 0; n < PATHNODENEIGHBOURS; ++n)
		{
			t_PathNodes[index].Neighbours[n].distance = new_node.Neighbours[n].distance;
			t_PathNodes[index].Neighbours[n].DoorID = new_node.Neighbours[n].DoorID;
			t_PathNodes[index].Neighbours[n].id = new_node.Neighbours[n].id;
			t_PathNodes[index].Neighbours[n].Teleport = new_node.Neighbours[n].Teleport;
		}

		delete[] PathNodes;
		PathNodes = t_PathNodes;

		NPCType* npc_type = new NPCType;
		memset(npc_type, 0, sizeof(NPCType));
		if(new_id < 10)
			sprintf(npc_type->name, "%s", DigitToWord(new_id));
		else if(new_id < 100)
			sprintf(npc_type->name, "%s_%s", DigitToWord(new_id/10), DigitToWord(new_id % 10));
		else
			sprintf(npc_type->name, "%s_%s_%s", DigitToWord(new_id/100), DigitToWord((new_id % 100)/10), 
				DigitToWord(((new_id % 100) %10)));

		sprintf(npc_type->lastname, "%i", new_id);
		npc_type->cur_hp = 4000000;
		npc_type->max_hp = 4000000;
		npc_type->race = 151;
		npc_type->gender = 2;
		npc_type->class_ = 9;
		npc_type->deity= 1;
		npc_type->level = 75;
		npc_type->npc_id = 0;
		npc_type->loottable_id = 0;
		npc_type->texture = 1;
		npc_type->light = 0;
		npc_type->runspeed = 0;
		npc_type->d_meele_texture1 = 1;
		npc_type->d_meele_texture2 = 1;
		npc_type->merchanttype = 1;
		npc_type->MerchantMinLevel = 0;
		npc_type->bodytype = 1;
		npc_type->STR = 150;
		npc_type->STA = 150;
		npc_type->DEX = 150;
		npc_type->AGI = 150;
		npc_type->INT = 150;
		npc_type->WIS = 150;
		npc_type->CHA = 150;
		npc_type->findable = 1;

		NPC* npc = new NPC(npc_type, 0, new_node.v.x, new_node.v.y, new_node.v.z, 0, FlyMode1);
		npc->GiveNPCTypeData(npc_type);
		entity_list.AddNPC(npc, true, true);

		safe_delete_array(ClosedListFlag);
		ClosedListFlag = new int[Head.PathNodeCount];
		return new_id;
	}
	else
	{
		PathNodes = new PathNode[Head.PathNodeCount];
		PathNodes[0].v.x = new_node.v.x;
		PathNodes[0].v.y = new_node.v.y;
		PathNodes[0].v.z = new_node.v.z;
		PathNodes[0].bestz = new_node.bestz;
		PathNodes[0].id = new_node.id;
		for(int n = 0; n < PATHNODENEIGHBOURS; ++n)
		{
			PathNodes[0].Neighbours[n].distance = new_node.Neighbours[n].distance;
			PathNodes[0].Neighbours[n].DoorID = new_node.Neighbours[n].DoorID;
			PathNodes[0].Neighbours[n].id = new_node.Neighbours[n].id;
			PathNodes[0].Neighbours[n].Teleport = new_node.Neighbours[n].Teleport;
		}

		NPCType* npc_type = new NPCType;
		memset(npc_type, 0, sizeof(NPCType));
		if(new_id < 10)
			sprintf(npc_type->name, "%s", DigitToWord(new_id));
		else if(new_id < 100)
			sprintf(npc_type->name, "%s_%s", DigitToWord(new_id/10), DigitToWord(new_id % 10));
		else
			sprintf(npc_type->name, "%s_%s_%s", DigitToWord(new_id/100), DigitToWord((new_id % 100)/10), 
				DigitToWord(((new_id % 100) %10)));

		sprintf(npc_type->lastname, "%i", new_id);
		npc_type->cur_hp = 4000000;
		npc_type->max_hp = 4000000;
		npc_type->race = 151;
		npc_type->gender = 2;
		npc_type->class_ = 9;
		npc_type->deity= 1;
		npc_type->level = 75;
		npc_type->npc_id = 0;
		npc_type->loottable_id = 0;
		npc_type->texture = 1;
		npc_type->light = 0;
		npc_type->runspeed = 0;
		npc_type->d_meele_texture1 = 1;
		npc_type->d_meele_texture2 = 1;
		npc_type->merchanttype = 1;
		npc_type->MerchantMinLevel = 0;
		npc_type->bodytype = 1;
		npc_type->STR = 150;
		npc_type->STA = 150;
		npc_type->DEX = 150;
		npc_type->AGI = 150;
		npc_type->INT = 150;
		npc_type->WIS = 150;
		npc_type->CHA = 150;
		npc_type->findable = 1;

		NPC* npc = new NPC(npc_type, 0, new_node.v.x, new_node.v.y, new_node.v.z, 0, FlyMode1);
		npc->GiveNPCTypeData(npc_type);
		entity_list.AddNPC(npc, true, true);

		ClosedListFlag = new int[Head.PathNodeCount];

		return new_id;
	}
}

bool PathManager::DeleteNode(Client *c)
{
	if(!c)
	{
		return false;
	}

	if(!c->GetTarget())
	{
		c->Message(0, "You must target a node.");
		return false;
	}

	PathNode *Node = zone->pathing->FindPathNodeByCoordinates(c->GetTarget()->GetX(), c->GetTarget()->GetY(), c->GetTarget()->GetZ());
	if(!Node)
	{
		return false;
	}

	return DeleteNode(Node->id);
}

bool PathManager::DeleteNode(sint32 id)
{
	//if the current list is > 1 in size create a new list of size current size - 1
	//transfer all but the current node to this new list and delete our current list
	//set this new list to be our current list
	//else if the size is 1 just delete our current list and set it to zero.
	//go through and delete all ref in neighbors...

	if(Head.PathNodeCount > 1)
	{
		PathNode *t_PathNodes = new PathNode[Head.PathNodeCount-1];
		int32 index = 0;
		for(int x = 0; x < Head.PathNodeCount; x++)
		{
			if(PathNodes[x].id != id)
			{
				t_PathNodes[index].id = PathNodes[x].id;
				t_PathNodes[index].v.x = PathNodes[x].v.x;
				t_PathNodes[index].v.y = PathNodes[x].v.y;
				t_PathNodes[index].v.z = PathNodes[x].v.z;
				t_PathNodes[index].bestz = PathNodes[x].bestz;
				for(int n = 0; n < PATHNODENEIGHBOURS; ++n)
				{
					t_PathNodes[index].Neighbours[n].distance = PathNodes[x].Neighbours[n].distance;
					t_PathNodes[index].Neighbours[n].DoorID = PathNodes[x].Neighbours[n].DoorID;
					t_PathNodes[index].Neighbours[n].id = PathNodes[x].Neighbours[n].id;
					t_PathNodes[index].Neighbours[n].Teleport = PathNodes[x].Neighbours[n].Teleport;
				}
				index++;
			}
		}
		Head.PathNodeCount--;
		delete[] PathNodes;
		PathNodes = t_PathNodes;

		for(int y = 0; y < Head.PathNodeCount; ++y)
		{
			for(int n = 0; n < PATHNODENEIGHBOURS; ++n)
			{
				if(PathNodes[y].Neighbours[n].id == id)
				{
					PathNodes[y].Neighbours[n].Teleport = 0;
					PathNodes[y].Neighbours[n].DoorID = -1;
					PathNodes[y].Neighbours[n].distance = 0.0;
					PathNodes[y].Neighbours[n].id = -1;
				}
			}
		}
		safe_delete_array(ClosedListFlag);
		ClosedListFlag = new int[Head.PathNodeCount];
	}
	else
	{
		delete[] PathNodes;
		PathNodes = NULL;
	}
	return true;
}

void PathManager::ConnectNodeToNode(Client *c, sint32 Node2, sint32 teleport, sint32 doorid)
{
	if(!c)
	{
		return;
	}

	if(!c->GetTarget())
	{
		c->Message(0, "You must target a node.");
		return;
	}

	PathNode *Node = zone->pathing->FindPathNodeByCoordinates(c->GetTarget()->GetX(), c->GetTarget()->GetY(), c->GetTarget()->GetZ());
	if(!Node)
	{
		return;
	}

	c->Message(0, "Connecting %i to %i", Node->id, Node2);

	if(doorid == 0)
		ConnectNodeToNode(Node->id, Node2, teleport);
	else
		ConnectNodeToNode(Node->id, Node2, teleport, doorid);
}

void PathManager::ConnectNodeToNode(sint32 Node1, sint32 Node2, sint32 teleport, sint32 doorid)
{
	PathNode *a = NULL;
	PathNode *b = NULL;
	for(int x = 0; x < Head.PathNodeCount; ++x)
	{
		if(PathNodes[x].id == Node1)
		{
			a = &PathNodes[x];
			if(b)
				break;
		}
		else if(PathNodes[x].id == Node2)
		{
			b = &PathNodes[x];
			if(a)
				break;
		}
	}

	if(a == NULL || b == NULL)
		return;

	bool connect_a_to_b = true;
	if(NodesConnected(a, b))
		connect_a_to_b = false;

	bool connect_b_to_a = true;
	if(NodesConnected(b, a))
		connect_b_to_a = false;


	if(connect_a_to_b)
	{
		for(int a_i = 0; a_i < PATHNODENEIGHBOURS; ++a_i)
		{
			if(a->Neighbours[a_i].id == -1)
			{
				a->Neighbours[a_i].id = b->id;
				a->Neighbours[a_i].DoorID = doorid;
				a->Neighbours[a_i].Teleport = teleport;
				a->Neighbours[a_i].distance = VertexDistance(a->v, b->v);
				break;
			}
		}
	}

	if(connect_b_to_a)
	{
		for(int b_i = 0; b_i < PATHNODENEIGHBOURS; ++b_i)
		{
			if(b->Neighbours[b_i].id == -1)
			{
				b->Neighbours[b_i].id = a->id;
				b->Neighbours[b_i].DoorID = doorid;
				b->Neighbours[b_i].Teleport = teleport;
				b->Neighbours[b_i].distance = VertexDistance(a->v, b->v);
				break;
			}
		}
	}
}

void PathManager::ConnectNode(Client *c, sint32 Node2, sint32 teleport, sint32 doorid)
{
	if(!c)
	{
		return;
	}

	if(!c->GetTarget())
	{
		c->Message(0, "You must target a node.");
		return;
	}

	PathNode *Node = zone->pathing->FindPathNodeByCoordinates(c->GetTarget()->GetX(), c->GetTarget()->GetY(), c->GetTarget()->GetZ());
	if(!Node)
	{
		return;
	}

	c->Message(0, "Connecting %i to %i", Node->id, Node2);

	if(doorid == 0)
		ConnectNode(Node->id, Node2, teleport);
	else
		ConnectNode(Node->id, Node2, teleport, doorid);
}

void PathManager::ConnectNode(sint32 Node1, sint32 Node2, sint32 teleport, sint32 doorid)
{
	PathNode *a = NULL;
	PathNode *b = NULL;
	for(int x = 0; x < Head.PathNodeCount; ++x)
	{
		if(PathNodes[x].id == Node1)
		{
			a = &PathNodes[x];
			if(b)
				break;
		}
		else if(PathNodes[x].id == Node2)
		{
			b = &PathNodes[x];
			if(a)
				break;
		}
	}

	if(a == NULL || b == NULL)
		return;

	bool connect_a_to_b = true;
	if(NodesConnected(a, b))
		connect_a_to_b = false;

	if(connect_a_to_b)
	{
		for(int a_i = 0; a_i < PATHNODENEIGHBOURS; ++a_i)
		{
			if(a->Neighbours[a_i].id == -1)
			{
				a->Neighbours[a_i].id = b->id;
				a->Neighbours[a_i].DoorID = doorid;
				a->Neighbours[a_i].Teleport = teleport;
				a->Neighbours[a_i].distance = VertexDistance(a->v, b->v);
				break;
			}
		}
	}
}

void PathManager::DisconnectNodeToNode(Client *c, sint32 Node2)
{
	if(!c)
	{
		return;
	}

	if(!c->GetTarget())
	{
		c->Message(0, "You must target a node.");
		return;
	}

	PathNode *Node = zone->pathing->FindPathNodeByCoordinates(c->GetTarget()->GetX(), c->GetTarget()->GetY(), c->GetTarget()->GetZ());
	if(!Node)
	{
		return;
	}

	DisconnectNodeToNode(Node->id, Node2);
}

void PathManager::DisconnectNodeToNode(sint32 Node1, sint32 Node2)
{
	PathNode *a = NULL;
	PathNode *b = NULL;
	for(int x = 0; x < Head.PathNodeCount; ++x)
	{
		if(PathNodes[x].id == Node1)
		{
			a = &PathNodes[x];
			if(b)
				break;
		}
		else if(PathNodes[x].id == Node2)
		{
			b = &PathNodes[x];
			if(a)
				break;
		}
	}

	if(a == NULL || b == NULL)
		return;

	bool disconnect_a_from_b = false;
	if(NodesConnected(a, b))
		 disconnect_a_from_b = true;

	bool disconnect_b_from_a = false;
	if(NodesConnected(b, a))
		disconnect_b_from_a = true;

	if(disconnect_a_from_b)
	{
		for(int a_i = 0; a_i < PATHNODENEIGHBOURS; ++a_i)
		{
			if(a->Neighbours[a_i].id == b->id)
			{
				a->Neighbours[a_i].distance = 0.0;
				a->Neighbours[a_i].DoorID = -1;
				a->Neighbours[a_i].id = -1;
				a->Neighbours[a_i].Teleport = 0;
				break;
			}
		}
	}

	if(disconnect_b_from_a)
	{
		for(int b_i = 0; b_i < PATHNODENEIGHBOURS; ++b_i)
		{
			if(b->Neighbours[b_i].id == a->id)
			{
				b->Neighbours[b_i].distance = 0.0;
				b->Neighbours[b_i].DoorID = -1;
				b->Neighbours[b_i].id = -1;
				b->Neighbours[b_i].Teleport = 0;
				break;
			}
		}
	}
}

void PathManager::MoveNode(Client *c)
{
	if(!c)
	{
		return;
	}

	if(!c->GetTarget())
	{
		c->Message(0, "You must target a node.");
		return;
	}

	PathNode *Node = zone->pathing->FindPathNodeByCoordinates(c->GetTarget()->GetX(), c->GetTarget()->GetY(), c->GetTarget()->GetZ());
	if(!Node)
	{
		return;
	}

	Node->v.x = c->GetX();
	Node->v.y = c->GetY();
	Node->v.z = c->GetZ();

	if(zone->zonemap)
	{
		VERTEX loc(c->GetX(), c->GetY(), c->GetZ());
		Node->bestz = zone->zonemap->FindBestZ(MAP_ROOT_NODE, loc, NULL, NULL);
	}
	else
	{
		Node->bestz = Node->v.z;
	}
}

void PathManager::DisconnectAll(Client *c)
{
	if(!c)
	{
		return;
	}

	if(!c->GetTarget())
	{
		c->Message(0, "You must target a node.");
		return;
	}

	PathNode *Node = zone->pathing->FindPathNodeByCoordinates(c->GetTarget()->GetX(), c->GetTarget()->GetY(), c->GetTarget()->GetZ());
	if(!Node)
	{
		return;
	}

	for(int x = 0; x < PATHNODENEIGHBOURS; ++x)
	{
		Node->Neighbours[x].distance = 0;
		Node->Neighbours[x].Teleport = 0;
		Node->Neighbours[x].DoorID = -1;
		Node->Neighbours[x].id = -1;
	}

	for(int i = 0; i < Head.PathNodeCount; ++i)
	{
		if(PathNodes[i].id == Node->id)
			continue;

		for(int ix = 0; ix < PATHNODENEIGHBOURS; ++ix)
		{
			if(PathNodes[i].Neighbours[ix].id == Node->id)
			{
				PathNodes[i].Neighbours[ix].distance = 0;
				PathNodes[i].Neighbours[ix].Teleport = 0;
				PathNodes[i].Neighbours[ix].id = -1;
				PathNodes[i].Neighbours[ix].DoorID = -1;
			}
		}
	}
}

//checks if anything in a points to b
bool PathManager::NodesConnected(PathNode *a, PathNode *b)
{
	if(!a)
		return false;

	if(!b)
		return false;

	for(int x = 0; x < PATHNODENEIGHBOURS; ++x)
	{
		if(a->Neighbours[x].id == b->id)
			return true;
	}
	return false;
}

bool PathManager::CheckLosFN(VERTEX a, VERTEX b)
{
	if(zone->zonemap)
	{
		FACE *onhit;
		NodeRef mynode;
		NodeRef onode;
		VERTEX hit;

		VERTEX myloc; 
		VERTEX oloc;

		myloc.x = a.x;
		myloc.y = a.y;
		myloc.z = a.z;

		oloc.x = b.x;
		oloc.y = b.y;
		oloc.z = b.z;


		if(zone->zonemap->LineIntersectsZone(myloc, oloc, 1.0f, NULL, NULL))
		{
			return false;
		}
	}
	return true;
}

void PathManager::ProcessNodesAndSave(string filename)
{
	if(zone->zonemap)
	{
		for(int i = 0; i < Head.PathNodeCount; ++i)
		{
			for(int in = 0; in < PATHNODENEIGHBOURS; ++in)
			{
				PathNodes[i].Neighbours[in].distance = 0.0;
				PathNodes[i].Neighbours[in].DoorID = -1;
				PathNodes[i].Neighbours[in].id = -1;
				PathNodes[i].Neighbours[in].Teleport = 0;
			}
		}

		for(int x = 0; x < Head.PathNodeCount; ++x)
		{
			for(int y = 0; y < Head.PathNodeCount; ++y)
			{
				if(y == x) //can't connect to ourselves.
					continue;

				if(!NodesConnected(&PathNodes[x], &PathNodes[y]))
				{
					if(VertexDistance(PathNodes[x].v, PathNodes[y].v) <= 200)
					{
						if(CheckLosFN(PathNodes[x].v, PathNodes[y].v))
						{
							if(NoHazardsAccurate(PathNodes[x].v, PathNodes[y].v))
							{
								ConnectNodeToNode(PathNodes[x].id, PathNodes[y].id, 0, 0);
							}
						}
					}
				}
			}
		}
	}
	DumpPath(filename);
}

void PathManager::ResortConnections()
{
	NeighbourNode Neigh[PATHNODENEIGHBOURS];
	for(int x = 0; x < Head.PathNodeCount; ++x)
	{
		int index = 0;
		for(int y = 0; y < PATHNODENEIGHBOURS; ++y)
		{
			Neigh[y].distance = 0;
			Neigh[y].DoorID = -1;
			Neigh[y].id = -1;
			Neigh[y].Teleport = 0;
		}

		for(int z = 0; z < PATHNODENEIGHBOURS; ++z)
		{
			if(PathNodes[x].Neighbours[z].id != -1)
			{
				Neigh[index].id = PathNodes[x].Neighbours[z].id;
				Neigh[index].distance = PathNodes[x].Neighbours[z].distance;
				Neigh[index].DoorID = PathNodes[x].Neighbours[z].DoorID;
				Neigh[index].Teleport = PathNodes[x].Neighbours[z].Teleport;
				index++;
			}
		}

		for(int i = 0; i < PATHNODENEIGHBOURS; ++i)
		{
			PathNodes[x].Neighbours[i].distance = 0;
			PathNodes[x].Neighbours[i].DoorID = -1;
			PathNodes[x].Neighbours[i].id = -1;
			PathNodes[x].Neighbours[i].Teleport = 0;
		}

		for(int z = 0; z < PATHNODENEIGHBOURS; ++z)
		{
			PathNodes[x].Neighbours[z].distance = Neigh[z].distance;
			PathNodes[x].Neighbours[z].DoorID = Neigh[z].DoorID;
			PathNodes[x].Neighbours[z].id = Neigh[z].id;
			PathNodes[x].Neighbours[z].Teleport = Neigh[z].Teleport;
		}
	}
}

void PathManager::QuickConnect(Client *c, bool set)
{
	if(!c)
	{
		return;
	}

	if(!c->GetTarget())
	{
		c->Message(0, "You must target a node.");
		return;
	}

	PathNode *Node = zone->pathing->FindPathNodeByCoordinates(c->GetTarget()->GetX(), c->GetTarget()->GetY(), c->GetTarget()->GetZ());
	if(!Node)
	{
		return;
	}

	if(set)
	{
		c->Message(0, "Setting %i to the quick connect target", Node->id);
		QuickConnectTarget = Node->id;
	}
	else
	{
		if(QuickConnectTarget >= 0)
		{
			ConnectNodeToNode(QuickConnectTarget, Node->id);
		}
	}
}

struct InternalPathSort
{
	sint16 old_id;
	sint16 new_id;
};

void PathManager::SortNodes()
{
	std::vector<InternalPathSort> sorted_vals;
	for(int x = 0; x < Head.PathNodeCount; ++x)
	{
		InternalPathSort tmp;
		tmp.old_id = PathNodes[x].id;
		sorted_vals.push_back(tmp);
	}

	PathNode *t_PathNodes = new PathNode[Head.PathNodeCount];
	memcpy(t_PathNodes, PathNodes, sizeof(PathNode)*Head.PathNodeCount);
	for(int i = 0; i < Head.PathNodeCount; ++i)
	{
		for(int j = 0; j < sorted_vals.size(); ++j)
		{
			if(sorted_vals[j].old_id == PathNodes[i].id)
			{
				if(i != PathNodes[i].id)
				{
					printf("Assigning new id of index %i differs from old id %i\n", i, PathNodes[i].id);
				}
				sorted_vals[j].new_id = i;
			}
		}
		t_PathNodes[i].id = i;
	}

	for(int y = 0; y < Head.PathNodeCount; ++y)
	{
		for(int z = 0; z < PATHNODENEIGHBOURS; ++z)
		{
			if(PathNodes[y].Neighbours[z].id != -1)
			{
				int new_val = -1;
				for(int c = 0; c < sorted_vals.size(); ++c)
				{
					if(PathNodes[y].Neighbours[z].id == sorted_vals[c].old_id)
					{
						new_val = sorted_vals[c].new_id;
						break;
					}
				}
				if(new_val != -1)
				{
					if(t_PathNodes[y].Neighbours[z].id != new_val)
					{
						printf("changing neighbor value to %i from %i\n", new_val, t_PathNodes[y].Neighbours[z].id);
					}
					t_PathNodes[y].Neighbours[z].id = new_val;
				}
			}
		}
	}
	safe_delete_array(PathNodes);
	PathNodes = t_PathNodes;
}
