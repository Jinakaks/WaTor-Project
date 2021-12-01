#include "WorldT.h"

bool WorldT::CoordIsGood(const CoordT & coord) {
	return (coord.x < worldWidth && coord.x >= 0) && (coord.y < worldHeight && coord.y >= 0);
}



CoordT WorldT::Up(const CoordT& currentLoc) {
    int newY = currentLoc.y;
    newY--;
    if(newY < 0) {
        newY = WorldHeight() - 1;
    }

    CoordT newcoord(currentLoc.x,newY);
    return newcoord;
}

CoordT WorldT::Down(const CoordT& currentLoc) {
    int newY = currentLoc.y;
    newY++;
    if(newY >= WorldHeight()) {
        newY = 0;
    }

    CoordT newcoord(currentLoc.x,newY);
    return newcoord;
}

CoordT WorldT::Left(const CoordT& currentLoc) {
    int newX = currentLoc.x;
    newX--;
    if(newX < 0) {
        newX = WorldWidth() - 1;
    }

    CoordT newcoord(newX,currentLoc.y);
    return newcoord;
}

CoordT WorldT::Right(const CoordT& currentLoc) {
    int newX = currentLoc.x;
    newX++;
    if(newX >= WorldWidth() ) {
        newX = 0;
    }

    CoordT newcoord(newX,currentLoc.y);
    return newcoord;
}

CoordT WorldT::ZeroCoord() {
    if(worldWidth > 0 && worldHeight > 0) {
        return CoordT(0,0);
    }

}


CoordT WorldT::RandomCoord(bool unoccupied) {
    CoordT newCoord(0,0);
    
    std::default_random_engine generator;
    std::uniform_int_distribution<int> xRand(0,worldWidth - 1);
    std::uniform_int_distribution<int> yRand(0,worldHeight - 1);

    newCoord.x = xRand(generator);
    newCoord.y = yRand(generator);

    int totalCoords = worldHeight*worldWidth;
    if(unoccupied && (totalCoords > existingEntities.size() - positionsToDelete.size())) {
        while(not IsEmpty(newCoord)) {
            newCoord.x = xRand(generator);
            newCoord.y = yRand(generator);
        }
    }
    return newCoord;
}

int WorldT::WorldWidth() const {
    return worldWidth;
}

int WorldT::WorldHeight() const {
    return worldHeight;
}


bool WorldT::IsEmpty(const CoordT& entityLoc) {
    return Map[entityLoc.x][entityLoc.y] == nullptr;
}

void WorldT::WhatIsNearby(const CoordT& entityLoc,int sight,std::vector<CoordT>& nearbyCoords) {
    
    CoordT currentCoord(entityLoc);
    if(sight >= 1) {
        
        for(int i = 0; i < sight; i++) {
            currentCoord = Up(currentCoord);
            nearbyCoords.emplace_back(currentCoord);

            for(int x = 0; x < i + 1; x++) {
                currentCoord = Right(currentCoord);
                currentCoord = Down(currentCoord);
                nearbyCoords.emplace_back(currentCoord);
            }
            for(int x = 0; x < i + 1; x++) {
                currentCoord = Left(currentCoord);
                currentCoord = Down(currentCoord);
                nearbyCoords.emplace_back(currentCoord);
            }
            for(int x = 0; x < i + 1; x++) {
                currentCoord = Left(currentCoord);
                currentCoord = Up(currentCoord);
                nearbyCoords.emplace_back(currentCoord);
            }
            for(int x = 0; x < i + 1; x++) {
                currentCoord = Right(currentCoord);
                currentCoord = Up(currentCoord);
                nearbyCoords.emplace_back(currentCoord);
            }
            //this was done to prevent the repeated up
            //and so "up" will always be first
            nearbyCoords.pop_back();
        }
    }
}

void WorldT::WhatIsNearbyOcto(const CoordT& entityLoc,int sight,std::vector<CoordT>& nearbyCoords) {
    CoordT currentCoord(entityLoc);
    if(sight >= 1) {
        
        for(int i = 0; i < sight; i++) {
            currentCoord = Up(currentCoord);
            nearbyCoords.emplace_back(currentCoord);

            for(int x = 0; x < i + 1; x++) {
                currentCoord = Right(currentCoord);
                nearbyCoords.emplace_back(currentCoord);
            }
            for(int x = 0; x < 2*(i + 1); x++) {
                currentCoord = Down(currentCoord);
                nearbyCoords.emplace_back(currentCoord);
            }

            for(int x = 0; x < 2*(i + 1); x++) {
                currentCoord = Left(currentCoord);
                nearbyCoords.emplace_back(currentCoord);
            }
            for(int x = 0; x < 2*(i + 1); x++) {
                currentCoord = Up(currentCoord);
                nearbyCoords.emplace_back(currentCoord);
            }
            for(int x = 0; x < i + 1; x++) {
                currentCoord = Right(currentCoord);
                nearbyCoords.emplace_back(currentCoord);
            }
            //this was done to prevent the repeated up
            //and so "up" will always be first
            nearbyCoords.pop_back();
        }
    }
}

std::shared_ptr<EntityT> WorldT::EntityAt(const CoordT& entityLoc) {
    return Map[entityLoc.x][entityLoc.y];
}


void WorldT::MoveEntity(const CoordT& locFrom, const CoordT& locTo) {
    if(CoordIsGood(locFrom) && CoordIsGood(locTo)) {
        Map[locTo.x][locTo.y] = Map[locFrom.x][locFrom.y];
        Map[locFrom.x][locFrom.y] = nullptr;

        std::list<CoordT>::iterator oldPos;
        oldPos = std::find(existingEntities.begin(),existingEntities.end(),locFrom);
        *oldPos = locTo;
    }
    return;
}

void WorldT::AddEntity(std::shared_ptr<EntityT> entityToAdd, const CoordT& locTo) {
    if(CoordIsGood(locTo)) {
        Map[locTo.x][locTo.y] = entityToAdd;
        existingEntities.push_front(locTo);
    }
    return;
}

void WorldT::RemoveEntity(const CoordT& locTo) {
    if(CoordIsGood(locTo)) {
        Map[locTo.x][locTo.y] = nullptr;
        
        std::list<CoordT>::iterator oldPos;
        oldPos = std::find(existingEntities.begin(),existingEntities.end(),locTo);
        positionsToDelete.push_back(oldPos);
        //we wait to delete the postions until the end of the tick
        existingEntities.push_front(locTo);
    }
    return;
}

void WorldT::Tick() {
    
    std::list<CoordT>::iterator ptr;

    for(ptr = existingEntities.begin(); ptr != existingEntities.end(); ptr++) {
        if(Map[ptr->x][ptr->y] != nullptr) {
            Map[ptr->x][ptr->y]->TakeTurn(*ptr,*this);
        }
    }

    for(size_t i = 0;i < positionsToDelete.size();i++) {
        existingEntities.erase(positionsToDelete[i]);
    }
    positionsToDelete.clear();

}

WorldT::WorldT(int wW, int wH): worldWidth(wW), worldHeight(wH) {
    Map.resize(worldWidth);
    for(int i = 0; i < worldWidth; i++) {
        Map[i].resize(worldHeight);
    }
    return;
}
